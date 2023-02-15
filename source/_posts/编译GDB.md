---
title: 编译GDB
date: 2020-09-14 13:11:38
tags: Debug
category: Android
---

# 编译 gdb 和 gdbserver

## Download gdb source code:
```bash
wget ftp://sourceware.org/pub/gdb/releases/gdb-9.1.tar.gz
```

## Extract file:
```bash
tar xzvf gdb-9.1.tar.gz
```

## Move into source folder
```bash
 cd gdb-9.1
```

## Edit file `gdb/gdbserver/linux-low.c` :

### Add two lines : Line 107 & Line 122 with the content below
```bash
#define HAVE_ELF32_AUXV_T  //  Line 107 (Added)
#ifndef HAVE_ELF32_AUXV_T

#define HAVE_ELF64_AUXV_T // Line 122 (Added)
#ifndef HAVE_ELF64_AUXV_T
```
This modification is neccessary to build Android, since Android system libraries already define struct Elf32_auxv_t and Elf64_auxv_t .(Please see this for detail: https://github.com/android/ndk/issues/1008)

### Modify function linux_request_interrupt:
```cpp
static void
linux_request_interrupt (void)
 {
   /* .... */
-  kill (-signal_pid, SIGINT); // replace this line with next 3 lines
+  int r = kill (-signal_pid, SIGINT);
+  if (r != 0)
+      kill (signal_pid, SIGINT);
 }
```
This fixes bug "gdbserver not handling Ctrl+C", detail at: https://sourceware.org/bugzilla/show_bug.cgi?id=18772

## Build gdb for linux:
```bash
sudo apt-get install build-essential \
  gcc g++ make autogen m4 \
  bison gettext libpython-dev

mkdir build-linux

cd build-linux/

../configure --enable-targets=all --with-python=/usr/bin/python

make -j4

sudo make install
```
我使用 ubuntu 18.04 按照以上编译过程中，出现了一个错误。
```bash
xcoffread.c:180:1: error: invalid conversion from ‘int’ to ‘const char*’ [-fpermissive]
```
看源码注释，那段代码冒是 XCOFF 文件格式个 dwarf 节， 最后将出错那一行注释掉就好了，不影响使用。

## Build gdbserver for android:

### Download android-sdk
```bash
cd ~

mkdir android

cd android

wget https://dl.google.com/android/repository/commandlinetools-linux-6200805_latest.zip

unzip commandlinetools-linux-6200805_latest.zip

export PATH=$PATH:~/android/tools/bin
```

### Install Ndk
```bash
sdkmanager --install "ndk;21.0.6113669" --sdk_root=.
```

### Create standalone toolchain for NDK
```bash
cd ~/android/ndk/21.0.6113669/

./build/tools/make-standalone-toolchain.sh \ 
--toolchain=aarch64-linux-android-4.9 \
--install-dir=~/android/ndk_21
```
This step create the standalone toolchain at: ~/android/ndk_21

### Configure and build gdbserver for android
```bash
cd ~/gdb-9.1

mkdir build-android

cd build-android

export PATH=$PATH:~/android/ndk_21/bin

CC=aarch64-linux-android-gcc ../configure \
    --target=aarch64-linux-android \
    --host=aarch64-linux-android \
     LDFLAGS="-static-libstdc++"

make -j4
```
If get error related to "source-highlight", add --disable-source-highlight to the configure flag.

After build finishes, gdbserver is located at: gdb/gdbserver/gdbserver

编译过程中可能出现错误：
```bash
aarch64.c:50: undefined reference to `set_tdesc_osabi
```
但是 gdbserver 已经编译好了，所以这个错误可以不用管。

以上参考来自 https://stackoverflow.com/questions/60973768/build-gdb-and-gdbserver-for-android/61157179#61157179 。

------------

**下面的内容可以忽略**


# Window 下编译 gdb

**可能会遇到很多坑，不建议。**

## 准备环境

1. 安装 MSYS2 

直接去 https://www.msys2.org/ 网站[下载](https://repo.msys2.org/distrib/x86_64/msys2-x86_64-20200903.exe)并安装 MSYS2 。


2. 安装 mingw64

下载 [mingw64](https://sourceforge.net/projects/mingw-w64/files/) 并解压到 `mingw64/mingw64` 目录。

3. 安装其他工具

```
pacman -S pactoys
# 这里我使用 pacboy 安装了python3和 gcc 无法使用，所以还是使用下列网站的 gcc 吧。
# pacboy -S gcc:x python3:x  # mingw-w64-x86_64-gcc mingw-w64-x86_64-python3
pacman -S make texinfo bison git dejagnu 
```
如果后续编译报错，则根据报错信息安装相应的工具即可。

[gcc下载地址](https://sourceforge.net/projects/mingw-w64/files/)。

## 开始编译
1. 下载源码
直接去官网下载对应的源码 https://sourceware.org/gdb/ 

2. 启动 `msys64/mingw64.exe`, 执行下列命令
```
cd gdb-9.2
mkdir build
cd build
../configure 
make
```
等待编译完成。

# ubuntu 18.04 下编译

首先说明一下，由于我编译过 Android 源码，所以可能很多需要的库已经安装了，下面是给出其他博客上的环境配置，仅作参考，不一定需要安装，可以根据编译报错信息搜寻解答方法。
```bash
sudo apt-get install texinfo 
sudo apt-get install libncurses5-dev 
sudo apt-get install m4
sudo apt-get install flex 
sudo apt-get install bison
为了支持python脚本扩展功能
apt-get install python-dev
```
由于我要编译 Android 平台的 gdb 所以使用了 Android 源码中的 gdb 源码，直接使用 gdb 官网的源码应该也不影响。
> https://android.googlesource.com/toolchain/gdb/  Android 源码中的 gdb 。
> https://sourceware.org/gdb/    gdb 官网。

好了废话了那么多，那么开始编译吧。可以按照如下命令进行编译，其中当前目录为 gdb 源码根目录。
```bash
➜  mkdir build
➜  cd build
➜  ../configure --with-python=/usr/bin/python3.6 --target=arm-linux-androideabi --prefix=/home/ckcat/tools/gdb9.2
➜  make -j8
```
编译成功后，`build/gdb` 目录下会出现 gdb 的二进制可执行文件。

简单的说一下 configure 的参数。
```bash
--with-python=/usr/bin/python3.6     # gdb 支持python3
--target=arm-linux-androideabi    # 目标平台为 Android ARM 如果需要支持aarch64，将参数改为 aarch64-linux-android 即可。
--prefix=/home/ckcat/tools/gdb9.2    # make install 安装目录
```
当执行 `make install` 命令后，可以看到 `/home/ckcat/tools/gdb9.2` 已经包含了 gdb 相关文件。

如果目标平台为全平台，则应该用下面参数：
```bash
../configure --with-python=/usr/bin/python3.6 --enable-targets=all --prefix=/home/ckcat/tools/gdb9.2
```

说一下我在编译过程中碰到的错误
```bash
Makefile:541: recipe for target 'bfd.info' failed
```
这个错误说明没有安装 texinfo 库，安装一下就好了。

```bash
configure: error: no usable python found at /usr
```
这个错误是因为我之前的参数 `--with-python=/usr` 没有指定对应的 python 文件。后面修改为  `--with-python=/usr/bin/python3.6 ` 就可以了。


参考：
```
https://sourceware.org/gdb/wiki/BuildingOnWindows
https://blog.csdn.net/pfysw/article/details/105451883
https://github.com/ikonst/gdb-7.7-android
https://www.msys2.org/
https://www.cntofu.com/book/46/gdb/188.md
https://mudongliang.github.io/2017/08/12/compile-gdb-with-python-script-support.html
https://stackoverflow.com/questions/60973768/build-gdb-and-gdbserver-for-android
https://medium.com/@k3vinlusec/how-to-compile-gdb-9-x-and-gdbserver-for-android-24ef6625ee9a
https://segmentfault.com/a/1190000021029824
```