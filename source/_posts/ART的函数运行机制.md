---
title: ART的函数运行机制
date: 2020-06-03 19:25:03
tag: Android源码阅读
category: Android源码
---

# ART的函数运行机制

## 类的加载

`dalvik` 的类加载的主要过程如下：

1. `app_process` 作为 `zygote server` 通过 `local socket` 处理进程创建请求，`zygote server` 是在 `ZygoteInit.main` 函数里调用 `ZygoteInit.runSelectLoop` 监听。

2. 接收到 `zygote client` 的 `fork` 请求之后，调用 `ZygoteConnection.runOnce` ，调用 `Zygote.forkAndSpecialize` 创建新进程

3. 进程创建之后，由 `ZygoteConnection.handleParentProc` 来初始化进程，最终会调用 `ActivityThread.main` 函数

4. `ActivityThread.main -> ActivityThread.attach ->  ActivityThread.bindApplication -> Activity.handleBindApplication，handleBindApplication` 会初始化 `BaseDexClassLoader` 。

5. 类的加载经过了 `ClassLoader.loadClass->BaseDexClassLoader.findClass->DexPathList.findClass->DexFile.loadClassBinaryName->DexFile.defineClassNative->DexFile_defineClassNative` (`runtime/native/dalvik_system_DexFile.cc`) 。

这个初始化过程， `art` 和 `dalvik` 都是一样的。 `art` 的 `DexFile_defineClassNative` 由 `ClassLinker` 的 `DefineClass` 来加载类。

```cpp
static jclass DexFile_defineClassNative(JNIEnv* env,
                                        jclass,
                                        jstring javaName,
                                        jobject javaLoader,
                                        jobject cookie,
                                        jobject dexFile) {
  std::vector<const DexFile*> dex_files;
  const OatFile* oat_file;
  if (!ConvertJavaArrayToDexFiles(env, cookie, /*out*/ dex_files, /*out*/ oat_file)) {
    VLOG(class_linker) << "Failed to find dex_file";
    DCHECK(env->ExceptionCheck());
    return nullptr;
  }

  ScopedUtfChars class_name(env, javaName);
  if (class_name.c_str() == nullptr) {
    VLOG(class_linker) << "Failed to find class_name";
    return nullptr;
  }
  const std::string descriptor(DotToDescriptor(class_name.c_str()));
  const size_t hash(ComputeModifiedUtf8Hash(descriptor.c_str()));
  for (auto& dex_file : dex_files) {
    const DexFile::ClassDef* dex_class_def =
        OatDexFile::FindClassDef(*dex_file, descriptor.c_str(), hash);
    if (dex_class_def != nullptr) {
      ScopedObjectAccess soa(env);
      ClassLinker* class_linker = Runtime::Current()->GetClassLinker();
      StackHandleScope<1> hs(soa.Self());
      Handle<mirror::ClassLoader> class_loader(
          hs.NewHandle(soa.Decode<mirror::ClassLoader>(javaLoader)));
      ObjPtr<mirror::DexCache> dex_cache =
          class_linker->RegisterDexFile(*dex_file, class_loader.Get());
      if (dex_cache == nullptr) {
        // OOME or InternalError (dexFile already registered with a different class loader).
        soa.Self()->AssertPendingException();
        return nullptr;
      }
      ObjPtr<mirror::Class> result = class_linker->DefineClass(soa.Self(),
                                                               descriptor.c_str(),
                                                               hash,
                                                               class_loader,
                                                               *dex_file,
                                                               *dex_class_def);
      // Add the used dex file. This only required for the DexFile.loadClass API since normal
      // class loaders already keep their dex files live.
      class_linker->InsertDexFileInToClassLoader(soa.Decode<mirror::Object>(dexFile),
                                                 class_loader.Get());
      if (result != nullptr) {
        VLOG(class_linker) << "DexFile_defineClassNative returning " << result
                           << " for " << class_name.c_str();
        return soa.AddLocalReference<jclass>(result);
      }
    }
  }
  VLOG(class_linker) << "Failed to find dex_class_def " << class_name.c_str();
  return nullptr;
}
```

类的加载除了创建 `Class` 只外，还有加载类的字段和方法，这个由 `ClassLinker::LoadClass` 来完成。

```cpp
void ClassLinker::LoadClass(Thread* self,
                            const DexFile& dex_file,
                            const DexFile::ClassDef& dex_class_def,
                            Handle<mirror::Class> klass) {
  const uint8_t* class_data = dex_file.GetClassData(dex_class_def);
  if (class_data == nullptr) {
    return;  // no fields or methods - for example a marker interface
  }
  LoadClassMembers(self, dex_file, class_data, klass);
}
```

## 函数的执行

一旦类的加载完成，那么就可以调用类的成员函数了，之前的解释器运行机制那篇文章介绍过， `Java` 的执行是以 `Method` 为执行单元的，所以我们分析 `art` 的运行机制，其实就是分析 `Method` 的运行机制。

`ActivityThread` 是进程在启动的时候传类名，在进程启动之后，由 `handleParentProc` 执行 `main` 函数，因此第一个被执行的 `java` 函数是 `ActivityThread.main` 。

```java
Process.ProcessStartResult startResult = Process.start("android.app.ActivityThread",
                    app.processName, uid, uid, gids, debugFlags, mountExternal,
                    app.info.targetSdkVersion, app.info.seinfo, null);
```

`ActivityThread.main` 是最终由 `AndroidRuntime::callMain` 执行。

```cpp
status_t AndroidRuntime::callMain(const String8& className, jclass clazz,
    const Vector<String8>& args)
{
    JNIEnv* env;
    jmethodID methodId;

    ALOGD("Calling main entry %s", className.string());

    env = getJNIEnv();
    if (clazz == NULL || env == NULL) {
        return UNKNOWN_ERROR;
    }

    methodId = env->GetStaticMethodID(clazz, "main", "([Ljava/lang/String;)V");
    if (methodId == NULL) {
        ALOGE("ERROR: could not find method %s.main(String[])\n", className.string());
        return UNKNOWN_ERROR;
    }

    /*
     * We want to call main() with a String array with our arguments in it.
     * Create an array and populate it.
     */
    jclass stringClass;
    jobjectArray strArray;

    const size_t numArgs = args.size();
    stringClass = env->FindClass("java/lang/String");
    strArray = env->NewObjectArray(numArgs, stringClass, NULL);

    for (size_t i = 0; i < numArgs; i++) {
        jstring argStr = env->NewStringUTF(args[i].string());
        env->SetObjectArrayElement(strArray, i, argStr);
    }
    /*
    关键函数
    */
    env->CallStaticVoidMethod(clazz, methodId, strArray);
    return NO_ERROR;
}
```

实际会调用 `JNINativeInterface` 的 `CallStaticVoidMethod` ，上面已经介绍过，该函数的定义在 `runtime/jni_internal.cc` 里。

```cpp
  static void CallStaticVoidMethod(JNIEnv* env, jclass, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    ScopedVAArgs free_args_later(&ap);
    CHECK_NON_NULL_ARGUMENT_RETURN_VOID(mid);
    ScopedObjectAccess soa(env);
    /*
    关键函数
    */
    InvokeWithVarArgs(soa, nullptr, mid, ap);
  }
```

`InvokeWithVarArgs` 是执行函数的入口，定义在 `runtime/reflection.cc` ，最终是调用了 `ArtMethod::Invoke` 。

```cpp
JValue InvokeWithVarArgs(const ScopedObjectAccessAlreadyRunnable& soa, jobject obj, jmethodID mid,
                         va_list args)
    REQUIRES_SHARED(Locks::mutator_lock_) {
  // We want to make sure that the stack is not within a small distance from the
  // protected region in case we are calling into a leaf function whose stack
  // check has been elided.
  if (UNLIKELY(__builtin_frame_address(0) < soa.Self()->GetStackEnd())) {
    ThrowStackOverflowError(soa.Self());
    return JValue();
  }

  ArtMethod* method = jni::DecodeArtMethod(mid);
  bool is_string_init = method->GetDeclaringClass()->IsStringClass() && method->IsConstructor();
  if (is_string_init) {
    // Replace calls to String.<init> with equivalent StringFactory call.
    method = WellKnownClasses::StringInitToStringFactory(method);
  }
  ObjPtr<mirror::Object> receiver = method->IsStatic() ? nullptr : soa.Decode<mirror::Object>(obj);
  uint32_t shorty_len = 0;
  const char* shorty =
      method->GetInterfaceMethodIfProxy(kRuntimePointerSize)->GetShorty(&shorty_len);
  JValue result;
  ArgArray arg_array(shorty, shorty_len);
  arg_array.BuildArgArrayFromVarArgs(soa, receiver, args);
  /*
  关键函数
  */
  InvokeWithArgArray(soa, method, &arg_array, &result, shorty);
  if (is_string_init) {
    // For string init, remap original receiver to StringFactory result.
    UpdateReference(soa.Self(), obj, result.GetL());
  }
  return result;
}

void InvokeWithArgArray(const ScopedObjectAccessAlreadyRunnable& soa,
                               ArtMethod* method, ArgArray* arg_array, JValue* result,
                               const char* shorty)
    REQUIRES_SHARED(Locks::mutator_lock_) {
  uint32_t* args = arg_array->GetArray();
  if (UNLIKELY(soa.Env()->IsCheckJniEnabled())) {
    CheckMethodArguments(soa.Vm(), method->GetInterfaceMethodIfProxy(kRuntimePointerSize), args);
  }

  /*
  关键函数
  */
  method->Invoke(soa.Self(), args, arg_array->GetNumBytes(), result, shorty);
}
```

我们知道 `ART` 的运行模式是 `AOT` 的，在 `apk` 安装的时候，每个 `DexMethod` 都会由 `dex2oat` 编译成目标代码，而不再是虚拟机执行的字节码，但同时 `Dex` 字节码仍然还在 `OAT` 里存在，所以 `ART` 的代码执行既支持 `QuickCompiledCode` 模式，也同时支持解释器模式以及 `JIT` 执行模式。看 `ArtMethod::Invoke` 。

```cpp
void ArtMethod::Invoke(Thread* self, uint32_t* args, uint32_t args_size, JValue* result,
                       const char* shorty) {
  if (UNLIKELY(__builtin_frame_address(0) < self->GetStackEnd())) {
    ThrowStackOverflowError(self);
    return;
  }

  if (kIsDebugBuild) {
    self->AssertThreadSuspensionIsAllowable();
    CHECK_EQ(kRunnable, self->GetState());
    CHECK_STREQ(GetInterfaceMethodIfProxy(kRuntimePointerSize)->GetShorty(), shorty);
  }

  // Push a transition back into managed code onto the linked list in thread.
  ManagedStack fragment;
  self->PushManagedStackFragment(&fragment);

  Runtime* runtime = Runtime::Current();
  // Call the invoke stub, passing everything as arguments.
  // If the runtime is not yet started or it is required by the debugger, then perform the
  // Invocation by the interpreter, explicitly forcing interpretation over JIT to prevent
  // cycling around the various JIT/Interpreter methods that handle method invocation.
  if (UNLIKELY(!runtime->IsStarted() || Dbg::IsForcedInterpreterNeededForCalling(self, this))) {
    /*
      Interpreter 模式
    */
    if (IsStatic()) {
      art::interpreter::EnterInterpreterFromInvoke(
          self, this, nullptr, args, result, /*stay_in_interpreter*/ true);
    } else {
      mirror::Object* receiver =
          reinterpret_cast<StackReference<mirror::Object>*>(&args[0])->AsMirrorPtr();
      art::interpreter::EnterInterpreterFromInvoke(
          self, this, receiver, args + 1, result, /*stay_in_interpreter*/ true);
    }
  } else {
    DCHECK_EQ(runtime->GetClassLinker()->GetImagePointerSize(), kRuntimePointerSize);

    constexpr bool kLogInvocationStartAndReturn = false;
    bool have_quick_code = GetEntryPointFromQuickCompiledCode() != nullptr;
    if (LIKELY(have_quick_code)) {
      if (kLogInvocationStartAndReturn) {
        LOG(INFO) << StringPrintf(
            "Invoking '%s' quick code=%p static=%d", PrettyMethod().c_str(),
            GetEntryPointFromQuickCompiledCode(), static_cast<int>(IsStatic() ? 1 : 0));
      }

      // Ensure that we won't be accidentally calling quick compiled code when -Xint.
      if (kIsDebugBuild && runtime->GetInstrumentation()->IsForcedInterpretOnly()) {
        CHECK(!runtime->UseJitCompilation());
        const void* oat_quick_code =
            (IsNative() || !IsInvokable() || IsProxyMethod() || IsObsolete())
            ? nullptr
            : GetOatMethodQuickCode(runtime->GetClassLinker()->GetImagePointerSize());
        CHECK(oat_quick_code == nullptr || oat_quick_code != GetEntryPointFromQuickCompiledCode())
            << "Don't call compiled code when -Xint " << PrettyMethod();
      }

      /*
        OAT 模式
      */
      if (!IsStatic()) {
        (*art_quick_invoke_stub)(this, args, args_size, self, result, shorty);
      } else {
        (*art_quick_invoke_static_stub)(this, args, args_size, self, result, shorty);
      }
      if (UNLIKELY(self->GetException() == Thread::GetDeoptimizationException())) {
        // Unusual case where we were running generated code and an
        // exception was thrown to force the activations to be removed from the
        // stack. Continue execution in the interpreter.
        self->DeoptimizeWithDeoptimizationException(result);
      }
      if (kLogInvocationStartAndReturn) {
        LOG(INFO) << StringPrintf("Returned '%s' quick code=%p", PrettyMethod().c_str(),
                                  GetEntryPointFromQuickCompiledCode());
      }
    } else {
      LOG(INFO) << "Not invoking '" << PrettyMethod() << "' code=null";
      if (result != nullptr) {
        result->SetJ(0);
      }
    }
  }

  // Pop transition.
  self->PopManagedStackFragment(fragment);
}
```

`Invoke` 可以进入 `OAT` ，`Interpreter` 模式执行 `Method` 。如果当前是 `Interpreter` 模式，就调用 `art::interpreter::EnterInterpreterFromInvoke` ，如果是 `OAT` 模式，就调用 `art_quick_invoke_stub/art_quick_invoke_static_stub` 。
`EnterInterpreterFromInvoke` 函数里会判断是 `native` 还是解释器执行。

```cpp
void EnterInterpreterFromInvoke(Thread* self, ArtMethod* method, Object* receiver,
                                uint32_t* args, JValue* result) {
  DCHECK_EQ(self, Thread::Current());
  bool implicit_check = !Runtime::Current()->ExplicitStackOverflowChecks();
  if (UNLIKELY(__builtin_frame_address(0) < self->GetStackEndForInterpreter(implicit_check))) {
    ThrowStackOverflowError(self);
    return;
  }

  const char* old_cause = self->StartAssertNoThreadSuspension("EnterInterpreterFromInvoke");
  const DexFile::CodeItem* code_item = method->GetCodeItem();
  uint16_t num_regs;
  uint16_t num_ins;
  if (code_item != nullptr) {
    num_regs =  code_item->registers_size_;
    num_ins = code_item->ins_size_;
  } else if (method->IsAbstract()) {
    self->EndAssertNoThreadSuspension(old_cause);
    ThrowAbstractMethodError(method);
    return;
  } else {
    DCHECK(method->IsNative());
    num_regs = num_ins = ArtMethod::NumArgRegisters(method->GetShorty());
    if (!method->IsStatic()) {
      num_regs++;
      num_ins++;
    }
  }
  // Set up shadow frame with matching number of reference slots to vregs.
  ShadowFrame* last_shadow_frame = self->GetManagedStack()->GetTopShadowFrame();
  void* memory = alloca(ShadowFrame::ComputeSize(num_regs));
  ShadowFrame* shadow_frame(ShadowFrame::Create(num_regs, last_shadow_frame, method, 0, memory));
  self->PushShadowFrame(shadow_frame);

  size_t cur_reg = num_regs - num_ins;
  if (!method->IsStatic()) {
    CHECK(receiver != nullptr);
    shadow_frame->SetVRegReference(cur_reg, receiver);
    ++cur_reg;
  }
  uint32_t shorty_len = 0;
  const char* shorty = method->GetShorty(&shorty_len);
  for (size_t shorty_pos = 0, arg_pos = 0; cur_reg < num_regs; ++shorty_pos, ++arg_pos, cur_reg++) {
    DCHECK_LT(shorty_pos + 1, shorty_len);
    switch (shorty[shorty_pos + 1]) {
      case 'L': {
        Object* o = reinterpret_cast<StackReference<Object>*>(&args[arg_pos])->AsMirrorPtr();
        shadow_frame->SetVRegReference(cur_reg, o);
        break;
      }
      case 'J': case 'D': {
        uint64_t wide_value = (static_cast<uint64_t>(args[arg_pos + 1]) << 32) | args[arg_pos];
        shadow_frame->SetVRegLong(cur_reg, wide_value);
        cur_reg++;
        arg_pos++;
        break;
      }
      default:
        shadow_frame->SetVReg(cur_reg, args[arg_pos]);
        break;
    }
  }
  self->EndAssertNoThreadSuspension(old_cause);
  // Do this after populating the shadow frame in case EnsureInitialized causes a GC.
  if (method->IsStatic() && UNLIKELY(!method->GetDeclaringClass()->IsInitialized())) {
    ClassLinker* class_linker = Runtime::Current()->GetClassLinker();
    StackHandleScope<1> hs(self);
    Handle<mirror::Class> h_class(hs.NewHandle(method->GetDeclaringClass()));
    if (UNLIKELY(!class_linker->EnsureInitialized(self, h_class, true, true))) {
      CHECK(self->IsExceptionPending());
      self->PopShadowFrame();
      return;
    }
  }
  if (LIKELY(!method->IsNative())) {
    /*
      非 native 方法则调用下列函数
    */
    JValue r = Execute(self, code_item, *shadow_frame, JValue());
    if (result != nullptr) {
      *result = r;
    }
  } else {
    // We don't expect to be asked to interpret native code (which is entered via a JNI compiler
    // generated stub) except during testing and image writing.
    // Update args to be the args in the shadow frame since the input ones could hold stale
    // references pointers due to moving GC.
    args = shadow_frame->GetVRegArgs(method->IsStatic() ? 0 : 1);
    if (!Runtime::Current()->IsStarted()) {
      UnstartedRuntime::Jni(self, method, receiver, args, result);
    } else {
      InterpreterJni(self, method, shorty, receiver, args, result);
    }
  }
  self->PopShadowFrame();
}
```

这个函数前面部分都在做参数压栈操作，最后几行进入主题，如果不是 `Native` ，那么调用 `Execute` 执行； `Native` 函数则调用 `InterpreterJni` 。 `Execute` 就是 `art` 的解释器代码， `Dex` 的字节码是通过 `ArtMethod::GetCodeItem` 函数获得，由 `Execute` 逐条执行。 `InterpreterJni` 通过 `GetEntryPointFromJni` 来获得 `native` 的函数，并执行。

```cpp
if (LIKELY(!method->IsNative())) {
    JValue r = Execute(self, code_item, *shadow_frame, JValue());
    if (result != nullptr) {
      *result = r;
    }
  } else {
    // We don't expect to be asked to interpret native code (which is entered via a JNI compiler
    // generated stub) except during testing and image writing.
    // Update args to be the args in the shadow frame since the input ones could hold stale
    // references pointers due to moving GC.
    args = shadow_frame->GetVRegArgs(method->IsStatic() ? 0 : 1);
    if (!Runtime::Current()->IsStarted()) {
      UnstartedRuntime::Jni(self, method, receiver, args, result);
    } else {
      InterpreterJni(self, method, shorty, receiver, args, result);
    }
  }
```

再回调 `OAT` 的模式， `art_quick_invoke_stub/art_quick_invoke_static_stub` 最终会调用到 `art_quick_invoke_stub_internal` (`arch/arm/quick_entrypoints_arm.S` )。

```armasm
ENTRY art_quick_invoke_stub_internal
    push   {r4, r5, r6, r7, r8, r9, r10, r11, lr}               @ spill regs
    .cfi_adjust_cfa_offset 16
    .cfi_rel_offset r4, 0
    .cfi_rel_offset r5, 4
    .cfi_rel_offset r6, 8
    .cfi_rel_offset r7, 12
    .cfi_rel_offset r8, 16
    .cfi_rel_offset r9, 20
    .cfi_rel_offset r10, 24
    .cfi_rel_offset r11, 28
    .cfi_rel_offset lr, 32
    mov    r11, sp                         @ save the stack pointer
    .cfi_def_cfa_register r11

    mov    r9, r3                          @ move managed thread pointer into r9

    add    r4, r2, #4                      @ create space for method pointer in frame
    sub    r4, sp, r4                      @ reserve & align *stack* to 16 bytes: native calling
    and    r4, #0xFFFFFFF0                 @ convention only aligns to 8B, so we have to ensure ART
    mov    sp, r4                          @ 16B alignment ourselves.

    mov    r4, r0                          @ save method*
    add    r0, sp, #4                      @ pass stack pointer + method ptr as dest for memcpy
    bl     memcpy                          @ memcpy (dest, src, bytes)
    mov    ip, #0                          @ set ip to 0
    str    ip, [sp]                        @ store null for method* at bottom of frame

    ldr    ip, [r11, #48]                  @ load fp register argument array pointer
    vldm   ip, {s0-s15}                    @ copy s0 - s15

    ldr    ip, [r11, #44]                  @ load core register argument array pointer
    mov    r0, r4                          @ restore method*
    add    ip, ip, #4                      @ skip r0
    ldm    ip, {r1-r3}                     @ copy r1 - r3
#ifdef ARM_R4_SUSPEND_FLAG
    mov    r4, #SUSPEND_CHECK_INTERVAL     @ reset r4 to suspend check interval
#endif

    ldr    ip, [r0, #ART_METHOD_QUICK_CODE_OFFSET_32]  @ get pointer to the code
    blx    ip                              @ call the method

    mov    sp, r11                         @ restore the stack pointer
    .cfi_def_cfa_register sp

    ldr    r4, [sp, #40]                   @ load result_is_float
    ldr    r9, [sp, #36]                   @ load the result pointer
    cmp    r4, #0
    ite    eq
    strdeq r0, [r9]                        @ store r0/r1 into result pointer
    vstrne d0, [r9]                        @ store s0-s1/d0 into result pointer

    pop    {r4, r5, r6, r7, r8, r9, r10, r11, pc}               @ restore spill regs
END art_quick_invoke_stub_internal
```

找到 `ArtMethod` 的 `entry_point_from_quick_compiled_code_` 字段，这个就是 `EntryPointFromQuickCompiledCode` ，从而进入 OAT 函数执行。

```cpp
#define ART_METHOD_QUICK_CODE_OFFSET_32 36
ADD_TEST_EQ(ART_METHOD_QUICK_CODE_OFFSET_32,
            art::ArtMethod::EntryPointFromQuickCompiledCodeOffset(4).Int32Value())
```

`EntryPointFromQuickCompiledCode` 的初始化在 `class_linker` 的 `LoadClassMembers` 时调用的 `LinkCode` ，有下面几种类型

1. SetEntryPointFromQuickCompiledCode(GetQuickCode());   // 这个是执行 OatMethod

2. SetEntryPointFromQuickCompiledCode(GetQuickToInterpreterBridge());  //  Dex Method

3. SetEntryPointFromQuickCompiledCode(GetQuickGenericJniStub())；  // Native Method

4. SetEntryPointFromQuickCompiledCode(GetQuickResolutionStub());   // method->IsStatic() && !method->IsConstructor()

如果是强制使用了解释器模式，那么执行的是代码 `GetQuickToInterpreterBridge(non-static, non-native)` 或 `GetQuickGenericJniStub(non-static, native)` 或 `GetQuickResolutionStub(static)` ，这几个 `EntryPoint` 对应的实际执行函数如下。

```
GetQuickGenericJniStub —  artQuickGenericJniTrampoline

GetQuickResolutionStub —  artQuickResolutionTrampoline

GetQuickToInterpreterBridge —  artQuickToInterpreterBridge
```

`ArtMthod` 被 `Resolve` 之后，如果是走 `Oat` 模式就会执行 `GetQuickCode` 。

以上是 `EntryPointFromQuickCompiledCode` 的情况。

不同的执行模式有不同的 `EntryPoint` 。

1. 解释器 - `EntryPointFromInterpreter`

   在 `interpreter/interpreter_common.cc` 里会在执行解释器函数时，会获得 `ArtMethod` 的 `Interpret EntryPoint` 执行

2. Jni - ` EntryPointFromJni`

   `interpreter/interpreter.cc`， `InterpreterJni` 函数会获得 `ArtMethod的Jni EntryPoint` 执行

3. Oat - `EntryPointFromQuickCompiledCode`

   `DexCache` 在 `Init` 的时候会将 `Method` 都初始化为 `ResolutionMethod` ，这个 `Resolution Method` 是没有 `dex method id` 的，是个 `RuntimeMethod` ，这是 `lazy load method` ，运行时 `resolve` 之后才会替换成实际的 `ArtMethod` 。

```
void DexCache::Init(const DexFile* dex_file, String* location, ObjectArray<String>* strings,
                    ObjectArray<Class>* resolved_types, PointerArray* resolved_methods,
                    PointerArray* resolved_fields, size_t pointer_size) {
  CHECK(dex_file != nullptr);
  CHECK(location != nullptr);
  CHECK(strings != nullptr);
  CHECK(resolved_types != nullptr);
  CHECK(resolved_methods != nullptr);
  CHECK(resolved_fields != nullptr);

  SetDexFile(dex_file);
  SetFieldObject<false>(OFFSET_OF_OBJECT_MEMBER(DexCache, location_), location);
  SetFieldObject<false>(StringsOffset(), strings);
  SetFieldObject<false>(ResolvedFieldsOffset(), resolved_fields);
  SetFieldObject<false>(OFFSET_OF_OBJECT_MEMBER(DexCache, resolved_types_), resolved_types);
  SetFieldObject<false>(ResolvedMethodsOffset(), resolved_methods);

  Runtime* const runtime = Runtime::Current();
  if (runtime->HasResolutionMethod()) {
    // Initialize the resolve methods array to contain trampolines for resolution.
    Fixup(runtime->GetResolutionMethod(), pointer_size);
  }
}

void DexCache::Fixup(ArtMethod* trampoline, size_t pointer_size) {
  // Fixup the resolve methods array to contain trampoline for resolution.
  CHECK(trampoline != nullptr);
  CHECK(trampoline->IsRuntimeMethod());
  auto* resolved_methods = GetResolvedMethods();
  for (size_t i = 0, length = resolved_methods->GetLength(); i < length; i++) {
    if (resolved_methods->GetElementPtrSize<ArtMethod*>(i, pointer_size) == nullptr) {
      resolved_methods->SetElementPtrSize(i, trampoline, pointer_size);
    }
  }
}
```

`resolution method` 的 `EntryPointFromQuickCompiledCode` 指向 `GetQuickResolutionStub` ，意思就是一开始，这些函数的执行点都是从 `artQuickResolutionTrampoline` 开始。

```
// Lazily resolve a method for quick. Called by stub code.
extern "C" const void* artQuickResolutionTrampoline(
    ArtMethod* called, mirror::Object* receiver, Thread* self, ArtMethod** sp)
    SHARED_LOCKS_REQUIRED(Locks::mutator_lock_) {
  ScopedQuickEntrypointChecks sqec(self);
  // Start new JNI local reference state
  JNIEnvExt* env = self->GetJniEnv();
  ScopedObjectAccessUnchecked soa(env);
  ScopedJniEnvLocalRefState env_state(env);
  const char* old_cause = self->StartAssertNoThreadSuspension("Quick method resolution set up");

  // Compute details about the called method (avoid GCs)
  ClassLinker* linker = Runtime::Current()->GetClassLinker();
  ArtMethod* caller = QuickArgumentVisitor::GetCallingMethod(sp);
  InvokeType invoke_type;
  MethodReference called_method(nullptr, 0);
  const bool called_method_known_on_entry = !called->IsRuntimeMethod();
  if (!called_method_known_on_entry) {
    uint32_t dex_pc = caller->ToDexPc(QuickArgumentVisitor::GetCallingPc(sp));
    const DexFile::CodeItem* code;
    called_method.dex_file = caller->GetDexFile();
    code = caller->GetCodeItem();
    CHECK_LT(dex_pc, code->insns_size_in_code_units_);
    const Instruction* instr = Instruction::At(&code->insns_[dex_pc]);
    Instruction::Code instr_code = instr->Opcode();
    bool is_range;
    switch (instr_code) {
      case Instruction::INVOKE_DIRECT:
        invoke_type = kDirect;
        is_range = false;
        break;
      case Instruction::INVOKE_DIRECT_RANGE:
        invoke_type = kDirect;
        is_range = true;
        break;
      case Instruction::INVOKE_STATIC:
        invoke_type = kStatic;
        is_range = false;
        break;
      case Instruction::INVOKE_STATIC_RANGE:
        invoke_type = kStatic;
        is_range = true;
        break;
      case Instruction::INVOKE_SUPER:
        invoke_type = kSuper;
        is_range = false;
        break;
      case Instruction::INVOKE_SUPER_RANGE:
        invoke_type = kSuper;
        is_range = true;
        break;
      case Instruction::INVOKE_VIRTUAL:
        invoke_type = kVirtual;
        is_range = false;
        break;
      case Instruction::INVOKE_VIRTUAL_RANGE:
        invoke_type = kVirtual;
        is_range = true;
        break;
      case Instruction::INVOKE_INTERFACE:
        invoke_type = kInterface;
        is_range = false;
        break;
      case Instruction::INVOKE_INTERFACE_RANGE:
        invoke_type = kInterface;
        is_range = true;
        break;
      default:
        LOG(FATAL) << "Unexpected call into trampoline: " << instr->DumpString(nullptr);
        UNREACHABLE();
    }
    called_method.dex_method_index = (is_range) ? instr->VRegB_3rc() : instr->VRegB_35c();
  } else {
    invoke_type = kStatic;
    called_method.dex_file = called->GetDexFile();
    called_method.dex_method_index = called->GetDexMethodIndex();
  }
  uint32_t shorty_len;
  const char* shorty =
      called_method.dex_file->GetMethodShorty(
          called_method.dex_file->GetMethodId(called_method.dex_method_index), &shorty_len);
  RememberForGcArgumentVisitor visitor(sp, invoke_type == kStatic, shorty, shorty_len, &soa);
  visitor.VisitArguments();
  self->EndAssertNoThreadSuspension(old_cause);
  const bool virtual_or_interface = invoke_type == kVirtual || invoke_type == kInterface;
  // Resolve method filling in dex cache.
  if (!called_method_known_on_entry) {
    StackHandleScope<1> hs(self);
    mirror::Object* dummy = nullptr;
    HandleWrapper<mirror::Object> h_receiver(
        hs.NewHandleWrapper(virtual_or_interface ? &receiver : &dummy));
    DCHECK_EQ(caller->GetDexFile(), called_method.dex_file);
    called = linker->ResolveMethod(self, called_method.dex_method_index, caller, invoke_type);
  }
  const void* code = nullptr;
  if (LIKELY(!self->IsExceptionPending())) {
    // Incompatible class change should have been handled in resolve method.
    CHECK(!called->CheckIncompatibleClassChange(invoke_type))
        << PrettyMethod(called) << " " << invoke_type;
    if (virtual_or_interface) {
      // Refine called method based on receiver.
      CHECK(receiver != nullptr) << invoke_type;

      ArtMethod* orig_called = called;
      if (invoke_type == kVirtual) {
        called = receiver->GetClass()->FindVirtualMethodForVirtual(called, sizeof(void*));
      } else {
        called = receiver->GetClass()->FindVirtualMethodForInterface(called, sizeof(void*));
      }

      CHECK(called != nullptr) << PrettyMethod(orig_called) << " "
                               << PrettyTypeOf(receiver) << " "
                               << invoke_type << " " << orig_called->GetVtableIndex();

      // We came here because of sharpening. Ensure the dex cache is up-to-date on the method index
      // of the sharpened method avoiding dirtying the dex cache if possible.
      // Note, called_method.dex_method_index references the dex method before the
      // FindVirtualMethodFor... This is ok for FindDexMethodIndexInOtherDexFile that only cares
      // about the name and signature.
      uint32_t update_dex_cache_method_index = called->GetDexMethodIndex();
      if (!called->HasSameDexCacheResolvedMethods(caller)) {
        // Calling from one dex file to another, need to compute the method index appropriate to
        // the caller's dex file. Since we get here only if the original called was a runtime
        // method, we've got the correct dex_file and a dex_method_idx from above.
        DCHECK(!called_method_known_on_entry);
        DCHECK_EQ(caller->GetDexFile(), called_method.dex_file);
        const DexFile* caller_dex_file = called_method.dex_file;
        uint32_t caller_method_name_and_sig_index = called_method.dex_method_index;
        update_dex_cache_method_index =
            called->FindDexMethodIndexInOtherDexFile(*caller_dex_file,
                                                     caller_method_name_and_sig_index);
      }
      if ((update_dex_cache_method_index != DexFile::kDexNoIndex) &&
          (caller->GetDexCacheResolvedMethod(
              update_dex_cache_method_index, sizeof(void*)) != called)) {
        caller->SetDexCacheResolvedMethod(update_dex_cache_method_index, called, sizeof(void*));
      }
    } else if (invoke_type == kStatic) {
      const auto called_dex_method_idx = called->GetDexMethodIndex();
      // For static invokes, we may dispatch to the static method in the superclass but resolve
      // using the subclass. To prevent getting slow paths on each invoke, we force set the
      // resolved method for the super class dex method index if we are in the same dex file.
      // b/19175856
      if (called->GetDexFile() == called_method.dex_file &&
          called_method.dex_method_index != called_dex_method_idx) {
        called->GetDexCache()->SetResolvedMethod(called_dex_method_idx, called, sizeof(void*));
      }
    }

    // Ensure that the called method's class is initialized.
    StackHandleScope<1> hs(soa.Self());
    Handle<mirror::Class> called_class(hs.NewHandle(called->GetDeclaringClass()));
    linker->EnsureInitialized(soa.Self(), called_class, true, true);
    if (LIKELY(called_class->IsInitialized())) {
      if (UNLIKELY(Dbg::IsForcedInterpreterNeededForResolution(self, called))) {
        // If we are single-stepping or the called method is deoptimized (by a
        // breakpoint, for example), then we have to execute the called method
        // with the interpreter.
        code = GetQuickToInterpreterBridge();
      } else if (UNLIKELY(Dbg::IsForcedInstrumentationNeededForResolution(self, caller))) {
        // If the caller is deoptimized (by a breakpoint, for example), we have to
        // continue its execution with interpreter when returning from the called
        // method. Because we do not want to execute the called method with the
        // interpreter, we wrap its execution into the instrumentation stubs.
        // When the called method returns, it will execute the instrumentation
        // exit hook that will determine the need of the interpreter with a call
        // to Dbg::IsForcedInterpreterNeededForUpcall and deoptimize the stack if
        // it is needed.
        code = GetQuickInstrumentationEntryPoint();
      } else {
        code = called->GetEntryPointFromQuickCompiledCode();
      }
    } else if (called_class->IsInitializing()) {
      if (UNLIKELY(Dbg::IsForcedInterpreterNeededForResolution(self, called))) {
        // If we are single-stepping or the called method is deoptimized (by a
        // breakpoint, for example), then we have to execute the called method
        // with the interpreter.
        code = GetQuickToInterpreterBridge();
      } else if (invoke_type == kStatic) {
        // Class is still initializing, go to oat and grab code (trampoline must be left in place
        // until class is initialized to stop races between threads).
        code = linker->GetQuickOatCodeFor(called);
      } else {
        // No trampoline for non-static methods.
        code = called->GetEntryPointFromQuickCompiledCode();
      }
    } else {
      DCHECK(called_class->IsErroneous());
    }
  }
  CHECK_EQ(code == nullptr, self->IsExceptionPending());
  // Fixup any locally saved objects may have moved during a GC.
  visitor.FixupReferences();
  // Place called method in callee-save frame to be placed as first argument to quick method.
  *sp = called;

  return code;
}
```

上面代码可知，找到当前 `ArtMethod` 的流程大致的逻辑就是，根据 `caller` 函数 `ArtMethod` 的 `dex` 代码，可以找到这个 `ArtMethod` 的函数调用类型（`INVOKE_DIRECT, INVOKE_STATIC, INVOKE_SUPER, INVOKE_VIRTUAL etc`.），不同的类型查找的方式不一样，比如 `Virtual Method` 要从虚表里找，`Super Method` 要从父类的 `Method` 里去找，找到之后调用 `ClassLinker` 的 `ResolveMethod` 来解析，解析出来的 `ArtMethod` 的就是上面 `LinkCode` 过的 `ArtMethod` 。
下面就是 `ResolveMethod` 函数的实现， `Calss` 查找到 `Method` ，之后在赋值到 `DexCache` 里，这样下次再执行就能直接找到 `Resolved Method` 。

```
ArtMethod* ClassLinker::ResolveMethod(const DexFile& dex_file, uint32_t method_idx,
                                      Handle<mirror::DexCache> dex_cache,
                                      Handle<mirror::ClassLoader> class_loader,
                                      ArtMethod* referrer, InvokeType type) {
  DCHECK(dex_cache.Get() != nullptr);
  // Check for hit in the dex cache.
  ArtMethod* resolved = dex_cache->GetResolvedMethod(method_idx, image_pointer_size_);
  if (resolved != nullptr && !resolved->IsRuntimeMethod()) {
    DCHECK(resolved->GetDeclaringClassUnchecked() != nullptr) << resolved->GetDexMethodIndex();
    return resolved;
  }
  // Fail, get the declaring class.
  const DexFile::MethodId& method_id = dex_file.GetMethodId(method_idx);
  mirror::Class* klass = ResolveType(dex_file, method_id.class_idx_, dex_cache, class_loader);
  if (klass == nullptr) {
    DCHECK(Thread::Current()->IsExceptionPending());
    return nullptr;
  }
  // Scan using method_idx, this saves string compares but will only hit for matching dex
  // caches/files.
  switch (type) {
    case kDirect:  // Fall-through.
    case kStatic:
      resolved = klass->FindDirectMethod(dex_cache.Get(), method_idx, image_pointer_size_);
      DCHECK(resolved == nullptr || resolved->GetDeclaringClassUnchecked() != nullptr);
      break;
    case kInterface:
      resolved = klass->FindInterfaceMethod(dex_cache.Get(), method_idx, image_pointer_size_);
      DCHECK(resolved == nullptr || resolved->GetDeclaringClass()->IsInterface());
      break;
    case kSuper:  // Fall-through.
    case kVirtual:
      resolved = klass->FindVirtualMethod(dex_cache.Get(), method_idx, image_pointer_size_);
      break;
    default:
      LOG(FATAL) << "Unreachable - invocation type: " << type;
      UNREACHABLE();
  }
  if (resolved == nullptr) {
    // Search by name, which works across dex files.
    const char* name = dex_file.StringDataByIdx(method_id.name_idx_);
    const Signature signature = dex_file.GetMethodSignature(method_id);
    switch (type) {
      case kDirect:  // Fall-through.
      case kStatic:
        resolved = klass->FindDirectMethod(name, signature, image_pointer_size_);
        DCHECK(resolved == nullptr || resolved->GetDeclaringClassUnchecked() != nullptr);
        break;
      case kInterface:
        resolved = klass->FindInterfaceMethod(name, signature, image_pointer_size_);
        DCHECK(resolved == nullptr || resolved->GetDeclaringClass()->IsInterface());
        break;
      case kSuper:  // Fall-through.
      case kVirtual:
        resolved = klass->FindVirtualMethod(name, signature, image_pointer_size_);
        break;
    }
  }
  // If we found a method, check for incompatible class changes.
  if (LIKELY(resolved != nullptr && !resolved->CheckIncompatibleClassChange(type))) {
    // Be a good citizen and update the dex cache to speed subsequent calls.
    dex_cache->SetResolvedMethod(method_idx, resolved, image_pointer_size_);
    return resolved;
  } else {
    // If we had a method, it's an incompatible-class-change error.
    if (resolved != nullptr) {
      ThrowIncompatibleClassChangeError(type, resolved->GetInvokeType(), resolved, referrer);
    } else {
      // We failed to find the method which means either an access error, an incompatible class
      // change, or no such method. First try to find the method among direct and virtual methods.
      const char* name = dex_file.StringDataByIdx(method_id.name_idx_);
      const Signature signature = dex_file.GetMethodSignature(method_id);
      switch (type) {
        case kDirect:
        case kStatic:
          resolved = klass->FindVirtualMethod(name, signature, image_pointer_size_);
          // Note: kDirect and kStatic are also mutually exclusive, but in that case we would
          //       have had a resolved method before, which triggers the "true" branch above.
          break;
        case kInterface:
        case kVirtual:
        case kSuper:
          resolved = klass->FindDirectMethod(name, signature, image_pointer_size_);
          break;
      }

      // If we found something, check that it can be accessed by the referrer.
      bool exception_generated = false;
      if (resolved != nullptr && referrer != nullptr) {
        mirror::Class* methods_class = resolved->GetDeclaringClass();
        mirror::Class* referring_class = referrer->GetDeclaringClass();
        if (!referring_class->CanAccess(methods_class)) {
          ThrowIllegalAccessErrorClassForMethodDispatch(referring_class, methods_class, resolved,
                                                        type);
          exception_generated = true;
        } else if (!referring_class->CanAccessMember(methods_class, resolved->GetAccessFlags())) {
          ThrowIllegalAccessErrorMethod(referring_class, resolved);
          exception_generated = true;
        }
      }
      if (!exception_generated) {
        // Otherwise, throw an IncompatibleClassChangeError if we found something, and check
        // interface methods and throw if we find the method there. If we find nothing, throw a
        // NoSuchMethodError.
        switch (type) {
          case kDirect:
          case kStatic:
            if (resolved != nullptr) {
              ThrowIncompatibleClassChangeError(type, kVirtual, resolved, referrer);
            } else {
              resolved = klass->FindInterfaceMethod(name, signature, image_pointer_size_);
              if (resolved != nullptr) {
                ThrowIncompatibleClassChangeError(type, kInterface, resolved, referrer);
              } else {
                ThrowNoSuchMethodError(type, klass, name, signature);
              }
            }
            break;
          case kInterface:
            if (resolved != nullptr) {
              ThrowIncompatibleClassChangeError(type, kDirect, resolved, referrer);
            } else {
              resolved = klass->FindVirtualMethod(name, signature, image_pointer_size_);
              if (resolved != nullptr) {
                ThrowIncompatibleClassChangeError(type, kVirtual, resolved, referrer);
              } else {
                ThrowNoSuchMethodError(type, klass, name, signature);
              }
            }
            break;
          case kSuper:
            if (resolved != nullptr) {
              ThrowIncompatibleClassChangeError(type, kDirect, resolved, referrer);
            } else {
              ThrowNoSuchMethodError(type, klass, name, signature);
            }
            break;
          case kVirtual:
            if (resolved != nullptr) {
              ThrowIncompatibleClassChangeError(type, kDirect, resolved, referrer);
            } else {
              resolved = klass->FindInterfaceMethod(name, signature, image_pointer_size_);
              if (resolved != nullptr) {
                ThrowIncompatibleClassChangeError(type, kInterface, resolved, referrer);
              } else {
                ThrowNoSuchMethodError(type, klass, name, signature);
              }
            }
            break;
        }
      }
    }
    Thread::Current()->AssertPendingException();
    return nullptr;
  }
}
```

至此，Art Method 的执行机制就算介绍完了，我们对整个函数执行机制都有个全局的概念了。

# 参考
https://blog.csdn.net/threepigs/article/details/52884904

https://www.jianshu.com/p/2ff1b63f686b
