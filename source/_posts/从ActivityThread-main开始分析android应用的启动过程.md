---
title: 从ActivityThread.main开始分析android应用的启动过程
date: 2020-06-20 17:13:11
tag: Android源码阅读
category: Android源码
---

最近准备学习一下 [FART](https://github.com/hanbinglengyue/FART) 的源码及其相关文章的过程中发现绕不开 Android 应用的启动过程，所以准备阅读一下 android 的源码，了解一下相关知识。

本文研究的源码为 Android 9 。

<!-- TOC -->

- [ActivityThread.main](#activitythreadmain)
- [ActivityThread.attach](#activitythreadattach)
- [ActivityManagerService.attachApplication](#activitymanagerserviceattachapplication)
- [ActivityManagerService.attachApplicationLocked](#activitymanagerserviceattachapplicationlocked)
- [ActivityThread.ApplicationThread.bindApplication](#activitythreadapplicationthreadbindapplication)
- [ActivityThread.handleBindApplication](#activitythreadhandlebindapplication)
- [ActivityManagerService.attachApplicationLocked](#activitymanagerserviceattachapplicationlocked-1)
- [ActivityStackSupervisor.realStartActivityLocked](#activitystacksupervisorrealstartactivitylocked)
- [TransactionExecutor.execute](#transactionexecutorexecute)
- [ActivityThread.handleLaunchActivity](#activitythreadhandlelaunchactivity)
- [Instrumentation.callActivityOnCreate](#instrumentationcallactivityoncreate)

<!-- /TOC -->

# ActivityThread.main

在 `ActivityThread.main` 方法中对 `ActivityThread` 进行了初始化，创建了主线程的 `Looper` 对象并调用 `Looper.loop()` 方法启动 Looper，把自定义 Handler 类 H 的对象作为主线程的 handler 。接下来跳转到 `ActivityThread.attach` 方法。

```
// frameworks/base/core/java/android/app/ActivityThread.java

public static void main(String[] args) {

    ...
    
    // 创建 ActivityThread 实例
    ActivityThread thread = new ActivityThread();
    // 完成一系列初始化工作，需要跟进分析
    thread.attach(false, startSeq);

    if (sMainThreadHandler == null) {
        sMainThreadHandler = thread.getHandler();
    }

    if (false) {
        Looper.myLooper().setMessageLogging(new
                LogPrinter(Log.DEBUG, "ActivityThread"));
    }

    // End of event ActivityThreadMain.
    Trace.traceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER);
    //// 主线程进入消息循环
    Looper.loop();

    throw new RuntimeException("Main thread loop unexpectedly exited");
}
```

# ActivityThread.attach

在 `ActivityThread.attach` 方法中，会通过 `ActivityManagerService` 为这个应用绑定一个 `Application` , 这里的关键方法为 `attachApplication` , 我们需要进一步跟进。

```
// frameworks/base/core/java/android/app/ActivityThread.java

private void attach(boolean system, long startSeq) {
    sCurrentActivityThread = this;
    mSystemThread = system;
    if (!system) {
        ViewRootImpl.addFirstDrawHandler(new Runnable() {
            @Override
            public void run() {
                ensureJitEnabled();
            }
        });
        android.ddm.DdmHandleAppName.setAppName("<pre-initialized>",
                                                UserHandle.myUserId());
        RuntimeInit.setApplicationObject(mAppThread.asBinder());

        // 获得AMS(ActivityManagerService)实例, AMS的log tag: "ActivityManager"
        final IActivityManager mgr = ActivityManager.getService();
        try {
            // 把 ApplicationThread 对象传给AMS
            mgr.attachApplication(mAppThread, startSeq);
        } catch (RemoteException ex) {
            throw ex.rethrowFromSystemServer();
        }
        ...
    }
}
```

# ActivityManagerService.attachApplication

这个方法没什么好看的，直接无脑跟进 `attachApplicationLocked` 。
```
// frameworks/base/services/core/java/com/android/server/am/ActivityManagerService.java

public final void attachApplication(IApplicationThread thread, long startSeq) {
    synchronized (this) {
        int callingPid = Binder.getCallingPid();
        final int callingUid = Binder.getCallingUid();
        final long origId = Binder.clearCallingIdentity();
        //继续跟进
        attachApplicationLocked(thread, callingPid, callingUid, startSeq);
        Binder.restoreCallingIdentity(origId);
    }
}
```

# ActivityManagerService.attachApplicationLocked

通过 binder , 跨进程调用 ApplicationThread 的 bindApplication() 方法。
```
// frameworks/base/services/core/java/com/android/server/am/ActivityManagerService.java

private final boolean attachApplicationLocked(IApplicationThread thread,
        int pid, int callingUid, long startSeq) {

        ...


        else if (app.instr != null) {

            // 通过 binder , 跨进程调用 ApplicationThread 的 bindApplication() 方法, 下面代码逻辑重回 ActivityThread.java
            thread.bindApplication(processName, appInfo, providers,
                    app.instr.mClass,
                    profilerInfo, app.instr.mArguments,
                    app.instr.mWatcher,
                    app.instr.mUiAutomationConnection, testMode,
                    mBinderTransactionTrackingEnabled, enableTrackAllocation,
                    isRestrictedBackupMode || !normalMode, app.persistent,
                    new Configuration(getGlobalConfiguration()), app.compat,
                    getCommonServicesLocked(app.isolated),
                    mCoreSettingsObserver.getCoreSettingsLocked(),
                    buildSerial, isAutofillCompatEnabled);
        } else {
            thread.bindApplication(processName, appInfo, providers, null, profilerInfo,
                    null, null, null, testMode,
                    mBinderTransactionTrackingEnabled, enableTrackAllocation,
                    isRestrictedBackupMode || !normalMode, app.persistent,
                    new Configuration(getGlobalConfiguration()), app.compat,
                    getCommonServicesLocked(app.isolated),
                    mCoreSettingsObserver.getCoreSettingsLocked(),
                    buildSerial, isAutofillCompatEnabled);
        }
        if (profilerInfo != null) {
            profilerInfo.closeFd();
            profilerInfo = null;
        }

    ...
}
```

# ActivityThread.ApplicationThread.bindApplication
`bindApplication` 最终通过发送消息 `BIND_APPLICATION` 给 `H` 处理，然后调用 `handleBindApplication` 处理该消息。
```
// frameworks/base/core/java/android/app/ActivityThread.java

public final void bindApplication(String processName, ApplicationInfo appInfo,
        List<ProviderInfo> providers, ComponentName instrumentationName,
        ProfilerInfo profilerInfo, Bundle instrumentationArgs,
        IInstrumentationWatcher instrumentationWatcher,
        IUiAutomationConnection instrumentationUiConnection, int debugMode,
        boolean enableBinderTracking, boolean trackAllocation,
        boolean isRestrictedBackupMode, boolean persistent, Configuration config,
        CompatibilityInfo compatInfo, Map services, Bundle coreSettings,
        String buildSerial, boolean autofillCompatibilityEnabled) {
    ...

    // 发消息
    sendMessage(H.BIND_APPLICATION, data);
}

// frameworks/base/core/java/android/app/ActivityThread.java

private void sendMessage(int what, Object obj, int arg1, int arg2, boolean async) {
    if (DEBUG_MESSAGES) Slog.v(
        TAG, "SCHEDULE " + what + " " + mH.codeToString(what)
        + ": " + arg1 + " / " + obj);
    Message msg = Message.obtain();
    msg.what = what;
    msg.obj = obj;
    msg.arg1 = arg1;
    msg.arg2 = arg2;
    if (async) {
        msg.setAsynchronous(true);
    }
    //通过mH把BIND_APPLICATION消息发给H处理
    mH.sendMessage(msg);
}

// frameworks/base/core/java/android/app/ActivityThread.java

public void handleMessage(Message msg) {
    if (DEBUG_MESSAGES) Slog.v(TAG, ">>> handling: " + codeToString(msg.what));
    switch (msg.what) {
        case BIND_APPLICATION:
            Trace.traceBegin(Trace.TRACE_TAG_ACTIVITY_MANAGER, "bindApplication");
            AppBindData data = (AppBindData)msg.obj;
            // 调用ActivityThread的handleBindApplication()方法处理
            handleBindApplication(data);
            Trace.traceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER);
            break;
        ...
    }
}
```

# ActivityThread.handleBindApplication
`handleBindApplication` 开始创建 `Application` 类相关的的数据，并调用 `Application` 对象的 `attach` 和 `onCreate` 方法。
```
// frameworks/base/core/java/android/app/ActivityThread.java

private void handleBindApplication(AppBindData data) {

    // 1: 创建LoadedApk对象
    data.info = getPackageInfoNoCheck(data.appInfo, data.compatInfo);

    ...

    // 2: 创建ContextImpl对象; 并对其进行初始化.
    final ContextImpl appContext = ContextImpl.createAppContext(this, data.info);
    updateLocaleListFromAppContext(appContext,
            mResourcesManager.getConfiguration().getLocales());

        ...

        try {
            //  3: 创建 Instrumentation
            final ClassLoader cl = instrContext.getClassLoader();
            mInstrumentation = (Instrumentation)
                cl.loadClass(data.instrumentationName.getClassName()).newInstance();
        } catch (Exception e) {
            throw new RuntimeException(
                "Unable to instantiate instrumentation "
                + data.instrumentationName + ": " + e.toString(), e);
        }
        ...

        //  4: 创建Application对象;在makeApplication函数中调用了newApplication，
        //  在该函数中又调用了app.attach(context)，在attach函数中调用了Application.attachBaseContext函数
        app = data.info.makeApplication(data.restrictedBackupMode, null);

       ....
        try {
            // 调Application的生命周期函数 onCreate()
            mInstrumentation.callApplicationOnCreate(app);
        } catch (Exception e) {
        if (!mInstrumentation.onException(app, e)) {
            throw new RuntimeException(
                "Unable to create application " + app.getClass().getName()
                + ": " + e.toString(), e);
        }
}
}

//frameworks/base/core/java/android/app/LoadedApk.java

public Application makeApplication(boolean forceDefaultAppClass,
        Instrumentation instrumentation) {
    ...
    try {
        java.lang.ClassLoader cl = getClassLoader();
        if (!mPackageName.equals("android")) {
            Trace.traceBegin(Trace.TRACE_TAG_ACTIVITY_MANAGER,
                    "initializeJavaContextClassLoader");
            initializeJavaContextClassLoader();
            Trace.traceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER);
        }
        ContextImpl appContext = ContextImpl.createAppContext(mActivityThread, this);
        // 调用 attch 函数
        app = mActivityThread.mInstrumentation.newApplication(
                cl, appClass, appContext);
        appContext.setOuterContext(app);
    } catch (Exception e) {
        ...
    }
    return app;
}

// frameworks/base/core/java/android/app/Instrumentation.java

public Application newApplication(ClassLoader cl, String className, Context context)
        throws InstantiationException, IllegalAccessException, 
        ClassNotFoundException {
    Application app = getFactory(context.getPackageName())
            .instantiateApplication(cl, className);
    app.attach(context);
    return app;
}


// frameworks/base/core/java/android/app/Instrumentation.java

public void callApplicationOnCreate(Application app) {
    app.onCreate();
}
    
```
至此整个 `Application` 类的流程就结束了，下面将继续跟进 Activity 的流程 。

# ActivityManagerService.attachApplicationLocked
回到 `attachApplicationLocked` 方法，继续向下跟，可以发现一段注释 `See if the top visible activity is waiting to run in this process` , 这里就是调用 `Activity` 的关键, 跟进 `mStackSupervisor.attachApplicationLocked` 后，发现关键方法为 `realStartActivityLocked` 。

```
// frameworks/base/services/core/java/com/android/server/am/ActivityManagerService.java

private final boolean attachApplicationLocked(IApplicationThread thread,
        int pid, int callingUid, long startSeq) {

    ...

    // See if the top visible activity is waiting to run in this process...
    if (normalMode) {
        try {
            // 开始调用 activity 的 oncreate 方法
            if (mStackSupervisor.attachApplicationLocked(app)) {
                didSomething = true;
            }
        } catch (Exception e) {
            Slog.wtf(TAG, "Exception thrown launching activities in " + app, e);
            badApp = true;
        }
    }
    ...
}

// frameworks/base/services/core/java/com/android/server/am/ActivityStackSupervisor.java

boolean attachApplicationLocked(ProcessRecord app) throws RemoteException {

    ...

    if (realStartActivityLocked(activity, app,
            top == activity /* andResume */, true /* checkConfig */)) {
        didSomething = true;
    }
    ...
    return didSomething;
}
```
# ActivityStackSupervisor.realStartActivityLocked
在 `realStartActivityLocked` 创建了一个 `activity launch transaction`, 然后再调用 `scheduleTransaction` 来实现将 `EXECUTE_TRANSACTION` 消息发送给 `ActivityThread.H` 进行处理。

```
// frameworks/base/services/core/java/com/android/server/am/ActivityStackSupervisor.java
final boolean realStartActivityLocked(ActivityRecord r, ProcessRecord app,
        boolean andResume, boolean checkConfig) throws RemoteException {

    ...

    // Create activity launch transaction.
    final ClientTransaction clientTransaction = ClientTransaction.obtain(app.thread,
            r.appToken);
    clientTransaction.addCallback(LaunchActivityItem.obtain(new Intent(r.intent),
            System.identityHashCode(r), r.info,
            // TODO: Have this take the merged configuration instead of separate global
            // and override configs.
            mergedConfiguration.getGlobalConfiguration(),
            mergedConfiguration.getOverrideConfiguration(), r.compat,
            r.launchedFromPackage, task.voiceInteractor, app.repProcState, r.icicle,
            r.persistentState, results, newIntents, mService.isNextTransitionForward(),
            profilerInfo));

    // Set desired final state.
    final ActivityLifecycleItem lifecycleItem;
    if (andResume) {
        lifecycleItem = ResumeActivityItem.obtain(mService.isNextTransitionForward());
    } else {
        lifecycleItem = PauseActivityItem.obtain();
    }
    clientTransaction.setLifecycleStateRequest(lifecycleItem);

    // Schedule transaction.
    mService.getLifecycleManager().scheduleTransaction(clientTransaction);

    ...

    return true;
}

// frameworks/base/services/core/java/com/android/server/am/ClientLifecycleManager.java
void scheduleTransaction(ClientTransaction transaction) throws RemoteException {
    final IApplicationThread client = transaction.getClient();
    transaction.schedule();
    if (!(client instanceof Binder)) {
        // If client is not an instance of Binder - it's a remote call and at this point it is
        // safe to recycle the object. All objects used for local calls will be recycled after
        // the transaction is executed on client in ActivityThread.
        transaction.recycle();
    }
}

// frameworks/base/core/java/android/app/servertransaction/ClientTransaction.java

public void schedule() throws RemoteException {
    mClient.scheduleTransaction(this);
}


// frameworks/base/core/java/android/app/ActivityThread.java

public void scheduleTransaction(ClientTransaction transaction) throws RemoteException {
    ActivityThread.this.scheduleTransaction(transaction);
}

// frameworks/base/core/java/android/app/ClientTransactionHandler.java

void scheduleTransaction(ClientTransaction transaction) {
    transaction.preExecute(this);
    sendMessage(ActivityThread.H.EXECUTE_TRANSACTION, transaction);
}

// frameworks/base/core/java/android/app/ActivityThread.java
public void handleMessage(Message msg) {
    case EXECUTE_TRANSACTION:
        final ClientTransaction transaction = (ClientTransaction) msg.obj;
        mTransactionExecutor.execute(transaction);
        if (isSystem()) {
            // Client transactions inside system process are recycled on the client side
            // instead of ClientLifecycleManager to avoid being cleared before this
            // message is handled.
            transaction.recycle();
        }
        // TODO(lifecycler): Recycle locally scheduled transactions.
        break;
}
```
# TransactionExecutor.execute
`execute` 经过一系列处理后最终调用了 `LaunchActivityItem.execute` 方法, 看到 `LaunchActivityItem` 这个类名就可以知道距离关键地方已经不远了, 继续跟进 `handleLaunchActivity` 。
```
// frameworks/base/core/java/android/app/servertransaction/TransactionExecutor.java

public void execute(ClientTransaction transaction) {
    final IBinder token = transaction.getActivityToken();
    log("Start resolving transaction for client: " + mTransactionHandler + ", token: " + token);

    executeCallbacks(transaction);

    executeLifecycleState(transaction);
    mPendingActions.clear();
    log("End resolving transaction");
}


// frameworks/base/core/java/android/app/servertransaction/TransactionExecutor.java

private void executeLifecycleState(ClientTransaction transaction) {
    final ActivityLifecycleItem lifecycleItem = transaction.getLifecycleStateRequest();
    if (lifecycleItem == null) {
        // No lifecycle request, return early.
        return;
    }
    log("Resolving lifecycle state: " + lifecycleItem);

    final IBinder token = transaction.getActivityToken();
    final ActivityClientRecord r = mTransactionHandler.getActivityClient(token);

    if (r == null) {
        // Ignore requests for non-existent client records for now.
        return;
    }

    // Cycle to the state right before the final requested state.
    cycleToPath(r, lifecycleItem.getTargetState(), true /* excludeLastState */);

    // Execute the final transition with proper parameters.
    lifecycleItem.execute(mTransactionHandler, token, mPendingActions);
    lifecycleItem.postExecute(mTransactionHandler, token, mPendingActions);
}

// frameworks/base/core/java/android/app/servertransaction/LaunchActivityItem.java

public void execute(ClientTransactionHandler client, IBinder token,
        PendingTransactionActions pendingActions) {
    Trace.traceBegin(TRACE_TAG_ACTIVITY_MANAGER, "activityStart");
    ActivityClientRecord r = new ActivityClientRecord(token, mIntent, mIdent, mInfo,
            mOverrideConfig, mCompatInfo, mReferrer, mVoiceInteractor, mState, mPersistentState,
            mPendingResults, mPendingNewIntents, mIsForward,
            mProfilerInfo, client);
    client.handleLaunchActivity(r, pendingActions, null /* customIntent */);
    Trace.traceEnd(TRACE_TAG_ACTIVITY_MANAGER);
}

```

# ActivityThread.handleLaunchActivity

经过上面代码一步步的跳转，执行到 `ActivityThread.performLaunchActivity` 方法。在 `ActivityThread.performLaunchActivity` 方法中首先对 `Activity` 的 `ComponentName` 、 `ContextImpl` 、 `Activity` 以及 `Application` 对象进行了初始化并相互关联，然后设置 `Activity` 主题，最后调用 `Instrumentation.callActivityOnCreate` 方法。

```
// frameworks/base/core/java/android/app/ActivityThread.java

public Activity handleLaunchActivity(ActivityClientRecord r,
        PendingTransactionActions pendingActions, Intent customIntent) {
    ...
    
    WindowManagerGlobal.initialize();

    // 启动一个Activity，涉及到创建Activity对象，最终返回Activity对象
    final Activity a = performLaunchActivity(r, customIntent);

    if (a != null) {
        r.createdConfig = new Configuration(mConfiguration);
        reportSizeConfigurations(r);
        if (!r.activity.mFinished && pendingActions != null) {
            pendingActions.setOldState(r.state);
            pendingActions.setRestoreInstanceState(true);
            pendingActions.setCallOnPostCreate(true);
        }
    } else {
        // If there was an error, for any reason, tell the activity manager to stop us.
        try {
            //  activity启动失败，则通知AMS finish掉这个Activity
            ActivityManager.getService()
                    .finishActivity(r.token, Activity.RESULT_CANCELED, null,
                            Activity.DONT_FINISH_TASK_WITH_ACTIVITY);
        } catch (RemoteException ex) {
            throw ex.rethrowFromSystemServer();
        }
    }

    return a;
}

// frameworks/base/core/java/android/app/ActivityThread.java

private Activity performLaunchActivity(ActivityClientRecord r, Intent customIntent) {

    ...

    ContextImpl appContext = createBaseContextForActivity(r);
    Activity activity = null;

    // 创建一个 activity
    java.lang.ClassLoader cl = appContext.getClassLoader();
    activity = mInstrumentation.newActivity(
            cl, component.getClassName(), r.intent);
    StrictMode.incrementExpectedActivityCount(activity.getClass());
    r.intent.setExtrasClassLoader(cl);
    r.intent.prepareToEnterProcess();

    ...

    Application app = r.packageInfo.makeApplication(false, mInstrumentation);

    ...

    appContext.setOuterContext(activity);
    // 调用 activity.attach
    activity.attach(appContext, this, getInstrumentation(), r.token,
            r.ident, app, r.intent, r.activityInfo, title, r.parent,
            r.embeddedID, r.lastNonConfigurationInstances, config,
            r.referrer, r.voiceInteractor, window, r.configCallback);
    ...
    
    int theme = r.activityInfo.getThemeResource();
    if (theme != 0) {
        activity.setTheme(theme);
    }

    activity.mCalled = false;
    if (r.isPersistable()) {
        // 调用 activity 的 OnCreate
        mInstrumentation.callActivityOnCreate(activity, r.state, r.persistentState);
    } else {
        mInstrumentation.callActivityOnCreate(activity, r.state);
    }
    ...

    return activity;
}
```

# Instrumentation.callActivityOnCreate

```
// frameworks/base/core/java/android/app/Instrumentation.java

public void callActivityOnCreate(Activity activity, Bundle icicle) {
    prePerformCreate(activity);
    activity.performCreate(icicle);
    postPerformCreate(activity);
}

// frameworks/base/core/java/android/app/Activity.java

final void performCreate(Bundle icicle) {
    performCreate(icicle, null);
}

final void performCreate(Bundle icicle, PersistableBundle persistentState) {
    mCanEnterPictureInPicture = true;
    restoreHasCurrentPermissionRequest(icicle);
    if (persistentState != null) {
        onCreate(icicle, persistentState);
    } else {
        // 调用 onCreate
        onCreate(icicle);
    }
    writeEventLog(LOG_AM_ON_CREATE_CALLED, "performCreate");
    mActivityTransitionState.readState(icicle);

    mVisibleFromClient = !mWindow.getWindowStyle().getBoolean(
            com.android.internal.R.styleable.Window_windowNoDisplay, false);
    mFragments.dispatchActivityCreated();
    mActivityTransitionState.setEnterActivityOptions(this, getActivityOptions());
}
```

至此，我们就基本分析了Activity的启动流程。 这里只是简单的过了一遍，让自己对 Android 应用的启动过程中 frameworks 层做了那些工作，主要还是为了理解 FART 做的一些准备。

参考：
> https://juejin.im/post/5dda8504e51d452306073434#heading-10
> 
> https://www.jianshu.com/p/a1f40b39b3de
> 
> https://bbs.pediy.com/thread-252630.htm