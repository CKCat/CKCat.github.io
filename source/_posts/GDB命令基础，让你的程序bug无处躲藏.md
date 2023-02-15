---
title: GDB命令基础，让你的程序bug无处躲藏
date: 2020-05-05 00:23:42
tags: Debug
category: linux
---

> 转载: https://deepzz.com/post/gdb-debug.html

<!-- TOC -->

- [使用GDB](#使用gdb)
  - [启动](#启动)
  - [运行](#运行)
  - [设置/查看断点(breakpoint)](#设置查看断点breakpoint)
  - [设置/查看观察点(watchpoint)](#设置查看观察点watchpoint)
  - [设置/查看捕捉点(catchpoint)](#设置查看捕捉点catchpoint)
  - [维护停止点](#维护停止点)
  - [维护条件停止点](#维护条件停止点)
  - [停止点设置运行命令](#停止点设置运行命令)
  - [断点菜单](#断点菜单)
  - [恢复程序运行和单步调试](#恢复程序运行和单步调试)
  - [信号](#信号)
  - [产生信号量](#产生信号量)
  - [线程](#线程)
  - [查看栈信息](#查看栈信息)
  - [查看源码](#查看源码)
  - [搜索源代码](#搜索源代码)
  - [指定源文件路径](#指定源文件路径)
  - [源代码内存](#源代码内存)
  - [查看运行时数据](#查看运行时数据)
  - [查看内存](#查看内存)
  - [自动显示](#自动显示)
  - [设置显示选项](#设置显示选项)
  - [历史纪录](#历史纪录)
  - [环境变量](#环境变量)
  - [查看寄存器](#查看寄存器)
  - [修改程序的执行](#修改程序的执行)
  - [跳转执行](#跳转执行)
  - [强制函数返回](#强制函数返回)
  - [强制调用函数](#强制调用函数)
  - [GDB语言环境](#gdb语言环境)
  - [退出](#退出)
  - [技巧](#技巧)

<!-- /TOC -->

软件开发，或多或少会走上调试这条路。调试工具可以帮你更加深入了解整个程序的运行状态，对程序运行有更多的主动权。你可以随心所欲的改变程序运行流程，如：有变量a，你需要不断改变该a的值，让程序运行出理想的结果，那么你可以在调试工具中轻易的实现。

* 运行你的程序，设置所有的能影响程序运行的东西。
* 保证你的程序在指定的条件下停止。
* 当你程序停止时，让你检查发生了什么。
* 改变你的程序。那样你可以试着修正某个bug引起的问题，然后继续查找另一 个bug

今天给大家带来的gdb调试工具。GDB是GNU开源组织发布的一个强大的UNIX下的程序调试工具。GDB可以调试C、C++、D、Go、python、pascal、assemble(ANSI 汇编标准)等等语言。

# 使用GDB

## 启动
```
$ gdb program           # program是你的可执行文件，一般在当前目录
$ gdb program core      # gdb同时调试运行程序和core文件，core是程序非法执行产生的文件
$ gdb program pid       # 如果你的程序是一个服务程序，那么你可以指定这个服务程序运行时的进程ID。gdb会自动attach上去，并调试他。program应该在PATH环境变量中搜索得到。
```

## 运行
```
(gdb) r/run             # 开始运行程序
(gdb) c/continue        # 继续运行
(gdb) n/next            # 下一行，不进入函数调用
(gdb) s/step            # 下一行，进入函数调用
(gdb) ni/si             # 下一条指令，ni和si区别同上
(gdb) fini/finish       # 继续运行至函数退出/当前栈帧
(gdb) u/util            # 继续运行至某一行，在循环中，u可以实现运行至循环刚刚退出，但这取决于循环的实现

(gdb) set args          # 设置程序启动参数，如：set args 10 20 30
(gdb) show args         # 查看程序启动参数
(gdb) path <dir>        # 设置程序的运行路径
(gdb) show paths        # 查看程序的运行路径
(gdb) set env <name=val># 设置环境变量，如：set env USER=chen
(gdb) show env [name]   # 查看环境变量
(gdb) cd <dir>          # 相当于shell的cd命令
(gdb) pwd               # 显示当前所在目录

(gdb) shell <commond>   # 执行shell命令
```

## 设置/查看断点(breakpoint)
```
(gdb) b/break linenum/func      # 在第linenum行或function处停住
(gdb) b/break +/-offset         # 在当前行号后/前offset行停住
(gdb) b/break filename:linenum  # 在源文件filename的linenum行停住
(gdb) b/break filename:func     # 在源文件的function入口停住
(gdb) b/break *address          # 在内存地址address处停住
(gdb) b/break                   # 没有参数，表示下一跳指令处停住
(gdb) b/break if <condition>    # 条件成立是停住，如在循环中：break if i=100

(gdb) info break [n]            # 查看断点， n表示断点号
```

## 设置/查看观察点(watchpoint)
```
# 观察点一搬来观察某个表达式或变量的值是否有变化了，有：程序停住
(gdb) watch <expr>              # 观察值是否有变化
(gdb) rwatch <expr>             # 当expr被读取时，停住
(gdb) awatch <expr>             # 当expr被读取或写入时，停住

(gdb) info watchpoints          # 查看所有观察点
```

## 设置/查看捕捉点(catchpoint)
```
# 你可设置捕捉点来补捉程序运行时的一些事件。如：载入共享库（动态链接库）或是C++的异常。
(gdb) tcatch <event>            # 只设置一次捕捉点
(gdb) catch <event>             # 当event发生时，停住程序，如下：
# throw             一个c++抛出的异常（throw为关键字）
# catch             一个C++捕捉到的异常（catch为关键字）
# exec              调用系统调用exec时（只在HP-UX下有用）
# fork              调用系统调用fork时（只在HP-UX下有用）
# vfork             调用系统调用vfork时（只在HP-UX下有用）
# load [file]       载入共享库（动态链接库）时（只在HP-UX下有用）
# unload [libname]  卸载共享库（动态链接库）时（只在HP-UX下有用）

```

## 维护停止点
```
# 上面说了三种如何设置停止点的方法。在gdb中如果你觉得已经定义好的停止点没有用，那么你可以delete、clear、disable、enable进行维护
(gdb) clear                     # 清除所有已定义的停止点。如果程序运行，清除当前行之后的
(gdb) clear <fuction>           # 清除所有设置在函数上的停止点
(gdb) clear <file:line>         # 清除所有设置在指定行上的停止点
(gdb) d/delete [n]/[m-n]        # 删除断点号，不设置则删除全部，也可以范围m-n

# 比删除更好的一种方法是disable停止点，disable了的停止点，GDB不会删除，当你还需要时，enable即可，就好像回收站一样。
(gdb) disable [n]/[m-n]         # disable指定断点号n，不指定则disable所有，也可以范围m-n

(gdb) enable [n]/[m-n]          # enable断点n，也可以范围m-n
(gdb) enable once [n]/[m-n]     # enable断点n一次，程序停止后自动disable，也可以范围m-n
(gdb) enable delete [n]/[m-n]   # enable断点，程序结束自动删除，也可以范围m-n
```

## 维护条件停止点
```
# 前面说到设置breakpoint可以设置成一个条件，这里列出相关的维护命令
(gdb) condition <bunm> <expr>   # 修改断掉号bnum的停止条件
(gdb) condition <bnum>          # 清除断点号bnum的停止条件

# ignore 可以指定程序运行时，忽略停止条件几次
(gdb) ignore <bnum> <count>     # 忽略断点号hnum的停止条件count次
```

## 停止点设置运行命令
```
# 当程序停住时，我们可以通过command设置其自动执行的命令，这很利于自动化调试。
(gdb) commands [bnum]
> ... commands list ...
> end                   # 这里为断点号bnum设置一个命令列表

如：
(gdb) break foo if x>0
(gdb) commands
> printf "x is %dn",x
> continue
> end
# 断点设置在函数foo中，断点条件是x>0，如果程序被断住后，也就是，一旦x的值在foo函数中大于0，GDB会自动打印出x的值，并继续运行程序。
# 如果你要清除断点上的命令序列，那么只要简单的执行一下commands命令，并直接在打个end就行了。
```

## 断点菜单
```
# 如果你使用c++，有可能下断点时遇到相同名字的函数，gdb会为你列出该函数菜单供你选择。
如：
(gdb) b String::after
[0] cancel
[1] all
[2] file:String.cc; line number:867
[3] file:String.cc; line number:860
[4] file:String.cc; line number:875
[5] file:String.cc; line number:853
[6] file:String.cc; line number:846
[7] file
> 2 4 6
Breakpoint 1 at 0xb26c: file String.cc, line 867.
Breakpoint 2 at 0xb344: file String.cc, line 875.
Breakpoint 3 at 0xafcc: file String.cc, line 846.
```

## 恢复程序运行和单步调试
```
# 当程序被停住了，你可以用c/continue恢复运行，或下一个断点到来。也可以使用step或next命令单步跟踪程序。
(gdb) c/continue [ignore-count]     # 恢复程序运行，ignore-count忽略后面断点数

# 单步跟踪，如果有函数调用，他会进入该函数。进入函数的前提是，此函数被编译有debug信息。很像VC等工具中的stepin。后面可以加count也可以不加，不加表示一条条地执行，加表示执行后面的count条指令，然后再停住。
(gdb) step <count>

# 打开step-mode模式，于是，在进行单步跟踪时，程序不会因为没有debug信息而不停住。这个参数有很利于查看机器码。
(gdb) set step-mode on

# 当你厌倦了在一个循环体内单步跟踪时，这个命令可以运行程序直到退出循环体。
(gdb) u/until

# 单步跟踪一条机器指令！一条程序代码有可能由数条机器指令完成，stepi和nexti可以单步执行机器指令。与之一样有相同功能的命令是“display/i $pc” ，当运行完这个命令后，单步跟踪会在打出程序代码的同时打出机器指令（也就是汇编代码）
(gdb) si/stepi
(gdb) ni/stepi
```

## 信号
```
# 信号是一种软中断，是一种处理异步事件的方法。一般来说，操作系统都支持许多信号。尤其是UNIX，比较重要应用程序一般都会处理信号。UNIX定义了许多信号，比如SIGINT表示中断字符信号，也就是Ctrl+C的信号，SIGBUS表示硬件故障的信号；SIGCHLD表示子进程状态改变信号；SIGKILL表示终止程序运行的信号，等等。信号量编程是UNIX下非常重要的一种技术。
# GDB有能力在你调试程序的时候处理任何一种信号，你可以告诉GDB需要处理哪一种信号。你可以要求GDB收到你所指定的信号时，马上停住正在运行的程序，以供你进行调试。你可以用GDB的handle命令来完成这一功能。
(gdb) handle <signal> <keywords...>
# 在GDB中定义一个信号处理。信号<signal>可以以SIG开头或不以SIG开头，可以用定义一个要处理信号的范围（如：SIGIO-SIGKILL，表示处理从SIGIO信号到SIGKILL的信号，其中包括SIGIO，SIGIOT，SIGKILL三个信号），也可以使用关键字all来标明要处理所有的信号。一旦被调试的程序接收到信号，运行程序马上会被GDB停住，以供调试。其<keywords>可以是以下几种关键字的一个或多个。
  nostop            # 当被调试的程序收到信号时，GDB不会停住程序的运行，但会打出消息告诉你收到这种信号。
  stop          # 当被调试的程序收到信号时，GDB会停住你的程序。
  print         # 当被调试的程序收到信号时，GDB会显示出一条信息。
  noprint           # 当被调试的程序收到信号时，GDB不会告诉你收到信号的信息。
  pass/noignore # 当被调试的程序收到信号时，GDB不处理信号。这表示，GDB会把这个信号交给被调试程序会处理。
  nopass/ignore # 当被调试的程序收到信号时，GDB不会让被调试程序来处理这个信号。

# 查看有哪些信号在被GDB检测中。
(gdb) info signals
(gdb) info handle
```

## 产生信号量
```
# 使用singal命令，可以产生一个信号量给被调试的程序。如：中断信号Ctrl+C。这非常方便于程序的调试，可以在程序运行的任意位置设置断点，并在该断点用GDB产生一个信号量，这种精确地在某处产生信号非常有利程序的调试。语法是：
(gdb) signal <singal>
# UNIX的系统信号量通常从1到15。所以<singal>取值也在这个范围。
# single命令和shell的kill命令不同，系统的kill命令发信号给被调试程序时，是由GDB截获的，而single命令所发出一信号则是直接发给被调试程序的。
```

## 线程
```
# 当你的程序时多线程的，你可以定义断点是否在所有线程或某个线程
(gdb) info threads                          # 查看线程
(gdb) break <line> thread <threadno>        # 指定源程序line行，线程threadno停住
(gdb) break <line> thread <threadno> if...  # 指定源程序line行，线程threadno停住，跟上条件

如：
(gdb) break frik.c:13 thread 28 if bartab > lim
```

## 查看栈信息
```
# 当程序被停住了，你需要做的第一件事就是查看程序是在哪里停住的。当你的程序调用了一个函数，函数的地址，函数参数，函数内的局部变量都会被压入“栈”（Stack）中。你可以用GDB命令来查看当前的栈中的信息。
(gdb) bt/backtrace          # 打印当前的啊还能输调用栈的所有信息
(gdb) bt/backtrace <n>      # 当n为正数，打印栈顶n层。为负数，打印栈低n层

# 如果你要查看某一层的信息，你需要在切换当前的栈，一般来说，程序停止时，最顶层的栈就是当前栈，如果你要查看栈下面层的详细信息，首先要做的是切换当前栈。
(gdb) f/frame <n>           # n从0开始，是栈中的编号
(gdb) up <n>                # 向栈的上面移动n层。如无n，向上移动一层
(gdb) down <n>              # 向栈的下面移动n层。如无n，向下移动一层

# 栈的层编号，当前的函数名，函数参数值，函数所在文件及行号，函数执行到的语句。
(gdb) f/frame

# 这个命令会打印出更为详细的当前栈层的信息，只不过，大多数都是运行时的内内地址。比如：函数地址，调用函数的地址，被调用函数的地址，目前的函数是由什么样的程序语言写成的、函数参数地址及值、局部变量的地址等等。
(gdb) info f/frame

(gdb) info args             # 打印当前函数的参数名及值
(gdb) info locals           # 打印当前函数中所有局部变量及值
(gdb) info catch            # 打印当前函数中的异常处理信息
```

## 查看源码
```
# 在程序编译时一定要加上-g的参数，把源程序信息编译到执行文件中。不然就看不到源程序了。
(gdb) l                         # 显示当前行前后的源码
(gdb) l -                       # 显示当前行前的源码
(gdb) l +                       # 显示当前行后的源码
(gdb) l/list <linuenum/func>    # 查看第linenum行或者function所在行附近的10行
(gdb) l/list                    # 查看上一次list命令列出的代码后面的10行
(gdb) l/list m,n                # 查看从第m行到第n行的源码。m不填，则从当前行到n
(gdb) l/list -/+offset          # 查看想对当前行偏移offset源码
(gdb) l/list <file:line>        # 查看文件file的line行的源码
(gdb) l/list <func>             # 查看函数名func源码
(gdb) l/list <file:func>        # 查看文件file的函数func源码
(gdb) l/list <*address>         # 查看运行时内存地址address的源码

(gdb) set listsize              # 设置一次显示源码的行数
(gdb) show listsize             # 查看listsize的值
```

## 搜索源代码
```
(gdb) forward-search <regexp>   # 向前搜索，regexp是一个正则表达式
(gdb) search <regexp>           # 向前搜索
(gdb) reverse-search <regexp>   # 全局搜索
```

## 指定源文件路径
```
# 某些时候，用-g编译过后的执行程序中只是包括了源文件的名字，没有路径名。GDB提供了可以让你指定源文件的路径的命令，以便GDB进行搜索。
(gdb) dir/directory <dirname ... >  # 加一个源文件路径到当前路径的前面。如果你要指定多个路径，UNIX下你可以使用“:”，Windows下你可以使用“;”。
(gdb) dir/directory                 # 清除所有的自定义的源文件搜索路径信息。
(gdb) show directories              # 显示定义了的源文件搜索路径。
```

## 源代码内存
```
(gdb) info line                     # 查看源代码在内存中的地址，还可以:
(gdb) info line <num>
(gdb) info line <file:num>
(gdb) info line <func>
(gdb) info line <file:func>

# 还有一个命令（disassemble）你可以查看源程序的当前执行时的机器码，这个命令会把目前内存中的指令dump出来。如下面的示例表示查看函数func的汇编代码。
(gdb) disassemble func
```

## 查看运行时数据
```
# <expr>是表达式，是你所调试的程序的语言的表达式（GDB可以调试多种编程语言），<f>是输出的格式，比如，如果要把表达式按16进制的格式输出，那么就是/x。
(gdb) p/print <expr>            # expr可以是const常量、变量、函数等内容
(gdb) p/print /<f> <expr>

# 在表达式中，有几种GDB所支持的操作符，它们可以用在任何一种语言中。
    @                   是一个和数组有关的操作符，在后面会有更详细的说明。
    ::                  指定一个在文件或是一个函数中的变量。
    {<type>} <addr>     表示一个指向内存地址<addr>的类型为type的一个对象。
# 需要注意的是，如果你的程序编译时开启了优化选项，那么在用GDB调试被优化过的程序时，可能会发生某些变量不能访问，或是取值错误码的情况。

# 输出格式
# 一般来说，GDB会根据变量的类型输出变量的值。但你也可以自定义GDB的输出的格式。例如，你想输出一个整数的十六进制，或是二进制来查看这个整型变量的中的位的情况。要做到这样，你可以使用GDB的数据显示格式：
    x 按十六进制格式显示变量。
    d 按十进制格式显示变量。
    u 按十六进制格式显示无符号整型。
    o 按八进制格式显示变量。
    t 按二进制格式显示变量。
    a 按十六进制格式显示变量。 
    c 按字符格式显示变量。 
    f 按浮点数格式显示变量。
  
如：
(gdb) p i
$21 = 101
(gdb) p/a i
$22 = 0x65
```

## 查看内存
```
# 你可以使用examine命令（简写是x）来查看内存地址中的值。x命令的语法如下所示：
(gdb) x/<n/f/u> <addr>          # n, f, u可选

    n       是一个正整数，表示显示内存的长度，也就是说从当前地址向后显示几个地址的内容。
    f       表示显示的格式，参见上面。如果地址所指的是字符串，那么格式可以是s，如果地址是指令地址，那么格式可以是i。
    u       表示从当前地址往后请求的字节数，如果不指定的话，GDB默认是4个bytes。u参数可以用下面的字符来代替，b表示单字节，h表示双字节，w表示四字节，g表示八字节。当我们指定了字节长度后，GDB会从指内存定的内存地址开始，读写指定字节，并把其当作一个值取出来。
    <addr>  表示一个内存地址。

# n/f/u三个参数可以一起使用。例如：
(gdb) x/3uh 0x54320             #表示，从内存地址0x54320读取内容，h表示以双字节为一个单位，3表示三个单位，u表示按十六进制显示。
```

## 自动显示
```
# 你可以设置一些自动显示的变量，当程序停住时，或是在你单步跟踪时，这些变量会自动显示。相关的GDB命令是display。
(gdb) display <expr>
(gdb) display/<fmt> <expr>
(gdb) display/<fmt> <addr>
# expr是一个表达式，fmt表示显示的格式，addr表示内存地址，当你用display设定好了一个或多个表达式后，只要你的程序被停下来，GDB会自动显示你所设置的这些表达式的值。

# 格式i和s同样被display支持，一个非常有用的命令是：
(gdb) display/i $pc
# $pc是GDB的环境变量，表示着指令的地址，/i则表示输出格式为机器指令码，也就是汇编。于是当程序停下后，就会出现源代码和机器指令码相对应的情形，这是一个很有意思的功能。
下面是一些和display相关的GDB命令：
(gdb) undisplay <dnums...>
(gdb) delete display <dnums...>
# 删除自动显示，dnums意为所设置好了的自动显式的编号。如果要同时删除几个，编号可以用空格分隔，如果要删除一个
# 范围内的编号，可以用减号表示（如：2-5）
(gdb) disable display <dnums...>
(gdb) enable display <dnums...>
# disable和enalbe不删除自动显示的设置，而只是让其失效和恢复。
(gdb) info display
# 查看display设置的自动显示的信息。GDB会打出一张表格，向你报告当然调试中设置了多少个自动显示设置，其中包括，设置的编号，表达式，是否enable。
```

## 设置显示选项
```
# GDB中关于显示的选项比较多，这里我只例举大多数常用的选项。
# 1、打开地址输出，当程序显示函数信息时，GDB会显出函数的参数地址。系统默认为打开的
(gdb) set print address
(gdb) set print address on 
(gdb) set print address off     # 关闭函数的参数地址显示
(gdb) show print address        # 查看当前地址显示选项是否打开。

# 2、打开数组显示，打开后当数组显示时，每个元素占一行，如果不打开的话，每个元素则以逗号分隔。这个选项默认是关闭的。
(gdb) set print array
(gdb) set print array on 
(gdb) set print array off
(gdb) show print array
(gdb) show print elements       # 查看print elements的选项信息。
(gdb) set print elements <number-of-elements>
# 这个选项主要是设置数组的，如果你的数组太大了，那么就可以指定一个<number-of-elements>来指定数据显示的最大长度，当到达这个长度时，GDB就不再往下显示了。如果设置为0，则表示不限制。

# 3、如果打开了这个选项，那么当显示字符串时，遇到结束符则停止显示。这个选项默认为off
(gdb) set print null-stop <on/off>  

# 4、如果打开printf pretty这个选项，那么当GDB显示结构体时会比较漂亮。
(gdb) set print pretty on 
(gdb) show print pretty         # 查看GDB是如何显示结构体的。\

# 5、设置字符显示，是否按“nnn”的格式显示，如果打开，则字符串或字符数据按nnn显示，如“65”。
(gdb) set print sevenbit-strings <on/off>
(gdb) show print sevenbit-strings   # 查看字符显示开关是否打开。

# 6、设置显示结构体时，是否显式其内的联合体数据。
(gdb) set print union <on/off>
(gdb) show print union              # 查看联合体数据的显示方式
如：
$1 = {it = Tree, form = {tree = Acorn, bug = Cocoon}}   # 开
$1 = {it = Tree, form = {...}}                          # 关

# 7、在C++中，如果一个对象指针指向其派生类，如果打开这个选项，GDB会自动按照虚方法调用的规则显示输出，如果关闭这个选项的话，GDB就不管虚函数表了。这个选项默认是off。
(gdb) set print object <on/off>
(gdb) show print object             # 查看对象选项的设置。

# 8、这个选项表示，当显示一个C++对象中的内容是，是否显示其中的静态数据成员。默认是on。
(gdb) set print static-members <on/off>
(gdb) show print static-members     # 查看静态数据成员选项设置。

# 9、当此选项打开时，GDB将用比较规整的格式来显示虚函数表时。其默认是关闭的。
(gdb) set print vtbl <on/off>
(gdb) show print vtbl               # 查看虚函数显示格式的选项。
```

## 历史纪录
```
当你用GDB的print查看程序运行时的数据时，你每一个print都会被GDB记录下来。GDB会以$1, $2, $3 .....这样的方式为你每一个print命令编上号。于是，你可以使用这个编号访问以前的表达式，如$1。这个功能所带来的好处是，如果你先前输入了一个比较长的表达式，如果你还想查看这个表达式的值，你可以使用历史记录来访问，省去了重复输入。
```
## 环境变量
```
# 你可以在GDB的调试环境中定义自己的变量，用来保存一些调试程序中的运行数据。要定义一个GDB的变量很简单只需。 使用GDB的set命令。GDB的环境变量和UNIX一样，也是以$起头。如：
(gdb) set $foo = *object_ptr
# 使用环境变量时，GDB会在你第一次使用时创建这个变量，而在以后的使用中，则直接对其賦值。环境变量没有类型，你可以给环境变量定义任一的类型。包括结构体和数组。
(gdb) show convenience
# 该命令查看当前所设置的所有的环境变量。这是一个比较强大的功能，环境变量和程序变量的交互使用，将使得程序调试更为灵活便捷。例如：
(gdb) set $i = 0
(gdb) print bar[$i++]->contents
# 于是，当你就不必，print bar[0]->contents, print bar[1]->contents地输入命令了。输入这样的命令后，只用敲回车，重复执行上一条语句，环境变量会自动累加，从而完成逐个输出的功能。
```

## 查看寄存器
```
# 寄存器中放置了程序运行时的数据，比如程序当前运行的指令地址（ip），程序的当前堆栈地址（sp）等等。你同样可以使用print命令来访问寄存器的情况，只需要在寄存器名字前加一个$符号就可以了。如：p $eip。
(gdb) info registers        # 查看寄存器状态(除浮点寄存器)
(gdb) info all-registers    # 查看所有寄存器状态
(gdb) info registers regname# 查看指定寄存器状态，如：info rbp
```

## 修改程序的执行
```
# 一旦使用GDB挂上被调试程序，当程序运行起来后，你可以根据自己的调试思路来动态地在GDB中更改当前被调试程序的运行线路或是其变量的值，这个强大的功能能够让你更好的调试你的程序，比如，你可以在程序的一次运行中走遍程序的所有分支。

# 一、修改变量值
# 修改被调试程序运行时的变量值，在GDB中很容易实现，使用GDB的print命令即可完成。如：
(gdb) print x=4
# x=4这个表达式是C/C++的语法，意为把变量x的值修改为4，如果你当前调试的语言是Pascal，那么你可以使用Pascal的语法：x:=4。
# 在某些时候，很有可能你的变量和GDB中的参数冲突，如：
(gdb) whatis width
type = double
(gdb) p width
$4 = 13
(gdb) set width=47
Invalid syntax in expression.
# 因为，set width是GDB的命令，所以，出现了“Invalid syntax in expression”的设置错误，此时，你可以使用set
# var命令来告诉GDB，width不是你GDB的参数，而是程序的变量名，如：
(gdb) set var width=47
# 另外，还可能有些情况，GDB并不报告这种错误，所以保险起见，在你改变程序变量取值时，最好都使用set var格式的GDB命令。
```

## 跳转执行
```
# 一般来说，被调试程序会按照程序代码的运行顺序依次执行。GDB提供了乱序执行的功能，也就是说，GDB可以修改程序的执行顺序，可以让程序执行随意跳跃。这个功能可以由GDB的jump命令来完：
(gdb) jump <linespec>
# 指定下一条语句的运行点。<linespce>可以是文件的行号，可以是file:line格式，可以是+num这种偏移量格式。表式着下一条运行语句从哪里开始。
(gdb) jump <address>
# 这里的<address>是代码行的内存地址。注意，jump命令不会改变当前的程序栈中的内容，所以，当你从一个函数跳到另一个函数时，当函数运行完返回时进行弹栈操作时必然会发生错误，可能结果还是非常奇怪的，甚至于产生程序Core Dump。所以最好是同一个函数中进行跳转。
# 熟悉汇编的人都知道，程序运行时，有一个寄存器用于保存当前代码所在的内存地址。所以，jump命令也就是改变了这个寄存器中的值。于是，你可以使用“set $pc”来更改跳转执行的地址。如：
(gdb) set $pc = 0x485
```

## 强制函数返回
```
# 如果你的调试断点在某个函数中，并还有语句没有执行完。你可以使用return命令强制函数忽略还没有执行的语句并返回。
(gdb) return
(gdb) return <expression>
# 使用return命令取消当前函数的执行，并立即返回，如果指定了<expression>，那么该表达式的值会被认作函数的返回值。
```

## 强制调用函数
```
(gdb) call <expr>
# 表达式中可以一是函数，以此达到强制调用函数的目的。并显示函数的返回值，如果函数返回值是void，那么就不显示。
# 另一个相似的命令也可以完成这一功能——print，print后面可以跟表达式，所以也可以用他来调用函数，print和call的不同是，如果函数返回void，call则不显示，print则显示函数返回值，并把该值存入历史数据中。
```

## GDB语言环境
```
(gdb) show language
# 查看当前的语言环境。如果GDB不能识为你所调试的编程语言，那么，C语言被认为是默认的环境。
(gdb) info frame
# 查看当前函数的程序语言。
(gdb) info source
# 查看当前文件的程序语言。如果GDB没有检测出当前的程序语言，那么你也可以手动设置当前的程序语言。使用set language命令即可做到。当set language命令后什么也不跟的话，你可以查看GDB所支持的语言种类：
(gdb) set language
The currently understood settings are:
local or auto Automatic setting based on source file
c Use the C language
c++ Use the C++ language
asm Use the Asm language
chill Use the Chill language
fortran Use the Fortran language
java Use the Java language
modula-2 Use the Modula-2 language
pascal Use the Pascal language
scheme Use the Scheme language
# 于是你可以在set language后跟上被列出来的程序语言名，来设置当前的语言环境。
```

## 退出
```
(gdb) q/quit                # 退出GDB调试
```

## 技巧
```
(gdb) b                     # 敲入b按两次TAB键
backtrace break bt
```
