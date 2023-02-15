---
title: Android源码阅读-dlopen
date: 2019-11-17 14:45:44
tag: Android源码阅读
category: Android源码
---

> 本次阅读源码来自aosp Android 8.1.0_r1，在阅读过程中根据阅读的进度随手记录

有过 `linux` 编成经验的都应该知道使用 `dlopen` 需要包含 `dlfcn.h` 头文件，所以直接去`aosp/bionic/libc/include/dlfcn.h` 中找到 `dlopen` 的函数定义。

```
void* dlopen(const char* filename, int flag);
```
通过`dlopen`的定义找到其实现在 `aosp/bionic/libdl/libdl.c` 中，
```
// Proxy calls to bionic loader
void* dlopen(const char* filename, int flag) {
    /*
  __builtin_return_address(0)的含义是，得到当前函数返回地址，即此函数被别的函数调用，然后此函数执行完毕后，返回，所谓返回地址就是那时候的地址。
  __builtin_return_address(1)的含义是，得到当前函数的调用者的返回地址。注意是调用者的返回地址，而不是函数起始地址。 
    */
  const void* caller_addr = __builtin_return_address(0);//可以理解为汇编 MOV R0, LR
  return __loader_dlopen(filename, flag, caller_addr);
}
```
可以发现 `dlopen` 的是通过 `__loader_dlopen` 完成其实现的。查看 `__loader_dlopen` 的实现，会发现找不到其是实现方法，只能找到相关的定义 `aosp/bionic/libdl/libdl.c`：
```
__attribute__((__weak__, visibility("default")))
void* __loader_dlopen(const char* filename, int flags, const void* caller_addr);
```
通过搜索 `__loader_dlopen` 字符串，发现 `aosp/bionic/linker/dlfcn.cpp` 存在相关字符串。

![](/Android源码阅读-dlopen/2019-11-17-15-12-04.png

跟进去后发现，其被定义的在 `ANDROID_LIBDL_STRTAB` 字符串数组中
```
static const char ANDROID_LIBDL_STRTAB[] =
  // 0000000000111111 11112222222222333 333333344444444 44555555555566666 6666677777777778 8888888889999999999
  // 0123456789012345 67890123456789012 345678901234567 89012345678901234 5678901234567890 1234567890123456789
    "__loader_dlopen\0__loader_dlclose\0__loader_dlsym\0__loader_dlerror\0__loader_dladdr\0__loader_android_up"
  // 1*
  // 000000000011111111112 2222222223333333333444444444455555555 5566666666667777777777888 88888889999999999
  // 012345678901234567890 1234567890123456789012345678901234567 8901234567890123456789012 34567890123456789
    "date_LD_LIBRARY_PATH\0__loader_android_get_LD_LIBRARY_PATH\0__loader_dl_iterate_phdr\0__loader_android_"
  // 2*
  // 00000000001 1111111112222222222333333333344444444445555555555666 6666666777777777788888888889999999999
  // 01234567890 1234567890123456789012345678901234567890123456789012 3456789012345678901234567890123456789
    "dlopen_ext\0__loader_android_set_application_target_sdk_version\0__loader_android_get_application_targ"
  // 3*
  // 000000000011111 111112222222222333333333344444444445555555 5556666666666777777777788888888889 999999999
  // 012345678901234 567890123456789012345678901234567890123456 7890123456789012345678901234567890 123456789
    "et_sdk_version\0__loader_android_init_anonymous_namespace\0__loader_android_create_namespace\0__loader_"
  // 4*
  // 0000000 000111111111122222222223333 333333444444444455 555555556666666666777777777788888 888889999999999
  // 0123456 789012345678901234567890123 456789012345678901 234567890123456789012345678901234 567890123456789
    "dlvsym\0__loader_android_dlwarning\0__loader_cfi_fail\0__loader_android_link_namespaces\0__loader_androi"
  // 5*
  // 0000000000111111111122222 22222
  // 0123456789012345678901234 56789
    "d_get_exported_namespace\0"
#if defined(__arm__)
  // 525
    "__loader_dl_unwind_find_exidx\0"
#endif
    ;
```
继续跟进，发现 `g_libdl_symtab` 中使用了 `ANDROID_LIBDL_STRTAB` 
```
static ElfW(Sym) g_libdl_symtab[] = {
  // Total length of libdl_info.strtab, including trailing 0.
  // This is actually the STH_UNDEF entry. Technically, it's
  // supposed to have st_name == 0, but instead, it points to an index
  // in the strtab with a \0 to make iterating through the symtab easier.
  ELFW(SYM_INITIALIZER)(sizeof(ANDROID_LIBDL_STRTAB) - 1, nullptr, 0),
  ELFW(SYM_INITIALIZER)(  0, &__dlopen, 1),
  ELFW(SYM_INITIALIZER)( 16, &__dlclose, 1),
  ELFW(SYM_INITIALIZER)( 33, &__dlsym, 1),
  ELFW(SYM_INITIALIZER)( 48, &__dlerror, 1),
  ELFW(SYM_INITIALIZER)( 65, &__dladdr, 1),
  ELFW(SYM_INITIALIZER)( 81, &__android_update_LD_LIBRARY_PATH, 1),
  ELFW(SYM_INITIALIZER)(121, &__android_get_LD_LIBRARY_PATH, 1),
  ELFW(SYM_INITIALIZER)(158, &dl_iterate_phdr, 1),
  ELFW(SYM_INITIALIZER)(183, &__android_dlopen_ext, 1),
  ELFW(SYM_INITIALIZER)(211, &__android_set_application_target_sdk_version, 1),
  ELFW(SYM_INITIALIZER)(263, &__android_get_application_target_sdk_version, 1),
  ELFW(SYM_INITIALIZER)(315, &__android_init_anonymous_namespace, 1),
  ELFW(SYM_INITIALIZER)(357, &__android_create_namespace, 1),
  ELFW(SYM_INITIALIZER)(391, &__dlvsym, 1),
  ELFW(SYM_INITIALIZER)(407, &__android_dlwarning, 1),
  ELFW(SYM_INITIALIZER)(434, &__cfi_fail, 1),
  ELFW(SYM_INITIALIZER)(452, &__android_link_namespaces, 1),
  ELFW(SYM_INITIALIZER)(485, &__android_get_exported_namespace, 1),
#if defined(__arm__)
  ELFW(SYM_INITIALIZER)(525, &__dl_unwind_find_exidx, 1),
#endif
};
```

将相关代码复制到一个C源码文件，使用 `g++ -E` 宏展开并优化后：

```
static ElfW(Sym) g_libdl_symtab[] = {

  { sizeof(ANDROID_LIBDL_STRTAB) - 1, 0, 0, 0, reinterpret_cast<Elf64_Addr>(nullptr), 0, },
  { 0, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__dlopen), 0, },
  { 16, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__dlclose), 0, },
  { 33, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__dlsym), 0, },
  { 48, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__dlerror), 0, },
  { 65, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__dladdr), 0, },
  { 81, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__android_update_LD_LIBRARY_PATH), 0, },
  { 121, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__android_get_LD_LIBRARY_PATH), 0, },
  { 158, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&dl_iterate_phdr), 0, },
  { 183, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__android_dlopen_ext), 0, },
  { 211, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__android_set_application_target_sdk_version), 0, },
  { 263, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__android_get_application_target_sdk_version), 0, },
  { 315, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__android_init_anonymous_namespace), 0, },
  { 357, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__android_create_namespace), 0, },
  { 391, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__dlvsym), 0, },
  { 407, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__android_dlwarning), 0, },
  { 434, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__cfi_fail), 0, },
  { 452, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__android_link_namespaces), 0, },
  { 485, (STB_GLOBAL << 4), 0, 1, reinterpret_cast<Elf64_Addr>(&__android_get_exported_namespace), 0, },

};
```
2019-11-17 01:47:00.768 10666-10666/com.example.ndkdebug E/linker: library "/system/lib64/libandroid_runtime.so" ("/system/lib64/libandroid_runtime.so") needed or dlopened by "/data/app/com.example.ndkdebug-tJLtYCKjLoRt2XhNQlIu9A==/lib/arm64/libnative-lib.so" is not accessible for the namespace: [name="classloader-namespace", ld_library_paths="", default_library_paths="/data/app/com.example.ndkdebug-tJLtYCKjLoRt2XhNQlIu9A==/lib/arm64:/data/app/com.example.ndkdebug-tJLtYCKjLoRt2XhNQlIu9A==/base.apk!/lib/arm64-v8a", permitted_paths="/data:/mnt/expand:/data/data/com.example.ndkdebug"]

未完待续。。。
