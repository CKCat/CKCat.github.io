---
title: Android 双开沙箱 VirtualApp 源码分析（二）
date: 2020-08-08 15:18:04
tags: VirtualApp
category: Android
---

> 转载 : https://blog.csdn.net/ganyao939543405/article/details/76150725

# VA 初始化
## VirtualCore.startup
```
public void startup(Context context) throws Throwable {
    if (!isStartUp) {
        // 确保 MainThread
        if (Looper.myLooper() != Looper.getMainLooper()) {
            throw new IllegalStateException("VirtualCore.startup() must called in main thread.");
        }
        VASettings.STUB_CP_AUTHORITY = context.getPackageName() + "." + VASettings.STUB_DEF_AUTHORITY;
        ServiceManagerNative.SERVICE_CP_AUTH = context.getPackageName() + "." + ServiceManagerNative.SERVICE_DEF_AUTH;
        this.context = context;
        // 获取 ActivityThread 实例
        mainThread = ActivityThread.currentActivityThread.call();
        unHookPackageManager = context.getPackageManager();
        hostPkgInfo = unHookPackageManager.getPackageInfo(context.getPackageName(), PackageManager.GET_PROVIDERS);
        detectProcessType();
        // hook 系统类
        InvocationStubManager invocationStubManager = InvocationStubManager.getInstance();
        invocationStubManager.init();
        invocationStubManager.injectAll();
        // 修复权限管理
        ContextFixer.fixContext(context);
        isStartUp = true;
        if (initLock != null) {
            initLock.open();
            initLock = null;
        }
    }
}
```

整个 `VA` 会运行在四种进程, 分别是前面提到的 `VAService` 进程、`Client App` 进程、 `VA` 自身的 App 主进程、子进程。
```
/**
    * Process type
    */
private enum ProcessType {
    /**
        * Server process
        */
    Server,
    /**
        * Virtual app process
        */
    VAppClient,
    /**
        * Main process
        */
    Main,
    /**
        * Child process
        */
    CHILD
}
```
这样的话， `Application` 就会被初始化多次，所以要在初始化的时候根据进程类型有选择的做对应的初始化工作。

## InvocationStubManager.injectInternal
主要完成对 `Java` 层 `framework` 的 `Hook` ，将其定位到 `VA` 伪造 `VA framework` 上去。
```
private void injectInternal() throws Throwable {
    // VA 自身的 App 进程不需要 Hook
    if (VirtualCore.get().isMainProcess()) {
        return;
    }
    // VAService 需要 Hook AMS 和 PMS
    if (VirtualCore.get().isServerProcess()) {
        addInjector(new ActivityManagerStub());
        addInjector(new PackageManagerStub());
        return;
    }
    // Client APP 需要 Hook 整个 framework，来使其调用到 VA framework
    if (VirtualCore.get().isVAppProcess()) {
        addInjector(new LibCoreStub());
        addInjector(new ActivityManagerStub());
        addInjector(new PackageManagerStub());
        addInjector(HCallbackStub.getDefault());
        addInjector(new ISmsStub());
        addInjector(new ISubStub());
        addInjector(new DropBoxManagerStub());
        addInjector(new NotificationManagerStub());
        addInjector(new LocationManagerStub());
        addInjector(new WindowManagerStub());
        addInjector(new ClipBoardStub());
        addInjector(new MountServiceStub());
    ...
```
`VA` 初始化主要就是这些。

# Client App 的安装
## VirtualCore.installPackage

首先调用远程 `VAService` 。
```
public InstallResult installPackage(String apkPath, int flags) {
    try {
        // 调用远程 VAService
        return getService().installPackage(apkPath, flags);
    } catch (RemoteException e) {
        return VirtualRuntime.crash(e);
    }
}
```
最终调用 `VAServcie` 中的 `VAppManagerService.installPackage()`:
```
// 安装 apk 先于 installPackageAsUser，主要目的是生成 VPackage 结构
public synchronized InstallResult installPackage(String path, int flags, boolean notify) {
    long installTime = System.currentTimeMillis();
    if (path == null) {
        return InstallResult.makeFailure("path = NULL");
    }
    // 是否 OPT 优化(dex -> binary)
    boolean skipDexOpt = (flags & InstallStrategy.SKIP_DEX_OPT) != 0;
    // apk path
    File packageFile = new File(path);
    if (!packageFile.exists() || !packageFile.isFile()) {
        return InstallResult.makeFailure("Package File is not exist.");
    }
    VPackage pkg = null;
    try {
        // 进入解析包结构，该结构是可序列化的，为了持久化在磁盘上
        pkg = PackageParserEx.parsePackage(packageFile);
    } catch (Throwable e) {
        e.printStackTrace();
    }
    if (pkg == null || pkg.packageName == null) {
        return InstallResult.makeFailure("Unable to parse the package.");
    }
    InstallResult res = new InstallResult();
    res.packageName = pkg.packageName;
    // PackageCache holds all packages, try to check if we need to update.
    VPackage existOne = PackageCacheManager.get(pkg.packageName);
    PackageSetting existSetting = existOne != null ? (PackageSetting) existOne.mExtras : null;
    if (existOne != null) {
        if ((flags & InstallStrategy.IGNORE_NEW_VERSION) != 0) {
            res.isUpdate = true;
            return res;
        }
        if (!canUpdate(existOne, pkg, flags)) {
            return InstallResult.makeFailure("Not allowed to update the package.");
        }
        res.isUpdate = true;
    }
    // 获得 app 安装文件夹
    File appDir = VEnvironment.getDataAppPackageDirectory(pkg.packageName);
    // so 文件夹
    File libDir = new File(appDir, "lib");
    if (res.isUpdate) {
        FileUtils.deleteDir(libDir);
        VEnvironment.getOdexFile(pkg.packageName).delete();
        VActivityManagerService.get().killAppByPkg(pkg.packageName, VUserHandle.USER_ALL);
    }
    if (!libDir.exists() && !libDir.mkdirs()) {
        return InstallResult.makeFailure("Unable to create lib dir.");
    }

    // 是否基于系统的 apk 加载，前提是安装过的 apk 并且 dependSystem 开关打开
    boolean dependSystem = (flags & InstallStrategy.DEPEND_SYSTEM_IF_EXIST) != 0
            && VirtualCore.get().isOutsideInstalled(pkg.packageName);

    if (existSetting != null && existSetting.dependSystem) {
        dependSystem = false;
    }
    // 复制 so 到 sandbox lib
    NativeLibraryHelperCompat.copyNativeBinaries(new File(path), libDir);

    // 如果不基于系统，一些必要的拷贝工作
    if (!dependSystem) {
        File privatePackageFile = new File(appDir, "base.apk");
        File parentFolder = privatePackageFile.getParentFile();
        if (!parentFolder.exists() && !parentFolder.mkdirs()) {
            VLog.w(TAG, "Warning: unable to create folder : " + privatePackageFile.getPath());
        } else if (privatePackageFile.exists() && !privatePackageFile.delete()) {
            VLog.w(TAG, "Warning: unable to delete file : " + privatePackageFile.getPath());
        }
        try {
            FileUtils.copyFile(packageFile, privatePackageFile);
        } catch (IOException e) {
            privatePackageFile.delete();
            return InstallResult.makeFailure("Unable to copy the package file.");
        }
        packageFile = privatePackageFile;
    }
    if (existOne != null) {
        PackageCacheManager.remove(pkg.packageName);
    }

    // 给上可执行权限，5.0 之后在 SD 卡上执行 bin 需要可执行权限
    chmodPackageDictionary(packageFile);

    // PackageSetting 的一些配置，后面会序列化在磁盘上
    PackageSetting ps;
    if (existSetting != null) {
        ps = existSetting;
    } else {
        ps = new PackageSetting();
    }
    ps.skipDexOpt = skipDexOpt;
    ps.dependSystem = dependSystem;
    ps.apkPath = packageFile.getPath();
    ps.libPath = libDir.getPath();
    ps.packageName = pkg.packageName;
    ps.appId = VUserHandle.getAppId(mUidSystem.getOrCreateUid(pkg));
    if (res.isUpdate) {
        ps.lastUpdateTime = installTime;
    } else {
        ps.firstInstallTime = installTime;
        ps.lastUpdateTime = installTime;
        for (int userId : VUserManagerService.get().getUserIds()) {
            boolean installed = userId == 0;
            ps.setUserState(userId, false/*launched*/, false/*hidden*/, installed);
        }
    }
    //保存 VPackage Cache 到 Disk
    PackageParserEx.savePackageCache(pkg);
    //保存到 RamCache
    PackageCacheManager.put(pkg, ps);
    mPersistenceLayer.save();
    BroadcastSystem.get().startApp(pkg);
    //发送通知 安装完成
    if (notify) {
        notifyAppInstalled(ps, -1);
    }
    res.isSuccess = true;
    return res;
}
```
APK 的安装主要完成以下几件事情:
1. 解析 `menifest` 拿到 `apk` 内部信息，包括组件信息，权限信息等。并将这些信息序列化到磁盘和内存中，以备打开时调用。
2. 准备 `App` 在 `VA` 沙箱环境中的私有空间，并且复制一些必要的 `apk` 和 `so libs`。
3. 最后通知前台安装完成。

## PackageParserEx.parsePackage
解析 `apk menifest` 。
```
// 解析包结构
public static VPackage parsePackage(File packageFile) throws Throwable {
    PackageParser parser = PackageParserCompat.createParser(packageFile);
    // 调用对应系统版本的 parsePackage 方法
    PackageParser.Package p = PackageParserCompat.parsePackage(parser, packageFile, 0);
    // 包含此信息代表其是 debug 签名或者其他签名
    if (p.requestedPermissions.contains("android.permission.FAKE_PACKAGE_SIGNATURE")
            && p.mAppMetaData != null
            && p.mAppMetaData.containsKey("fake-signature")) {
        String sig = p.mAppMetaData.getString("fake-signature");
        p.mSignatures = new Signature[]{new Signature(sig)};
        VLog.d(TAG, "Using fake-signature feature on : " + p.packageName);
    } else {
        // 验证签名
        PackageParserCompat.collectCertificates(parser, p, PackageParser.PARSE_IS_SYSTEM);
    }
    // 转换成可以序列化在磁盘上的 Cache
    return buildPackageCache(p);
}
```
这里解析 `Menifest` 的方法其实是调用了 `framework` 隐藏方法 `android.content.pm.PackageParser.parsePackage` 来实现的，这个方法返回 `android.content.pm.Package` 结构，这个类型也是隐藏的，怎么办？可以从 `sdk` 中复制这个类到自己的项目中欺骗编译器。这就是上一章一开始提到的。

这里还有一个问题，就是 `Package` 类是不可序列化的，换句话说就是不能直接保存在磁盘上，我们需要将其转换成可以序列化的 `VPackage` 类型，这就是 `buildPackageCache()` 的作用。

## VPackage
```
public class VPackage implements Parcelable {

    public static final Creator<VPackage> CREATOR = new Creator<VPackage>() {
        @Override
        public VPackage createFromParcel(Parcel source) {
            return new VPackage(source);
        }

        @Override
        public VPackage[] newArray(int size) {
            return new VPackage[size];
        }
    };
    public ArrayList<ActivityComponent> activities;
    public ArrayList<ActivityComponent> receivers;
    public ArrayList<ProviderComponent> providers;
    public ArrayList<ServiceComponent> services;
    public ArrayList<InstrumentationComponent> instrumentation;
    public ArrayList<PermissionComponent> permissions;
    public ArrayList<PermissionGroupComponent> permissionGroups;
    public ArrayList<String> requestedPermissions;
    public ArrayList<String> protectedBroadcasts;
    public ApplicationInfo applicationInfo;
    public Signature[] mSignatures;
    public Bundle mAppMetaData;
    public String packageName;
    public int mPreferredOrder;
    public String mVersionName;
    public String mSharedUserId;
    public ArrayList<String> usesLibraries;
    public int mVersionCode;
    public int mSharedUserLabel;
    // Applications hardware preferences
    public ArrayList<ConfigurationInfo> configPreferences = null;
    // Applications requested features
    public ArrayList<FeatureInfo> reqFeatures = null;
    public Object mExtras;

    public VPackage() {
    }
    ...
```
可以看到 `VPackage` 几乎保存了 `apk` 中所有的关键信息，尤其是组件的数据结构会在 `app` 在 `VA` 中运行的时候给 `VAMS` 、 `VPMS` 这些 `VAService` 提供 `apk` 的组件信息。

关于是否 `dependSystem` 和 `isInstallOutside` ，这个有关 `apk` 的动态加载，如果 `dependSysytem` 并且 `apk` 已经在外部环境安装了，那么 `VA` 会调用系统提供的 `API` 就可以动态加载 `APK` 。反之 `VA` 需要做一些必要的复制工作然后再费劲的去加载 `APK` 。

