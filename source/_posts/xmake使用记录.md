---
title: xmake使用记录
date: 2023-02-16 17:56:23
tags: xmake
category: C++
---

xmake 是一个基于 Lua 的轻量级跨平台构建工具，使用 xmake.lua 维护项目构建，相比 makefile/CMakeLists.txt，配置语法更加简洁直观，对新手非常友好，短时间内就能快速入门，能够让用户把更多的精力集中在实际的项目开发上。

其官网为：https://xmake.io 。

# xmake 基础

## 创建项目

`xmake` 可以使用 `create` 选项创建一个 `xmake` 项目，例如创建一个名为 `helloxmake` 的项目。

```bash
 $ xmake create helloxmake
create helloxmake ...
  [+]: src\main.cpp
  [+]: xmake.lua
  [+]: .gitignore
create ok!
```

进入 `helloxmake` 项目根目录，执行 `xamke` 命令即可编译，执行 `xmake run` 即可运行。

```bash
 $ cd helloxmake\
 $ xmake
checking for platform ... windows
checking for architecture ... x64
checking for Microsoft Visual Studio (x64) version ... 2019
[ 25%]: ccache compiling.release src\main.cpp
[ 50%]: linking.release helloxmake.exe
[100%]: build ok!
 $ xmake run
hello world!
```

可以用 `xmake create -h` 命令查看 `create` 选项的更多用法。

## 使用本地包

### 创建本地包

首先创建一个库，添加 `hello.h` 和 `hello.cpp` 到 `src` 文件，其内容如下。

```c
// hello.h
int add(int a, int b);

// hello.cpp
int add(int a, int b){
    return a + b;
}
```

修改 `xmake.lua` 文件内容如下：

```lua
add_rules("mode.debug", "mode.release")

target("helloxmake")
    set_kind("static")
    add_headerfiles("src/hello.h")
    add_files("src/hello.cpp")
```

执行 `xmake` 命令即可编译出 `lib` 文件。

```bash
 $ xmake
checking for Microsoft Visual Studio (x64) version ... 2019
[ 25%]: ccache compiling.release src\hello.cpp
[ 50%]: archiving.release helloxmake.lib
[100%]: build ok!
```

使用 `xmake package` 即可打包为本地包。

```bash
 $ xmake package
[ 25%]: ccache compiling.release src\hello.cpp
[ 50%]: archiving.release helloxmake.lib
[100%]: build ok!
package(helloxmake): build\packages\h\helloxmake generated
```

### add_requires 添加本地包

前面编译的 `build\packages\h\helloxmake` 目录即为创建的本地包。

为了方便，将 `build` 目录复制到需要使用本地库的项目中，将 `xmake.lua` 内容修改如下即可使用本地库：

```bash
add_rules("mode.debug", "mode.release")

add_repositories("local-repo build")
add_requires("helloxmake")

target("usepackge")
    set_kind("binary")
    add_files("src/main.cpp")
    add_packages("helloxmake")
```

`main.cpp` 内容如下：

```cpp
#include <iostream>
#include <hello.h>

using namespace std;

int main(int argc, char** argv)
{
    int n = add(1, 2);
    printf("%d\n", n);
    cout << "hello world!" << endl;
    return 0;
}
```

执行 `xmake` 命令即可编译。

```bash
 $ xmake
[ 25%]: ccache compiling.release src\main.cpp
[ 50%]: linking.release usepackge.exe
[100%]: build ok!
```

### add_links 添加本地包

修改 `xmake.lua` 内容如下，然后执行 `xmake` 命令即可编译。

```lua
add_rules("mode.debug", "mode.release")

target("usepackge")
    set_kind("binary")
    add_files("src/*.cpp")
    -- 添加 include 目录
    add_includedirs("../helloxmake/src")
    -- 添加 lib 目录
    add_linkdirs("../helloxmake/build/windows/x64/release")
    -- 添加 lib
    add_links("helloxmake")
```

## 使用远程包

这里我们使用 Apache 提供的跨平台运行库 [apr](https://github.com/apache/apr) 作为示例 。

### 使用官方提供的远程包

首先创建一个项目，修改 `xmake.lua` 内容，使用 `add_requires("apr")` 添加官方提供的 [xrepo](https://xrepo.xmake.io) 远程包。
```lua
add_rules("mode.debug", "mode.release")

add_requires("apr 1.7.0")

target("testapr")
    set_kind("binary")
    add_files("src/main.cpp")
    add_packages("apr")
```

`main.cpp` 内容如下：
```cpp
#include <iostream>
#include <apr_general.h>
using namespace std;

int main(int argc, char** argv)
{
    // Initialization
    apr_initialize();
    cout << "hello world!" << endl;
    apr_terminate();
    return 0;
}
```

然后运行 `xmake` 命令，将会自动下载对应的包，并编译。


### 使用本地远程包
由于 apr 最新版本为 1.7.2，而 [xrepo](https://xrepo.xmake.io) 提供的最新版本为1.7.0，我们可以参考 [xrepo](https://xrepo.xmake.io) 对应的 xmake.lua，使用本地远程包。

```lua
package("apr")
    set_homepage("https://github.com/apache/apr")
    set_description("Mirror of Apache Portable Runtime")
    set_license("Apache-2.0")

    add_urls("https://github.com/apache/apr/archive/refs/tags/$(version).tar.gz",
             "https://github.com/apache/apr.git")
    add_versions("1.7.2", "8be046191792776815b5e123298a2c379a962abbdf124fddd2b487b5203d7da9")

    if is_plat("linux") then
        add_deps("libtool", "python")
        add_patches("1.7.0", path.join(os.scriptdir(), "patches", "1.7.0", "common.patch"), "bbfef69c914ca1ab98a9d94fc4794958334ce5f47d8c08c05e0965a48a44c50d")
    elseif is_plat("windows") then
        add_deps("cmake")
        add_syslinks("wsock32", "ws2_32", "advapi32", "shell32", "rpcrt4")
    end

    on_install("linux", "macosx", function (package)
        local configs = {}
        if package:is_plat("linux") then
            os.vrunv("sh", {"./buildconf"})
            io.replace("configure", "RM='$RM'", "RM='$RM -f'")
        else
            io.replace("configure.in", "pid_t_fmt='#error Can not determine the proper size for pid_t'", "pid_t_fmt='#define APR_PID_T_FMT \"d\"'")
            os.vrunv("sh", {"./buildconf"})
            table.insert(configs, "CFLAGS=-DAPR_IOVEC_DEFINED")
        end
        import("package.tools.autoconf").install(package, configs)
        if package:config("shared") then
            os.rm(package:installdir("lib/*.a"))
        else
            os.tryrm(package:installdir("lib/*.so*"))
            os.tryrm(package:installdir("lib/*.dylib"))
        end
        package:add("links", "apr-1")
        package:add("includedirs", "include/apr-1")
    end)

    on_install("windows", function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DAPR_BUILD_SHARED=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DAPR_BUILD_STATIC=" .. (package:config("shared") and "OFF" or "ON"))
        import("package.tools.cmake").install(package, configs)
        -- libapr-1 is shared, apr-1 is static
        if package:config("shared") then
            package:add("defines", "APR_DECLARE_EXPORT")
            os.rm(package:installdir("lib/apr-1.lib"))
            os.rm(package:installdir("lib/aprapp-1.lib"))
        else
            package:add("defines", "APR_DECLARE_STATIC")
            os.rm(package:installdir("lib/lib*.lib"))
            os.rm(package:installdir("bin/lib*.dll"))
        end
    end)

    on_test(function (package)
        assert(package:has_cfuncs("apr_initialize", {includes = "apr_general.h"}))
    end)
package_end()

add_requires("apr 1.7.2")

target("testapr")
    set_kind("binary")
    add_files("src/main.cpp")
    add_packages("apr")
```
然后运行 `xmake` 命令，将会自动下载对应源码进行编译打包，最终编译成可执行程序。




# 参考
https://github.com/myuanz/getting-started-with-xmake-package

https://xmake.io/#/zh-cn/guide/installation

https://xrepo.xmake.io/#/zh-cn/getting_started