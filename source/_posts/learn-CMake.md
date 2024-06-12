---
title: CMake的使用记录
date: 2020-06-11 13:04:32
tags: C++
category: C++
---


# 什么是 CMake

[CMake](https://cmake.org/) 是一个开源、跨平台的工具系列，可用于构建、测试和打包软件。它通过一个名为 `CMakeLists.txt` 的配置文件来管理软件的编译流程，并根据用户所选择的目标平台生成构建软件所需的本地化 makefile 或 workspace。通俗来讲，使用 CMake 可以生成 UNIX-like 上构建软件所需的 Makefile 和 Windows 上构建软件所需的 vcxproj，而无需为它们单独写一份 `Makefile/vcxproj` 。

[CMake 官方文档](https://cmake.org/documentation/)中有详细的使用手册，可以帮助用户更深入地了解 CMake。另外，还有一个详细的使用教程：[CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)。

# 入门案例

## 单个源文件

对于简单的项目，只需要写几行代码就可以了。例如，假设现在我们的项目中只有一个源文件 `main.cc` ，该程序的用途是计算一个数的指数幂。

```cpp
#include <stdio.h>
#include <stdlib.h>

/**
 * power - Calculate the power of number.
 * @param base: Base value.
 * @param exponent: Exponent value.
 *
 * @return base raised to the power exponent.
 */
double power(double base, int exponent){
    double result = base;
    int i;

    if (exponent == 0)
        return 1;

    for(i = 1; i < exponent; ++i)
        result = result * base;

    return result;
}

int main(int argc, char *argv[]){
    if (argc < 3){
        printf("Usage: %s base exponent \n", argv[0]);
        return 1;
    }
    double base = atof(argv[1]);
    int exponent = atoi(argv[2]);
    double result = power(base, exponent);
    printf("%g ^ %d is %g\n", base, exponent, result);
    return 0;
}
```

### 编写 CMakeLists.txt

首先编写 `CMakeLists.txt` 文件，并保存在与 `main.cc` 源文件同个目录下：

```cmake
# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (Demo1)

# 指定生成目标
add_executable(Demo main.cc)
```

`CMakeLists.txt` 的语法比较简单，由命令、注释和空格组成，其中命令是不区分大小写的。符号 `#` 后面的内容被认为是注释。命令由命令名称、小括号和参数组成，参数之间使用空格进行间隔。

对于上面的 `CMakeLists.txt` 文件，依次出现了几个命令：

1.  `cmake_minimum_required`：指定运行此配置文件所需的 CMake 的最低版本；
2.  `project`：参数值是 `Demo1`，该命令表示项目的名称是 `Demo1` 。
3.  `add_executable`： 将名为 `main.cc` 的源文件编译成一个名称为 Demo 的可执行文件。

### 编译项目

之后，在当前目录下创建 build 目录，并进入 build 目录，然后执行 `cmake ..` 命令 ，最后执行 `cmake --build .` 命令编译得到 Demo1 可执行文件。

```bash
$ mkdir build
$ cd build
$ cmake ..
-- Building for: Visual Studio 16 2019

...

-- Detecting CXX compile features - done
-- Configuring done (9.4s)
-- Generating done (0.0s)
-- Build files have been written to: D:/src/01/build

$ cmake --build .
用于 .NET Framework 的 Microsoft (R) 生成引擎版本 16.11.2+f32259642
版权所有(C) Microsoft Corporation。保留所有权利。

  1>Checking Build System
  Building Custom Rule D:/src/01/CMakeLists.txt
  main.cc
  Demo.vcxproj -> D:\src\01\build\Debug\Demo.exe
  'pwsh.exe' 不是内部或外部命令，也不是可运行的程序
  或批处理文件。
  Building Custom Rule D:/src/01/CMakeLists.txt

$ ./Debug/Demo.exe 4, 5
4 ^ 5 is 1024

$ ./Debug/Demo.exe 2, 10
2 ^ 10 is 1024
```

## 多个源文件

### 同一目录，多个源文件

上面的例子只有单个源文件。现在假如把 `power` 函数单独写进一个名为 `MathFunctions.cc` 的源文件里，使得这个工程变成如下的形式：

```
./Demo2
    |
    +--- main.cc
    |
    +--- MathFunctions.cc
    |
    +--- MathFunctions.h
```

这个时候，`CMakeLists.txt` 可以改成如下的形式：

```cmake
# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (Demo2)

# 指定生成目标
add_executable(Demo main.cc MathFunctions.cc)
```

唯一的改动只是在 `add_executable` 命令中增加了一个 `MathFunctions.cc` 源文件。这样写当然没什么问题，但是如果源文件很多，把所有源文件的名字都加进去将是一件烦人的工作。更省事的方法是使用 `aux_source_directory` 命令，该命令会查找指定目录下的所有源文件，然后将结果存进指定变量名。其语法如下：

```cmake
aux_source_directory(<dir> <variable>)
```

因此，可以修改 CMakeLists.txt 如下：

```cmake
# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (Demo2)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 指定生成目标
add_executable(Demo ${DIR_SRCS})
```

这样，CMake 会将当前目录所有源文件的文件名赋值给变量 `DIR_SRCS` ，再指示变量 `DIR_SRCS` 中的源文件需要编译成一个名称为 Demo 的可执行文件。

### 多个目录，多个源文件

现在进一步将 `MathFunctions.h` 和 `MathFunctions.cc` 文件移动到 math 目录下。

```
./Demo3
    |
    +--- main.cc
    |
    +--- math/
          |
          +--- MathFunctions.cc
          |
          +--- MathFunctions.h
```

对于这种情况，需要分别在项目根目录 Demo3 和 math 目录里各编写一个 `CMakeLists.txt` 文件。为了方便，我们可以先将 math 目录里的文件编译成静态库再由 main 函数调用。

根目录中的 `CMakeLists.txt` ：

```cmake
# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (Demo3)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 添加 math 子目录
add_subdirectory(math)

# 指定生成目标
add_executable(Demo main.cc)

# 添加链接库
target_link_libraries(Demo MathFunctions)
```

该文件添加了下面的内容: 第 3 行，使用命令 `add_subdirectory` 指明本项目包含一个子目录 math，这样 math 目录下的 `CMakeLists.txt` 文件和源代码也会被处理 。第 6 行，使用命令 `target_link_libraries` 指明可执行文件 main 需要连接一个名为 MathFunctions 的链接库 。

子目录中的 `CMakeLists.txt`：

```cmake
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

# 生成链接库
add_library (MathFunctions ${DIR_LIB_SRCS})
```

在该文件中使用命令 `add_library` 将 src 目录中的源文件编译为静态链接库。

## 自定义编译选项

CMake 允许为项目增加编译选项，从而可以根据用户的环境和需求选择最合适的编译方案。

例如，可以将 MathFunctions 库设为一个可选的库，如果该选项为 `ON` ，就使用该库定义的数学函数来进行运算。否则就调用标准库中的数学函数库。

我们要做的第一步是在顶层的 `CMakeLists.txt` 文件中添加该选项：

```cmake
# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (Demo4)

# 加入一个配置头文件，用于处理 CMake 对源码的设置
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

# 是否使用自己的 MathFunctions 库
option (USE_MYMATH
       "Use provided math implementation" ON)

# 是否加入 MathFunctions 库
if (USE_MYMATH)
  include_directories ("${PROJECT_SOURCE_DIR}/math")
  add_subdirectory (math)
  set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)
endif (USE_MYMATH)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 指定生成目标
add_executable(Demo ${DIR_SRCS})
target_link_libraries (Demo  ${EXTRA_LIBS})
```

其中：

1.  第 7 行的 `configure_file` 命令用于加入一个配置头文件 `config.h` ，这个文件由 CMake 从 `config.h.in` 生成，通过这样的机制，将可以通过预定义一些参数和变量来控制代码的生成。
2.  第 13 行的 `option` 命令添加了一个 `USE_MYMATH` 选项，并且默认值为 `ON` 。
3.  第 17 行根据 `USE_MYMATH` 变量的值来决定是否使用我们自己编写的 MathFunctions 库。

之后修改 `main.cc` 文件，让其根据 `USE_MYMATH` 的预定义值来决定是否调用标准库还是 MathFunctions 库：

```c
#include <stdio.h>
#include <stdlib.h>
#include "config.h"

#ifdef USE_MYMATH
  #include "math/MathFunctions.h"
#else
  #include <math.h>
#endif


int main(int argc, char *argv[])
{
    if (argc < 3){
        printf("Usage: %s base exponent \n", argv[0]);
        return 1;
    }
    double base = atof(argv[1]);
    int exponent = atoi(argv[2]);

#ifdef USE_MYMATH
    printf("Now we use our own Math library. \n");
    double result = power(base, exponent);
#else
    printf("Now we use the standard library. \n");
    double result = pow(base, exponent);
#endif
    printf("%g ^ %d is %g\n", base, exponent, result);
    return 0;
}
```

上面的程序值得注意的是第 2 行，这里引用了一个 `config.h` 文件，这个文件预定义了 `USE_MYMATH` 的值。但我们并不直接编写这个文件，为了方便从 `CMakeLists.txt` 中导入配置，我们编写一个 `config.h.in` 文件，内容如下：

```c
#cmakedefine USE_MYMATH
```

这样 CMake 会自动根据 CMakeLists 配置文件中的设置自动生成 `config.h` 文件。

现在编译一下这个项目，为了便于交互式的选择该变量的值，可以使用 `cmake -D` 命令。

USE_MYMATH 为 ON 运行结果：

```bash
$ cmake .. -DUSE_MYMATH=ON
-- Building for: Visual Studio 16 2019
...
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (11.7s)
-- Generating done (0.1s)
-- Build files have been written to: D:/src/04/build

$ cmake --build .
用于 .NET Framework 的 Microsoft (R) 生成引擎版本 16.11.2+f32259642
版权所有(C) Microsoft Corporation。保留所有权利。

  1>Checking Build System
  Building Custom Rule D:/src/04/math/CMakeLists.txt
  MathFunctions.cc
  MathFunctions.vcxproj -> D:\src\04\build\math\Debug\MathFunctions.lib
  Building Custom Rule D:/src/04/CMakeLists.txt
  main.cc
  Demo.vcxproj -> D:\src\04\build\Debug\Demo.exe
  'pwsh.exe' 不是内部或外部命令，也不是可运行的程序
  或批处理文件。
  Building Custom Rule D:/src/04/CMakeLists.txt

$ ./Debug/Demo.exe 2, 10
Now we use our own Math library.
2 ^ 10 is 1024
```

此时 `config.h` 的内容为：

```c
#define USE_MYMATH
```

USE_MYMATH 为 OFF 运行结果：

```
[ehome@xman Demo4]$ ./Demo
Now we use the standard library.
 7 ^ 3 = 343.000000
 10 ^ 5 = 100000.000000
 2 ^ 10 = 1024.000000
```

此时 `config.h` 的内容为：

```
/* #undef USE_MYMATH */
```

## 安装和测试

CMake 也可以指定安装规则，以及添加测试。这两个功能分别可以通过在产生 Makefile 后使用 `make install` 和 `make test` 来执行。在以前的 GNU Makefile 里，你可能需要为此编写 `install` 和 `test` 两个伪目标和相应的规则，但在 CMake 里，这样的工作同样只需要简单的调用几条命令。

### 定制安装规则

首先先在 `math/CMakeLists.txt` 文件里添加下面两行：

```cmake
# 指定 MathFunctions 库的安装路径
install (TARGETS MathFunctions DESTINATION bin)
install (FILES MathFunctions.h DESTINATION include)
```

指明 MathFunctions 库的安装路径。之后同样修改根目录的 CMakeLists 文件，在末尾添加下面几行：

```cmake
# 指定安装路径
install (TARGETS Demo DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/config.h"
         DESTINATION include)
```

通过上面的定制，生成的 Demo 文件和 MathFunctions 函数库 libMathFunctions.o 文件将会被复制到 `/usr/local/bin` 中，而 MathFunctions.h 和生成的 config.h 文件则会被复制到 `/usr/local/include` 中。我们可以验证一下（顺带一提的是，这里的 `/usr/local/` 是默认安装到的根目录，可以通过修改 `CMAKE_INSTALL_PREFIX` 变量的值来指定这些文件应该拷贝到哪个根目录）：

```
[ehome@xman Demo5]$ sudo make install
[ 50%] Built target MathFunctions
[100%] Built target Demo
Install the project...
-- Install configuration: ""
-- Installing: /usr/local/bin/Demo
-- Installing: /usr/local/include/config.h
-- Installing: /usr/local/bin/libMathFunctions.a
-- Up-to-date: /usr/local/include/MathFunctions.h
[ehome@xman Demo5]$ ls /usr/local/bin
Demo  libMathFunctions.a
[ehome@xman Demo5]$ ls /usr/local/include
config.h  MathFunctions.h
```

### 为工程添加测试

添加测试同样很简单。CMake 提供了一个称为 CTest 的测试工具。我们要做的只是在项目根目录的 CMakeLists 文件中调用一系列的 `add_test` 命令。

```cmake
# 启用测试
enable_testing()

# 测试程序是否成功运行
add_test (test_run Demo 5 2)

# 测试帮助信息是否可以正常提示
add_test (test_usage Demo)
set_tests_properties (test_usage
  PROPERTIES PASS_REGULAR_EXPRESSION "Usage: .* base exponent")

# 测试 5 的平方
add_test (test_5_2 Demo 5 2)

set_tests_properties (test_5_2
 PROPERTIES PASS_REGULAR_EXPRESSION "is 25")

# 测试 10 的 5 次方
add_test (test_10_5 Demo 10 5)

set_tests_properties (test_10_5
 PROPERTIES PASS_REGULAR_EXPRESSION "is 100000")

# 测试 2 的 10 次方
add_test (test_2_10 Demo 2 10)

set_tests_properties (test_2_10
 PROPERTIES PASS_REGULAR_EXPRESSION "is 1024")
```

上面的代码包含了四个测试。第一个测试 `test_run` 用来测试程序是否成功运行并返回 0 值。剩下的三个测试分别用来测试 5 的 平方、10 的 5 次方、2 的 10 次方是否都能得到正确的结果。其中 `PASS_REGULAR_EXPRESSION` 用来测试输出是否包含后面跟着的字符串。

让我们看看测试的结果：

```
[ehome@xman Demo5]$ make test
Running tests...
Test project /home/ehome/Documents/programming/C/power/Demo5
    Start 1: test_run
1/4 Test
    Start 2: test_5_2
2/4 Test
    Start 3: test_10_5
3/4 Test
    Start 4: test_2_10
4/4 Test

100% tests passed, 0 tests failed out of 4

Total Test time (real) =   0.01 sec
```

如果要测试更多的输入数据，像上面那样一个个写测试用例未免太繁琐。这时可以通过编写宏来实现：

```
macro (do_test arg1 arg2 result)
  add_test (test_${arg1}_${arg2} Demo ${arg1} ${arg2})
  set_tests_properties (test_${arg1}_${arg2}
    PROPERTIES PASS_REGULAR_EXPRESSION ${result})
endmacro (do_test)


do_test (5 2 "is 25")
do_test (10 5 "is 100000")
do_test (2 10 "is 1024")
```

关于 CTest 的更详细的用法可以通过 `man 1 ctest` 参考 CTest 的文档。

## 支持 gdb

让 CMake 支持 gdb 的设置也很容易，只需要指定 `Debug` 模式下开启 `-g` 选项：

```
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
```

之后可以直接对生成的程序使用 gdb 来调试。

## 添加环境检查

有时候可能要对系统环境做点检查，例如要使用一个平台相关的特性的时候。在这个例子中，我们检查系统是否自带 pow 函数。如果带有 pow 函数，就使用它；否则使用我们定义的 power 函数。

#### 添加 CheckFunctionExists 宏

首先在顶层 CMakeLists 文件中添加 CheckFunctionExists.cmake 宏，并调用 `check_function_exists` 命令测试链接器是否能够在链接阶段找到 `pow` 函数。

```
# 检查系统是否支持 pow 函数
include (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
check_function_exists (pow HAVE_POW)
```

将上面这段代码放在 `configure_file` 命令前。

接下来修改 `config.h.in` 文件，预定义相关的宏变量。

```c
#cmakedefine HAVE_POW
```

最后一步是修改 `main.cc` ，在代码中使用宏和函数：

```c
#ifdef HAVE_POW
    printf("Now we use the standard library. \n");
    double result = pow(base, exponent);
#else
    printf("Now we use our own Math library. \n");
    double result = power(base, exponent);
#endif
```

## 添加版本号

给项目添加和维护版本号是一个好习惯，这样有利于用户了解每个版本的维护情况，并及时了解当前所用的版本是否过时，或是否可能出现不兼容的情况。

首先修改顶层 CMakeLists 文件，在 `project` 命令之后加入如下两行：

```cmake
set (Demo_VERSION_MAJOR 1)
set (Demo_VERSION_MINOR 0)
```

分别指定当前的项目的主版本号和副版本号。

之后，为了在代码中获取版本信息，我们可以修改 `config.h.in` 文件，添加两个预定义变量：

```c
// the configured options and settings for Tutorial
#define Demo_VERSION_MAJOR @Demo_VERSION_MAJOR@
#define Demo_VERSION_MINOR @Demo_VERSION_MINOR@
```

这样就可以直接在代码中打印版本信息了：

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"
#include "math/MathFunctions.h"

int main(int argc, char *argv[])
{
    if (argc < 3){

        printf("%s Version %d.%d\n",
            argv[0],
            Demo_VERSION_MAJOR,
            Demo_VERSION_MINOR);
        printf("Usage: %s base exponent \n", argv[0]);
        return 1;
    }
    double base = atof(argv[1]);
    int exponent = atoi(argv[2]);

#if defined (HAVE_POW)
    printf("Now we use the standard library. \n");
    double result = pow(base, exponent);
#else
    printf("Now we use our own Math library. \n");
    double result = power(base, exponent);
#endif

    printf("%g ^ %d is %g\n", base, exponent, result);
    return 0;
}
```

## 生成安装包

本节将学习如何配置生成各种平台上的安装包，包括二进制安装包和源码安装包。为了完成这个任务，我们需要用到 CPack ，它同样也是由 CMake 提供的一个工具，专门用于打包。

首先在顶层的 `CMakeLists.txt` 文件尾部添加下面几行：

```cmake
# 构建一个 CPack 安装包
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE
  "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR "${Demo_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${Demo_VERSION_MINOR}")
include (CPack)
```

上面的代码做了以下几个工作：

1.  导入 InstallRequiredSystemLibraries 模块，以便之后导入 CPack 模块；
2.  设置一些 CPack 相关变量，包括版权信息和版本信息，其中版本信息用了上一节定义的版本号；
3.  导入 CPack 模块。

接下来的工作是像往常一样构建工程，并执行 `cpack` 命令。

- 生成二进制安装包：

```
cpack -C CPackConfig.cmake
```

- 生成源码安装包

```
cpack -C CPackSourceConfig.cmake
```

我们可以试一下。在生成项目后，执行 `cpack -C CPackConfig.cmake` 命令：

```
[ehome@xman Demo8]$ cpack -C CPackSourceConfig.cmake
CPack: Create package using STGZ
CPack: Install projects
CPack: - Run preinstall target for: Demo8
CPack: - Install project: Demo8
CPack: Create package
CPack: - package: /home/ehome/Documents/programming/C/power/Demo8/Demo8-1.0.1-Linux.sh generated.
CPack: Create package using TGZ
CPack: Install projects
CPack: - Run preinstall target for: Demo8
CPack: - Install project: Demo8
CPack: Create package
CPack: - package: /home/ehome/Documents/programming/C/power/Demo8/Demo8-1.0.1-Linux.tar.gz generated.
CPack: Create package using TZ
CPack: Install projects
CPack: - Run preinstall target for: Demo8
CPack: - Install project: Demo8
CPack: Create package
CPack: - package: /home/ehome/Documents/programming/C/power/Demo8/Demo8-1.0.1-Linux.tar.Z generated.
```

此时会在该目录下创建 3 个不同格式的二进制包文件：

```
[ehome@xman Demo8]$ ls Demo8-*
Demo8-1.0.1-Linux.sh  Demo8-1.0.1-Linux.tar.gz  Demo8-1.0.1-Linux.tar.Z
```

这 3 个二进制包文件所包含的内容是完全相同的。我们可以执行其中一个。此时会出现一个由 CPack 自动生成的交互式安装界面：

```
[ehome@xman Demo8]$ sh Demo8-1.0.1-Linux.sh
Demo8 Installer Version: 1.0.1, Copyright (c) Humanity
This is a self-extracting archive.
The archive will be extracted to: /home/ehome/Documents/programming/C/power/Demo8

If you want to stop extracting, please press <ctrl-C>.
The MIT License (MIT)

Copyright (c) 2013 Joseph Pan(http://hahack.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


Do you accept the license? [yN]:
y
By default the Demo8 will be installed in:
  "/home/ehome/Documents/programming/C/power/Demo8/Demo8-1.0.1-Linux"
Do you want to include the subdirectory Demo8-1.0.1-Linux?
Saying no will install in: "/home/ehome/Documents/programming/C/power/Demo8" [Yn]:
y

Using target directory: /home/ehome/Documents/programming/C/power/Demo8/Demo8-1.0.1-Linux
Extracting, please wait...

Unpacking finished successfully
```

完成后提示安装到了 Demo8-1.0.1-Linux 子目录中，我们可以进去执行该程序：

```
[ehome@xman Demo8]$ ./Demo8-1.0.1-Linux/bin/Demo 5 2
Now we use our own Math library.
5 ^ 2 is 25
```

关于 CPack 的更详细的用法可以通过 `man 1 cpack` 参考 CPack 的文档。

# 常用 CMake 命令

## 语法

### 注释

CMake 使用 `#` 进行行注释，可以放在任何位置。 使用 `#[[ ]]` 形式进行块注释。

```cmake
# 这是一个 CMakeLists.txt 文件

#[[ 这是一个 CMakeLists.txt 文件。
这是一个 CMakeLists.txt 文件
这是一个 CMakeLists.txt 文件]]
```

### 定义变量

使用 `set` 命令显式定义及赋值, 在非 `if` 语句中, 使用 `${}` 引用变量, `if` 中直接使用变量名引用; 后续的 `set` 命令会清理变量原来的值, 例如

```cmake
# SET 指令的语法是：
# [] 中的参数为可选项, 如不需要可以不写
SET(VAR [VALUE] [CACHE TYPE DOCSTRING [FORCE]])

# 定义变量 var 并赋值为 a;b;c 这样一个 string list
set(var a;b;c) <=> set(var a b c)

set(var a) # 赋值var变量的值为a

add_executable(${var}) <=> add_executable(a b c)   # 变量使用 ${xxx} 引用

# 指定使用的C++标准
# 增加-std=c++11
set(CMAKE_CXX_STANDARD 11)
# 增加-std=c++14
set(CMAKE_CXX_STANDARD 14)
# 增加-std=c++17
set(CMAKE_CXX_STANDARD 17)

# 指定可执行程序输出的路径
set(HOME /home/robin/Linux/bin)
set(EXECUTABLE_OUTPUT_PATH ${HOME}/bin)

```

### 条件语句

cmake 中的流程判断相对简单，与 c 语言接近。形式如下:

```cmake
if(expression)
  # then section.
  COMMAND1(ARGS ...)
  COMMAND2(ARGS ...)
  #...
elseif(expression2)
  # elseif section.
  COMMAND1(ARGS ...)
  COMMAND2(ARGS ...)
  #...
else(expression)
  # else section.
  COMMAND1(ARGS ...)
  COMMAND2(ARGS ...)
  #...
endif(expression)
```

其中 else 和 endif 中的表达式是可以省略的。

if 表达式可以用长表达式，优先级顺序如下：

```camke
EXISTS, COMMAND, DEFINED
EQUAL, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL, STREQUAL, STRLESS, STRLESS_EQUAL, STRGREATER, STRGREATER_EQUAL, VERSION_EQUAL, VERSION_LESS, VERSION_LESS_EQUAL, VERSION_GREATER, VERSION_GREATER_EQUAL, MATCHES
NOT,AND,OR
```

表达式为 true 的情况：

- 1, ON, YES, TRUE, Y,或者是非 0 数字。

表达式为 false 的情况：

- 0, OFF, NO, FALSE, N, IGNORE, NOTFOUND，空字符串,或者带 `-NOTFOUND` 后缀。

### 循环语句

```
foreach(loop_var arg1 arg2 ...)
  COMMAND1(ARGS ...)
  COMMAND2(ARGS ...)
  ...
endforeach(loop_var)

foreach(loop_var RANGE total)

foreach(loop_var RANGE start stop [step])

foreach(loop_var IN [LISTS [list1 [...]]]
                    [ITEMS [item1 [...]]])

while(condition)
  COMMAND1(ARGS ...)
  COMMAND2(ARGS ...)
  ...
endwhile(condition)
```

在 while 和 foreach 循环中，取变量的值请用 `${var}`。break 和 continue 的用法基本与 c 一样，放心使用。

### 日志

`message()` 命令用于在 CMake 运行时向用户显示消息。它接受一个或多个参数，作为要显示的消息。例如：

```
message("Hello, world!")
set(SRC_FILES main.cpp)
message("Source files: ${SRC_FILES}")
```

`message` 命令还有其他用途，

- 输出警告信息：`message(WARNING "This is a warning message")`
- 输出错误信息：`message(FATAL_ERROR "This is an error message")`
- 输出调试信息：`message(STATUS "This is a status message")`

## 预定义宏

下面的列表中为大家整理了一些 `CMake` 中常用的宏：

- `PROJECT_SOURCE_DIR` ：使用 cmake 命令后紧跟的目录，一般是工程的根目录。
- `PROJECT_BINARY_DIR` ：执行 cmake 命令的目录。
- `CMAKE_CURRENT_SOURCE_DIR` ：当前处理的 `CMakeLists.txt` 所在的路径。
- `CMAKE_CURRENT_BINARY_DIR` ：target 编译目录。
- `EXECUTABLE_OUTPUT_PATH` ：目标二进制可执行文件的存放位置。
- `LIBRARY_OUTPUT_PATH` ：目标链接库文件的存放位置。
- `PROJECT_NAME` ：返回通过 PROJECT 指令定义的项目名称。
- `CMAKE_BINARY_DIR` ：项目实际构建路径，假设在 `build` 目录进行的构建，那么得到的就是这个目录的路径。
- `CMAKE_C_COMPILER` ：指定 C 编译器。
- `CMAKE_CXX_COMPILER` ：指定 C++编译器
- `CMAKE_C_FLAGS` ：编译 C 文件时的选项，如 `-g` , 也可以通过 `add_definitions` ：添加编译选项。
- `CMAKE_BUILD_TYPE` ：`build` 类型(Debug, Release, ...)，可以通过 cmake 命令指定，如 `cmake -DCMAKE_BUILD_TYPE=Debug` 。

## 命令

### list

`list()` 命令用于处理列表类型的变量。它可以接受多种子命令：

`LENGTH` ：子命令 LENGTH 用于读取列表长度。
`APPEND` ：在列表尾部添加元素。
`INSERT` ：在指定位置插入元素。
`REMOVE_ITEM` ：删除指定的元素。
`JOIN` : 将列表中的元素用连接符（字符串）连接起来组成一个字符串。

### aux_source_directory

`aux_source_directory(<dir> <variable>)` 将 dir 中所有源文件（不包括头文件）保存到变量 variable 中。

### file

`file(GLOB/GLOB_RECURSE 变量名 要搜索的文件路径和文件类型)`

`GLOB`: 将指定目录下搜索到的满足条件的所有文件名生成一个列表，并将其存储到变量中。
`GLOB_RECURSE`：递归搜索指定目录，将搜索到的满足条件的文件名生成一个列表，并将其存储到变量中。

例子，搜索当前目录的 src 目录下所有的源文件，并存储到变量中：

```cmake
file(GLOB MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB MAIN_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
```

### project

project 命令指定项目编译目标。

### include_directories

`include_directories(headpath)` 指定头文件路径。

### add_library

`add_library` 命令用于定义一个库目标。它至少需要两个参数：库的名称和源文件。

CMake 支持创建静态库和动态库。默认情况下，`add_library()` 命令会创建一个静态库，也可以显示使用 `STATIC` 参数创建静态库。如果你想要创建一个动态库，你需要在命令中添加 `SHARED` 参数。

### find_library

`find_library` 命令用于查找并定位系统上的库文件。你需要提供一个变量名（用于存储找到的库的路径）和库的名称。

### target_include_directories

`target_include_directories()` 命令用于为指定的目标添加包含目录。这个命令需要至少两个参数：目标名称和要添加的目录。

### target_compile_options

`target_compile_options()` 命令用于为指定的目标设置编译选项。这个命令至少需要两个参数：目标名称和编译选项。

### target_link_libraries

`target_link_libraries()` 命令用于将指定的库链接到目标。这个命令至少需要两个参数：目标名称和库名称。

### link_libraries

`link_libraries(<static lib> [<static lib>...])` 包含库路径。

### link_directories

指定库文件路径，`link_directories` 仅对其后面的 targets 起作用

### add_definitions

添加编译宏定义。

### add_dependencies

添加编译依赖项用于确保编译目标项目前依赖项必须先构建好。

# 参考

https://www.hahack.com/codes/cmake/

https://www.zybuluo.com/khan-lau/note/254724

https://subingwen.cn/cmake/CMake-primer/index.html

https://juejin.cn/post/6844903565207535630
