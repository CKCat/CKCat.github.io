---
title: Linux内核模块编程指南
date: 2023-09-18 18:04:17
tags: linux 内核
category: linux
---



本文内容来自：https://sysprog21.github.io/lkmpg/ ，使用 [Immersive Translate](https://github.com/immersive-translate/immersive-translate) 进行翻译。

### 简介

The Linux Kernel Module Programming Guide is a free book; you may reproduce and/or modify it under the terms of the [Open Software License](https://opensource.org/licenses/OSL-3.0), version 3.0.
《Linux 内核模块编程指南》是一本免费书籍；您可以根据[开放软件许可证](https://opensource.org/licenses/OSL-3.0) 3.0 版的条款复制和/或修改它。

This book is distributed in the hope that it would be useful, but without any warranty, without even the implied warranty of merchantability or fitness for a particular purpose.
分发本书的目的是希望它有用，但没有任何保证，甚至没有对适销性或针对特定用途的适用性的默示保证。

The author encourages wide distribution of this book for personal or commercial use, provided the above copyright notice remains intact and the method adheres to the provisions of the [Open Software License](https://opensource.org/licenses/OSL-3.0). In summary, you may copy and distribute this book free of charge or for a profit. No explicit permission is required from the author for reproduction of this book in any medium, physical or electronic.
作者鼓励将本书广泛分发用于个人或商业用途，前提是上述版权声明保持完整并且方法遵守[开放软件许可证](https://opensource.org/licenses/OSL-3.0)的规定。总之，您可以免费或以营利为目的复制和分发本书。以任何物理或电子媒介复制本书无需获得作者的明确许可。

Derivative works and translations of this document must be placed under the [Open Software License](https://opensource.org/licenses/OSL-3.0), and the original copyright notice must remain intact. If you have contributed new material to this book, you must make the material and source code available for your revisions. Please make revisions and updates available directly to the document maintainer, Jim Huang <jserv@ccns.ncku.edu.tw>. This will allow for the merging of updates and provide consistent revisions to the Linux community.
本文档的衍生作品和翻译必须置于[开放软件许可证](https://opensource.org/licenses/OSL-3.0)之下，并且原始版权声明必须保持完整。如果您为本书贡献了新材料，则必须提供这些材料和源代码以供修订。请直接向文档维护者 Jim Huang 提供修订和更新。这将允许合并更新并为 Linux 社区提供一致的修订。

If you publish or distribute this book commercially, donations, royalties, and/or printed copies are greatly appreciated by the author and the [Linux Documentation Project](https://tldp.org/) (LDP). Contributing in this way shows your support for free software and the LDP. If you have questions or comments, please contact the address above.
如果您以商业方式出版或分发本书，作者和 [Linux 文档项目](https://tldp.org/) (LDP) 将非常感谢您的捐赠、版税和/或印刷版。以这种方式做出贡献表明您对自由软件和自民党的支持。如果您有疑问或意见，请联系上述地址。

#### Authorship 作者身份

The Linux Kernel Module Programming Guide was initially authored by Ori Pomerantz for Linux v2.2. As the Linux kernel evolved, Ori’s availability to maintain the document diminished. Consequently, Peter Jay Salzman assumed the role of maintainer and updated the guide for Linux v2.4. Similar constraints arose for Peter when tracking developments in Linux v2.6, leading to Michael Burian joining as a co-maintainer to bring the guide up to speed with Linux v2.6. Bob Mottram contributed to the guide by updating examples for Linux v3.8 and later. Jim Huang then undertook the task of updating the guide for recent Linux versions (v5.0 and beyond), along with revising the LaTeX document.
Linux 内核模块编程指南最初由 Ori Pomerantz 针对 Linux v2.2 编写。随着 Linux 内核的发展，Ori 维护文档的可用性逐渐减弱。因此，Peter Jay Salzman 承担了维护者的角色并更新了 Linux v2.4 的指南。 Peter 在跟踪 Linux v2.6 的开发时也遇到了类似的限制，导致 Michael Burian 作为共同维护者加入，以加快该指南与 Linux v2.6 的速度。 Bob Mottram 通过更新 Linux v3.8 及更高版本的示例为本指南做出了贡献。 Jim Huang 随后承担了更新最新 Linux 版本（v5.0 及更高版本）指南以及修订 LaTeX 文档的任务。

#### Acknowledgements 致谢

The following people have contributed corrections or good suggestions:
以下人员提供了更正或好的建议：

Andy Shevchenko, Arush Sharma, Benno Bielmeier, Bob Lee, Brad Baker, Che-Chia Chang, Chih-En Lin, Chih-Hsuan Yang, Chih-Yu Chen, Ching-Hua (Vivian) Lin, Chin Yik Ming, Cyril Brulebois, Daniele Paolo Scarpazza, David Porter, demonsome, Dimo Velev, Ekang Monyet, Ethan Chan, fennecJ, Francois Audeon, Gilad Reti, Horst Schirmeier, Hsin-Hsiang Peng, Ignacio Martin, Iûnn Kiàn-îng, Jian-Xing Wu, Johan Calle, keytouch, Kohei Otsuka, manbing, Marconi Jiang, mengxinayan, Peter Lin, Roman Lakeev, Sam Erickson, Shao-Tse Hung, Stacy Prowell, Steven Lung, Tristan Lelong, Tse-Wei Lin, Tucker Polomik, Tyler Fanelli, VxTeemo, Wei-Lun Tsai, Xatierlike Lee, Yin-Chiuan Chen, Yi-Wei Lin, Ylowy, Yu-Hsiang Tseng.

#### What Is A Kernel Module? 什么是内核模块？

Involvement in the development of Linux kernel modules requires a foundation in the C programming language and a track record of creating conventional programs intended for process execution. This pursuit delves into a domain where an unregulated pointer, if disregarded, may potentially trigger the total elimination of an entire file system, resulting in a scenario that necessitates a complete system reboot.
参与 Linux 内核模块的开发需要具备 C 编程语言基础以及创建用于进程执行的传统程序的记录。这种追求深入研究了一个领域，其中不受监管的指针如果被忽视，可能会触发整个文件系统的彻底消除，从而导致需要完全重新启动系统的情况。

A Linux kernel module is precisely defined as a code segment capable of dynamic loading and unloading within the kernel as needed. These modules enhance kernel capabilities without necessitating a system reboot. A notable example is seen in the device driver module, which facilitates kernel interaction with hardware components linked to the system. In the absence of modules, the prevailing approach leans toward monolithic kernels, requiring direct integration of new functionalities into the kernel image. This approach leads to larger kernels and necessitates kernel rebuilding and subsequent system rebooting when new functionalities are desired.
Linux 内核模块被精确地定义为能够根据需要在内核内动态加载和卸载的代码段。这些模块增强了内核功能，而无需重新启动系统。一个值得注意的例子是设备驱动程序模块，它促进内核与链接到系统的硬件组件的交互。在没有模块的情况下，流行的方法倾向于整体内核，需要将新功能直接集成到内核映像中。这种方法会导致内核更大，并且在需要新功能时需要重建内核并随后重新启动系统。

#### Kernel module package

1.4 内核模块包

Linux distributions provide the commands modprobe , insmod and depmod within a package.
Linux 发行版在包中提供了命令 modprobe 、 insmod 和 depmod 。

On Ubuntu/Debian GNU/Linux:
在 Ubuntu/Debian GNU/Linux 上：

```bash
sudo apt-get install build-essential kmod
```

On Arch Linux:
在 Arch Linux 上：

```bash
sudo pacman -S gcc kmod
```

#### What Modules are in my Kernel? 我的内核中有哪些模块？

To discover what modules are already loaded within your current kernel use the command lsmod .
要发现当前内核中已加载哪些模块，请使用命令 lsmod 。

```bash
sudo lsmod
```

Modules are stored within the file `/proc/modules`, so you can also see them with:
模块存储在文件 `/proc/modules` 中，因此您还可以使用以下命令查看它们：

```bash
sudo cat /proc/modules
```

This can be a long list, and you might prefer to search for something particular. To search for the fat module:
这可能是一个很长的列表，您可能更喜欢搜索特定的内容。搜索 fat 模块：

```bash
sudo lsmod | grep fat
```

#### Is there a need to download and compile the kernel? 是否需要下载编译内核？

To effectively follow this guide, there is no obligatory requirement for performing such actions. Nonetheless, a prudent approach involves executing the examples within a test distribution on a virtual machine, thus mitigating any potential risk of disrupting the system.
为了有效遵循本指南，执行此类操作没有强制性要求。尽管如此，谨慎的方法是在虚拟机上执行测试发行版中的示例，从而减轻破坏系统的任何潜在风险。

#### Before We Begin 开始之前

Before delving into code, certain matters require attention. Variances exist among individuals’ systems, and distinct personal approaches are evident. The achievement of successful compilation and loading of the inaugural “hello world” program may, at times, present challenges. It is reassuring to note that overcoming the initial obstacle in the first attempt paves the way for subsequent endeavors to proceed seamlessly.
在深入研究代码之前，需要注意某些事项。个体系统之间存在差异，并且明显存在不同的个人方法。成功编译和加载首个“hello world”程序有时可能会带来挑战。令人欣慰的是，克服第一次尝试中最初的障碍将为后续努力的顺利进行铺平道路。

1. Modversioning. A module compiled for one kernel will not load if a different kernel is booted, unless CONFIG_MODVERSIONS is enabled in the kernel. Module versioning will be discussed later in this guide. Until module versioning is covered, the examples in this guide may not work correctly if running a kernel with modversioning turned on. However, most stock Linux distribution kernels come with modversioning enabled. If difficulties arise when loading the modules due to versioning errors, consider compiling a kernel with modversioning turned off.
   改装。如果启动不同的内核，则不会加载为一个内核编译的模块，除非在内核中启用了 CONFIG_MODVERSIONS 。本指南稍后将讨论模块版本控制。在介绍模块版本控制之前，如果在打开 modversioning 的情况下运行内核，本指南中的示例可能无法正常工作。然而，大多数现有的 Linux 发行版内核都启用了 modversioning。如果由于版本控制错误而在加载模块时出现困难，请考虑在关闭 modversioning 的情况下编译内核。

2. Using X Window System. It is highly recommended to extract, compile, and load all the examples discussed in this guide from a console. Working on these tasks within the X Window System is discouraged.
   使用 X 窗口系统。强烈建议从控制台提取、编译和加载本指南中讨论的所有示例。不鼓励在 X Window 系统内执行这些任务。
   Modules cannot directly print to the screen like printf() can, but they can log information and warnings that are eventually displayed on the screen, specifically within a console. If a module is loaded from an xterm , the information and warnings will be logged, but solely within the systemd journal. These logs will not be visible unless consulting the journalctl . Refer to 4 for more information. For instant access to this information, it is advisable to perform all tasks from the console.
   模块无法像 printf() 那样直接打印到屏幕，但它们可以记录最终显示在屏幕上的信息和警告，特别是在控制台中。如果从 xterm 加载模块，则会记录信息和警告，但仅记录在 systemd 日志中。除非查阅 journalctl ，否则这些日志将不可见。请参阅 4 了解更多信息。为了即时访问此信息，建议从控制台执行所有任务。

3. SecureBoot. Numerous modern computers arrive pre-configured with UEFI SecureBoot enabled—an essential security standard ensuring booting exclusively through trusted software endorsed by the original equipment manufacturer. Certain Linux distributions even ship with the default Linux kernel configured to support SecureBoot. In these cases, the kernel module necessitates a signed security key.
   安全启动。许多现代计算机都预先配置了 UEFI SecureBoot，这是一项重要的安全标准，可确保仅通过原始设备制造商认可的可信软件进行启动。某些 Linux 发行版甚至附带配置为支持 SecureBoot 的默认 Linux 内核。在这些情况下，内核模块需要一个签名的安全密钥。
   Failing this, an attempt to insert your first “hello world” module would result in the message: “ERROR: could not insert module”. If this message Lockdown: insmod: unsigned module loading is restricted; see man kernel lockdown.7 appears in the dmesg output, the simplest approach involves disabling UEFI SecureBoot from the boot menu of your PC or laptop, allowing the successful insertion of ‘hello world” module. Naturally, an alternative involves undergoing intricate procedures such as generating keys, system key installation, and module signing to achieve functionality. However, this intricate process is less appropriate for beginners. If interested, more detailed steps for [SecureBoot](https://wiki.debian.org/SecureBoot) can be explored and followed.
   如果失败，尝试插入第一个“hello world”模块将导致消息：“错误：无法插入模块”。如果出现此消息 Lockdown：insmod：未签名的模块加载受到限制；看到 man kernellockdown.7 出现在 dmesg 输出中，最简单的方法是从 PC 或笔记本电脑的启动菜单中禁用 UEFI SecureBoot，从而允许成功插入“hello world”模块。当然，替代方案涉及经历复杂的过程，例如生成密钥、系统密钥安装和模块签名以实现功能。然而，这个复杂的过程不太适合初学者。如果有兴趣，可以探索并遵循更详细的 [SecureBoot](https://wiki.debian.org/SecureBoot) 步骤。

### Headers 头文件

Before you can build anything you’ll need to install the header files for your kernel.
在构建任何东西之前，您需要安装内核的头文件。

On Ubuntu/Debian GNU/Linux:
在 Ubuntu/Debian GNU/Linux 上：

```bash
sudo apt-get update
apt-cache search linux-headers-`uname -r`
```

This will tell you what kernel header files are available. Then for example:
这将告诉您哪些内核头文件可用。那么例如：

```bash
sudo apt-get install kmod linux-headers-5.4.0-80-generic
```

On Arch Linux:
在 Arch Linux 上：

```bash
sudo pacman -S linux-headers
```

On Fedora:  
在 Fedora 上：

```bash
sudo dnf install kernel-devel kernel-headers
```

### Examples 示例

All the examples from this document are available within the examples subdirectory.
本文档中的所有示例都可以在示例子目录中找到。

If there are any compile errors then you might have a more recent kernel version or need to install the corresponding kernel header files.
如果存在任何编译错误，则您可能拥有更新的内核版本或需要安装相应的内核头文件。

### Hello World 4 你好世界

#### The Simplest Module 最简单的模块

Most people learning programming start out with some sort of "hello world" example. I don’t know what happens to people who break with this tradition, but I think it is safer not to find out. We will start with a series of hello world programs that demonstrate the different aspects of the basics of writing a kernel module.
大多数学习编程的人都是从某种“hello world”示例开始的。我不知道打破这一传统的人会发生什么，但我认为不去发现更安全。我们将从一系列 hello world 程序开始，这些程序演示了编写内核模块的基础知识的不同方面。

Here is the simplest module possible.
这是最简单的模块。

Make a test directory:
建立一个测试目录：

```bash
mkdir -p ~/develop/kernel/hello-1
cd ~/develop/kernel/hello-1
```

Paste this into your favorite editor and save it as hello-1.c:
将其粘贴到您最喜欢的编辑器中并将其另存为 `hello-1.c`：

```c
/*
 * hello-1.c - The simplest kernel module.
 */

#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */

int init_module(void){

    pr_info("Hello world 1.\n");
    /* A non 0 return means init_module failed; module can't be loaded. */
    return 0;
}

void cleanup_module(void){
    pr_info("Goodbye world 1.\n");
}

MODULE_LICENSE("GPL");
```

Now you will need a Makefile. If you copy and paste this, change the indentation to use tabs, not spaces.
现在您需要一个 Makefile。如果复制并粘贴此内容，请将缩进更改为使用制表符，而不是空格。

```makefile
obj-m += hello-1.o

PWD := $(CURDIR)

all:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

In Makefile, `$(CURDIR)` can set to the absolute pathname of the current working directory(after all -C options are processed, if any). See more about CURDIR in GNU make manual.
在 Makefile 中，`$(CURDIR)`可以设置为当前工作目录的绝对路径名（在处理所有-C 选项之后，如果有的话）。有关 CURDIR 的更多信息，请参阅 GNU make 手册。

And finally, just run make directly.
最后，直接运行 make 即可。

```bash
make
```

If there is no `PWD := $(CURDIR)` statement in Makefile, then it may not compile correctly with sudo make. Because some environment variables are specified by the security policy, they can’t be inherited. The default security policy is sudoers. In the sudoers security policy, env_reset is enabled by default, which restricts environment variables. Specifically, path variables are not retained from the user environment, they are set to default values (For more information see: [sudoers manual](https://www.sudo.ws/docs/man/sudoers.man/))). You can see the environment variable settings by:
如果 Makefile 中没有 `PWD := $(CURDIR)` 语句，则可能无法使用 `sudo make` 正确编译。由于某些环境变量是由安全策略指定的，因此无法继承。默认安全策略是 sudoers。在 sudoers 安全策略中，env_reset 默认是开启的，它限制了环境变量。具体来说，路径变量不会从用户环境中保留，而是设置为默认值（有关更多信息，请参阅：sudoers 手册）。您可以通过以下方式查看环境变量设置：

```bash
$ sudo -s
# sudo -V
```

Here is a simple Makefile as an example to demonstrate the problem mentioned above.
这里以一个简单的 Makefile 为例来演示上述问题。

```makefile
all:
    echo $(PWD)
```

Then, we can use -p flag to print out the environment variable values from the Makefile.
然后，我们可以使用 -p 标志从 Makefile 中打印出环境变量值。

```bash
$ make -p | grep PWD
PWD = /home/ubuntu/temp
OLDPWD = /home/ubuntu
    echo $(PWD)
```

The PWD variable won’t be inherited with sudo.
PWD 变量不会被 sudo 继承。

```bash
$ sudo make -p | grep PWD
    echo $(PWD)
```

However, there are three ways to solve this problem.  
然而，有三种方法可以解决这个问题。

1.  You can use the -E flag to temporarily preserve them.  
    您可以使用 -E 标志暂时保留它们。

    ```bash
    $ sudo -E make -p | grep PWD

    PWD = /home/ubuntu/temp

    OLDPWD = /home/ubuntu

    echo $(PWD)
    ```

2.  You can set the env_reset disabled by editing the /etc/sudoers with root and visudo.  
    您可以通过使用 root 和 visudo 编辑 /etc/sudoers 来设置禁用 env_reset。

    ```bash
      ## sudoers file.

      ##

      ...

      Defaults env_reset

      ## Change env_reset to !env_reset in previous line to keep all environment variables
    ```

    Then execute env and sudo env individually.  
    然后分别执行 env 和 sudo env。

    ```bash
    # disable the env_reset

    echo "user:" > non-env_reset.log; env >> non-env_reset.log

    echo "root:" >> non-env_reset.log; sudo env >> non-env_reset.log

    # enable the env_reset

    echo "user:" > env_reset.log; env >> env_reset.log

    echo "root:" >> env_reset.log; sudo env >> env_reset.log
    ```

    You can view and compare these logs to find differences between env_reset and !env_reset.  
    您可以查看并比较这些日志以查找 env_reset 和 !env_reset 之间的差异。

3.  You can preserve environment variables by appending them to env_keep in /etc/sudoers.  
    您可以通过将环境变量附加到 /etc/sudoers 中的 env_keep 来保留环境变量。

    ```bash
    Defaults env_keep += "PWD"
    ```

    After applying the above change, you can check the environment variable settings by:  
    应用上述更改后，您可以通过以下方式检查环境变量设置：

    ```bash
    $ sudo -s
    # sudo -V
    ```

If all goes smoothly you should then find that you have a compiled hello-1.ko module. You can find info on it with the command:  
如果一切顺利，您应该会发现您已经编译了 hello-1.ko 模块。您可以使用以下命令找到有关它的信息：

```bash
modinfo hello-1.ko
```

At this point the command:  
此时命令：

```bash
sudo lsmod | grep hello
```

should return nothing. You can try loading your shiny new module with:  
不应该返回任何内容。您可以尝试使用以下命令加载闪亮的新模块：

```bash
sudo insmod hello-1.ko
```

The dash character will get converted to an underscore, so when you again try:  
破折号字符将转换为下划线，因此当您再次尝试时：

```bash
sudo lsmod | grep hello
```

You should now see your loaded module. It can be removed again with:  
您现在应该看到已加载的模块。可以使用以下方法再次将其删除：

```bash
sudo rmmod hello_1
```

Notice that the dash was replaced by an underscore. To see what just happened in the logs:  
请注意，破折号已替换为下划线。要查看日志中刚刚发生的情况：

```bash
sudo journalctl --since "1 hour ago" | grep kernel
```

You now know the basics of creating, compiling, installing and removing modules. Now for more of a description of how this module works.  
您现在已经了解了创建、编译、安装和删除模块的基础知识。现在更多地描述该模块如何工作。

Kernel modules must have at least two functions: a "start" (initialization) function called `init_module()` which is called when the module is `insmod` ed into the kernel, and an "end" (cleanup) function called `cleanup_module()` which is called just before it is removed from the kernel. Actually, things have changed starting with kernel 2.3.13. You can now use whatever name you like for the start and end functions of a module, and you will learn how to do this in Section [4.2](https://sysprog21.github.io/lkmpg/#hello-and-goodbye). In fact, the new method is the preferred method. However, many people still use `init_module()` and `cleanup_module()` for their start and end functions.  
内核模块必须至少有两个函数：一个名为 `init_module()` 的“启动”（初始化）函数，当模块 `insmod` 插入内核时调用该函数，以及一个“结束”（ cleanup) 函数称为 `cleanup_module()` ，该函数在从内核中删除之前被调用。实际上，从内核 2.3.13 开始，情况发生了变化。现在，您可以为模块的开始和结束函数使用任何您喜欢的名称，您将在第 4.2 节中学习如何执行此操作。事实上，新方法是首选方法。然而，许多人仍然使用 `init_module()` 和 `cleanup_module()` 作为开始和结束函数。

Typically, `init_module()` either registers a handler for something with the kernel, or it replaces one of the kernel functions with its own code (usually code to do something and then call the original function). The `cleanup_module()` function is supposed to undo whatever `init_module()` did, so the module can be unloaded safely.  
通常， `init_module()` 要么向内核注册某个处理程序，要么用自己的代码替换内核函数之一（通常是执行某些操作然后调用原始函数的代码）。 `cleanup_module()` 函数应该撤消 `init_module()` 所做的任何事情，因此可以安全地卸载模块。

Lastly, every kernel module needs to include <linux/module.h>. We needed to include <linux/printk.h> only for the macro expansion for the `pr_alert()` log level, which you’ll learn about in Section [2](https://sysprog21.github.io/lkmpg/#x1-121662).  
最后，每个内核模块都需要包含 。我们只需要包含 来进行 `pr_alert()` 日志级别的宏扩展，您将在第 2 节中了解这一点。

1.  A point about coding style. Another thing which may not be immediately obvious to anyone getting started with kernel programming is that indentation within your code should be using tabs and not spaces. It is one of the coding conventions of the kernel. You may not like it, but you’ll need to get used to it if you ever submit a patch upstream.  
    关于编码风格的一点。对于任何开始内核编程的人来说，另一件事可能不会立即显而易见，那就是代码中的缩进应该使用制表符而不是空格。它是内核的编码约定之一。您可能不喜欢它，但如果您向上游提交补丁，则需要习惯它。
2.  Introducing print macros. In the beginning there was `printk` , usually followed by a priority such as `KERN_INFO` or `KERN_DEBUG` . More recently this can also be expressed in abbreviated form using a set of print macros, such as `pr_info` and `pr_debug` . This just saves some mindless keyboard bashing and looks a bit neater. They can be found within [include/linux/printk.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/printk.h). Take time to read through the available priority macros.  
    介绍打印宏。一开始有 `printk` ，通常后面跟着一个优先级，例如 `KERN_INFO` 或 `KERN_DEBUG` 。最近，这也可以使用一组打印宏以缩写形式表示，例如 `pr_info` 和 `pr_debug` 。这只是节省了一些无意识的键盘敲打，而且看起来更整洁一些。它们可以在 include/linux/printk.h 中找到。花时间阅读可用的优先级宏。
3.  About Compiling. Kernel modules need to be compiled a bit differently from regular userspace apps. Former kernel versions required us to care much about these settings, which are usually stored in Makefiles. Although hierarchically organized, many redundant settings accumulated in sublevel Makefiles and made them large and rather difficult to maintain. Fortunately, there is a new way of doing these things, called kbuild, and the build process for external loadable modules is now fully integrated into the standard kernel build mechanism. To learn more on how to compile modules which are not part of the official kernel (such as all the examples you will find in this guide), see file [Documentation/kbuild/modules.rst](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/Documentation/kbuild/modules.rst).  
    关于编译。内核模块的编译方式与常规用户空间应用程序略有不同。以前的内核版本需要我们非常关心这些设置，这些设置通常存储在 Makefile 中。尽管是分层组织的，但许多冗余设置累积在子级 Makefile 中，导致它们变得庞大且难以维护。幸运的是，有一种新的方法可以完成这些事情，称为 kbuild，并且外部可加载模块的构建过程现在已完全集成到标准内核构建机制中。要了解有关如何编译不属于官方内核的模块的更多信息（例如您将在本指南中找到的所有示例），请参阅文件 Documentation/kbuild/modules.rst。

    Additional details about Makefiles for kernel modules are available in [Documentation/kbuild/makefiles.rst](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/Documentation/kbuild/makefiles.rst). Be sure to read this and the related files before starting to hack Makefiles. It will probably save you lots of work.  
    有关内核模块 Makefile 的更多详细信息，请参阅 Documentation/kbuild/makefiles.rst。在开始破解 Makefile 之前，请务必阅读本文和相关文件。它可能会为您节省大量工作。

    > Here is another exercise for the reader. See that comment above the return statement in `init_module()` ? Change the return value to something negative, recompile and load the module again. What happens?  
    > 这是给读者的另一个练习。看到 `init_module()` 中 return 语句上方的注释了吗？将返回值更改为负值，重新编译并再次加载模块。会发生什么？

#### 4.2 Hello and Goodbye 4.2 你好和再见

In early kernel versions you had to use the `init_module` and `cleanup_module` functions, as in the first hello world example, but these days you can name those anything you want by using the `module_init` and `module_exit` macros. These macros are defined in [include/linux/module.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/module.h). The only requirement is that your init and cleanup functions must be defined before calling the those macros, otherwise you’ll get compilation errors. Here is an example of this technique:  
在早期的内核版本中，您必须使用 `init_module` 和 `cleanup_module` 函数，如第一个 hello world 示例中所示，但现在您可以使用 `module_init` 和 `module_exit` 宏。这些宏在 include/linux/module.h 中定义。唯一的要求是你的 init 和 cleanup 函数必须在调用这些宏之前定义，否则你会得到编译错误。下面是该技术的一个示例：

```c
/*
 * hello-2.c - Demonstrating the module_init() and module_exit() macros.
 * This is preferred over using init_module() and cleanup_module().
 */
#include <linux/init.h> /* Needed for the macros */
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */

static int __init hello_2_init(void)
{
    pr_info("Hello, world 2\n");
    return 0;
}

static void __exit hello_2_exit(void)
{
    pr_info("Goodbye, world 2\n");
}

module_init(hello_2_init);
module_exit(hello_2_exit);

MODULE_LICENSE("GPL");
```

So now we have two real kernel modules under our belt. Adding another module is as simple as this:  
现在我们有了两个真正的内核模块。添加另一个模块就像这样简单：

```makefile
obj-m += hello-1.o
obj-m += hello-2.o

PWD := $(CURDIR)

all:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

Now have a look at [drivers/char/Makefile](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/drivers/char/Makefile) for a real world example. As you can see, some things got hardwired into the kernel (obj-y) but where have all those obj-m gone? Those familiar with shell scripts will easily be able to spot them. For those who are not, the obj-$(CONFIG_FOO) entries you see everywhere expand into obj-y or obj-m, depending on whether the CONFIG_FOO variable has been set to y or m. While we are at it, those were exactly the kind of variables that you have set in the .config file in the top-level directory of Linux kernel source tree, the last time when you said `make menuconfig` or something like that.  
现在看一下 drivers/char/Makefile 的真实示例。正如您所看到的，有些东西被硬连线到内核（obj-y）中，但是所有这些 obj-m 到哪里去了？熟悉 shell 脚本的人很容易就能发现它们。对于那些不这样做的人，您随处看到的 obj-$(CONFIG_FOO) 条目会扩展为 obj-y 或 obj-m，具体取决于 CONFIG_FOO 变量是否设置为 y 还是 m。当我们这样做时，这些正是您在 Linux 内核源代码树顶级目录的 .config 文件中设置的变量类型，上次您说 `make menuconfig` 或类似的内容时那。

#### 4.3 The **init and **exit Macros

4.3 **init 和 **exit 宏

The `__init` macro causes the init function to be discarded and its memory freed once the init function finishes for built-in drivers, but not loadable modules. If you think about when the init function is invoked, this makes perfect sense.  
一旦内置驱动程序（但不是可加载模块）的 init 函数完成， `__init` 宏会导致 init 函数被丢弃并释放其内存。如果您考虑一下 init 函数何时被调用，这就很有意义了。

There is also an `__initdata` which works similarly to `__init` but for init variables rather than functions.  
还有一个 `__initdata` ，其工作方式与 `__init` 类似，但用于初始化变量而不是函数。

The `__exit` macro causes the omission of the function when the module is built into the kernel, and like `__init` , has no effect for loadable modules. Again, if you consider when the cleanup function runs, this makes complete sense; built-in drivers do not need a cleanup function, while loadable modules do.  
当模块内置到内核中时， `__exit` 宏会导致函数的省略，并且与 `__init` 一样，对于可加载模块没有影响。同样，如果您考虑清理函数何时运行，这是完全有道理的；内置驱动程序不需要清理功能，而可加载模块则需要。

These macros are defined in [include/linux/init.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/init.h) and serve to free up kernel memory. When you boot your kernel and see something like Freeing unused kernel memory: 236k freed, this is precisely what the kernel is freeing.  
这些宏在 include/linux/init.h 中定义，用于释放内核内存。当您启动内核并看到类似“释放未使用的内核内存：已释放 236k”之类的内容时，这正是内核正在释放的内容。

```c
/*
 * hello-3.c - Illustrating the __init, __initdata and __exit macros.
 */
#include <linux/init.h> /* Needed for the macros */
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */

static int hello3_data __initdata = 3;

static int __init hello_3_init(void)
{
    pr_info("Hello, world %d\n", hello3_data);
    return 0;
}

static void __exit hello_3_exit(void)
{
    pr_info("Goodbye, world 3\n");
}

module_init(hello_3_init);
module_exit(hello_3_exit);

MODULE_LICENSE("GPL");
```

#### 4.4 Licensing and Module Documentation

4.4 许可和模块文档

Honestly, who loads or even cares about proprietary modules? If you do then you might have seen something like this:  
老实说，谁加载甚至关心专有模块？如果您这样做，那么您可能会看到类似这样的内容：

```bash
$ sudo insmod xxxxxx.ko
loading out-of-tree module taints kernel.
module license 'unspecified' taints kernel.
```

You can use a few macros to indicate the license for your module. Some examples are "GPL", "GPL v2", "GPL and additional rights", "Dual BSD/GPL", "Dual MIT/GPL", "Dual MPL/GPL" and "Proprietary". They are defined within [include/linux/module.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/module.h).  
您可以使用一些宏来指示模块的许可证。一些示例包括“GPL”、“GPL v2”、“GPL 和附加权利”、“双 BSD/GPL”、“双 MIT/GPL”、“双 MPL/GPL”和“专有”。它们在 include/linux/module.h 中定义。

To reference what license you’re using a macro is available called `MODULE_LICENSE` . This and a few other macros describing the module are illustrated in the below example.  
要引用您正在使用的许可证，可以使用名为 `MODULE_LICENSE` 的宏。下面的示例说明了这个宏和描述该模块的其他一些宏。

```c
/*
 * hello-4.c - Demonstrates module documentation.
 */

#include <linux/init.h> /* Needed for the macros */
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LKMPG");
MODULE_DESCRIPTION("A sample driver");

static int __init init_hello_4(void)
{
    pr_info("Hello, world 4\n");
    return 0;
}

static void __exit cleanup_hello_4(void)
{
    pr_info("Goodbye, world 4\n");
}

module_init(init_hello_4);
module_exit(cleanup_hello_4);
```

#### 4.5 Passing Command Line Arguments to a Module

4.5 将命令行参数传递给模块

Modules can take command line arguments, but not with the argc/argv you might be used to.  
模块可以接受命令行参数，但不能使用您可能习惯的 argc/argv。

To allow arguments to be passed to your module, declare the variables that will take the values of the command line arguments as global and then use the `module_param()` macro, (defined in [include/linux/moduleparam.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/moduleparam.h)) to set the mechanism up. At runtime, `insmod` will fill the variables with any command line arguments that are given, like `insmod mymodule.ko myvariable=5` . The variable declarations and macros should be placed at the beginning of the module for clarity. The example code should clear up my admittedly lousy explanation.  
要允许将参数传递到模块，请声明将命令行参数的值作为全局变量，然后使用 `module_param()` 宏（在 include/linux/moduleparam.h 中定义）来建立机制。在运行时， `insmod` 将使用给定的任何命令行参数填充变量，例如 `insmod mymodule.ko myvariable=5` 。为了清晰起见，变量声明和宏应放置在模块的开头。示例代码应该可以澄清我公认的糟糕的解释。

The `module_param()` macro takes 3 arguments: the name of the variable, its type and permissions for the corresponding file in sysfs. Integer types can be signed as usual or unsigned. If you’d like to use arrays of integers or strings see `module_param_array()` and `module_param_string()` .  
`module_param()` 宏有 3 个参数：变量的名称、变量的类型以及 sysfs 中相应文件的权限。整数类型可以照常带符号或不带符号。如果您想使用整数或字符串数组，请参阅 `module_param_array()` 和 `module_param_string()` 。

```c
int myint = 3;
module_param(myint, int, 0);
```

Arrays are supported too, but things are a bit different now than they were in the olden days. To keep track of the number of parameters you need to pass a pointer to a count variable as third parameter. At your option, you could also ignore the count and pass `NULL` instead. We show both possibilities here:  
数组也受支持，但现在的情况与以前有些不同。要跟踪参数的数量，您需要将指向计数变量的指针作为第三个参数传递。您也可以选择忽略计数并传递 `NULL` 。我们在这里展示了两种可能性：

```c
int myintarray[2];
module_param_array(myintarray, int, NULL, 0); /* not interested in count */

short myshortarray[4];
int count;
module_param_array(myshortarray, short, &count, 0); /* put count into "count" variable */
```

A good use for this is to have the module variable’s default values set, like a port or IO address. If the variables contain the default values, then perform autodetection (explained elsewhere). Otherwise, keep the current value. This will be made clear later on.  
一个很好的用途是设置模块变量的默认值，例如端口或 IO 地址。如果变量包含默认值，则执行自动检测（在其他地方解释）。否则，保持当前值。这一点稍后会明确。

Lastly, there is a macro function, `MODULE_PARM_DESC()` , that is used to document arguments that the module can take. It takes two parameters: a variable name and a free form string describing that variable.  
最后，有一个宏函数 `MODULE_PARM_DESC()` ，用于记录模块可以采用的参数。它需要两个参数：变量名称和描述该变量的自由格式字符串。

```c
/*
 * hello-5.c - Demonstrates command line argument passing to a module.
 */
#include <linux/init.h>
#include <linux/kernel.h> /* for ARRAY_SIZE() */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");

static short int myshort = 1;
static int myint = 420;
static long int mylong = 9999;
static char *mystring = "blah";
static int myintarray[2] = { 420, 420 };
static int arr_argc = 0;

/* module_param(foo, int, 0000)
 * The first param is the parameters name.
 * The second param is its data type.
 * The final argument is the permissions bits,
 * for exposing parameters in sysfs (if non-zero) at a later stage.
 */
module_param(myshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(myshort, "A short integer");
module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(myint, "An integer");
module_param(mylong, long, S_IRUSR);
MODULE_PARM_DESC(mylong, "A long integer");
module_param(mystring, charp, 0000);
MODULE_PARM_DESC(mystring, "A character string");

/* module_param_array(name, type, num, perm);
 * The first param is the parameter's (in this case the array's) name.
 * The second param is the data type of the elements of the array.
 * The third argument is a pointer to the variable that will store the number
 * of elements of the array initialized by the user at module loading time.
 * The fourth argument is the permission bits.
 */
module_param_array(myintarray, int, &arr_argc, 0000);
MODULE_PARM_DESC(myintarray, "An array of integers");

static int __init hello_5_init(void)
{
    int i;

    pr_info("Hello, world 5\n=============\n");
    pr_info("myshort is a short integer: %hd\n", myshort);
    pr_info("myint is an integer: %d\n", myint);
    pr_info("mylong is a long integer: %ld\n", mylong);
    pr_info("mystring is a string: %s\n", mystring);

    for (i = 0; i < ARRAY_SIZE(myintarray); i++)
        pr_info("myintarray[%d] = %d\n", i, myintarray[i]);

    pr_info("got %d arguments for myintarray.\n", arr_argc);
    return 0;
}

static void __exit hello_5_exit(void)
{
    pr_info("Goodbye, world 5\n");
}

module_init(hello_5_init);
module_exit(hello_5_exit);
```

I would recommend playing around with this code:  
我建议使用以下代码：

```bash
$ sudo insmod hello-5.ko mystring="bebop" myintarray=-1
$ sudo dmesg -t | tail -7
myshort is a short integer: 1
myint is an integer: 420
mylong is a long integer: 9999
mystring is a string: bebop
myintarray[0] = -1
myintarray[1] = 420
got 1 arguments for myintarray.

$ sudo rmmod hello-5
$ sudo dmesg -t | tail -1
Goodbye, world 5

$ sudo insmod hello-5.ko mystring="supercalifragilisticexpialidocious" myintarray=-1,-1
$ sudo dmesg -t | tail -7
myshort is a short integer: 1
myint is an integer: 420
mylong is a long integer: 9999
mystring is a string: supercalifragilisticexpialidocious
myintarray[0] = -1
myintarray[1] = -1
got 2 arguments for myintarray.

$ sudo rmmod hello-5
$ sudo dmesg -t | tail -1
Goodbye, world 5

$ sudo insmod hello-5.ko mylong=hello
insmod: ERROR: could not insert module hello-5.ko: Invalid parameters
```

#### 4.6 Modules Spanning Multiple Files

4.6 跨多个文件的模块

Sometimes it makes sense to divide a kernel module between several source files.  
有时，将内核模块划分为多个源文件是有意义的。

Here is an example of such a kernel module.  
这是此类内核模块的示例。

```c
/*
 * start.c - Illustration of multi filed modules
 */

#include <linux/kernel.h> /* We are doing kernel work */
#include <linux/module.h> /* Specifically, a module */

int init_module(void)
{
    pr_info("Hello, world - this is the kernel speaking\n");
    return 0;
}

MODULE_LICENSE("GPL");
```

The next file:  
下一个文件：

```c
/*
 * stop.c - Illustration of multi filed modules
 */

#include <linux/kernel.h> /* We are doing kernel work */
#include <linux/module.h> /* Specifically, a module  */

void cleanup_module(void)
{
    pr_info("Short is the life of a kernel module\n");
}

MODULE_LICENSE("GPL");
```

And finally, the makefile:  
最后是 makefile：

```makefile
obj-m += hello-1.o
obj-m += hello-2.o
obj-m += hello-3.o
obj-m += hello-4.o
obj-m += hello-5.o
obj-m += startstop.o
startstop-objs := start.o stop.o

PWD := $(CURDIR)

all:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

This is the complete makefile for all the examples we have seen so far. The first five lines are nothing special, but for the last example we will need two lines. First we invent an object name for our combined module, second we tell `make` what object files are part of that module.  
这是迄今为止我们看到的所有示例的完整 makefile。前五行没什么特别的，但对于最后一个例子，我们需要两行。首先，我们为组合模块发明一个对象名称，其次我们告诉 `make` 哪些对象文件是该模块的一部分。

#### 4.7 Building modules for a precompiled kernel

4.7 为预编译内核构建模块

Obviously, we strongly suggest you to recompile your kernel, so that you can enable a number of useful debugging features, such as forced module unloading ( `MODULE_FORCE_UNLOAD` ): when this option is enabled, you can force the kernel to unload a module even when it believes it is unsafe, via a `sudo rmmod -f module` command. This option can save you a lot of time and a number of reboots during the development of a module. If you do not want to recompile your kernel then you should consider running the examples within a test distribution on a virtual machine. If you mess anything up then you can easily reboot or restore the virtual machine (VM).  
显然，我们强烈建议您重新编译内核，以便您可以启用许多有用的调试功能，例如强制模块卸载（ `MODULE_FORCE_UNLOAD` ）：启用此选项后，您可以强制内核即使模块认为不安全，也可以通过 `sudo rmmod -f module` 命令卸载模块。此选项可以在模块开发过程中为您节省大量时间和多次重新启动。如果您不想重新编译内核，那么您应该考虑在虚拟机上运行测试发行版中的示例。如果出现任何问题，您可以轻松重新启动或恢复虚拟机 (VM)。

There are a number of cases in which you may want to load your module into a precompiled running kernel, such as the ones shipped with common Linux distributions, or a kernel you have compiled in the past. In certain circumstances you could require to compile and insert a module into a running kernel which you are not allowed to recompile, or on a machine that you prefer not to reboot. If you can’t think of a case that will force you to use modules for a precompiled kernel you might want to skip this and treat the rest of this chapter as a big footnote.  
在许多情况下，您可能希望将模块加载到预编译的运行内核中，例如常见 Linux 发行版附带的模块，或者您过去编译过的内核。在某些情况下，您可能需要编译模块并将其插入到正在运行的内核中，而不允许重新编译该模块，或者插入到您不希望重新启动的计算机上。如果您想不出强制您为预编译内核使用模块的情况，您可能想跳过这一点并将本章的其余部分视为一个大脚注。

Now, if you just install a kernel source tree, use it to compile your kernel module and you try to insert your module into the kernel, in most cases you would obtain an error as follows:  
现在，如果您只是安装内核源代码树，使用它来编译内核模块，然后尝试将模块插入内核中，在大多数情况下您会收到如下错误：

```bash
insmod: ERROR: could not insert module poet.ko: Invalid module format
```

Less cryptic information is logged to the systemd journal:  
不太神秘的信息会记录到 systemd 日志中：

```bash
kernel: poet: disagrees about version of symbol module_layout
```

In other words, your kernel refuses to accept your module because version strings (more precisely, version magic, see [include/linux/vermagic.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/vermagic.h)) do not match. Incidentally, version magic strings are stored in the module object in the form of a static string, starting with `vermagic:` . Version data are inserted in your module when it is linked against the kernel/module.o file. To inspect version magics and other strings stored in a given module, issue the command `modinfo module.ko` :  
换句话说，您的内核拒绝接受您的模块，因为版本字符串（更准确地说，版本魔法，请参阅 include/linux/vermagic.h）不匹配。顺便说一句，版本魔术字符串以静态字符串的形式存储在模块对象中，以 `vermagic:` 开头。当链接到 kernel/module.o 文件时，版本数据将插入到模块中。要检查给定模块中存储的版本魔法和其他字符串，请发出命令 `modinfo module.ko` ：

```bash
$ modinfo hello-4.ko
description:    A sample driver
author:         LKMPG
license:        GPL
srcversion:     B2AA7FBFCC2C39AED665382
depends:
retpoline:      Y
name:           hello_4
vermagic:       5.4.0-70-generic SMP mod_unload modversions
```

To overcome this problem we could resort to the --force-vermagic option, but this solution is potentially unsafe, and unquestionably unacceptable in production modules. Consequently, we want to compile our module in an environment which was identical to the one in which our precompiled kernel was built. How to do this, is the subject of the remainder of this chapter.  
为了解决这个问题，我们可以求助于 --force-vermagic 选项，但这种解决方案可能不安全，并且在生产模块中无疑是不可接受的。因此，我们希望在与构建预编译内核的环境相同的环境中编译我们的模块。如何做到这一点是本章剩余部分的主题。

First of all, make sure that a kernel source tree is available, having exactly the same version as your current kernel. Then, find the configuration file which was used to compile your precompiled kernel. Usually, this is available in your current boot directory, under a name like config-5.14.x. You may just want to copy it to your kernel source tree: `` cp /boot/config-`uname -r` .config `` .  
首先，确保内核源代码树可用，并且具有与当前内核完全相同的版本。然后，找到用于编译预编译内核的配置文件。通常，它可以在您当前的引导目录中使用，名称如 config-5.14.x。您可能只想将其复制到内核源代码树： `` cp /boot/config-`uname -r` .config `` 。

Let’s focus again on the previous error message: a closer look at the version magic strings suggests that, even with two configuration files which are exactly the same, a slight difference in the version magic could be possible, and it is sufficient to prevent insertion of the module into the kernel. That slight difference, namely the custom string which appears in the module’s version magic and not in the kernel’s one, is due to a modification with respect to the original, in the makefile that some distributions include. Then, examine your Makefile, and make sure that the specified version information matches exactly the one used for your current kernel. For example, your makefile could start as follows:  
让我们再次关注前面的错误消息：仔细观察版本魔术字符串表明，即使有两个完全相同的配置文件，版本魔术也可能存在细微差别，并且足以防止插入模块进入内核。这种细微的差异，即出现在模块版本魔法中而不是内核版本魔法中的自定义字符串，是由于某些发行版包含的 makefile 中对原始字符串的修改造成的。然后，检查您的 Makefile，并确保指定的版本信息与当前内核使用的版本信息完全匹配。例如，您的 makefile 可以如下启动：

```makefile
VERSION = 5
PATCHLEVEL = 14
SUBLEVEL = 0
EXTRAVERSION = -rc2
```

In this case, you need to restore the value of symbol EXTRAVERSION to -rc2. We suggest to keep a backup copy of the makefile used to compile your kernel available in /lib/modules/5.14.0-rc2/build. A simple command as following should suffice.  
在这种情况下，您需要将符号 EXTRAVERSION 的值恢复为-rc2。我们建议在 /lib/modules/5.14.0-rc2/build 中保留用于编译内核的 makefile 的备份副本。下面的简单命令就足够了。

```makefile
cp /lib/modules/`uname -r`/build/Makefile linux-`uname -r`
```

Here `` linux-`uname -r` `` is the Linux kernel source you are attempting to build.  
这里 `` linux-`uname -r` `` 是您尝试构建的 Linux 内核源代码。

Now, please run `make` to update configuration and version headers and objects:  
现在，请运行 `make` 来更新配置和版本标头和对象：

```bash
$ make
  SYNC    include/config/auto.conf.cmd
  HOSTCC  scripts/basic/fixdep
  HOSTCC  scripts/kconfig/conf.o
  HOSTCC  scripts/kconfig/confdata.o
  HOSTCC  scripts/kconfig/expr.o
  LEX     scripts/kconfig/lexer.lex.c
  YACC    scripts/kconfig/parser.tab.[ch]
  HOSTCC  scripts/kconfig/preprocess.o
  HOSTCC  scripts/kconfig/symbol.o
  HOSTCC  scripts/kconfig/util.o
  HOSTCC  scripts/kconfig/lexer.lex.o
  HOSTCC  scripts/kconfig/parser.tab.o
  HOSTLD  scripts/kconfig/conf
```

If you do not desire to actually compile the kernel, you can interrupt the build process (CTRL-C) just after the SPLIT line, because at that time, the files you need are ready. Now you can turn back to the directory of your module and compile it: It will be built exactly according to your current kernel settings, and it will load into it without any errors.  
如果您不想实际编译内核，您可以在 SPLIT 行之后中断构建过程 (CTRL-C)，因为此时您需要的文件已准备就绪。现在您可以返回模块的目录并编译它：它将完全根据您当前的内核设置构建，并且加载到其中时不会出现任何错误。

### 5 Preliminaries 5 预赛

#### 5.1 How modules begin and end

5.1 模块如何开始和结束

A program usually begins with a `main()` function, executes a bunch of instructions and terminates upon completion of those instructions. Kernel modules work a bit differently. A module always begin with either the `init_module` or the function you specify with `module_init` call. This is the entry function for modules; it tells the kernel what functionality the module provides and sets up the kernel to run the module’s functions when they are needed. Once it does this, entry function returns and the module does nothing until the kernel wants to do something with the code that the module provides.  
程序通常以 `main()` 函数开始，执行一堆指令并在完成这些指令后终止。内核模块的工作方式略有不同。模块始终以 `init_module` 或您通过 `module_init` 调用指定的函数开头。这是模块的入口函数；它告诉内核该模块提供什么功能，并设置内核在需要时运行该模块的功能。一旦完成此操作，入口函数返回并且模块不执行任何操作，直到内核想要使用模块提供的代码执行某些操作。

All modules end by calling either `cleanup_module` or the function you specify with the `module_exit` call. This is the exit function for modules; it undoes whatever entry function did. It unregisters the functionality that the entry function registered.  
所有模块都以调用 `cleanup_module` 或您通过 `module_exit` 调用指定的函数结束。这是模块的退出函数；它会撤消输入函数所做的任何操作。它取消注册入口函数注册的功能。

Every module must have an entry function and an exit function. Since there’s more than one way to specify entry and exit functions, I will try my best to use the terms “entry function” and “exit function”, but if I slip and simply refer to them as `init_module` and `cleanup_module` , I think you will know what I mean.  
每个模块必须有一个入口函数和一个出口函数。由于指定进入和退出函数的方法不止一种，我会尽力使用术语“进入函数”和“退出函数”，但如果我忽略并简单地将它们称为 `init_module` 和 `cleanup_module` ，我想你会明白我的意思。

#### 5.2 Functions available to modules

5.2 模块可用的功能

Programmers use functions they do not define all the time. A prime example of this is `printf()` . You use these library functions which are provided by the standard C library, libc. The definitions for these functions do not actually enter your program until the linking stage, which insures that the code (for `printf()` for example) is available, and fixes the call instruction to point to that code.  
程序员始终使用他们未定义的函数。一个典型的例子是 `printf()` 。您可以使用标准 C 库 libc 提供的这些库函数。这些函数的定义直到链接阶段才真正进入您的程序，这确保了代码（例如 `printf()` ）可用，并修复了调用指令以指向该代码。

Kernel modules are different here, too. In the hello world example, you might have noticed that we used a function, `pr_info()` but did not include a standard I/O library. That is because modules are object files whose symbols get resolved upon running `insmod` or `modprobe` . The definition for the symbols comes from the kernel itself; the only external functions you can use are the ones provided by the kernel. If you’re curious about what symbols have been exported by your kernel, take a look at /proc/kallsyms.  
这里的内核模块也不同。在 hello world 示例中，您可能已经注意到我们使用了函数 `pr_info()` 但没有包含标准 I/O 库。这是因为模块是对象文件，其符号在运行 `insmod` 或 `modprobe` 时得到解析。符号的定义来自内核本身；您可以使用的唯一外部函数是内核提供的函数。如果您对内核导出了哪些符号感到好奇，请查看 /proc/kallsyms。

One point to keep in mind is the difference between library functions and system calls. Library functions are higher level, run completely in user space and provide a more convenient interface for the programmer to the functions that do the real work — system calls. System calls run in kernel mode on the user’s behalf and are provided by the kernel itself. The library function `printf()` may look like a very general printing function, but all it really does is format the data into strings and write the string data using the low-level system call `write()` , which then sends the data to standard output.  
需要记住的一点是库函数和系统调用之间的区别。库函数是更高级别的，完全在用户空间中运行，并为程序员提供更方便的接口来执行实际工作的函数（系统调用）。系统调用代表用户在内核模式下运行，由内核本身提供。库函数 `printf()` 可能看起来像一个非常通用的打印函数，但它实际上所做的只是将数据格式化为字符串并使用低级系统调用 `write()` 写入字符串数据，然后将数据发送到标准输出。

Would you like to see what system calls are made by `printf()` ? It is easy! Compile the following program:  
您想查看 `printf()` 进行了哪些系统调用吗？这很容易！编译以下程序：

```
#include <stdio.h>

int main(void)
{
    printf("hello");
    return 0;
}
```

with `gcc -Wall -o hello hello.c` . Run the executable with `strace ./hello` . Are you impressed? Every line you see corresponds to a system call. [strace](https://strace.io/) is a handy program that gives you details about what system calls a program is making, including which call is made, what its arguments are and what it returns. It is an invaluable tool for figuring out things like what files a program is trying to access. Towards the end, you will see a line which looks like `write(1, "hello", 5hello)` . There it is. The face behind the `printf()` mask. You may not be familiar with write, since most people use library functions for file I/O (like `fopen` , `fputs` , `fclose` ). If that is the case, try looking at man 2 write. The 2nd man section is devoted to system calls (like `kill()` and `read()` ). The 3rd man section is devoted to library calls, which you would probably be more familiar with (like `cosh()` and `random()` ).  
与 `gcc -Wall -o hello hello.c` 。使用 `strace ./hello` 运行可执行文件。你印象深刻吗？您看到的每一行都对应一个系统调用。 strace 是一个方便的程序，它为您提供有关程序正在执行哪些系统调用的详细信息，包括进行了哪个调用、其参数是什么以及返回什么。它是一个非常宝贵的工具，可以用来找出诸如程序试图访问哪些文件之类的事情。最后，您将看到一行看起来像 `write(1, "hello", 5hello)` 的行。就在那里。 `printf()` 面具后面的脸。您可能不熟悉 write，因为大多数人使用库函数进行文件 I/O（例如 `fopen` 、 `fputs` 、 `fclose` ）。如果是这种情况，请尝试查看 man 2 write。第二部分专门讨论系统调用（如 `kill()` 和 `read()` ）。第三个人部分专门用于库调用，您可能会更熟悉它（例如 `cosh()` 和 `random()` ）。

You can even write modules to replace the kernel’s system calls, which we will do shortly. Crackers often make use of this sort of thing for backdoors or trojans, but you can write your own modules to do more benign things, like have the kernel write Tee hee, that tickles! every time someone tries to delete a file on your system.  
您甚至可以编写模块来替换内核的系统调用，我们很快就会这样做。破解者经常利用这种东西来创建后门或木马，但您可以编写自己的模块来做更多良性的事情，例如让内核写入 Tee hee，这很有趣！每次有人尝试删除您系统上的文件时。

#### 5.3 User Space vs Kernel Space

5.3 用户空间与内核空间

A kernel is all about access to resources, whether the resource in question happens to be a video card, a hard drive or even memory. Programs often compete for the same resource. As I just saved this document, updatedb started updating the locate database. My vim session and updatedb are both using the hard drive concurrently. The kernel needs to keep things orderly, and not give users access to resources whenever they feel like it. To this end, a CPU can run in different modes. Each mode gives a different level of freedom to do what you want on the system. The Intel 80386 architecture had 4 of these modes, which were called rings. Unix uses only two rings; the highest ring (ring 0, also known as “supervisor mode” where everything is allowed to happen) and the lowest ring, which is called “user mode”.  
内核的核心就是对资源的访问，无论所讨论的资源是显卡、硬盘还是内存。程序经常争夺相同的资源。当我刚刚保存此文档时，updatedb 开始更新 locate 数据库。我的 vim 会话和 updateb 都同时使用硬盘。内核需要保持一切有序，并且不让用户随时访问资源。为此，CPU 可以运行在不同的模式下。每种模式都提供不同程度的自由度，让您可以在系统上执行您想要的操作。 Intel 80386 架构有 4 种这样的模式，称为环。 Unix 只使用两个环；最高的环（环 0，也称为“管理员模式”，允许一切发生）和最低的环，称为“用户模式”。

Recall the discussion about library functions vs system calls. Typically, you use a library function in user mode. The library function calls one or more system calls, and these system calls execute on the library function’s behalf, but do so in supervisor mode since they are part of the kernel itself. Once the system call completes its task, it returns and execution gets transferred back to user mode.  
回想一下关于库函数与系统调用的讨论。通常，您在用户模式下使用库函数。库函数调用一个或多个系统调用，这些系统调用代表库函数执行，但在管理程序模式下执行，因为它们是内核本身的一部分。一旦系统调用完成其任务，它就会返回并将执行转移回用户模式。

#### 5.4 Name Space 5.4 命名空间

When you write a small C program, you use variables which are convenient and make sense to the reader. If, on the other hand, you are writing routines which will be part of a bigger problem, any global variables you have are part of a community of other peoples’ global variables; some of the variable names can clash. When a program has lots of global variables which aren’t meaningful enough to be distinguished, you get namespace pollution. In large projects, effort must be made to remember reserved names, and to find ways to develop a scheme for naming unique variable names and symbols.  
当您编写小型 C 程序时，您使用的变量对读者来说很方便且有意义。另一方面，如果您正在编写将成为更大问题的一部分的例程，那么您拥有的任何全局变量都是其他人的全局变量社区的一部分；一些变量名称可能会发生冲突。当一个程序有大量的全局变量，而这些变量的意义不足以区分时，就会出现命名空间污染。在大型项目中，必须努力记住保留名称，并找到方法来开发命名唯一变量名称和符号的方案。

When writing kernel code, even the smallest module will be linked against the entire kernel, so this is definitely an issue. The best way to deal with this is to declare all your variables as static and to use a well-defined prefix for your symbols. By convention, all kernel prefixes are lowercase. If you do not want to declare everything as static, another option is to declare a symbol table and register it with the kernel. We will get to this later.  
当编写内核代码时，即使是最小的模块也会链接到整个内核，所以这绝对是一个问题。处理这个问题的最佳方法是将所有变量声明为静态，并为符号使用定义明确的前缀。按照惯例，所有内核前缀都是小写的。如果您不想将所有内容声明为静态，另一种选择是声明符号表并将其注册到内核。我们稍后会讨论这个。

The file /proc/kallsyms holds all the symbols that the kernel knows about and which are therefore accessible to your modules since they share the kernel’s codespace.  
文件 /proc/kallsyms 保存了内核知道的所有符号，因此您的模块可以访问这些符号，因为它们共享内核的代码空间。

#### 5.5 Code space 5.5 代码空间

Memory management is a very complicated subject and the majority of O’Reilly’s [Understanding The Linux Kernel](https://www.oreilly.com/library/view/understanding-the-linux/0596005652/) exclusively covers memory management! We are not setting out to be experts on memory managements, but we do need to know a couple of facts to even begin worrying about writing real modules.  
内存管理是一个非常复杂的主题，O'Reilly 的《Understanding The Linux Kernel》的大部分内容都专门涉及内存管理！我们并不是想成为内存管理方面的专家，但我们确实需要了解一些事实才能开始担心编写真正的模块。

If you have not thought about what a segfault really means, you may be surprised to hear that pointers do not actually point to memory locations. Not real ones, anyway. When a process is created, the kernel sets aside a portion of real physical memory and hands it to the process to use for its executing code, variables, stack, heap and other things which a computer scientist would know about. This memory begins with 0x00000000 and extends up to whatever it needs to be. Since the memory space for any two processes do not overlap, every process that can access a memory address, say 0xbffff978, would be accessing a different location in real physical memory! The processes would be accessing an index named 0xbffff978 which points to some kind of offset into the region of memory set aside for that particular process. For the most part, a process like our Hello, World program can’t access the space of another process, although there are ways which we will talk about later.  
如果您还没有考虑过段错误的真正含义，您可能会惊讶地发现指针实际上并不指向内存位置。无论如何，不是真实的。当一个进程被创建时，内核会留出一部分真实的物理内存，并将其交给进程用于执行代码、变量、堆栈、堆和计算机科学家所知道的其他内容。该内存从 0x00000000 开始，一直延伸到它需要的任何位置。由于任何两个进程的内存空间不重叠，因此每个可以访问内存地址（例如 0xbffff978）的进程都将访问真实物理内存中的不同位置！这些进程将访问名为 0xbffff978 的索引，该索引指向为该特定进程预留的内存区域中的某种偏移量。在大多数情况下，像我们的 Hello, World 程序这样的进程无法访问另一个进程的空间，尽管我们稍后会讨论一些方法。

The kernel has its own space of memory as well. Since a module is code which can be dynamically inserted and removed in the kernel (as opposed to a semi-autonomous object), it shares the kernel’s codespace rather than having its own. Therefore, if your module segfaults, the kernel segfaults. And if you start writing over data because of an off-by-one error, then you’re trampling on kernel data (or code). This is even worse than it sounds, so try your best to be careful.  
内核也有自己的内存空间。由于模块是可以在内核中动态插入和删除的代码（与半自治对象相反），因此它共享内核的代码空间而不是拥有自己的代码空间。因此，如果您的模块出现段错误，内核也会出现段错误。如果您因为差一错误而开始重写数据，那么您就是在践踏内核数据（或代码）。这比听起来更糟糕，所以请尽量小心。

By the way, I would like to point out that the above discussion is true for any operating system which uses a monolithic kernel. This is not quite the same thing as "building all your modules into the kernel", although the idea is the same. There are things called microkernels which have modules which get their own codespace. The [GNU Hurd](https://www.gnu.org/software/hurd/) and the [Zircon kernel](https://fuchsia.dev/fuchsia-src/concepts/kernel) of Google Fuchsia are two examples of a microkernel.  
顺便说一句，我想指出，上述讨论对于任何使用单片内核的操作系统都是正确的。尽管想法是相同的，但这与“将所有模块构建到内核中”并不完全相同。有些东西称为微内核，它们的模块有自己的代码空间。 GNU Hurd 和 Google Fuchsia 的 Zircon 内核是微内核的两个示例。

#### 5.6 Device Drivers 5.6 设备驱动程序

One class of module is the device driver, which provides functionality for hardware like a serial port. On Unix, each piece of hardware is represented by a file located in /dev named a device file which provides the means to communicate with the hardware. The device driver provides the communication on behalf of a user program. So the es1370.ko sound card device driver might connect the /dev/sound device file to the Ensoniq IS1370 sound card. A userspace program like mp3blaster can use /dev/sound without ever knowing what kind of sound card is installed.  
一类模块是设备驱动程序，它为串行端口等硬件提供功能。在 Unix 上，每个硬件都由位于 /dev 中的一个名为设备文件的文件表示，该文件提供与硬件通信的方式。设备驱动程序代表用户程序提供通信。因此 es1370.ko 声卡设备驱动程序可能会将 /dev/sound 设备文件连接到 Ensoniq IS1370 声卡。像 mp3blaster 这样的用户空间程序可以使用 /dev/sound，而无需知道安装了哪种声卡。

Let’s look at some device files. Here are device files which represent the first three partitions on the primary master IDE hard drive:  
让我们看一些设备文件。以下是代表主要主 IDE 硬盘驱动器上前三个分区的设备文件：

```bash
$ ls -l /dev/hda[1-3]
brw-rw----  1 root  disk  3, 1 Jul  5  2000 /dev/hda1
brw-rw----  1 root  disk  3, 2 Jul  5  2000 /dev/hda2
brw-rw----  1 root  disk  3, 3 Jul  5  2000 /dev/hda3
```

Notice the column of numbers separated by a comma. The first number is called the device’s major number. The second number is the minor number. The major number tells you which driver is used to access the hardware. Each driver is assigned a unique major number; all device files with the same major number are controlled by the same driver. All the above major numbers are 3, because they’re all controlled by the same driver.  
请注意用逗号分隔的数字列。第一个数字称为设备的主号码。第二个数字是次要数字。主设备号告诉您使用哪个驱动程序来访问硬件。每个驱动程序都被分配了一个唯一的主编号；所有具有相同主设备号的设备文件都由同一个驱动程序控制。上面所有的主编号都是 3，因为它们都是由同一个驱动程序控制的。

The minor number is used by the driver to distinguish between the various hardware it controls. Returning to the example above, although all three devices are handled by the same driver they have unique minor numbers because the driver sees them as being different pieces of hardware.  
驱动程序使用次设备号来区分它控制的各种硬件。回到上面的示例，尽管所有三个设备都由同一驱动程序处理，但它们具有唯一的次设备号，因为驱动程序将它们视为不同的硬件。

Devices are divided into two types: character devices and block devices. The difference is that block devices have a buffer for requests, so they can choose the best order in which to respond to the requests. This is important in the case of storage devices, where it is faster to read or write sectors which are close to each other, rather than those which are further apart. Another difference is that block devices can only accept input and return output in blocks (whose size can vary according to the device), whereas character devices are allowed to use as many or as few bytes as they like. Most devices in the world are character, because they don’t need this type of buffering, and they don’t operate with a fixed block size. You can tell whether a device file is for a block device or a character device by looking at the first character in the output of `ls -l` . If it is ‘b’ then it is a block device, and if it is ‘c’ then it is a character device. The devices you see above are block devices. Here are some character devices (the serial ports):  
设备分为两种类型：字符设备和块设备。不同之处在于块设备有一个用于请求的缓冲区，因此它们可以选择响应请求的最佳顺序。这对于存储设备来说很重要，因为在存储设备中读取或写入彼此靠近的扇区比那些相距较远的扇区更快。另一个区别是块设备只能以块的形式接受输入和返回输出（块的大小可能根据设备而变化），而字符设备则允许使用任意多或尽可能少的字节。世界上大多数设备都是字符设备，因为它们不需要这种类型的缓冲，并且它们不以固定的块大小运行。您可以通过查看 `ls -l` 输出中的第一个字符来判断设备文件是用于块设备还是字符设备。如果是“b”，则它是块设备，如果是“c”，则它是字符设备。您在上面看到的设备是块设备。以下是一些字符设备（串行端口）：

```bash
crw-rw----  1 root  dial 4, 64 Feb 18 23:34 /dev/ttyS0
crw-r-----  1 root  dial 4, 65 Nov 17 10:26 /dev/ttyS1
crw-rw----  1 root  dial 4, 66 Jul  5  2000 /dev/ttyS2
crw-rw----  1 root  dial 4, 67 Jul  5  2000 /dev/ttyS3
```

If you want to see which major numbers have been assigned, you can look at [Documentation/admin-guide/devices.txt](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/Documentation/admin-guide/devices.txt).  
如果您想查看已分配了哪些主编号，可以查看 Documentation/admin-guide/devices.txt。

When the system was installed, all of those device files were created by the `mknod` command. To create a new char device named coffee with major/minor number 12 and 2, simply do `mknod /dev/coffee c 12 2` . You do not have to put your device files into /dev, but it is done by convention. Linus put his device files in /dev, and so should you. However, when creating a device file for testing purposes, it is probably OK to place it in your working directory where you compile the kernel module. Just be sure to put it in the right place when you’re done writing the device driver.  
当系统安装时，所有这些设备文件都是由 `mknod` 命令创建的。要创建一个名为 Coffee 且主次编号为 12 和 2 的新字符设备，只需执行 `mknod /dev/coffee c 12 2` 即可。您不必将设备文件放入 /dev，但这是按照惯例完成的。 Linus 将他的设备文件放在 /dev 中，你也应该这样做。但是，当创建用于测试目的的设备文件时，将其放置在编译内核模块的工作目录中可能是可以的。编写完设备驱动程序后，请务必将其放在正确的位置。

I would like to make a few last points which are implicit from the above discussion, but I would like to make them explicit just in case. When a device file is accessed, the kernel uses the major number of the file to determine which driver should be used to handle the access. This means that the kernel doesn’t really need to use or even know about the minor number. The driver itself is the only thing that cares about the minor number. It uses the minor number to distinguish between different pieces of hardware.  
我想提出上述讨论中隐含的最后几点，但为了以防万一，我想将它们明确化。当访问设备文件时，内核使用文件的主设备号来确定应使用哪个驱动程序来处理访问。这意味着内核实际上不需要使用甚至不需要知道次要编号。驱动程序本身是唯一关心次要号码的东西。它使用次要编号来区分不同的硬件。

By the way, when I say "hardware", I mean something a bit more abstract than a PCI card that you can hold in your hand. Look at these two device files:  
顺便说一句，当我说“硬件”时，我指的是比你可以握在手中的 PCI 卡更抽象的东西。查看这两个设备文件：

```bash
$ ls -l /dev/sda /dev/sdb
brw-rw---- 1 root disk 8,  0 Jan  3 09:02 /dev/sda
brw-rw---- 1 root disk 8, 16 Jan  3 09:02 /dev/sdb
```

By now you can look at these two device files and know instantly that they are block devices and are handled by same driver (block major 8). Sometimes two device files with the same major but different minor number can actually represent the same piece of physical hardware. So just be aware that the word “hardware” in our discussion can mean something very abstract.  
现在您可以查看这两个设备文件并立即知道它们是块设备并且由相同的驱动程序处理（块主要 8）。有时，两个具有相同主设备号但不同次设备号的设备文件实际上可以代表同一块物理硬件。因此请注意，我们讨论中的“硬件”一词可能意味着非常抽象的东西。

### 6 Character Device drivers

6 字符设备驱动程序

#### 6.1 The file_operations Structure

6.1 文件操作结构

The `file_operations` structure is defined in [include/linux/fs.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/fs.h), and holds pointers to functions defined by the driver that perform various operations on the device. Each field of the structure corresponds to the address of some function defined by the driver to handle a requested operation.  
`file_operations` 结构在 include/linux/fs.h 中定义，并保存指向驱动程序定义的函数的指针，这些函数在设备上执行各种操作。该结构的每个字段对应于驱动程序定义的某个函数的地址，用于处理请求的操作。

For example, every character driver needs to define a function that reads from the device. The `file_operations` structure holds the address of the module’s function that performs that operation. Here is what the definition looks like for kernel 5.4:  
例如，每个字符驱动程序都需要定义一个从设备读取的函数。 `file_operations` 结构保存执行该操作的模块函数的地址。以下是内核 5.4 的定义：

```c
struct file_operations {
    struct module *owner;
    loff_t (*llseek) (struct file *, loff_t, int);
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
    ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
    int (*iopoll)(struct kiocb *kiocb, bool spin);
    int (*iterate) (struct file *, struct dir_context *);
    int (*iterate_shared) (struct file *, struct dir_context *);
    __poll_t (*poll) (struct file *, struct poll_table_struct *);
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
    long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
    int (*mmap) (struct file *, struct vm_area_struct *);
    unsigned long mmap_supported_flags;
    int (*open) (struct inode *, struct file *);
    int (*flush) (struct file *, fl_owner_t id);
    int (*release) (struct inode *, struct file *);
    int (*fsync) (struct file *, loff_t, loff_t, int datasync);
    int (*fasync) (int, struct file *, int);
    int (*lock) (struct file *, int, struct file_lock *);
    ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
    unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
    int (*check_flags)(int);
    int (*flock) (struct file *, int, struct file_lock *);
    ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
    ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
    int (*setlease)(struct file *, long, struct file_lock **, void **);
    long (*fallocate)(struct file *file, int mode, loff_t offset,
        loff_t len);
    void (*show_fdinfo)(struct seq_file *m, struct file *f);
    ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
        loff_t, size_t, unsigned int);
    loff_t (*remap_file_range)(struct file *file_in, loff_t pos_in,
             struct file *file_out, loff_t pos_out,
             loff_t len, unsigned int remap_flags);
    int (*fadvise)(struct file *, loff_t, loff_t, int);
} __randomize_layout;
```

Some operations are not implemented by a driver. For example, a driver that handles a video card will not need to read from a directory structure. The corresponding entries in the `file_operations` structure should be set to `NULL` .  
有些操作不是由驱动程序实现的。例如，处理视频卡的驱动程序不需要从目录结构中读取。 `file_operations` 结构中的相应条目应设置为 `NULL` 。

There is a gcc extension that makes assigning to this structure more convenient. You will see it in modern drivers, and may catch you by surprise. This is what the new way of assigning to the structure looks like:  
有一个 gcc 扩展可以使分配给这个结构更加方便。您会在现代驾驶员中看到它，并且可能会让您感到惊讶。这就是分配给结构的新方法：

```c
struct file_operations fops = {
    read: device_read,
    write: device_write,
    open: device_open,
    release: device_release
};
```

However, there is also a C99 way of assigning to elements of a structure, [designated initializers](https://gcc.gnu.org/onlinedocs/gcc/Designated-Inits.html), and this is definitely preferred over using the GNU extension. You should use this syntax in case someone wants to port your driver. It will help with compatibility:  
然而，还有一种 C99 方法可以分配给结构的元素、指定的初始值设定项，这绝对优于使用 GNU 扩展。如果有人想要移植您的驱动程序，您应该使用此语法。它将有助于兼容性：

```c
struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};
```

The meaning is clear, and you should be aware that any member of the structure which you do not explicitly assign will be initialized to `NULL` by gcc.  
含义很清楚，您应该知道，您未显式分配的结构体中的任何成员都将被 gcc 初始化为 `NULL` 。

An instance of `struct file_operations` containing pointers to functions that are used to implement `read` , `write` , `open` , … system calls is commonly named `fops` .  
`struct file_operations` 的实例包含指向用于实现 `read` 、 `write` 、 `open` 等系统调用的函数的指针，通常命名为 < b4> 。

Since Linux v3.14, the read, write and seek operations are guaranteed for thread-safe by using the `f_pos` specific lock, which makes the file position update to become the mutual exclusion. So, we can safely implement those operations without unnecessary locking.  
从 Linux v3.14 开始，通过使用 `f_pos` 特定锁来保证读、写和查找操作的线程安全，这使得文件位置更新成为互斥。因此，我们可以安全地实现这些操作，而无需不必要的锁定。

Additionally, since Linux v5.6, the `proc_ops` structure was introduced to replace the use of the `file_operations` structure when registering proc handlers. See more information in the [7.1](https://sysprog21.github.io/lkmpg/#the-procops-structure) section.  
此外，从 Linux v5.6 开始，在注册 proc 处理程序时引入了 `proc_ops` 结构来替换 `file_operations` 结构的使用。更多信息请参见 7.1 节。

#### 6.2 The file structure 6.2 文件结构

Each device is represented in the kernel by a file structure, which is defined in [include/linux/fs.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/fs.h). Be aware that a file is a kernel level structure and never appears in a user space program. It is not the same thing as a `FILE` , which is defined by glibc and would never appear in a kernel space function. Also, its name is a bit misleading; it represents an abstract open ‘file’, not a file on a disk, which is represented by a structure named `inode` .  
每个设备在内核中都由一个文件结构表示，该结构在 include/linux/fs.h 中定义。请注意，文件是内核级结构，永远不会出现在用户空间程序中。它与 `FILE` 不同，后者由 glibc 定义，永远不会出现在内核空间函数中。另外，它的名字有点误导；它代表一个抽象的打开“文件”，而不是磁盘上的文件，它由名为 `inode` 的结构表示。

An instance of struct file is commonly named `filp` . You’ll also see it referred to as a struct file object. Resist the temptation.  
struct file 的实例通常命名为 `filp` 。您还会看到它被称为结构文件对象。抵制诱惑。

Go ahead and look at the definition of file. Most of the entries you see, like struct dentry are not used by device drivers, and you can ignore them. This is because drivers do not fill file directly; they only use structures contained in file which are created elsewhere.  
继续看一下文件的定义。您看到的大多数条目（例如 struct dentry）不被设备驱动程序使用，您可以忽略它们。这是因为驱动程序不直接填充文件；他们只使用在其他地方创建的文件中包含的结构。

#### 6.3 Registering A Device 6.3 注册设备

As discussed earlier, char devices are accessed through device files, usually located in /dev. This is by convention. When writing a driver, it is OK to put the device file in your current directory. Just make sure you place it in /dev for a production driver. The major number tells you which driver handles which device file. The minor number is used only by the driver itself to differentiate which device it is operating on, just in case the driver handles more than one device.  
如前所述，字符设备是通过设备文件访问的，通常位于 /dev 中。这是按照惯例。编写驱动程序时，将设备文件放在当前目录下就可以了。只需确保将其放置在 /dev 中作为生产驱动程序。主设备号告诉您哪个驱动程序处理哪个设备文件。次设备号仅由驱动程序本身用来区分正在运行的设备，以防驱动程序处理多个设备。

Adding a driver to your system means registering it with the kernel. This is synonymous with assigning it a major number during the module’s initialization. You do this by using the `register_chrdev` function, defined by [include/linux/fs.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/fs.h).  
将驱动程序添加到系统意味着将其注册到内核。这与在模块初始化期间为其分配主编号是同义的。您可以使用 include/linux/fs.h 定义的 `register_chrdev` 函数来执行此操作。

```c
int register_chrdev(unsigned int major, const char *name, struct file_operations *fops);
```

Where unsigned int major is the major number you want to request, `const char *name` is the name of the device as it will appear in /proc/devices and `struct file_operations *fops` is a pointer to the `file_operations` table for your driver. A negative return value means the registration failed. Note that we didn’t pass the minor number to `register_chrdev` . That is because the kernel doesn’t care about the minor number; only our driver uses it.  
其中 unsigned int Major 是您要请求的主设备号， `const char *name` 是将出现在 /proc/devices 中的设备名称， `struct file_operations *fops` 是指向 `file_operations` 为您的司机准备的表格。返回值为负数表示注册失败。请注意，我们没有将次要编号传递给 `register_chrdev` 。这是因为内核不关心次设备号；只有我们的司机使用它。

Now the question is, how do you get a major number without hijacking one that’s already in use? The easiest way would be to look through [Documentation/admin-guide/devices.txt](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/Documentation/admin-guide/devices.txt) and pick an unused one. That is a bad way of doing things because you will never be sure if the number you picked will be assigned later. The answer is that you can ask the kernel to assign you a dynamic major number.  
现在的问题是，如何在不劫持已在使用的号码的情况下获得主号码？最简单的方法是查看 Documentation/admin-guide/devices.txt 并选择一个未使用的。这是一种糟糕的做法，因为您永远无法确定您选择的号码稍后是否会被分配。答案是，您可以要求内核为您分配一个动态主编号。

If you pass a major number of 0 to `register_chrdev` , the return value will be the dynamically allocated major number. The downside is that you can not make a device file in advance, since you do not know what the major number will be. There are a couple of ways to do this. First, the driver itself can print the newly assigned number and we can make the device file by hand. Second, the newly registered device will have an entry in /proc/devices, and we can either make the device file by hand or write a shell script to read the file in and make the device file. The third method is that we can have our driver make the device file using the `device_create` function after a successful registration and `device_destroy` during the call to `cleanup_module` .  
如果将主设备号 0 传递给 `register_chrdev` ，则返回值将是动态分配的主设备号。缺点是您无法提前创建设备文件，因为您不知道主设备号是什么。有几种方法可以做到这一点。首先，驱动程序本身可以打印新分配的编号，我们可以手工制作设备文件。其次，新注册的设备将在/proc/devices 中有一个条目，我们可以手动制作设备文件，也可以编写 shell 脚本读取该文件并制作设备文件。第三种方法是，我们可以让驱动程序在成功注册后使用 `device_create` 函数创建设备文件，并在调用 `cleanup_module` 期间使用 `device_destroy` 函数创建设备文件。

However, `register_chrdev()` would occupy a range of minor numbers associated with the given major. The recommended way to reduce waste for char device registration is using cdev interface.  
但是， `register_chrdev()` 将占用与给定主要相关的一系列次要编号。减少 char 设备注册浪费的推荐方法是使用 cdev 接口。

The newer interface completes the char device registration in two distinct steps. First, we should register a range of device numbers, which can be completed with `register_chrdev_region` or `alloc_chrdev_region` .  
新的界面通过两个不同的步骤完成字符设备注册。首先，我们应该注册一系列设备编号，可以用 `register_chrdev_region` 或 `alloc_chrdev_region` 来完成。

```c
int register_chrdev_region(dev_t from, unsigned count, const char *name);
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
```

The choice between two different functions depends on whether you know the major numbers for your device. Using `register_chrdev_region` if you know the device major number and `alloc_chrdev_region` if you would like to allocate a dynamicly-allocated major number.  
两种不同功能之间的选择取决于您是否知道设备的主编号。如果您知道设备主设备号，请使用 `register_chrdev_region` ；如果您想分配动态分配的主设备号，请使用 `alloc_chrdev_region` 。

Second, we should initialize the data structure `struct cdev` for our char device and associate it with the device numbers. To initialize the `struct cdev` , we can achieve by the similar sequence of the following codes.  
其次，我们应该初始化字符设备的数据结构 `struct cdev` 并将其与设备号相关联。要初始化 `struct cdev` ，我们可以通过以下代码的类似序列来实现。

```c
struct cdev *my_dev = cdev_alloc();
my_cdev->ops = &my_fops;
```

However, the common usage pattern will embed the `struct cdev` within a device-specific structure of your own. In this case, we’ll need `cdev_init` for the initialization.  
但是，常见的使用模式会将 `struct cdev` 嵌入到您自己的设备特定结构中。在这种情况下，我们需要 `cdev_init` 进行初始化。

```c
void cdev_init(struct cdev *cdev, const struct file_operations *fops);
```

Once we finish the initialization, we can add the char device to the system by using the `cdev_add` .  
完成初始化后，我们可以使用 `cdev_add` 将字符设备添加到系统中。

```c
int cdev_add(struct cdev *p, dev_t dev, unsigned count);
```

To find an example using the interface, you can see ioctl.c described in section [9](https://sysprog21.github.io/lkmpg/#talking-to-device-files).  
要查找使用该接口的示例，您可以参见第 9 节中描述的 ioctl.c。

#### 6.4 Unregistering A Device

6.4 注销设备

We can not allow the kernel module to be `rmmod` ’ed whenever root feels like it. If the device file is opened by a process and then we remove the kernel module, using the file would cause a call to the memory location where the appropriate function (read/write) used to be. If we are lucky, no other code was loaded there, and we’ll get an ugly error message. If we are unlucky, another kernel module was loaded into the same location, which means a jump into the middle of another function within the kernel. The results of this would be impossible to predict, but they can not be very positive.  
只要 root 愿意，我们就不能允许内核模块被 `rmmod` 编辑。如果设备文件被进程打开，然后我们删除内核模块，则使用该文件将导致调用适当函数（读/写）曾经所在的内存位置。如果幸运的话，那里没有加载其他代码，我们会收到一条丑陋的错误消息。如果我们运气不好，另一个内核模块会被加载到同一位置，这意味着跳转到内核中另一个函数的中间。其结果无法预测，但也不会非常积极。

Normally, when you do not want to allow something, you return an error code (a negative number) from the function which is supposed to do it. With `cleanup_module` that’s impossible because it is a void function. However, there is a counter which keeps track of how many processes are using your module. You can see what its value is by looking at the 3rd field with the command `cat /proc/modules` or `sudo lsmod` . If this number isn’t zero, `rmmod` will fail. Note that you do not have to check the counter within `cleanup_module` because the check will be performed for you by the system call `sys_delete_module` , defined in [include/linux/syscalls.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/syscalls.h). You should not use this counter directly, but there are functions defined in [include/linux/module.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/module.h) which let you increase, decrease and display this counter:  
通常，当您不想允许某些操作时，您会从应该执行该操作的函数返回错误代码（负数）。对于 `cleanup_module` 这是不可能的，因为它是一个 void 函数。但是，有一个计数器可以跟踪有多少进程正在使用您的模块。您可以通过使用命令 `cat /proc/modules` 或 `sudo lsmod` 查看第三个字段来查看其值。如果该数字不为零， `rmmod` 将失败。请注意，您不必检查 `cleanup_module` 中的计数器，因为该检查将由 include/linux/syscalls.h 中定义的系统调用 `sys_delete_module` 为您执行。你不应该直接使用这个计数器，但是 include/linux/module.h 中定义了一些函数，可以让你增加、减少和显示这个计数器：

- `try_module_get(THIS_MODULE)` : Increment the reference count of current module.  
  `try_module_get(THIS_MODULE)` ：增加当前模块的引用计数。
- `module_put(THIS_MODULE)` : Decrement the reference count of current module.  
  `module_put(THIS_MODULE)` ：减少当前模块的引用计数。
- `module_refcount(THIS_MODULE)` : Return the value of reference count of current module.  
  `module_refcount(THIS_MODULE)` ：返回当前模块的引用计数值。

It is important to keep the counter accurate; if you ever do lose track of the correct usage count, you will never be able to unload the module; it’s now reboot time, boys and girls. This is bound to happen to you sooner or later during a module’s development.  
保持计数器准确非常重要；如果您确实忘记了正确的使用计数，您将永远无法卸载该模块；现在是重新启动的时间，男孩和女孩。在模块的开发过程中，这种情况迟早会发生在你身上。

#### 6.5 chardev.c

The next code sample creates a char driver named chardev. You can dump its device file.  
下一个代码示例创建一个名为 chardev 的字符驱动程序。您可以转储其设备文件。

```
cat /proc/devices
```

(or open the file with a program) and the driver will put the number of times the device file has been read from into the file. We do not support writing to the file (like `echo "hi" > /dev/hello` ), but catch these attempts and tell the user that the operation is not supported. Don’t worry if you don’t see what we do with the data we read into the buffer; we don’t do much with it. We simply read in the data and print a message acknowledging that we received it.  
（或用程序打开文件），驱动程序会将设备文件被读取的次数放入文件中。我们不支持写入文件（如 `echo "hi" > /dev/hello` ），但会捕获这些尝试并告诉用户不支持该操作。如果您看不到我们如何处理读入缓冲区的数据，请不要担心；我们并没有用它做太多事情。我们只需读入数据并打印一条消息来确认我们收到了它。

In the multiple-threaded environment, without any protection, concurrent access to the same memory may lead to the race condition, and will not preserve the performance. In the kernel module, this problem may happen due to multiple instances accessing the shared resources. Therefore, a solution is to enforce the exclusive access. We use atomic Compare-And-Swap (CAS) to maintain the states, `CDEV_NOT_USED` and `CDEV_EXCLUSIVE_OPEN` , to determine whether the file is currently opened by someone or not. CAS compares the contents of a memory location with the expected value and, only if they are the same, modifies the contents of that memory location to the desired value. See more concurrency details in the [12](https://sysprog21.github.io/lkmpg/#avoiding-collisions-and-deadlocks) section.  
在多线程环境下，在没有任何保护的情况下，并发访问同一内存可能会导致竞争条件，并且不会保留性能。在内核模块中，由于多个实例访问共享资源，可能会出现此问题。因此，解决方案是强制执行独占访问。我们使用原子比较和交换（CAS）来维护状态 `CDEV_NOT_USED` 和 `CDEV_EXCLUSIVE_OPEN` ，以确定文件当前是否被某人打开。 CAS 将内存位置的内容与预期值进行比较，只有当它们相同时，才会将该内存位置的内容修改为所需值。请参阅第 12 节中的更多并发详细信息。

```
/*
 * chardev.c: Creates a read-only char device that says how many times
 * you have read from the dev file
 */

#include <linux/atomic.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h> /* for sprintf() */
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h> /* for get_user and put_user */
#include <linux/version.h>

#include <asm/errno.h>

/*  Prototypes - this would normally go in a .h file */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t,
                            loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev" /* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80 /* Max length of the message from the device */

/* Global variables are declared as static, so are global within the file. */

static int major; /* major number assigned to our device driver */

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};

/* Is device open? Used to prevent multiple access to device */
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

static char msg[BUF_LEN + 1]; /* The msg the device will give when asked */

static struct class *cls;

static struct file_operations chardev_fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

static int __init chardev_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops);

    if (major < 0) {
        pr_alert("Registering char device failed with %d\n", major);
        return major;
    }

    pr_info("I was assigned major number %d.\n", major);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    cls = class_create(DEVICE_NAME);
#else
    cls = class_create(THIS_MODULE, DEVICE_NAME);
#endif
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    pr_info("Device created on /dev/%s\n", DEVICE_NAME);

    return SUCCESS;
}

static void __exit chardev_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);

    /* Unregister the device */
    unregister_chrdev(major, DEVICE_NAME);
}

/* Methods */

/* Called when a process tries to open the device file, like
 * "sudo cat /dev/chardev"
 */
static int device_open(struct inode *inode, struct file *file)
{
    static int counter = 0;

    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;

    sprintf(msg, "I already told you %d times Hello world!\n", counter++);
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

/* Called when a process closes the device file. */
static int device_release(struct inode *inode, struct file *file)
{
    /* We're now ready for our next caller */
    atomic_set(&already_open, CDEV_NOT_USED);

    /* Decrement the usage count, or else once you opened the file, you will
     * never get rid of the module.
     */
    module_put(THIS_MODULE);

    return SUCCESS;
}

/* Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp, /* see include/linux/fs.h   */
                           char __user *buffer, /* buffer to fill with data */
                           size_t length, /* length of the buffer     */
                           loff_t *offset)
{
    /* Number of bytes actually written to the buffer */
    int bytes_read = 0;
    const char *msg_ptr = msg;

    if (!*(msg_ptr + *offset)) { /* we are at the end of message */
        *offset = 0; /* reset the offset */
        return 0; /* signify end of file */
    }

    msg_ptr += *offset;

    /* Actually put the data into the buffer */
    while (length && *msg_ptr) {
        /* The buffer is in the user data segment, not the kernel
         * segment so "*" assignment won't work.  We have to use
         * put_user which copies data from the kernel data segment to
         * the user data segment.
         */
        put_user(*(msg_ptr++), buffer++);
        length--;
        bytes_read++;
    }

    *offset += bytes_read;

    /* Most read functions return the number of bytes put into the buffer. */
    return bytes_read;
}

/* Called when a process writes to dev file: echo "hi" > /dev/hello */
static ssize_t device_write(struct file *filp, const char __user *buff,
                            size_t len, loff_t *off)
{
    pr_alert("Sorry, this operation is not supported.\n");
    return -EINVAL;
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
```

#### 6.6 Writing Modules for Multiple Kernel Versions

6.6 为多个内核版本编写模块

The system calls, which are the major interface the kernel shows to the processes, generally stay the same across versions. A new system call may be added, but usually the old ones will behave exactly like they used to. This is necessary for backward compatibility – a new kernel version is not supposed to break regular processes. In most cases, the device files will also remain the same. On the other hand, the internal interfaces within the kernel can and do change between versions.  
系统调用是内核向进程显示的主要接口，通常在各个版本中保持相同。可能会添加新的系统调用，但通常旧的系统调用的行为与以前完全相同。这对于向后兼容是必要的——新的内核版本不应该破坏常规进程。在大多数情况下，设备文件也将保持不变。另一方面，内核中的内部接口可以并且确实在版本之间发生变化。

There are differences between different kernel versions, and if you want to support multiple kernel versions, you will find yourself having to code conditional compilation directives. The way to do this to compare the macro `LINUX_VERSION_CODE` to the macro `KERNEL_VERSION` . In version a.b.c of the kernel, the value of this macro would be ![216a+ 28b+ c  ](https://sysprog21.github.io/lkmpg/lkmpg-for-ht0x.svg).  
不同的内核版本之间存在差异，如果你想支持多个内核版本，你会发现自己必须编写条件编译指令。执行此操作的方法是将宏 `LINUX_VERSION_CODE` 与宏 `KERNEL_VERSION` 进行比较。在内核版本 a.b.c 中，该宏的值为 ![216a+ 28b+ c  ](https://sysprog21.github.io/lkmpg/lkmpg-for-ht0x.svg) 。

### 7 The /proc File System

7 /proc 文件系统

In Linux, there is an additional mechanism for the kernel and kernel modules to send information to processes — the /proc file system. Originally designed to allow easy access to information about processes (hence the name), it is now used by every bit of the kernel which has something interesting to report, such as /proc/modules which provides the list of modules and /proc/meminfo which gathers memory usage statistics.  
在 Linux 中，内核和内核模块有一个额外的机制来向进程发送信息 - /proc 文件系统。最初的设计目的是允许轻松访问有关进程的信息（因此得名），现在它被内核的每个部分使用，其中有一些有趣的报告，例如提供模块列表的 /proc/modules 和 /proc/meminfo 它收集内存使用统计信息。

The method to use the proc file system is very similar to the one used with device drivers — a structure is created with all the information needed for the /proc file, including pointers to any handler functions (in our case there is only one, the one called when somebody attempts to read from the /proc file). Then, `init_module` registers the structure with the kernel and `cleanup_module` unregisters it.  
使用 proc 文件系统的方法与设备驱动程序使用的方法非常相似 — 使用 /proc 文件所需的所有信息创建一个结构，包括指向任何处理函数的指针（在我们的例子中只有一个，当有人尝试读取 /proc 文件时调用）。然后， `init_module` 向内核注册该结构并 `cleanup_module` 取消注册它。

Normal file systems are located on a disk, rather than just in memory (which is where /proc is), and in that case the index-node (inode for short) number is a pointer to a disk location where the file’s inode is located. The inode contains information about the file, for example the file’s permissions, together with a pointer to the disk location or locations where the file’s data can be found.  
普通文件系统位于磁盘上，而不仅仅是内存中（/proc 所在的位置），在这种情况下，索引节点（简称 inode）编号是指向文件 inode 所在磁盘位置的指针。索引节点包含有关文件的信息，例如文件的权限，以及指向磁盘位置或可以找到文件数据的位置的指针。

Because we don’t get called when the file is opened or closed, there’s nowhere for us to put `try_module_get` and `module_put` in this module, and if the file is opened and then the module is removed, there’s no way to avoid the consequences.  
因为当文件打开或关闭时我们不会被调用，所以我们没有地方可以将 `try_module_get` 和 `module_put` 放入此模块中，并且如果打开文件然后模块被删除了，后果是无法避免的。

Here a simple example showing how to use a /proc file. This is the HelloWorld for the /proc filesystem. There are three parts: create the file /proc/helloworld in the function `init_module` , return a value (and a buffer) when the file /proc/helloworld is read in the callback function `procfile_read` , and delete the file /proc/helloworld in the function `cleanup_module` .  
这里有一个简单的例子展示如何使用 /proc 文件。这是 /proc 文件系统的 HelloWorld。分为三个部分：在函数 `init_module` 中创建文件 /proc/helloworld，在回调函数中读取文件 /proc/helloworld 时返回一个值（和一个缓冲区） `procfile_read` ，并删除函数 `cleanup_module` 中的文件 /proc/helloworld 。

The /proc/helloworld is created when the module is loaded with the function `proc_create` . The return value is a `struct proc_dir_entry` , and it will be used to configure the file /proc/helloworld (for example, the owner of this file). A null return value means that the creation has failed.  
/proc/helloworld 在使用函数 `proc_create` 加载模块时创建。返回值是 `struct proc_dir_entry` ，它将用于配置文件 /proc/helloworld （例如，该文件的所有者）。如果返回值为空，则表示创建失败。

Every time the file /proc/helloworld is read, the function `procfile_read` is called. Two parameters of this function are very important: the buffer (the second parameter) and the offset (the fourth one). The content of the buffer will be returned to the application which read it (for example the `cat` command). The offset is the current position in the file. If the return value of the function is not null, then this function is called again. So be careful with this function, if it never returns zero, the read function is called endlessly.  
每次读取文件 /proc/helloworld 时，都会调用函数 `procfile_read` 。该函数有两个参数非常重要：缓冲区（第二个参数）和偏移量（第四个参数）。缓冲区的内容将返回到读取它的应用程序（例如 `cat` 命令）。偏移量是文件中的当前位置。如果函数的返回值不为空，则再次调用该函数。所以要小心这个函数，如果它从不返回零，那么读取函数就会被无休止地调用。

```bash
$ cat /proc/helloworld
HelloWorld!
```

```c
/*
 * procfs1.c
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define procfs_name "helloworld"

static struct proc_dir_entry *our_proc_file;

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer,
                             size_t buffer_length, loff_t *offset)
{
    char s[13] = "HelloWorld!\n";
    int len = sizeof(s);
    ssize_t ret = len;

    if (*offset >= len || copy_to_user(buffer, s, len)) {
        pr_info("copy_to_user failed\n");
        ret = 0;
    } else {
        pr_info("procfile read %s\n", file_pointer->f_path.dentry->d_name.name);
        *offset += len;
    }

    return ret;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
};
#endif

static int __init procfs1_init(void)
{
    our_proc_file = proc_create(procfs_name, 0644, NULL, &proc_file_fops);
    if (NULL == our_proc_file) {
        proc_remove(our_proc_file);
        pr_alert("Error:Could not initialize /proc/%s\n", procfs_name);
        return -ENOMEM;
    }

    pr_info("/proc/%s created\n", procfs_name);
    return 0;
}

static void __exit procfs1_exit(void)
{
    proc_remove(our_proc_file);
    pr_info("/proc/%s removed\n", procfs_name);
}

module_init(procfs1_init);
module_exit(procfs1_exit);

MODULE_LICENSE("GPL");
```

#### 7.1 The proc_ops Structure

7.1 proc_ops 结构

The `proc_ops` structure is defined in [include/linux/proc_fs.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/proc_fs.h) in Linux v5.6+. In older kernels, it used `file_operations` for custom hooks in /proc file system, but it contains some members that are unnecessary in VFS, and every time VFS expands `file_operations` set, /proc code comes bloated. On the other hand, not only the space, but also some operations were saved by this structure to improve its performance. For example, the file which never disappears in /proc can set the `proc_flag` as `PROC_ENTRY_PERMANENT` to save 2 atomic ops, 1 allocation, 1 free in per open/read/close sequence.  
`proc_ops` 结构在 Linux v5.6+ 中的 include/linux/proc_fs.h 中定义。在较旧的内核中，它使用 `file_operations` 进行 /proc 文件系统中的自定义挂钩，但它包含一些 VFS 中不需要的成员，并且每次 VFS 扩展 `file_operations` 设置时，/proc 代码变得臃肿。另一方面，这种结构不仅节省了空间，还节省了一些操作，提高了性能。例如，/proc 中永不消失的文件可以将 `proc_flag` 设置为 `PROC_ENTRY_PERMANENT` ，以在每个打开/读取/关闭序列中保存 2 个原子操作、1 个分配、1 个空闲。

#### 7.2 Read and Write a /proc File

7.2 读写 /proc 文件

We have seen a very simple example for a /proc file where we only read the file /proc/helloworld. It is also possible to write in a /proc file. It works the same way as read, a function is called when the /proc file is written. But there is a little difference with read, data comes from user, so you have to import data from user space to kernel space (with `copy_from_user` or `get_user` )  
我们已经看到了一个非常简单的 /proc 文件示例，其中我们只读取文件 /proc/helloworld。也可以写入 /proc 文件中。它的工作方式与 read 相同，当写入/proc 文件时调用一个函数。但是和 read 有一点区别，数据来自用户，所以你必须将数据从用户空间导入到内核空间（使用 `copy_from_user` 或 `get_user` ）

The reason for `copy_from_user` or `get_user` is that Linux memory (on Intel architecture, it may be different under some other processors) is segmented. This means that a pointer, by itself, does not reference a unique location in memory, only a location in a memory segment, and you need to know which memory segment it is to be able to use it. There is one memory segment for the kernel, and one for each of the processes.  
`copy_from_user` 或 `get_user` 的原因是 Linux 内存（在 Intel 架构上，在某些其他处理器下可能有所不同）是分段的。这意味着指针本身并不引用内存中的唯一位置，而仅引用内存段中的位置，并且您需要知道它是哪个内存段才能使用它。内核有一个内存段，每个进程也有一个内存段。

The only memory segment accessible to a process is its own, so when writing regular programs to run as processes, there is no need to worry about segments. When you write a kernel module, normally you want to access the kernel memory segment, which is handled automatically by the system. However, when the content of a memory buffer needs to be passed between the currently running process and the kernel, the kernel function receives a pointer to the memory buffer which is in the process segment. The `put_user` and `get_user` macros allow you to access that memory. These functions handle only one character, you can handle several characters with `copy_to_user` and `copy_from_user` . As the buffer (in read or write function) is in kernel space, for write function you need to import data because it comes from user space, but not for the read function because data is already in kernel space.  
进程可以访问的唯一内存段是它自己的内存段，因此在编写常规程序作为进程运行时，无需担心段。当你编写内核模块时，通常你想要访问内核内存段，这是由系统自动处理的。然而，当内存缓冲区的内容需要在当前运行的进程和内核之间传递时，内核函数会收到一个指向进程段中的内存缓冲区的指针。 `put_user` 和 `get_user` 宏允许您访问该内存。这些函数仅处理一个字符，您可以使用 `copy_to_user` 和 `copy_from_user` 处理多个字符。由于缓冲区（在读或写函数中）位于内核空间中，因此对于写函数，您需要导入数据，因为它来自用户空间，但对于读函数则不需要导入数据，因为数据已经在内核空间中。

```c
/*
 * procfs2.c -  create a "file" in /proc
 */

#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/proc_fs.h> /* Necessary because we use the proc fs */
#include <linux/uaccess.h> /* for copy_from_user */
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROCFS_MAX_SIZE 1024
#define PROCFS_NAME "buffer1k"

/* This structure hold information about the /proc file */
static struct proc_dir_entry *our_proc_file;

/* The buffer used to store character for this module */
static char procfs_buffer[PROCFS_MAX_SIZE];

/* The size of the buffer */
static unsigned long procfs_buffer_size = 0;

/* This function is called then the /proc file is read */
static ssize_t procfile_read(struct file *file_pointer, char __user *buffer,
                             size_t buffer_length, loff_t *offset)
{
    char s[13] = "HelloWorld!\n";
    int len = sizeof(s);
    ssize_t ret = len;

    if (*offset >= len || copy_to_user(buffer, s, len)) {
        pr_info("copy_to_user failed\n");
        ret = 0;
    } else {
        pr_info("procfile read %s\n", file_pointer->f_path.dentry->d_name.name);
        *offset += len;
    }

    return ret;
}

/* This function is called with the /proc file is written. */
static ssize_t procfile_write(struct file *file, const char __user *buff,
                              size_t len, loff_t *off)
{
    procfs_buffer_size = len;
    if (procfs_buffer_size > PROCFS_MAX_SIZE)
        procfs_buffer_size = PROCFS_MAX_SIZE;

    if (copy_from_user(procfs_buffer, buff, procfs_buffer_size))
        return -EFAULT;

    procfs_buffer[procfs_buffer_size & (PROCFS_MAX_SIZE - 1)] = '\0';
    *off += procfs_buffer_size;
    pr_info("procfile write %s\n", procfs_buffer);

    return procfs_buffer_size;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
    .proc_write = procfile_write,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
    .write = procfile_write,
};
#endif

static int __init procfs2_init(void)
{
    our_proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_file_fops);
    if (NULL == our_proc_file) {
        pr_alert("Error:Could not initialize /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }

    pr_info("/proc/%s created\n", PROCFS_NAME);
    return 0;
}

static void __exit procfs2_exit(void)
{
    proc_remove(our_proc_file);
    pr_info("/proc/%s removed\n", PROCFS_NAME);
}

module_init(procfs2_init);
module_exit(procfs2_exit);

MODULE_LICENSE("GPL");
```

#### 7.3 Manage /proc file with standard filesystem

7.3 使用标准文件系统管理 /proc 文件

We have seen how to read and write a /proc file with the /proc interface. But it is also possible to manage /proc file with inodes. The main concern is to use advanced functions, like permissions.  
我们已经了解了如何使用 /proc 接口读写 /proc 文件。但也可以使用 inode 来管理 /proc 文件。主要关注的是使用高级功能，例如权限。

In Linux, there is a standard mechanism for file system registration. Since every file system has to have its own functions to handle inode and file operations, there is a special structure to hold pointers to all those functions, `struct inode_operations` , which includes a pointer to `struct proc_ops` .  
在 Linux 中，有一个标准的文件系统注册机制。由于每个文件系统都必须有自己的函数来处理 inode 和文件操作，因此有一个特殊的结构来保存指向所有这些函数的指针 `struct inode_operations` ，其中包括指向 `struct proc_ops` 的指针。

The difference between file and inode operations is that file operations deal with the file itself whereas inode operations deal with ways of referencing the file, such as creating links to it.  
文件和索引节点操作之间的区别在于，文件操作处理文件本身，而索引节点操作处理引用文件的方式，例如创建指向文件的链接。

In /proc, whenever we register a new file, we’re allowed to specify which `struct inode_operations` will be used to access to it. This is the mechanism we use, a `struct inode_operations` which includes a pointer to a `struct proc_ops` which includes pointers to our `procf_read` and `procfs_write` functions.  
在 /proc 中，每当我们注册一个新文件时，我们都可以指定使用哪个 `struct inode_operations` 来访问它。这是我们使用的机制， `struct inode_operations` 包含指向 `struct proc_ops` 的指针，而 `struct proc_ops` 包含指向 `procf_read` 和 `procfs_write` 函数的指针。

Another interesting point here is the `module_permission` function. This function is called whenever a process tries to do something with the /proc file, and it can decide whether to allow access or not. Right now it is only based on the operation and the uid of the current user (as available in current, a pointer to a structure which includes information on the currently running process), but it could be based on anything we like, such as what other processes are doing with the same file, the time of day, or the last input we received.  
这里另一个有趣的点是 `module_permission` 函数。每当进程尝试对 /proc 文件执行某些操作时都会调用此函数，并且它可以决定是否允许访问。现在它仅基于当前用户的操作和 uid（在 current 中可用，指向包含当前正在运行的进程的信息的结构的指针），但它可以基于我们喜欢的任何内容，例如其他进程正在处理同一文件、一天中的时间或我们收到的最后一个输入。

It is important to note that the standard roles of read and write are reversed in the kernel. Read functions are used for output, whereas write functions are used for input. The reason for that is that read and write refer to the user’s point of view — if a process reads something from the kernel, then the kernel needs to output it, and if a process writes something to the kernel, then the kernel receives it as input.  
值得注意的是，读和写的标准角色在内核中是相反的。读函数用于输出，而写函数用于输入。原因是读和写是指用户的观点——如果一个进程从内核读取一些东西，那么内核需要输出它，如果一个进程向内核写一些东西，那么内核会接收它输入。

```c
/*
 * procfs3.c
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#include <linux/minmax.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROCFS_MAX_SIZE 2048UL
#define PROCFS_ENTRY_FILENAME "buffer2k"

static struct proc_dir_entry *our_proc_file;
static char procfs_buffer[PROCFS_MAX_SIZE];
static unsigned long procfs_buffer_size = 0;

static ssize_t procfs_read(struct file *filp, char __user *buffer,
                           size_t length, loff_t *offset)
{
    if (*offset || procfs_buffer_size == 0) {
        pr_debug("procfs_read: END\n");
        *offset = 0;
        return 0;
    }
    procfs_buffer_size = min(procfs_buffer_size, length);
    if (copy_to_user(buffer, procfs_buffer, procfs_buffer_size))
        return -EFAULT;
    *offset += procfs_buffer_size;

    pr_debug("procfs_read: read %lu bytes\n", procfs_buffer_size);
    return procfs_buffer_size;
}
static ssize_t procfs_write(struct file *file, const char __user *buffer,
                            size_t len, loff_t *off)
{
    procfs_buffer_size = min(PROCFS_MAX_SIZE, len);
    if (copy_from_user(procfs_buffer, buffer, procfs_buffer_size))
        return -EFAULT;
    *off += procfs_buffer_size;

    pr_debug("procfs_write: write %lu bytes\n", procfs_buffer_size);
    return procfs_buffer_size;
}
static int procfs_open(struct inode *inode, struct file *file)
{
    try_module_get(THIS_MODULE);
    return 0;
}
static int procfs_close(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    return 0;
}

#ifdef HAVE_PROC_OPS
static struct proc_ops file_ops_4_our_proc_file = {
    .proc_read = procfs_read,
    .proc_write = procfs_write,
    .proc_open = procfs_open,
    .proc_release = procfs_close,
};
#else
static const struct file_operations file_ops_4_our_proc_file = {
    .read = procfs_read,
    .write = procfs_write,
    .open = procfs_open,
    .release = procfs_close,
};
#endif

static int __init procfs3_init(void)
{
    our_proc_file = proc_create(PROCFS_ENTRY_FILENAME, 0644, NULL,
                                &file_ops_4_our_proc_file);
    if (our_proc_file == NULL) {
        pr_debug("Error: Could not initialize /proc/%s\n",
                 PROCFS_ENTRY_FILENAME);
        return -ENOMEM;
    }
    proc_set_size(our_proc_file, 80);
    proc_set_user(our_proc_file, GLOBAL_ROOT_UID, GLOBAL_ROOT_GID);

    pr_debug("/proc/%s created\n", PROCFS_ENTRY_FILENAME);
    return 0;
}

static void __exit procfs3_exit(void)
{
    remove_proc_entry(PROCFS_ENTRY_FILENAME, NULL);
    pr_debug("/proc/%s removed\n", PROCFS_ENTRY_FILENAME);
}

module_init(procfs3_init);
module_exit(procfs3_exit);

MODULE_LICENSE("GPL");
```

Still hungry for procfs examples? Well, first of all keep in mind, there are rumors around, claiming that procfs is on its way out, consider using sysfs instead. Consider using this mechanism, in case you want to document something kernel related yourself.  
仍然渴望 procfs 示例吗？好吧，首先请记住，有传言称 procfs 即将被淘汰，请考虑使用 sysfs。如果您想自己记录与内核相关的内容，请考虑使用此机制。

#### 7.4 Manage /proc file with seq_file

7.4 使用 seq_file 管理/proc 文件

As we have seen, writing a /proc file may be quite “complex”. So to help people writing /proc file, there is an API named `seq_file` that helps formatting a /proc file for output. It is based on sequence, which is composed of 3 functions: `start()` , `next()` , and `stop()` . The `seq_file` API starts a sequence when a user read the /proc file.  
正如我们所见，编写 /proc 文件可能相当“复杂”。因此，为了帮助人们编写 /proc 文件，有一个名为 `seq_file` 的 API 可以帮助格式化 /proc 文件以进行输出。它基于序列，由 3 个函数组成： `start()` 、 `next()` 和 `stop()` 。当用户读取 /proc 文件时， `seq_file` API 启动一个序列。

A sequence begins with the call of the function `start()` . If the return is a non `NULL` value, the function `next()` is called. This function is an iterator, the goal is to go through all the data. Each time `next()` is called, the function `show()` is also called. It writes data values in the buffer read by the user. The function `next()` is called until it returns `NULL` . The sequence ends when `next()` returns `NULL` , then the function `stop()` is called.  
序列以调用函数 `start()` 开始。如果返回的是非 `NULL` 值，则调用函数 `next()` 。该函数是一个迭代器，目标是遍历所有数据。每次调用 `next()` 时，也会调用函数 `show()` 。它将数据值写入用户读取的缓冲区中。调用函数 `next()` 直到返回 `NULL` 。当 `next()` 返回 `NULL` 时序列结束，然后调用函数 `stop()` 。

BE CAREFUL: when a sequence is finished, another one starts. That means that at the end of function `stop()` , the function `start()` is called again. This loop finishes when the function `start()` returns `NULL` . You can see a scheme of this in the Figure [1](https://sysprog21.github.io/lkmpg/#ignorespaces-how-seqfile-works).  
请注意：一个序列完成后，另一个序列就会开始。这意味着在函数 `stop()` 结束时，函数 `start()` 被再次调用。当函数 `start()` 返回 `NULL` 时，此循环结束。您可以在图 1 中看到该方案。

![srYrsNNYtaeenetoooertusetupstrxr((ntn))( tis)istrr teeaNreNatUaUtmLtLmeLmLen?e?ntntt  ](https://sysprog21.github.io/lkmpg/lkmpg-for-ht1x.svg)

Figure 1:How seq_file works  
图 1：seq_file 的工作原理

The `seq_file` provides basic functions for `proc_ops` , such as `seq_read` , `seq_lseek` , and some others. But nothing to write in the /proc file. Of course, you can still use the same way as in the previous example.  
`seq_file` 为 `proc_ops` 提供基本功能，例如 `seq_read` 、 `seq_lseek` 等。但 /proc 文件中没有任何内容可写。当然，您仍然可以使用与上一个示例相同的方式。

```c
/*
 * procfs4.c -  create a "file" in /proc
 * This program uses the seq_file library to manage the /proc file.
 */

#include <linux/kernel.h> /* We are doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/proc_fs.h> /* Necessary because we use proc fs */
#include <linux/seq_file.h> /* for seq_file */
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROC_NAME "iter"

/* This function is called at the beginning of a sequence.
 * ie, when:
 *   - the /proc file is read (first time)
 *   - after the function stop (end of sequence)
 */
static void *my_seq_start(struct seq_file *s, loff_t *pos)
{
    static unsigned long counter = 0;

    /* beginning a new sequence? */
    if (*pos == 0) {
        /* yes => return a non null value to begin the sequence */
        return &counter;
    }

    /* no => it is the end of the sequence, return end to stop reading */
    *pos = 0;
    return NULL;
}

/* This function is called after the beginning of a sequence.
 * It is called untill the return is NULL (this ends the sequence).
 */
static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    unsigned long *tmp_v = (unsigned long *)v;
    (*tmp_v)++;
    (*pos)++;
    return NULL;
}

/* This function is called at the end of a sequence. */
static void my_seq_stop(struct seq_file *s, void *v)
{
    /* nothing to do, we use a static value in start() */
}

/* This function is called for each "step" of a sequence. */
static int my_seq_show(struct seq_file *s, void *v)
{
    loff_t *spos = (loff_t *)v;

    seq_printf(s, "%Ld\n", *spos);
    return 0;
}

/* This structure gather "function" to manage the sequence */
static struct seq_operations my_seq_ops = {
    .start = my_seq_start,
    .next = my_seq_next,
    .stop = my_seq_stop,
    .show = my_seq_show,
};

/* This function is called when the /proc file is open. */
static int my_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &my_seq_ops);
};

/* This structure gather "function" that manage the /proc file */
#ifdef HAVE_PROC_OPS
static const struct proc_ops my_file_ops = {
    .proc_open = my_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = seq_release,
};
#else
static const struct file_operations my_file_ops = {
    .open = my_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};
#endif

static int __init procfs4_init(void)
{
    struct proc_dir_entry *entry;

    entry = proc_create(PROC_NAME, 0, NULL, &my_file_ops);
    if (entry == NULL) {
        pr_debug("Error: Could not initialize /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    return 0;
}

static void __exit procfs4_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    pr_debug("/proc/%s removed\n", PROC_NAME);
}

module_init(procfs4_init);
module_exit(procfs4_exit);

MODULE_LICENSE("GPL");
```

If you want more information, you can read this web page:  
如果您想了解更多信息，您可以阅读此网页：

- [https://lwn.net/Articles/22355/](https://lwn.net/Articles/22355/)
- [https://kernelnewbies.org/Documents/SeqFileHowTo](https://kernelnewbies.org/Documents/SeqFileHowTo)

You can also read the code of [fs/seq_file.c](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/fs/seq_file.c) in the linux kernel.  
您还可以阅读 linux 内核中的 fs/seq_file.c 的代码。

### 8 sysfs: Interacting with your module

8 sysfs：与模块交互

sysfs allows you to interact with the running kernel from userspace by reading or setting variables inside of modules. This can be useful for debugging purposes, or just as an interface for applications or scripts. You can find sysfs directories and files under the /sys directory on your system.  
sysfs 允许您通过读取或设置模块内部的变量来从用户空间与正在运行的内核进行交互。这对于调试目的很有用，或者只是作为应用程序或脚本的接口。您可以在系统的 /sys 目录下找到 sysfs 目录和文件。

```
ls -l /sys
```

Attributes can be exported for kobjects in the form of regular files in the filesystem. Sysfs forwards file I/O operations to methods defined for the attributes, providing a means to read and write kernel attributes.  
可以以文件系统中常规文件的形式导出 kobject 的属性。 Sysfs 将文件 I/O 操作转发给为属性定义的方法，提供读写内核属性的方法。

An attribute definition in simply:  
简单的属性定义：

```c
struct attribute {
    char *name;
    struct module *owner;
    umode_t mode;
};

int sysfs_create_file(struct kobject * kobj, const struct attribute * attr);
void sysfs_remove_file(struct kobject * kobj, const struct attribute * attr);
```

For example, the driver model defines `struct device_attribute` like:  
例如，驱动程序模型定义 `struct device_attribute` 如下：

```c
struct device_attribute {
    struct attribute attr;
    ssize_t (*show)(struct device *dev, struct device_attribute *attr,
                    char *buf);
    ssize_t (*store)(struct device *dev, struct device_attribute *attr,
                    const char *buf, size_t count);
};

int device_create_file(struct device *, const struct device_attribute *);
void device_remove_file(struct device *, const struct device_attribute *);
```

To read or write attributes, `show()` or `store()` method must be specified when declaring the attribute. For the common cases [include/linux/sysfs.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/sysfs.h) provides convenience macros ( `__ATTR` , `__ATTR_RO` , `__ATTR_WO` , etc.) to make defining attributes easier as well as making code more concise and readable.  
要读取或写入属性，必须在声明属性时指定 `show()` 或 `store()` 方法。对于常见情况，include/linux/sysfs.h 提供了方便的宏（ `__ATTR` 、 `__ATTR_RO` 、 `__ATTR_WO` 等），使定义属性更容易使代码更加简洁和可读。

An example of a hello world module which includes the creation of a variable accessible via sysfs is given below.  
下面给出了一个 hello world 模块的示例，其中包括创建可通过 sysfs 访问的变量。

```c
/*
 * hello-sysfs.c sysfs example
 */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/sysfs.h>

static struct kobject *mymodule;

/* the variable you want to be able to change */
static int myvariable = 0;

static ssize_t myvariable_show(struct kobject *kobj,
                               struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", myvariable);
}

static ssize_t myvariable_store(struct kobject *kobj,
                                struct kobj_attribute *attr, char *buf,
                                size_t count)
{
    sscanf(buf, "%du", &myvariable);
    return count;
}

static struct kobj_attribute myvariable_attribute =
    __ATTR(myvariable, 0660, myvariable_show, (void *)myvariable_store);

static int __init mymodule_init(void)
{
    int error = 0;

    pr_info("mymodule: initialised\n");

    mymodule = kobject_create_and_add("mymodule", kernel_kobj);
    if (!mymodule)
        return -ENOMEM;

    error = sysfs_create_file(mymodule, &myvariable_attribute.attr);
    if (error) {
        pr_info("failed to create the myvariable file "
                "in /sys/kernel/mymodule\n");
    }

    return error;
}

static void __exit mymodule_exit(void)
{
    pr_info("mymodule: Exit success\n");
    kobject_put(mymodule);
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");
```

Make and install the module:  
制作并安装模块：

```bash
make
sudo insmod hello-sysfs.ko
```

Check that it exists:  
检查它是否存在：

```bash
sudo lsmod | grep hello_sysfs
```

What is the current value of `myvariable` ?  
`myvariable` 的当前值是多少？

```bash
cat /sys/kernel/mymodule/myvariable
```

Set the value of `myvariable` and check that it changed.  
设置 `myvariable` 的值并检查它是否改变。

```bash
echo "32" > /sys/kernel/mymodule/myvariable
cat /sys/kernel/mymodule/myvariable
```

Finally, remove the test module:  
最后，删除测试模块：

```bash
sudo rmmod hello_sysfs
```

In the above case, we use a simple kobject to create a directory under sysfs, and communicate with its attributes. Since Linux v2.6.0, the `kobject` structure made its appearance. It was initially meant as a simple way of unifying kernel code which manages reference counted objects. After a bit of mission creep, it is now the glue that holds much of the device model and its sysfs interface together. For more information about kobject and sysfs, see [Documentation/driver-api/driver-model/driver.rst](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/Documentation/driver-api/driver-model/driver.rst) and [https://lwn.net/Articles/51437/](https://lwn.net/Articles/51437/).  
在上面的例子中，我们使用一个简单的 kobject 在 sysfs 下创建一个目录，并与其属性进行通信。从 Linux v2.6.0 开始， `kobject` 结构出现了。它最初的目的是作为一种统一管理引用计数对象的内核代码的简单方法。经过一些任务的扩展之后，它现在成为将设备模型的大部分及其 sysfs 接口粘合在一起的粘合剂。有关 kobject 和 sysfs 的更多信息，请参阅 Documentation/driver-api/driver-model/driver.rst 和 https://lwn.net/Articles/51437/。

### 9 Talking To Device Files

9 与设备文件对话

Device files are supposed to represent physical devices. Most physical devices are used for output as well as input, so there has to be some mechanism for device drivers in the kernel to get the output to send to the device from processes. This is done by opening the device file for output and writing to it, just like writing to a file. In the following example, this is implemented by `device_write` .  
设备文件应该代表物理设备。大多数物理设备都用于输出和输入，因此内核中的设备驱动程序必须有某种机制来获取输出以从进程发送到设备。这是通过打开设备文件进行输出并写入它来完成的，就像写入文件一样。在以下示例中，这是由 `device_write` 实现的。

This is not always enough. Imagine you had a serial port connected to a modem (even if you have an internal modem, it is still implemented from the CPU’s perspective as a serial port connected to a modem, so you don’t have to tax your imagination too hard). The natural thing to do would be to use the device file to write things to the modem (either modem commands or data to be sent through the phone line) and read things from the modem (either responses for commands or the data received through the phone line). However, this leaves open the question of what to do when you need to talk to the serial port itself, for example to configure the rate at which data is sent and received.  
这并不总是足够的。想象一下您有一个连接到调制解调器的串行端口（即使您有一个内部调制解调器，从 CPU 的角度来看，它仍然是作为连接到调制解调器的串行端口实现的，因此您不必太考验您的想象力）。自然的做法是使用设备文件将内容写入调制解调器（调制解调器命令或通过电话线发送的数据）并从调制解调器读取内容（命令响应或通过电话接收的数据）线）。然而，这留下了一个问题：当您需要与串行端口本身通信时该怎么做，例如配置发送和接收数据的速率。

The answer in Unix is to use a special function called `ioctl` (short for Input Output ConTroL). Every device can have its own `ioctl` commands, which can be read ioctl’s (to send information from a process to the kernel), write ioctl’s (to return information to a process), both or neither. Notice here the roles of read and write are reversed again, so in ioctl’s read is to send information to the kernel and write is to receive information from the kernel.  
Unix 中的答案是使用一个名为 `ioctl` 的特殊函数（Input Output ConTroL 的缩写）。每个设备都可以有自己的 `ioctl` 命令，这些命令可以读取 ioctl（将信息从进程发送到内核）、写入 ioctl（将信息返回给进程），或者两者都可以。注意这里读和写的角色又颠倒了，所以在 ioctl 中读是向内核发送信息，写是从内核接收信息。

The ioctl function is called with three parameters: the file descriptor of the appropriate device file, the ioctl number, and a parameter, which is of type long so you can use a cast to use it to pass anything. You will not be able to pass a structure this way, but you will be able to pass a pointer to the structure. Here is an example:  
ioctl 函数使用三个参数调用：相应设备文件的文件描述符、ioctl 编号和一个参数，该参数的类型为 long，因此您可以使用强制转换来使用它来传递任何内容。您将无法以这种方式传递结构，但可以传递指向该结构的指针。这是一个例子：

```c
/*
 * ioctl.c
 */
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

struct ioctl_arg {
    unsigned int val;
};

/* Documentation/userspace-api/ioctl/ioctl-number.rst */
#define IOC_MAGIC '\x66'

#define IOCTL_VALSET _IOW(IOC_MAGIC, 0, struct ioctl_arg)
#define IOCTL_VALGET _IOR(IOC_MAGIC, 1, struct ioctl_arg)
#define IOCTL_VALGET_NUM _IOR(IOC_MAGIC, 2, int)
#define IOCTL_VALSET_NUM _IOW(IOC_MAGIC, 3, int)

#define IOCTL_VAL_MAXNR 3
#define DRIVER_NAME "ioctltest"

static unsigned int test_ioctl_major = 0;
static unsigned int num_of_dev = 1;
static struct cdev test_ioctl_cdev;
static int ioctl_num = 0;

struct test_ioctl_data {
    unsigned char val;
    rwlock_t lock;
};

static long test_ioctl_ioctl(struct file *filp, unsigned int cmd,
                             unsigned long arg)
{
    struct test_ioctl_data *ioctl_data = filp->private_data;
    int retval = 0;
    unsigned char val;
    struct ioctl_arg data;
    memset(&data, 0, sizeof(data));

    switch (cmd) {
    case IOCTL_VALSET:
        if (copy_from_user(&data, (int __user *)arg, sizeof(data))) {
            retval = -EFAULT;
            goto done;
        }

        pr_alert("IOCTL set val:%x .\n", data.val);
        write_lock(&ioctl_data->lock);
        ioctl_data->val = data.val;
        write_unlock(&ioctl_data->lock);
        break;

    case IOCTL_VALGET:
        read_lock(&ioctl_data->lock);
        val = ioctl_data->val;
        read_unlock(&ioctl_data->lock);
        data.val = val;

        if (copy_to_user((int __user *)arg, &data, sizeof(data))) {
            retval = -EFAULT;
            goto done;
        }

        break;

    case IOCTL_VALGET_NUM:
        retval = __put_user(ioctl_num, (int __user *)arg);
        break;

    case IOCTL_VALSET_NUM:
        ioctl_num = arg;
        break;

    default:
        retval = -ENOTTY;
    }

done:
    return retval;
}

static ssize_t test_ioctl_read(struct file *filp, char __user *buf,
                               size_t count, loff_t *f_pos)
{
    struct test_ioctl_data *ioctl_data = filp->private_data;
    unsigned char val;
    int retval;
    int i = 0;

    read_lock(&ioctl_data->lock);
    val = ioctl_data->val;
    read_unlock(&ioctl_data->lock);

    for (; i < count; i++) {
        if (copy_to_user(&buf[i], &val, 1)) {
            retval = -EFAULT;
            goto out;
        }
    }

    retval = count;
out:
    return retval;
}

static int test_ioctl_close(struct inode *inode, struct file *filp)
{
    pr_alert("%s call.\n", __func__);

    if (filp->private_data) {
        kfree(filp->private_data);
        filp->private_data = NULL;
    }

    return 0;
}

static int test_ioctl_open(struct inode *inode, struct file *filp)
{
    struct test_ioctl_data *ioctl_data;

    pr_alert("%s call.\n", __func__);
    ioctl_data = kmalloc(sizeof(struct test_ioctl_data), GFP_KERNEL);

    if (ioctl_data == NULL)
        return -ENOMEM;

    rwlock_init(&ioctl_data->lock);
    ioctl_data->val = 0xFF;
    filp->private_data = ioctl_data;

    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = test_ioctl_open,
    .release = test_ioctl_close,
    .read = test_ioctl_read,
    .unlocked_ioctl = test_ioctl_ioctl,
};

static int __init ioctl_init(void)
{
    dev_t dev;
    int alloc_ret = -1;
    int cdev_ret = -1;
    alloc_ret = alloc_chrdev_region(&dev, 0, num_of_dev, DRIVER_NAME);

    if (alloc_ret)
        goto error;

    test_ioctl_major = MAJOR(dev);
    cdev_init(&test_ioctl_cdev, &fops);
    cdev_ret = cdev_add(&test_ioctl_cdev, dev, num_of_dev);

    if (cdev_ret)
        goto error;

    pr_alert("%s driver(major: %d) installed.\n", DRIVER_NAME,
             test_ioctl_major);
    return 0;
error:
    if (cdev_ret == 0)
        cdev_del(&test_ioctl_cdev);
    if (alloc_ret == 0)
        unregister_chrdev_region(dev, num_of_dev);
    return -1;
}

static void __exit ioctl_exit(void)
{
    dev_t dev = MKDEV(test_ioctl_major, 0);

    cdev_del(&test_ioctl_cdev);
    unregister_chrdev_region(dev, num_of_dev);
    pr_alert("%s driver removed.\n", DRIVER_NAME);
}

module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is test_ioctl module");
```

You can see there is an argument called `cmd` in `test_ioctl_ioctl()` function. It is the ioctl number. The ioctl number encodes the major device number, the type of the ioctl, the command, and the type of the parameter. This ioctl number is usually created by a macro call ( `_IO` , `_IOR` , `_IOW` or `_IOWR` — depending on the type) in a header file. This header file should then be included both by the programs which will use ioctl (so they can generate the appropriate ioctl’s) and by the kernel module (so it can understand it). In the example below, the header file is chardev.h and the program which uses it is userspace_ioctl.c.  
您可以看到 `test_ioctl_ioctl()` 函数中有一个名为 `cmd` 的参数。它是 ioctl 编号。 ioctl 编号编码主设备编号、ioctl 类型、命令和参数类型。此 ioctl 编号通常由宏调用（ `_IO` 、 `_IOR` 、 `_IOW` 或 `_IOWR` - 取决于类型）在头文件。然后，将使用 ioctl 的程序（以便它们可以生成适当的 ioctl）和内核模块（以便它可以理解它）都应该包含该头文件。在下面的示例中，头文件是 chardev.h，使用它的程序是 userspace_ioctl.c。

If you want to use ioctls in your own kernel modules, it is best to receive an official ioctl assignment, so if you accidentally get somebody else’s ioctls, or if they get yours, you’ll know something is wrong. For more information, consult the kernel source tree at [Documentation/userspace-api/ioctl/ioctl-number.rst](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/Documentation/userspace-api/ioctl/ioctl-number.rst).  
如果您想在自己的内核模块中使用 ioctl，最好接受官方的 ioctl 分配，因此，如果您不小心获得了别人的 ioctl，或者他们获得了您的 ioctl，您就会知道出了问题。有关更多信息，请参阅位于 Documentation/userspace-api/ioctl/ioctl-number.rst 的内核源代码树。

Also, we need to be careful that concurrent access to the shared resources will lead to the race condition. The solution is using atomic Compare-And-Swap (CAS), which we mentioned at [6.5](https://sysprog21.github.io/lkmpg/#chardevc) section, to enforce the exclusive access.  
另外，我们需要注意对共享资源的并发访问会导致竞争条件。解决方案是使用我们在 6.5 节中提到的原子比较和交换 (CAS) 来强制执行独占访问。

```c
/*
 * chardev2.c - Create an input/output character device
 */

#include <linux/atomic.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h> /* Specifically, a module */
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h> /* for get_user and put_user */
#include <linux/version.h>

#include <asm/errno.h>

#include "chardev.h"
#define SUCCESS 0
#define DEVICE_NAME "char_dev"
#define BUF_LEN 80

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};

/* Is the device open right now? Used to prevent concurrent access into
 * the same device
 */
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

/* The message the device will give when asked */
static char message[BUF_LEN + 1];

static struct class *cls;

/* This is called whenever a process attempts to open the device file */
static int device_open(struct inode *inode, struct file *file)
{
    pr_info("device_open(%p)\n", file);

    try_module_get(THIS_MODULE);
    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
    pr_info("device_release(%p,%p)\n", inode, file);

    module_put(THIS_MODULE);
    return SUCCESS;
}

/* This function is called whenever a process which has already opened the
 * device file attempts to read from it.
 */
static ssize_t device_read(struct file *file, /* see include/linux/fs.h   */
                           char __user *buffer, /* buffer to be filled  */
                           size_t length, /* length of the buffer     */
                           loff_t *offset)
{
    /* Number of bytes actually written to the buffer */
    int bytes_read = 0;
    /* How far did the process reading the message get? Useful if the message
     * is larger than the size of the buffer we get to fill in device_read.
     */
    const char *message_ptr = message;

    if (!*(message_ptr + *offset)) { /* we are at the end of message */
        *offset = 0; /* reset the offset */
        return 0; /* signify end of file */
    }

    message_ptr += *offset;

    /* Actually put the data into the buffer */
    while (length && *message_ptr) {
        /* Because the buffer is in the user data segment, not the kernel
         * data segment, assignment would not work. Instead, we have to
         * use put_user which copies data from the kernel data segment to
         * the user data segment.
         */
        put_user(*(message_ptr++), buffer++);
        length--;
        bytes_read++;
    }

    pr_info("Read %d bytes, %ld left\n", bytes_read, length);

    *offset += bytes_read;

    /* Read functions are supposed to return the number of bytes actually
     * inserted into the buffer.
     */
    return bytes_read;
}

/* called when somebody tries to write into our device file. */
static ssize_t device_write(struct file *file, const char __user *buffer,
                            size_t length, loff_t *offset)
{
    int i;

    pr_info("device_write(%p,%p,%ld)", file, buffer, length);

    for (i = 0; i < length && i < BUF_LEN; i++)
        get_user(message[i], buffer + i);

    /* Again, return the number of input characters used. */
    return i;
}

/* This function is called whenever a process tries to do an ioctl on our
 * device file. We get two extra parameters (additional to the inode and file
 * structures, which all device functions get): the number of the ioctl called
 * and the parameter given to the ioctl function.
 *
 * If the ioctl is write or read/write (meaning output is returned to the
 * calling process), the ioctl call returns the output of this function.
 */
static long
device_ioctl(struct file *file, /* ditto */
             unsigned int ioctl_num, /* number and param for ioctl */
             unsigned long ioctl_param)
{
    int i;
    long ret = SUCCESS;

    /* We don't want to talk to two processes at the same time. */
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;

    /* Switch according to the ioctl called */
    switch (ioctl_num) {
    case IOCTL_SET_MSG: {
        /* Receive a pointer to a message (in user space) and set that to
         * be the device's message. Get the parameter given to ioctl by
         * the process.
         */
        char __user *tmp = (char __user *)ioctl_param;
        char ch;

        /* Find the length of the message */
        get_user(ch, tmp);
        for (i = 0; ch && i < BUF_LEN; i++, tmp++)
            get_user(ch, tmp);

        device_write(file, (char __user *)ioctl_param, i, NULL);
        break;
    }
    case IOCTL_GET_MSG: {
        loff_t offset = 0;

        /* Give the current message to the calling process - the parameter
         * we got is a pointer, fill it.
         */
        i = device_read(file, (char __user *)ioctl_param, 99, &offset);

        /* Put a zero at the end of the buffer, so it will be properly
         * terminated.
         */
        put_user('\0', (char __user *)ioctl_param + i);
        break;
    }
    case IOCTL_GET_NTH_BYTE:
        /* This ioctl is both input (ioctl_param) and output (the return
         * value of this function).
         */
        ret = (long)message[ioctl_param];
        break;
    }

    /* We're now ready for our next caller */
    atomic_set(&already_open, CDEV_NOT_USED);

    return ret;
}

/* Module Declarations */

/* This structure will hold the functions to be called when a process does
 * something to the device we created. Since a pointer to this structure
 * is kept in the devices table, it can't be local to init_module. NULL is
 * for unimplemented functions.
 */
static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl,
    .open = device_open,
    .release = device_release, /* a.k.a. close */
};

/* Initialize the module - Register the character device */
static int __init chardev2_init(void)
{
    /* Register the character device (atleast try) */
    int ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);

    /* Negative values signify an error */
    if (ret_val < 0) {
        pr_alert("%s failed with %d\n",
                 "Sorry, registering the character device ", ret_val);
        return ret_val;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    cls = class_create(DEVICE_FILE_NAME);
#else
    cls = class_create(THIS_MODULE, DEVICE_FILE_NAME);
#endif
    device_create(cls, NULL, MKDEV(MAJOR_NUM, 0), NULL, DEVICE_FILE_NAME);

    pr_info("Device created on /dev/%s\n", DEVICE_FILE_NAME);

    return 0;
}

/* Cleanup - unregister the appropriate file from /proc */
static void __exit chardev2_exit(void)
{
    device_destroy(cls, MKDEV(MAJOR_NUM, 0));
    class_destroy(cls);

    /* Unregister the device */
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

module_init(chardev2_init);
module_exit(chardev2_exit);

MODULE_LICENSE("GPL");
```

```c
/*
 * chardev.h - the header file with the ioctl definitions.
 *
 * The declarations here have to be in a header file, because they need
 * to be known both to the kernel module (in chardev2.c) and the process
 * calling ioctl() (in userspace_ioctl.c).
 */

#ifndef CHARDEV_H
#define CHARDEV_H

#include <linux/ioctl.h>

/* The major device number. We can not rely on dynamic registration
 * any more, because ioctls need to know it.
 */
#define MAJOR_NUM 100

/* Set the message of the device driver */
#define IOCTL_SET_MSG _IOW(MAJOR_NUM, 0, char *)
/* _IOW means that we are creating an ioctl command number for passing
 * information from a user process to the kernel module.
 *
 * The first arguments, MAJOR_NUM, is the major device number we are using.
 *
 * The second argument is the number of the command (there could be several
 * with different meanings).
 *
 * The third argument is the type we want to get from the process to the
 * kernel.
 */

/* Get the message of the device driver */
#define IOCTL_GET_MSG _IOR(MAJOR_NUM, 1, char *)
/* This IOCTL is used for output, to get the message of the device driver.
 * However, we still need the buffer to place the message in to be input,
 * as it is allocated by the process.
 */

/* Get the n'th byte of the message */
#define IOCTL_GET_NTH_BYTE _IOWR(MAJOR_NUM, 2, int)
/* The IOCTL is used for both input and output. It receives from the user
 * a number, n, and returns message[n].
 */

/* The name of the device file */
#define DEVICE_FILE_NAME "char_dev"
#define DEVICE_PATH "/dev/char_dev"

#endif
```

```c
/*  userspace_ioctl.c - the process to use ioctl's to control the kernel module
 *
 *  Until now we could have used cat for input and output.  But now
 *  we need to do ioctl's, which require writing our own process.
 */

/* device specifics, such as ioctl numbers and the
 * major device file. */
#include "../chardev.h"

#include <stdio.h> /* standard I/O */
#include <fcntl.h> /* open */
#include <unistd.h> /* close */
#include <stdlib.h> /* exit */
#include <sys/ioctl.h> /* ioctl */

/* Functions for the ioctl calls */

int ioctl_set_msg(int file_desc, char *message)
{
    int ret_val;

    ret_val = ioctl(file_desc, IOCTL_SET_MSG, message);

    if (ret_val < 0) {
        printf("ioctl_set_msg failed:%d\n", ret_val);
    }

    return ret_val;
}

int ioctl_get_msg(int file_desc)
{
    int ret_val;
    char message[100] = { 0 };

    /* Warning - this is dangerous because we don't tell
   * the kernel how far it's allowed to write, so it
   * might overflow the buffer. In a real production
   * program, we would have used two ioctls - one to tell
   * the kernel the buffer length and another to give
   * it the buffer to fill
   */
    ret_val = ioctl(file_desc, IOCTL_GET_MSG, message);

    if (ret_val < 0) {
        printf("ioctl_get_msg failed:%d\n", ret_val);
    }
    printf("get_msg message:%s", message);

    return ret_val;
}

int ioctl_get_nth_byte(int file_desc)
{
    int i, c;

    printf("get_nth_byte message:");

    i = 0;
    do {
        c = ioctl(file_desc, IOCTL_GET_NTH_BYTE, i++);

        if (c < 0) {
            printf("\nioctl_get_nth_byte failed at the %d'th byte:\n", i);
            return c;
        }

        putchar(c);
    } while (c != 0);

    return 0;
}

/* Main - Call the ioctl functions */
int main(void)
{
    int file_desc, ret_val;
    char *msg = "Message passed by ioctl\n";

    file_desc = open(DEVICE_PATH, O_RDWR);
    if (file_desc < 0) {
        printf("Can't open device file: %s, error:%d\n", DEVICE_PATH,
               file_desc);
        exit(EXIT_FAILURE);
    }

    ret_val = ioctl_set_msg(file_desc, msg);
    if (ret_val)
        goto error;
    ret_val = ioctl_get_nth_byte(file_desc);
    if (ret_val)
        goto error;
    ret_val = ioctl_get_msg(file_desc);
    if (ret_val)
        goto error;

    close(file_desc);
    return 0;
error:
    close(file_desc);
    exit(EXIT_FAILURE);
}
```

### 10 System Calls 10 系统调用

So far, the only thing we’ve done was to use well defined kernel mechanisms to register /proc files and device handlers. This is fine if you want to do something the kernel programmers thought you’d want, such as write a device driver. But what if you want to do something unusual, to change the behavior of the system in some way? Then, you are mostly on your own.  
到目前为止，我们所做的唯一一件事就是使用定义良好的内核机制来注册 /proc 文件和设备处理程序。如果您想做内核程序员认为您想做的事情，例如编写设备驱动程序，那么这很好。但是，如果您想做一些不寻常的事情，以某种方式改变系统的行为怎么办？然后，你就只能靠自己了。

If you are not being sensible and using a virtual machine then this is where kernel programming can become hazardous. While writing the example below, I killed the `open()` system call. This meant I could not open any files, I could not run any programs, and I could not shutdown the system. I had to restart the virtual machine. No important files got annihilated, but if I was doing this on some live mission critical system then that could have been a possible outcome. To ensure you do not lose any files, even within a test environment, please run `sync` right before you do the `insmod` and the `rmmod` .  
如果您不明智地使用虚拟机，那么这就是内核编程可能变得危险的地方。在编写下面的示例时，我终止了 `open()` 系统调用。这意味着我无法打开任何文件，无法运行任何程序，也无法关闭系统。我不得不重新启动虚拟机。没有重要的文件被消灭，但如果我在一些实时任务关键系统上执行此操作，那么这可能是一个可能的结果。为了确保即使在测试环境中也不会丢失任何文件，请在执行 `insmod` 和 `rmmod` 之前运行 `sync` 。

Forget about /proc files, forget about device files. They are just minor details. Minutiae in the vast expanse of the universe. The real process to kernel communication mechanism, the one used by all processes, is system calls. When a process requests a service from the kernel (such as opening a file, forking to a new process, or requesting more memory), this is the mechanism used. If you want to change the behaviour of the kernel in interesting ways, this is the place to do it. By the way, if you want to see which system calls a program uses, run `strace <arguments>` .  
忘记 /proc 文件，忘记设备文件。它们只是一些小细节。浩瀚宇宙中的细枝末节。所有进程都使用的真正的进程与内核通信机制是系统调用。当进程向内核请求服务（例如打开文件、分叉到新进程或请求更多内存）时，就会使用这种机制。如果你想以有趣的方式改变内核的行为，这里就是你可以做的地方。顺便说一句，如果您想查看程序使用哪个系统调用，请运行 `strace <arguments>` 。

In general, a process is not supposed to be able to access the kernel. It can not access kernel memory and it can’t call kernel functions. The hardware of the CPU enforces this (that is the reason why it is called “protected mode” or “page protection”).  
一般来说，进程不应该能够访问内核。它无法访问内核内存，也无法调用内核函数。 CPU 的硬件强制执行此操作（这就是它被称为“保护模式”或“页面保护”的原因）。

System calls are an exception to this general rule. What happens is that the process fills the registers with the appropriate values and then calls a special instruction which jumps to a previously defined location in the kernel (of course, that location is readable by user processes, it is not writable by them). Under Intel CPUs, this is done by means of interrupt 0x80. The hardware knows that once you jump to this location, you are no longer running in restricted user mode, but as the operating system kernel — and therefore you’re allowed to do whatever you want.  
系统调用是此一般规则的一个例外。发生的情况是，进程用适当的值填充寄存器，然后调用一条特殊指令，该指令跳转到内核中先前定义的位置（当然，该位置可由用户进程读取，但不可写入）。在 Intel CPU 下，这是通过中断 0x80 完成的。硬件知道，一旦你跳转到这个位置，你就不再运行在受限用户模式下，而是作为操作系统内核运行——因此你可以做任何你想做的事情。

The location in the kernel a process can jump to is called system_call. The procedure at that location checks the system call number, which tells the kernel what service the process requested. Then, it looks at the table of system calls ( `sys_call_table` ) to see the address of the kernel function to call. Then it calls the function, and after it returns, does a few system checks and then return back to the process (or to a different process, if the process time ran out). If you want to read this code, it is at the source file arch/$(architecture)/kernel/entry.S, after the line `ENTRY(system_call)` .  
进程在内核中可以跳转到的位置称为system_call。该位置的过程检查系统调用号，它告诉内核进程请求什么服务。然后，它查看系统调用表 ( `sys_call_table` ) 以查看要调用的内核函数的地址。然后它调用该函数，并在返回后执行一些系统检查，然后返回到该进程（或者返回到另一个进程，如果进程时间用完）。如果你想阅读这段代码，它位于源文件 arch/$(architecture)/kernel/entry.S 的 `ENTRY(system_call)` 行之后。

So, if we want to change the way a certain system call works, what we need to do is to write our own function to implement it (usually by adding a bit of our own code, and then calling the original function) and then change the pointer at `sys_call_table` to point to our function. Because we might be removed later and we don’t want to leave the system in an unstable state, it’s important for `cleanup_module` to restore the table to its original state.  
所以，如果我们想要改变某个系统调用的工作方式，我们需要做的就是编写自己的函数来实现它（通常是添加一点我们自己的代码，然后调用原来的函数），然后改变 `sys_call_table` 处的指针指向我们的函数。因为我们稍后可能会被删除，并且我们不想让系统处于不稳定状态，所以 `cleanup_module` 将表恢复到原始状态非常重要。

To modify the content of `sys_call_table` , we need to consider the control register. A control register is a processor register that changes or controls the general behavior of the CPU. For x86 architecture, the cr0 register has various control flags that modify the basic operation of the processor. The WP flag in cr0 stands for write protection. Once the WP flag is set, the processor disallows further write attempts to the read-only sections Therefore, we must disable the WP flag before modifying `sys_call_table` . Since Linux v5.3, the `write_cr0` function cannot be used because of the sensitive cr0 bits pinned by the security issue, the attacker may write into CPU control registers to disable CPU protections like write protection. As a result, we have to provide the custom assembly routine to bypass it.  
要修改 `sys_call_table` 的内容，我们需要考虑控制寄存器。控制寄存器是改变或控制 CPU 一般行为的处理器寄存器。对于 x86 架构，cr0 寄存器有各种控制标志，可以修改处理器的基本操作。 cr0 中的 WP 标志代表写保护。一旦设置了 WP 标志，处理器就不允许对只读部分进行进一步的写入尝试，因此，我们必须在修改 `sys_call_table` 之前禁用 WP 标志。从 Linux v5.3 开始，由于安全问题固定了敏感的 cr0 位， `write_cr0` 功能无法使用，攻击者可能会写入 CPU 控制寄存器以禁用写保护等 CPU 保护。因此，我们必须提供自定义汇编例程来绕过它。

However, `sys_call_table` symbol is unexported to prevent misuse. But there have few ways to get the symbol, manual symbol lookup and `kallsyms_lookup_name` . Here we use both depend on the kernel version.  
但是， `sys_call_table` 符号未导出以防止误用。但获取符号的方法很少，手动符号查找和 `kallsyms_lookup_name` 。这里我们使用两者取决于内核版本。

Because of the control-flow integrity, which is a technique to prevent the redirect execution code from the attacker, for making sure that the indirect calls go to the expected addresses and the return addresses are not changed. Since Linux v5.7, the kernel patched the series of control-flow enforcement (CET) for x86, and some configurations of GCC, like GCC versions 9 and 10 in Ubuntu Linux, will add with CET (the -fcf-protection option) in the kernel by default. Using that GCC to compile the kernel with retpoline off may result in CET being enabled in the kernel. You can use the following command to check out the -fcf-protection option is enabled or not:  
因为控制流完整性是一种防止攻击者重定向执行代码的技术，以确保间接调用到达预期地址并且返回地址不被改变。从 Linux v5.7 开始，内核修补了 x86 的一系列控制流强制 (CET)，并且 GCC 的某些配置（例如 Ubuntu Linux 中的 GCC 版本 9 和 10）将添加 CET（-fcf-protection 选项）默认情况下在内核中。使用该 GCC 在关闭 retpoline 的情况下编译内核可能会导致在内核中启用 CET。您可以使用以下命令来检查 -fcf-protection 选项是否启用：

```bash
$ gcc -v -Q -O2 --help=target | grep protection
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/9/lto-wrapper
...
gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
COLLECT_GCC_OPTIONS='-v' '-Q' '-O2' '--help=target' '-mtune=generic' '-march=x86-64'
 /usr/lib/gcc/x86_64-linux-gnu/9/cc1 -v ... -fcf-protection ...
 GNU C17 (Ubuntu 9.3.0-17ubuntu1~20.04) version 9.3.0 (x86_64-linux-gnu)
...
```

But CET should not be enabled in the kernel, it may break the Kprobes and bpf. Consequently, CET is disabled since v5.11. To guarantee the manual symbol lookup worked, we only use up to v5.4.  
但不应在内核中启用 CET，它可能会破坏 Kprobes 和 bpf。因此，自 v5.11 起 CET 被禁用。为了保证手动符号查找有效，我们最多只使用 v5.4。

Unfortunately, since Linux v5.7 `kallsyms_lookup_name` is also unexported, it needs certain trick to get the address of `kallsyms_lookup_name` . If `CONFIG_KPROBES` is enabled, we can facilitate the retrieval of function addresses by means of Kprobes to dynamically break into the specific kernel routine. Kprobes inserts a breakpoint at the entry of function by replacing the first bytes of the probed instruction. When a CPU hits the breakpoint, registers are stored, and the control will pass to Kprobes. It passes the addresses of the saved registers and the Kprobe struct to the handler you defined, then executes it. Kprobes can be registered by symbol name or address. Within the symbol name, the address will be handled by the kernel.  
不幸的是，由于 Linux v5.7 `kallsyms_lookup_name` 也未导出，因此需要一定的技巧来获取 `kallsyms_lookup_name` 的地址。如果启用 `CONFIG_KPROBES` ，我们可以方便地通过 Kprobes 检索函数地址，从而动态地侵入特定的内核例程。 Kprobes 通过替换探测指令的第一个字节在函数入口处插入断点。当 CPU 到达断点时，寄存器被存储，控制权将传递给 Kprobes。它将保存的寄存器的地址和 Kprobe 结构传递给您定义的处理程序，然后执行它。 Kprobes 可以通过符号名称或地址来注册。在符号名称中，地址将由内核处理。

Otherwise, specify the address of `sys_call_table` from /proc/kallsyms and /boot/System.map into `sym` parameter. Following is the sample usage for /proc/kallsyms:  
否则，将 /proc/kallsyms 和 /boot/System.map 中的 `sys_call_table` 地址指定到 `sym` 参数中。以下是 /proc/kallsyms 的示例用法：

```bash
$ sudo grep sys_call_table /proc/kallsyms
ffffffff82000280 R x32_sys_call_table
ffffffff820013a0 R sys_call_table
ffffffff820023e0 R ia32_sys_call_table
$ sudo insmod syscall.ko sym=0xffffffff820013a0
```

Using the address from /boot/System.map, be careful about KASLR (Kernel Address Space Layout Randomization). KASLR may randomize the address of kernel code and data at every boot time, such as the static address listed in /boot/System.map will offset by some entropy. The purpose of KASLR is to protect the kernel space from the attacker. Without KASLR, the attacker may find the target address in the fixed address easily. Then the attacker can use return-oriented programming to insert some malicious codes to execute or receive the target data by a tampered pointer. KASLR mitigates these kinds of attacks because the attacker cannot immediately know the target address, but a brute-force attack can still work. If the address of a symbol in /proc/kallsyms is different from the address in /boot/System.map, KASLR is enabled with the kernel, which your system running on.  
使用 /boot/System.map 中的地址，请注意 KASLR（内核地址空间布局随机化）。 KASLR 可能会在每次启动时随机化内核代码和数据的地址，例如 /boot/System.map 中列出的静态地址会偏移一些熵。 KASLR 的目的是保护内核空间免受攻击者的侵害。如果没有 KASLR，攻击者可以很容易地在固定地址中找到目标地址。那么攻击者就可以利用面向返回的编程方式插入一些恶意代码，通过被篡改的指针来执行或接收目标数据。 KASLR 可以缓解此类攻击，因为攻击者无法立即知道目标地址，但暴力攻击仍然可以发挥作用。如果 /proc/kallsyms 中的符号地址与 /boot/System.map 中的地址不同，则系统运行所在的内核会启用 KASLR。

```bash
$ grep GRUB_CMDLINE_LINUX_DEFAULT /etc/default/grub
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash"
$ sudo grep sys_call_table /boot/System.map-$(uname -r)
ffffffff82000300 R sys_call_table
$ sudo grep sys_call_table /proc/kallsyms
ffffffff820013a0 R sys_call_table
# Reboot
$ sudo grep sys_call_table /boot/System.map-$(uname -r)
ffffffff82000300 R sys_call_table
$ sudo grep sys_call_table /proc/kallsyms
ffffffff86400300 R sys_call_table
```

If KASLR is enabled, we have to take care of the address from /proc/kallsyms each time we reboot the machine. In order to use the address from /boot/System.map, make sure that KASLR is disabled. You can add the nokaslr for disabling KASLR in next booting time:  
如果启用了 KASLR，我们每次重新启动机器时都必须处理 /proc/kallsyms 中的地址。为了使用 /boot/System.map 中的地址，请确保禁用 KASLR。您可以添加 nokaslr 以在下次启动时禁用 KASLR：

```bash
$ grep GRUB_CMDLINE_LINUX_DEFAULT /etc/default/grub
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash"
$ sudo perl -i -pe 'm/quiet/ and s//quiet nokaslr/' /etc/default/grub
$ grep quiet /etc/default/grub
GRUB_CMDLINE_LINUX_DEFAULT="quiet nokaslr splash"
$ sudo update-grub
```

For more information, check out the following:  
欲了解更多信息，请查看以下内容：

- [Cook: Security things in Linux v5.3  
  Cook：Linux v5.3 中的安全问题](https://lwn.net/Articles/804849/)
- [Unexporting the system call table  
  取消导出系统调用表](https://lwn.net/Articles/12211/)
- [Control-flow integrity for the kernel  
  内核的控制流完整性](https://lwn.net/Articles/810077/)
- [Unexporting kallsyms_lookup_name()  
  取消导出 kallsyms_lookup_name()](https://lwn.net/Articles/813350/)
- [Kernel Probes (Kprobes)  
  内核探针（Kprobes）](https://www.kernel.org/doc/Documentation/kprobes.txt)
- [Kernel address space layout randomization  
  内核地址空间布局随机化](https://lwn.net/Articles/569635/)

The source code here is an example of such a kernel module. We want to “spy” on a certain user, and to `pr_info()` a message whenever that user opens a file. Towards this end, we replace the system call to open a file with our own function, called `our_sys_openat` . This function checks the uid (user’s id) of the current process, and if it is equal to the uid we spy on, it calls `pr_info()` to display the name of the file to be opened. Then, either way, it calls the original `openat()` function with the same parameters, to actually open the file.  
这里的源代码是此类内核模块的示例。我们想要“监视”某个用户，并在该用户打开文件时 `pr_info()` 发送一条消息。为此，我们用我们自己的函数 `our_sys_openat` 替换打开文件的系统调用。该函数检查当前进程的 uid（用户 id），如果它与我们监视的 uid 相同，则调用 `pr_info()` 来显示要打开的文件的名称。然后，无论哪种方式，它都会使用相同的参数调用原始的 `openat()` 函数来实际打开文件。

The `init_module` function replaces the appropriate location in `sys_call_table` and keeps the original pointer in a variable. The `cleanup_module` function uses that variable to restore everything back to normal. This approach is dangerous, because of the possibility of two kernel modules changing the same system call. Imagine we have two kernel modules, A and B. A’s openat system call will be `A_openat` and B’s will be `B_openat` . Now, when A is inserted into the kernel, the system call is replaced with `A_openat` , which will call the original `sys_openat` when it is done. Next, B is inserted into the kernel, which replaces the system call with `B_openat` , which will call what it thinks is the original system call, `A_openat` , when it’s done.  
`init_module` 函数替换 `sys_call_table` 中的适当位置，并将原始指针保留在变量中。 `cleanup_module` 函数使用该变量将一切恢复正常。这种方法很危险，因为两个内核模块可能会更改同一个系统调用。假设我们有两个内核模块，A 和 B。A 的 openat 系统调用为 `A_openat` ，B 的 openat 系统调用为 `B_openat` 。现在，当 A 插入内核时，系统调用被替换为 `A_openat` ，完成后将调用原始的 `sys_openat` 。接下来，B 被插入到内核中，内核用 `B_openat` 替换系统调用，完成后它将调用它认为是原始系统调用的 `A_openat` 。

Now, if B is removed first, everything will be well — it will simply restore the system call to `A_openat` , which calls the original. However, if A is removed and then B is removed, the system will crash. A’s removal will restore the system call to the original, `sys_openat` , cutting B out of the loop. Then, when B is removed, it will restore the system call to what it thinks is the original, `A_openat` , which is no longer in memory. At first glance, it appears we could solve this particular problem by checking if the system call is equal to our open function and if so not changing it at all (so that B won’t change the system call when it is removed), but that will cause an even worse problem. When A is removed, it sees that the system call was changed to `B_openat` so that it is no longer pointing to `A_openat` , so it will not restore it to `sys_openat` before it is removed from memory. Unfortunately, `B_openat` will still try to call `A_openat` which is no longer there, so that even without removing B the system would crash.  
现在，如果先删除 B，一切都会好起来 - 它只会将系统调用恢复到 `A_openat` ，它调用原始系统调用。但是，如果删除 A，然后删除 B，系统就会崩溃。 A 的删除会将系统调用恢复为原始 `sys_openat` ，从而将 B 切出循环。然后，当 B 被删除时，它会将系统调用恢复到它认为的原始 `A_openat` ，该调用不再位于内存中。乍一看，我们似乎可以通过检查系统调用是否等于我们的 open 函数来解决这个特殊问题，如果是则根本不改变它（这样当系统调用被删除时 B 就不会改变它），但是这会导致更严重的问题。当 A 被删除时，它看到系统调用已更改为 `B_openat` ，因此它不再指向 `A_openat` ，因此不会将其恢复为 `sys_openat` 在从内存中删除之前。不幸的是， `B_openat` 仍会尝试调用不再存在的 `A_openat` ，因此即使不删除 B，系统也会崩溃。

Note that all the related problems make syscall stealing unfeasible for production use. In order to keep people from doing potential harmful things `sys_call_table` is no longer exported. This means, if you want to do something more than a mere dry run of this example, you will have to patch your current kernel in order to have `sys_call_table` exported.  
请注意，所有相关问题使得系统调用窃取在生产使用中不可行。为了防止人们做潜在有害的事情 `sys_call_table` 不再导出。这意味着，如果您想要做的不仅仅是这个示例的演练，您将必须修补当前的内核才能导出 `sys_call_table` 。

```c
/*
 * syscall.c
 *
 * System call "stealing" sample.
 *
 * Disables page protection at a processor level by changing the 16th bit
 * in the cr0 register (could be Intel specific).
 *
 * Based on example by Peter Jay Salzman and
 * https://bbs.archlinux.org/viewtopic.php?id=139406
 */

#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h> /* which will have params */
#include <linux/unistd.h> /* The list of system calls */
#include <linux/cred.h> /* For current_uid() */
#include <linux/uidgid.h> /* For __kuid_val() */
#include <linux/version.h>

/* For the current (process) structure, we need this to know who the
 * current user is.
 */
#include <linux/sched.h>
#include <linux/uaccess.h>

/* The way we access "sys_call_table" varies as kernel internal changes.
 * - Prior to v5.4 : manual symbol lookup
 * - v5.5 to v5.6  : use kallsyms_lookup_name()
 * - v5.7+         : Kprobes or specific kernel module parameter
 */

/* The in-kernel calls to the ksys_close() syscall were removed in Linux v5.11+.
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 7, 0))

#if LINUX_VERSION_CODE <= KERNEL_VERSION(5, 4, 0)
#define HAVE_KSYS_CLOSE 1
#include <linux/syscalls.h> /* For ksys_close() */
#else
#include <linux/kallsyms.h> /* For kallsyms_lookup_name */
#endif

#else

#if defined(CONFIG_KPROBES)
#define HAVE_KPROBES 1
#include <linux/kprobes.h>
#else
#define HAVE_PARAM 1
#include <linux/kallsyms.h> /* For sprint_symbol */
/* The address of the sys_call_table, which can be obtained with looking up
 * "/boot/System.map" or "/proc/kallsyms". When the kernel version is v5.7+,
 * without CONFIG_KPROBES, you can input the parameter or the module will look
 * up all the memory.
 */
static unsigned long sym = 0;
module_param(sym, ulong, 0644);
#endif /* CONFIG_KPROBES */

#endif /* Version < v5.7 */

static unsigned long **sys_call_table;

/* UID we want to spy on - will be filled from the command line. */
static uid_t uid = -1;
module_param(uid, int, 0644);

/* A pointer to the original system call. The reason we keep this, rather
 * than call the original function (sys_openat), is because somebody else
 * might have replaced the system call before us. Note that this is not
 * 100% safe, because if another module replaced sys_openat before us,
 * then when we are inserted, we will call the function in that module -
 * and it might be removed before we are.
 *
 * Another reason for this is that we can not get sys_openat.
 * It is a static variable, so it is not exported.
 */
#ifdef CONFIG_ARCH_HAS_SYSCALL_WRAPPER
static asmlinkage long (*original_call)(const struct pt_regs *);
#else
static asmlinkage long (*original_call)(int, const char __user *, int, umode_t);
#endif

/* The function we will replace sys_openat (the function called when you
 * call the open system call) with. To find the exact prototype, with
 * the number and type of arguments, we find the original function first
 * (it is at fs/open.c).
 *
 * In theory, this means that we are tied to the current version of the
 * kernel. In practice, the system calls almost never change (it would
 * wreck havoc and require programs to be recompiled, since the system
 * calls are the interface between the kernel and the processes).
 */
#ifdef CONFIG_ARCH_HAS_SYSCALL_WRAPPER
static asmlinkage long our_sys_openat(const struct pt_regs *regs)
#else
static asmlinkage long our_sys_openat(int dfd, const char __user *filename,
                                      int flags, umode_t mode)
#endif
{
    int i = 0;
    char ch;

    if (__kuid_val(current_uid()) != uid)
        goto orig_call;

    /* Report the file, if relevant */
    pr_info("Opened file by %d: ", uid);
    do {
#ifdef CONFIG_ARCH_HAS_SYSCALL_WRAPPER
        get_user(ch, (char __user *)regs->si + i);
#else
        get_user(ch, (char __user *)filename + i);
#endif
        i++;
        pr_info("%c", ch);
    } while (ch != 0);
    pr_info("\n");

orig_call:
    /* Call the original sys_openat - otherwise, we lose the ability to
     * open files.
     */
#ifdef CONFIG_ARCH_HAS_SYSCALL_WRAPPER
    return original_call(regs);
#else
    return original_call(dfd, filename, flags, mode);
#endif
}

static unsigned long **acquire_sys_call_table(void)
{
#ifdef HAVE_KSYS_CLOSE
    unsigned long int offset = PAGE_OFFSET;
    unsigned long **sct;

    while (offset < ULLONG_MAX) {
        sct = (unsigned long **)offset;

        if (sct[__NR_close] == (unsigned long *)ksys_close)
            return sct;

        offset += sizeof(void *);
    }

    return NULL;
#endif

#ifdef HAVE_PARAM
    const char sct_name[15] = "sys_call_table";
    char symbol[40] = { 0 };

    if (sym == 0) {
        pr_alert("For Linux v5.7+, Kprobes is the preferable way to get "
                 "symbol.\n");
        pr_info("If Kprobes is absent, you have to specify the address of "
                "sys_call_table symbol\n");
        pr_info("by /boot/System.map or /proc/kallsyms, which contains all the "
                "symbol addresses, into sym parameter.\n");
        return NULL;
    }
    sprint_symbol(symbol, sym);
    if (!strncmp(sct_name, symbol, sizeof(sct_name) - 1))
        return (unsigned long **)sym;

    return NULL;
#endif

#ifdef HAVE_KPROBES
    unsigned long (*kallsyms_lookup_name)(const char *name);
    struct kprobe kp = {
        .symbol_name = "kallsyms_lookup_name",
    };

    if (register_kprobe(&kp) < 0)
        return NULL;
    kallsyms_lookup_name = (unsigned long (*)(const char *name))kp.addr;
    unregister_kprobe(&kp);
#endif

    return (unsigned long **)kallsyms_lookup_name("sys_call_table");
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 3, 0)
static inline void __write_cr0(unsigned long cr0)
{
    asm volatile("mov %0,%%cr0" : "+r"(cr0) : : "memory");
}
#else
#define __write_cr0 write_cr0
#endif

static void enable_write_protection(void)
{
    unsigned long cr0 = read_cr0();
    set_bit(16, &cr0);
    __write_cr0(cr0);
}

static void disable_write_protection(void)
{
    unsigned long cr0 = read_cr0();
    clear_bit(16, &cr0);
    __write_cr0(cr0);
}

static int __init syscall_start(void)
{
    if (!(sys_call_table = acquire_sys_call_table()))
        return -1;

    disable_write_protection();

    /* keep track of the original open function */
    original_call = (void *)sys_call_table[__NR_openat];

    /* use our openat function instead */
    sys_call_table[__NR_openat] = (unsigned long *)our_sys_openat;

    enable_write_protection();

    pr_info("Spying on UID:%d\n", uid);

    return 0;
}

static void __exit syscall_end(void)
{
    if (!sys_call_table)
        return;

    /* Return the system call back to normal */
    if (sys_call_table[__NR_openat] != (unsigned long *)our_sys_openat) {
        pr_alert("Somebody else also played with the ");
        pr_alert("open system call\n");
        pr_alert("The system may be left in ");
        pr_alert("an unstable state.\n");
    }

    disable_write_protection();
    sys_call_table[__NR_openat] = (unsigned long *)original_call;
    enable_write_protection();

    msleep(2000);
}

module_init(syscall_start);
module_exit(syscall_end);

MODULE_LICENSE("GPL");
```

### 11 Blocking Processes and threads

11 阻塞进程和线程

#### 11.1 Sleep 11.1 睡眠

What do you do when somebody asks you for something you can not do right away? If you are a human being and you are bothered by a human being, the only thing you can say is: "Not right now, I’m busy. Go away!". But if you are a kernel module and you are bothered by a process, you have another possibility. You can put the process to sleep until you can service it. After all, processes are being put to sleep by the kernel and woken up all the time (that is the way multiple processes appear to run on the same time on a single CPU).  
当有人向你提出一些你不能立即做的事情时，你会怎么做？如果你是一个人，并且你被一个人打扰，你唯一能说的是：“现在不行，我很忙。走开！”。但如果你是一个内核模块并且你被一个进程打扰，你还有另一种可能性。您可以让该进程进入休眠状态，直到可以为其提供服务为止。毕竟，进程一直被内核置于睡眠状态并被唤醒（这就是多个进程在单个 CPU 上同时运行的方式）。

This kernel module is an example of this. The file (called /proc/sleep) can only be opened by a single process at a time. If the file is already open, the kernel module calls `wait_event_interruptible` . The easiest way to keep a file open is to open it with:  
这个内核模块就是一个例子。该文件（称为 /proc/sleep）一次只能由一个进程打开。如果文件已打开，内核模块将调用 `wait_event_interruptible` 。保持文件打开的最简单方法是使用以下命令打开它：

```bash
tail -f
```

This function changes the status of the task (a task is the kernel data structure which holds information about a process and the system call it is in, if any) to `TASK_INTERRUPTIBLE` , which means that the task will not run until it is woken up somehow, and adds it to WaitQ, the queue of tasks waiting to access the file. Then, the function calls the scheduler to context switch to a different process, one which has some use for the CPU.  
该函数将任务的状态（任务是内核数据结构，保存有关进程及其所在系统调用的信息，如果有的话）更改为 `TASK_INTERRUPTIBLE` ，这意味着该任务将不会运行直到它以某种方式被唤醒，并将其添加到 WaitQ（等待访问文件的任务队列）。然后，该函数调用调度程序将上下文切换到另一个进程，该进程对 CPU 有一定的使用。

When a process is done with the file, it closes it, and `module_close` is called. That function wakes up all the processes in the queue (there’s no mechanism to only wake up one of them). It then returns and the process which just closed the file can continue to run. In time, the scheduler decides that that process has had enough and gives control of the CPU to another process. Eventually, one of the processes which was in the queue will be given control of the CPU by the scheduler. It starts at the point right after the call to `wait_event_interruptible` .  
当进程处理完该文件后，它会关闭该文件，并调用 `module_close` 。该函数会唤醒队列中的所有进程（没有机制只能唤醒其中一个进程）。然后它返回，刚刚关闭文件的进程可以继续运行。随着时间的推移，调度程序认为该进程已经受够了，并将 CPU 控制权交给另一个进程。最终，队列中的进程之一将由调度程序授予 CPU 控制权。它从调用 `wait_event_interruptible` 之后的位置开始。

This means that the process is still in kernel mode - as far as the process is concerned, it issued the open system call and the system call has not returned yet. The process does not know somebody else used the CPU for most of the time between the moment it issued the call and the moment it returned.  
这意味着该进程仍处于内核模式 - 就该进程而言，它发出了 open 系统调用，并且该系统调用尚未返回。从发出调用到返回的大部分时间里，进程并不知道其他人使用了 CPU。

It can then proceed to set a global variable to tell all the other processes that the file is still open and go on with its life. When the other processes get a piece of the CPU, they’ll see that global variable and go back to sleep.  
然后它可以继续设置一个全局变量来告诉所有其他进程该文件仍然打开并继续其生命周期。当其他进程获得 CPU 的一部分时，它们将看到该全局变量并返回睡眠状态。

So we will use `tail -f` to keep the file open in the background, while trying to access it with another process (again in the background, so that we need not switch to a different vt). As soon as the first background process is killed with kill %1 , the second is woken up, is able to access the file and finally terminates.  
因此，我们将使用 `tail -f` 使文件在后台保持打开状态，同时尝试使用另一个进程访问它（再次在后台，这样我们就不需要切换到不同的 vt）。一旦第一个后台进程被 Kill %1 杀死，第二个后台进程就会被唤醒，能够访问该文件并最终终止。

To make our life more interesting, `module_close` does not have a monopoly on waking up the processes which wait to access the file. A signal, such as Ctrl +c (SIGINT) can also wake up a process. This is because we used `wait_event_interruptible` . We could have used `wait_event` instead, but that would have resulted in extremely angry users whose Ctrl+c’s are ignored.  
为了让我们的生活更有趣， `module_close` 并不垄断唤醒等待访问文件的进程。信号，例如 Ctrl +c (SIGINT) 也可以唤醒进程。这是因为我们使用了 `wait_event_interruptible` 。我们本来可以使用 `wait_event` 来代替，但这会导致用户非常愤怒，他们的 Ctrl+c 会被忽略。

In that case, we want to return with `-EINTR` immediately. This is important so users can, for example, kill the process before it receives the file.  
在这种情况下，我们希望立即返回 `-EINTR` 。这很重要，例如，用户可以在进程收到文件之前终止该进程。

There is one more point to remember. Some times processes don’t want to sleep, they want either to get what they want immediately, or to be told it cannot be done. Such processes use the `O_NONBLOCK` flag when opening the file. The kernel is supposed to respond by returning with the error code `-EAGAIN` from operations which would otherwise block, such as opening the file in this example. The program `cat_nonblock` , available in the examples/other directory, can be used to open a file with `O_NONBLOCK` .  
还有一点要记住。有时进程不想休眠，它们想要立即得到它们想要的东西，或者被告知它无法完成。此类进程在打开文件时使用 `O_NONBLOCK` 标志。内核应该通过从否则会阻塞的操作返回错误代码 `-EAGAIN` 来做出响应，例如在本例中打开文件。程序 `cat_nonblock` 位于 Examples/other 目录中，可用于使用 `O_NONBLOCK` 打开文件。

```bash
$ sudo insmod sleep.ko
$ cat_nonblock /proc/sleep
Last input:
$ tail -f /proc/sleep &
Last input:
Last input:
Last input:
Last input:
Last input:
Last input:
Last input:
tail: /proc/sleep: file truncated
[1] 6540
$ cat_nonblock /proc/sleep
Open would block
$ kill %1
[1]+  Terminated              tail -f /proc/sleep
$ cat_nonblock /proc/sleep
Last input:
$
```

```c
/*
 * sleep.c - create a /proc file, and if several processes try to open it
 * at the same time, put all but one to sleep.
 */

#include <linux/atomic.h>
#include <linux/fs.h>
#include <linux/kernel.h> /* for sprintf() */
#include <linux/module.h> /* Specifically, a module */
#include <linux/printk.h>
#include <linux/proc_fs.h> /* Necessary because we use proc fs */
#include <linux/types.h>
#include <linux/uaccess.h> /* for get_user and put_user */
#include <linux/version.h>
#include <linux/wait.h> /* For putting processes to sleep and
                                   waking them up */

#include <asm/current.h>
#include <asm/errno.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

/* Here we keep the last message received, to prove that we can process our
 * input.
 */
#define MESSAGE_LENGTH 80
static char message[MESSAGE_LENGTH];

static struct proc_dir_entry *our_proc_file;
#define PROC_ENTRY_FILENAME "sleep"

/* Since we use the file operations struct, we can't use the special proc
 * output provisions - we have to use a standard read function, which is this
 * function.
 */
static ssize_t module_output(struct file *file, /* see include/linux/fs.h   */
                             char __user *buf, /* The buffer to put data to
                                                   (in the user segment)    */
                             size_t len, /* The length of the buffer */
                             loff_t *offset)
{
    static int finished = 0;
    int i;
    char output_msg[MESSAGE_LENGTH + 30];

    /* Return 0 to signify end of file - that we have nothing more to say
     * at this point.
     */
    if (finished) {
        finished = 0;
        return 0;
    }

    sprintf(output_msg, "Last input:%s\n", message);
    for (i = 0; i < len && output_msg[i]; i++)
        put_user(output_msg[i], buf + i);

    finished = 1;
    return i; /* Return the number of bytes "read" */
}

/* This function receives input from the user when the user writes to the
 * /proc file.
 */
static ssize_t module_input(struct file *file, /* The file itself */
                            const char __user *buf, /* The buffer with input */
                            size_t length, /* The buffer's length */
                            loff_t *offset) /* offset to file - ignore */
{
    int i;

    /* Put the input into Message, where module_output will later be able
     * to use it.
     */
    for (i = 0; i < MESSAGE_LENGTH - 1 && i < length; i++)
        get_user(message[i], buf + i);
    /* we want a standard, zero terminated string */
    message[i] = '\0';

    /* We need to return the number of input characters used */
    return i;
}

/* 1 if the file is currently open by somebody */
static atomic_t already_open = ATOMIC_INIT(0);

/* Queue of processes who want our file */
static DECLARE_WAIT_QUEUE_HEAD(waitq);

/* Called when the /proc file is opened */
static int module_open(struct inode *inode, struct file *file)
{
    /* If the file's flags include O_NONBLOCK, it means the process does not
     * want to wait for the file. In this case, if the file is already open,
     * we should fail with -EAGAIN, meaning "you will have to try again",
     * instead of blocking a process which would rather stay awake.
     */
    if ((file->f_flags & O_NONBLOCK) && atomic_read(&already_open))
        return -EAGAIN;

    /* This is the correct place for try_module_get(THIS_MODULE) because if
     * a process is in the loop, which is within the kernel module,
     * the kernel module must not be removed.
     */
    try_module_get(THIS_MODULE);

    while (atomic_cmpxchg(&already_open, 0, 1)) {
        int i, is_sig = 0;

        /* This function puts the current process, including any system
         * calls, such as us, to sleep.  Execution will be resumed right
         * after the function call, either because somebody called
         * wake_up(&waitq) (only module_close does that, when the file
         * is closed) or when a signal, such as Ctrl-C, is sent
         * to the process
         */
        wait_event_interruptible(waitq, !atomic_read(&already_open));

        /* If we woke up because we got a signal we're not blocking,
         * return -EINTR (fail the system call).  This allows processes
         * to be killed or stopped.
         */
        for (i = 0; i < _NSIG_WORDS && !is_sig; i++)
            is_sig = current->pending.signal.sig[i] & ~current->blocked.sig[i];

        if (is_sig) {
            /* It is important to put module_put(THIS_MODULE) here, because
             * for processes where the open is interrupted there will never
             * be a corresponding close. If we do not decrement the usage
             * count here, we will be left with a positive usage count
             * which we will have no way to bring down to zero, giving us
             * an immortal module, which can only be killed by rebooting
             * the machine.
             */
            module_put(THIS_MODULE);
            return -EINTR;
        }
    }

    return 0; /* Allow the access */
}

/* Called when the /proc file is closed */
static int module_close(struct inode *inode, struct file *file)
{
    /* Set already_open to zero, so one of the processes in the waitq will
     * be able to set already_open back to one and to open the file. All
     * the other processes will be called when already_open is back to one,
     * so they'll go back to sleep.
     */
    atomic_set(&already_open, 0);

    /* Wake up all the processes in waitq, so if anybody is waiting for the
     * file, they can have it.
     */
    wake_up(&waitq);

    module_put(THIS_MODULE);

    return 0; /* success */
}

/* Structures to register as the /proc file, with pointers to all the relevant
 * functions.
 */

/* File operations for our proc file. This is where we place pointers to all
 * the functions called when somebody tries to do something to our file. NULL
 * means we don't want to deal with something.
 */
#ifdef HAVE_PROC_OPS
static const struct proc_ops file_ops_4_our_proc_file = {
    .proc_read = module_output, /* "read" from the file */
    .proc_write = module_input, /* "write" to the file */
    .proc_open = module_open, /* called when the /proc file is opened */
    .proc_release = module_close, /* called when it's closed */
    .proc_lseek = noop_llseek, /* return file->f_pos */
};
#else
static const struct file_operations file_ops_4_our_proc_file = {
    .read = module_output,
    .write = module_input,
    .open = module_open,
    .release = module_close,
    .llseek = noop_llseek,
};
#endif

/* Initialize the module - register the proc file */
static int __init sleep_init(void)
{
    our_proc_file =
        proc_create(PROC_ENTRY_FILENAME, 0644, NULL, &file_ops_4_our_proc_file);
    if (our_proc_file == NULL) {
        pr_debug("Error: Could not initialize /proc/%s\n", PROC_ENTRY_FILENAME);
        return -ENOMEM;
    }
    proc_set_size(our_proc_file, 80);
    proc_set_user(our_proc_file, GLOBAL_ROOT_UID, GLOBAL_ROOT_GID);

    pr_info("/proc/%s created\n", PROC_ENTRY_FILENAME);

    return 0;
}

/* Cleanup - unregister our file from /proc.  This could get dangerous if
 * there are still processes waiting in waitq, because they are inside our
 * open function, which will get unloaded. I'll explain how to avoid removal
 * of a kernel module in such a case in chapter 10.
 */
static void __exit sleep_exit(void)
{
    remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
    pr_debug("/proc/%s removed\n", PROC_ENTRY_FILENAME);
}

module_init(sleep_init);
module_exit(sleep_exit);

MODULE_LICENSE("GPL");
```

```c
/*
 *  cat_nonblock.c - open a file and display its contents, but exit rather than
 *  wait for input.
 */
#include <errno.h> /* for errno */
#include <fcntl.h> /* for open */
#include <stdio.h> /* standard I/O */
#include <stdlib.h> /* for exit */
#include <unistd.h> /* for read */

#define MAX_BYTES 1024 * 4

int main(int argc, char *argv[])
{
    int fd; /* The file descriptor for the file to read */
    size_t bytes; /* The number of bytes read */
    char buffer[MAX_BYTES]; /* The buffer for the bytes */

    /* Usage */
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        puts("Reads the content of a file, but doesn't wait for input");
        exit(-1);
    }

    /* Open the file for reading in non blocking mode */
    fd = open(argv[1], O_RDONLY | O_NONBLOCK);

    /* If open failed */
    if (fd == -1) {
        puts(errno == EAGAIN ? "Open would block" : "Open failed");
        exit(-1);
    }

    /* Read the file and output its contents */
    do {
        /* Read characters from the file */
        bytes = read(fd, buffer, MAX_BYTES);

        /* If there's an error, report it and die */
        if (bytes == -1) {
            if (errno == EAGAIN)
                puts("Normally I'd block, but you told me not to");
            else
                puts("Another read error");
            exit(-1);
        }

        /* Print the characters */
        if (bytes > 0) {
            for (int i = 0; i < bytes; i++)
                putchar(buffer[i]);
        }

        /* While there are no errors and the file isn't over */
    } while (bytes > 0);

    return 0;
}
```

#### 11.2 Completions 11.2 竣工

Sometimes one thing should happen before another within a module having multiple threads. Rather than using `/bin/sleep` commands, the kernel has another way to do this which allows timeouts or interrupts to also happen.  
有时，在具有多个线程的模块中，一件事应该在另一件事之前发生。内核不使用 `/bin/sleep` 命令，而是使用另一种方法来执行此操作，该方法也允许发生超时或中断。

In the following example two threads are started, but one needs to start before another.  
在以下示例中，启动了两个线程，但其中一个线程需要在另一个线程之前启动。

```c
/*
 * completions.c
 */
#include <linux/completion.h>
#include <linux/err.h> /* for IS_ERR() */
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/version.h>

static struct {
    struct completion crank_comp;
    struct completion flywheel_comp;
} machine;

static int machine_crank_thread(void *arg)
{
    pr_info("Turn the crank\n");

    complete_all(&machine.crank_comp);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
    kthread_complete_and_exit(&machine.crank_comp, 0);
#else
    complete_and_exit(&machine.crank_comp, 0);
#endif
}

static int machine_flywheel_spinup_thread(void *arg)
{
    wait_for_completion(&machine.crank_comp);

    pr_info("Flywheel spins up\n");

    complete_all(&machine.flywheel_comp);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
    kthread_complete_and_exit(&machine.flywheel_comp, 0);
#else
    complete_and_exit(&machine.flywheel_comp, 0);
#endif
}

static int __init completions_init(void)
{
    struct task_struct *crank_thread;
    struct task_struct *flywheel_thread;

    pr_info("completions example\n");

    init_completion(&machine.crank_comp);
    init_completion(&machine.flywheel_comp);

    crank_thread = kthread_create(machine_crank_thread, NULL, "KThread Crank");
    if (IS_ERR(crank_thread))
        goto ERROR_THREAD_1;

    flywheel_thread = kthread_create(machine_flywheel_spinup_thread, NULL,
                                     "KThread Flywheel");
    if (IS_ERR(flywheel_thread))
        goto ERROR_THREAD_2;

    wake_up_process(flywheel_thread);
    wake_up_process(crank_thread);

    return 0;

ERROR_THREAD_2:
    kthread_stop(crank_thread);
ERROR_THREAD_1:

    return -1;
}

static void __exit completions_exit(void)
{
    wait_for_completion(&machine.crank_comp);
    wait_for_completion(&machine.flywheel_comp);

    pr_info("completions exit\n");
}

module_init(completions_init);
module_exit(completions_exit);

MODULE_DESCRIPTION("Completions example");
MODULE_LICENSE("GPL");
```

The `machine` structure stores the completion states for the two threads. At the exit point of each thread the respective completion state is updated, and `wait_for_completion` is used by the flywheel thread to ensure that it does not begin prematurely.  
`machine` 结构存储两个线程的完成状态。在每个线程的退出点，相应的完成状态都会更新，飞轮线程使用 `wait_for_completion` 来确保它不会过早开始。

So even though `flywheel_thread` is started first you should notice if you load this module and run `dmesg` that turning the crank always happens first because the flywheel thread waits for it to complete.  
因此，即使首先启动 `flywheel_thread` ，您也应该注意到，如果加载此模块并运行 `dmesg` ，转动曲柄总是首先发生，因为飞轮线程等待它完成。

There are other variations upon the `wait_for_completion` function, which include timeouts or being interrupted, but this basic mechanism is enough for many common situations without adding a lot of complexity.  
`wait_for_completion` 函数还有其他变体，其中包括超时或中断，但这种基本机制足以满足许多常见情况，而不会增加很多复杂性。

### 12 Avoiding Collisions and Deadlocks

12 避免冲突和死锁

If processes running on different CPUs or in different threads try to access the same memory, then it is possible that strange things can happen or your system can lock up. To avoid this, various types of mutual exclusion kernel functions are available. These indicate if a section of code is "locked" or "unlocked" so that simultaneous attempts to run it can not happen.  
如果在不同 CPU 或不同线程上运行的进程尝试访问相同的内存，则可能会发生奇怪的事情或者您的系统可能会锁定。为了避免这种情况，可以使用各种类型的互斥内核函数。这些指示一段代码是否被“锁定”或“解锁”，以便不能同时尝试运行它。

#### 12.1 Mutex 12.1 互斥体

You can use kernel mutexes (mutual exclusions) in much the same manner that you might deploy them in userland. This may be all that is needed to avoid collisions in most cases.  
您可以使用内核互斥体（互斥），其方式与在用户态中部署它们的方式大致相同。在大多数情况下，这可能是避免碰撞所需的全部内容。

```c
/*
 * example_mutex.c
 */
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/printk.h>

static DEFINE_MUTEX(mymutex);

static int __init example_mutex_init(void)
{
    int ret;

    pr_info("example_mutex init\n");

    ret = mutex_trylock(&mymutex);
    if (ret != 0) {
        pr_info("mutex is locked\n");

        if (mutex_is_locked(&mymutex) == 0)
            pr_info("The mutex failed to lock!\n");

        mutex_unlock(&mymutex);
        pr_info("mutex is unlocked\n");
    } else
        pr_info("Failed to lock\n");

    return 0;
}

static void __exit example_mutex_exit(void)
{
    pr_info("example_mutex exit\n");
}

module_init(example_mutex_init);
module_exit(example_mutex_exit);

MODULE_DESCRIPTION("Mutex example");
MODULE_LICENSE("GPL");
```

#### 12.2 Spinlocks 12.2 自旋锁

As the name suggests, spinlocks lock up the CPU that the code is running on, taking 100% of its resources. Because of this you should only use the spinlock mechanism around code which is likely to take no more than a few milliseconds to run and so will not noticeably slow anything down from the user’s point of view.  
顾名思义，自旋锁会锁定正在运行代码的 CPU，占用其 100% 的资源。因此，您应该只在运行时间可能不超过几毫秒的代码周围使用自旋锁机制，因此从用户的角度来看不会明显减慢任何速度。

The example here is "irq safe" in that if interrupts happen during the lock then they will not be forgotten and will activate when the unlock happens, using the `flags` variable to retain their state.  
这里的例子是“irq 安全”的，因为如果在锁定期间发生中断，那么它们将不会被忘记，并且会在解锁发生时激活，使用 `flags` 变量来保留它们的状态。

```c
/*
 * example_spinlock.c
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/spinlock.h>

static DEFINE_SPINLOCK(sl_static);
static spinlock_t sl_dynamic;

static void example_spinlock_static(void)
{
    unsigned long flags;

    spin_lock_irqsave(&sl_static, flags);
    pr_info("Locked static spinlock\n");

    /* Do something or other safely. Because this uses 100% CPU time, this
     * code should take no more than a few milliseconds to run.
     */

    spin_unlock_irqrestore(&sl_static, flags);
    pr_info("Unlocked static spinlock\n");
}

static void example_spinlock_dynamic(void)
{
    unsigned long flags;

    spin_lock_init(&sl_dynamic);
    spin_lock_irqsave(&sl_dynamic, flags);
    pr_info("Locked dynamic spinlock\n");

    /* Do something or other safely. Because this uses 100% CPU time, this
     * code should take no more than a few milliseconds to run.
     */

    spin_unlock_irqrestore(&sl_dynamic, flags);
    pr_info("Unlocked dynamic spinlock\n");
}

static int __init example_spinlock_init(void)
{
    pr_info("example spinlock started\n");

    example_spinlock_static();
    example_spinlock_dynamic();

    return 0;
}

static void __exit example_spinlock_exit(void)
{
    pr_info("example spinlock exit\n");
}

module_init(example_spinlock_init);
module_exit(example_spinlock_exit);

MODULE_DESCRIPTION("Spinlock example");
MODULE_LICENSE("GPL");
```

#### 12.3 Read and write locks

12.3 读写锁

Read and write locks are specialised kinds of spinlocks so that you can exclusively read from something or write to something. Like the earlier spinlocks example, the one below shows an "irq safe" situation in which if other functions were triggered from irqs which might also read and write to whatever you are concerned with then they would not disrupt the logic. As before it is a good idea to keep anything done within the lock as short as possible so that it does not hang up the system and cause users to start revolting against the tyranny of your module.  
读锁和写锁是特殊类型的自旋锁，因此您可以独占地读取某些内容或写入某些内容。与前面的自旋锁示例一样，下面的示例显示了一种“irq 安全”情况，其中如果从 irq 触发其他函数，这些函数也可能读取和写入您所关心的任何内容，那么它们不会破坏逻辑。和以前一样，最好让锁内完成的任何事情尽可能短，这样就不会挂起系统并导致用户开始反抗模块的暴政。

```c
/*
 * example_rwlock.c
 */
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/rwlock.h>

static DEFINE_RWLOCK(myrwlock);

static void example_read_lock(void)
{
    unsigned long flags;

    read_lock_irqsave(&myrwlock, flags);
    pr_info("Read Locked\n");

    /* Read from something */

    read_unlock_irqrestore(&myrwlock, flags);
    pr_info("Read Unlocked\n");
}

static void example_write_lock(void)
{
    unsigned long flags;

    write_lock_irqsave(&myrwlock, flags);
    pr_info("Write Locked\n");

    /* Write to something */

    write_unlock_irqrestore(&myrwlock, flags);
    pr_info("Write Unlocked\n");
}

static int __init example_rwlock_init(void)
{
    pr_info("example_rwlock started\n");

    example_read_lock();
    example_write_lock();

    return 0;
}

static void __exit example_rwlock_exit(void)
{
    pr_info("example_rwlock exit\n");
}

module_init(example_rwlock_init);
module_exit(example_rwlock_exit);

MODULE_DESCRIPTION("Read/Write locks example");
MODULE_LICENSE("GPL");
```

Of course, if you know for sure that there are no functions triggered by irqs which could possibly interfere with your logic then you can use the simpler `read_lock(&myrwlock)` and `read_unlock(&myrwlock)` or the corresponding write functions.  
当然，如果您确定没有由 irqs 触发的函数可能会干扰您的逻辑，那么您可以使用更简单的 `read_lock(&myrwlock)` 和 `read_unlock(&myrwlock)` 或相应的写入函数。

#### 12.4 Atomic operations 12.4 原子操作

If you are doing simple arithmetic: adding, subtracting or bitwise operations, then there is another way in the multi-CPU and multi-hyperthreaded world to stop other parts of the system from messing with your mojo. By using atomic operations you can be confident that your addition, subtraction or bit flip did actually happen and was not overwritten by some other shenanigans. An example is shown below.  
如果您正在做简单的算术：加法、减法或按位运算，那么在多 CPU 和多超线程世界中还有另一种方法可以阻止系统的其他部分扰乱您的魔力。通过使用原子操作，您可以确信您的加法、减法或位翻转确实发生了，并且没有被其他一些恶作剧覆盖。一个例子如下所示。

```c
/*
 * example_atomic.c
 */
#include <linux/atomic.h>
#include <linux/bitops.h>
#include <linux/module.h>
#include <linux/printk.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                   \
    ((byte & 0x80) ? '1' : '0'), ((byte & 0x40) ? '1' : '0'),                  \
        ((byte & 0x20) ? '1' : '0'), ((byte & 0x10) ? '1' : '0'),              \
        ((byte & 0x08) ? '1' : '0'), ((byte & 0x04) ? '1' : '0'),              \
        ((byte & 0x02) ? '1' : '0'), ((byte & 0x01) ? '1' : '0')

static void atomic_add_subtract(void)
{
    atomic_t debbie;
    atomic_t chris = ATOMIC_INIT(50);

    atomic_set(&debbie, 45);

    /* subtract one */
    atomic_dec(&debbie);

    atomic_add(7, &debbie);

    /* add one */
    atomic_inc(&debbie);

    pr_info("chris: %d, debbie: %d\n", atomic_read(&chris),
            atomic_read(&debbie));
}

static void atomic_bitwise(void)
{
    unsigned long word = 0;

    pr_info("Bits 0: " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(word));
    set_bit(3, &word);
    set_bit(5, &word);
    pr_info("Bits 1: " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(word));
    clear_bit(5, &word);
    pr_info("Bits 2: " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(word));
    change_bit(3, &word);

    pr_info("Bits 3: " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(word));
    if (test_and_set_bit(3, &word))
        pr_info("wrong\n");
    pr_info("Bits 4: " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(word));

    word = 255;
    pr_info("Bits 5: " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(word));
}

static int __init example_atomic_init(void)
{
    pr_info("example_atomic started\n");

    atomic_add_subtract();
    atomic_bitwise();

    return 0;
}

static void __exit example_atomic_exit(void)
{
    pr_info("example_atomic exit\n");
}

module_init(example_atomic_init);
module_exit(example_atomic_exit);

MODULE_DESCRIPTION("Atomic operations example");
MODULE_LICENSE("GPL");
```

Before the C11 standard adopts the built-in atomic types, the kernel already provided a small set of atomic types by using a bunch of tricky architecture-specific codes. Implementing the atomic types by C11 atomics may allow the kernel to throw away the architecture-specific codes and letting the kernel code be more friendly to the people who understand the standard. But there are some problems, such as the memory model of the kernel doesn’t match the model formed by the C11 atomics. For further details, see:  
在 C11 标准采用内置原子类型之前，内核已经通过使用一堆棘手的特定于体系结构的代码提供了一小部分原子类型。通过 C11 原子实现原子类型可以允许内核丢弃特定于体系结构的代码，并让内核代码对理解标准的人更加友好。但存在一些问题，例如内核的内存模型与 C11 原子形成的模型不匹配。有关更多详细信息，请参阅：

- [kernel documentation of atomic types  
  原子类型的内核文档](https://www.kernel.org/doc/Documentation/atomic_t.txt)
- [Time to move to C11 atomics?  
  是时候转向 C11 原子了吗？](https://lwn.net/Articles/691128/)
- [Atomic usage patterns in the kernel  
  内核中的原子使用模式](https://lwn.net/Articles/698315/)

### 13 Replacing Print Macros

13 替换打印宏

#### 13.1 Replacement 13.1 更换

In Section [1.7](https://sysprog21.github.io/lkmpg/#before-we-begin), it was noted that the X Window System and kernel module programming are not conducive to integration. This remains valid during the development of kernel modules. However, in practical scenarios, the necessity emerges to relay messages to the tty (teletype) originating the module load command.  
在 1.7 节中，指出 X Window 系统和内核模块编程不利于集成。这在内核模块的开发过程中仍然有效。然而，在实际场景中，需要将消息中继到发出模块加载命令的 tty（电传打字机）。

The term “tty” originates from teletype, which initially referred to a combined keyboard-printer for Unix system communication. Today, it signifies a text stream abstraction employed by Unix programs, encompassing physical terminals, xterms in X displays, and network connections like SSH.  
“tty”一词源自电传打字机，最初指的是用于 Unix 系统通信的组合键盘打印机。今天，它表示 Unix 程序使用的文本流抽象，包括物理终端、X 显示器中的 xterm 以及 SSH 等网络连接。

To achieve this, the “current” pointer is leveraged to access the active task’s tty structure. Within this structure lies a pointer to a string write function, facilitating the string’s transmission to the tty.  
为了实现这一点，利用“当前”指针来访问活动任务的 tty 结构。该结构中有一个指向字符串写入函数的指针，有助于将字符串传输到 tty。

```c
/*
 * print_string.c - Send output to the tty we're running on, regardless if
 * it is through X11, telnet, etc.  We do this by printing the string to the
 * tty associated with the current task.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h> /* For current */
#include <linux/tty.h> /* For the tty declarations */

static void print_string(char *str)
{
    /* The tty for the current task */
    struct tty_struct *my_tty = get_current_tty();

    /* If my_tty is NULL, the current task has no tty you can print to (i.e.,
     * if it is a daemon). If so, there is nothing we can do.
     */
    if (my_tty) {
        const struct tty_operations *ttyops = my_tty->driver->ops;
        /* my_tty->driver is a struct which holds the tty's functions,
         * one of which (write) is used to write strings to the tty.
         * It can be used to take a string either from the user's or
         * kernel's memory segment.
         *
         * The function's 1st parameter is the tty to write to, because the
         * same function would normally be used for all tty's of a certain
         * type.
         * The 2nd parameter is a pointer to a string.
         * The 3rd parameter is the length of the string.
         *
         * As you will see below, sometimes it's necessary to use
         * preprocessor stuff to create code that works for different
         * kernel versions. The (naive) approach we've taken here does not
         * scale well. The right way to deal with this is described in
         * section 2 of
         * linux/Documentation/SubmittingPatches
         */
        (ttyops->write)(my_tty, /* The tty itself */
                        str, /* String */
                        strlen(str)); /* Length */

        /* ttys were originally hardware devices, which (usually) strictly
         * followed the ASCII standard. In ASCII, to move to a new line you
         * need two characters, a carriage return and a line feed. On Unix,
         * the ASCII line feed is used for both purposes - so we can not
         * just use \n, because it would not have a carriage return and the
         * next line will start at the column right after the line feed.
         *
         * This is why text files are different between Unix and MS Windows.
         * In CP/M and derivatives, like MS-DOS and MS Windows, the ASCII
         * standard was strictly adhered to, and therefore a newline requires
         * both a LF and a CR.
         */
        (ttyops->write)(my_tty, "\015\012", 2);
    }
}

static int __init print_string_init(void)
{
    print_string("The module has been inserted.  Hello world!");
    return 0;
}

static void __exit print_string_exit(void)
{
    print_string("The module has been removed.  Farewell world!");
}

module_init(print_string_init);
module_exit(print_string_exit);

MODULE_LICENSE("GPL");
```

#### 13.2 Flashing keyboard LEDs

13.2 键盘 LED 闪烁

In certain conditions, you may desire a simpler and more direct way to communicate to the external world. Flashing keyboard LEDs can be such a solution: It is an immediate way to attract attention or to display a status condition. Keyboard LEDs are present on every hardware, they are always visible, they do not need any setup, and their use is rather simple and non-intrusive, compared to writing to a tty or a file.  
在某些情况下，您可能希望有一种更简单、更直接的方式与外部世界进行交流。闪烁的键盘 LED 可以是这样的解决方案：它是吸引注意力或显示状态条件的直接方法。每个硬件上都有键盘 LED，它们始终可见，不需要任何设置，并且与写入 tty 或文件相比，它们的使用相当简单且非侵入性。

From v4.14 to v4.15, the timer API made a series of changes to improve memory safety. A buffer overflow in the area of a `timer_list` structure may be able to overwrite the `function` and `data` fields, providing the attacker with a way to use return-object programming (ROP) to call arbitrary functions within the kernel. Also, the function prototype of the callback, containing a `unsigned long` argument, will prevent work from any type checking. Furthermore, the function prototype with `unsigned long` argument may be an obstacle to the forward-edge protection of control-flow integrity. Thus, it is better to use a unique prototype to separate from the cluster that takes an `unsigned long` argument. The timer callback should be passed a pointer to the `timer_list` structure rather than an `unsigned long` argument. Then, it wraps all the information the callback needs, including the `timer_list` structure, into a larger structure, and it can use the `container_of` macro instead of the `unsigned long` value. For more information see: [Improving the kernel timers API](https://lwn.net/Articles/735887/).  
从 v4.14 到 v4.15，定时器 API 进行了一系列更改以提高内存安全性。 `timer_list` 结构区域中的缓冲区溢出可能会覆盖 `function` 和 `data` 字段，为攻击者提供使用返回对象的方法编程（ROP）来调用内核中的任意函数。此外，包含 `unsigned long` 参数的回调函数原型将阻止任何类型检查的工作。此外，带有 `unsigned long` 参数的函数原型可能会成为控制流完整性的前缘保护的障碍。因此，最好使用唯一的原型来与采用 `unsigned long` 参数的集群分开。计时器回调应该传递一个指向 `timer_list` 结构的指针，而不是 `unsigned long` 参数。然后，它将回调所需的所有信息（包括 `timer_list` 结构）包装到一个更大的结构中，并且可以使用 `container_of` 宏代替 `unsigned long` 价值。有关更多信息，请参阅：改进内核计时器 API。

Before Linux v4.14, `setup_timer` was used to initialize the timer and the `timer_list` structure looked like:  
在 Linux v4.14 之前， `setup_timer` 用于初始化计时器， `timer_list` 结构如下：

```c
struct timer_list {
    unsigned long expires;
    void (*function)(unsigned long);
    unsigned long data;
    u32 flags;
    /* ... */
};

void setup_timer(struct timer_list *timer, void (*callback)(unsigned long),
                 unsigned long data);
```

Since Linux v4.14, `timer_setup` is adopted and the kernel step by step converting to `timer_setup` from `setup_timer` . One of the reasons why API was changed is it need to coexist with the old version interface. Moreover, the `timer_setup` was implemented by `setup_timer` at first.  
从 Linux v4.14 开始，采用 `timer_setup` ，内核逐步从 `setup_timer` 转换为 `timer_setup` 。 API 变更的原因之一是需要与旧版本接口共存。而且， `timer_setup` 最初是由 `setup_timer` 实现的。

```c
void timer_setup(struct timer_list *timer,
                 void (*callback)(struct timer_list *), unsigned int flags);
```

The `setup_timer` was then removed since v4.15. As a result, the `timer_list` structure had changed to the following.  
自 v4.15 起， `setup_timer` 被删除。结果， `timer_list` 结构更改为以下内容。

```c
struct timer_list {
    unsigned long expires;
    void (*function)(struct timer_list *);
    u32 flags;
    /* ... */
};
```

The following source code illustrates a minimal kernel module which, when loaded, starts blinking the keyboard LEDs until it is unloaded.  
下面的源代码说明了一个最小的内核模块，加载后，键盘 LED 开始闪烁，直到卸载。

```c
/*
 * kbleds.c - Blink keyboard leds until the module is unloaded.
 */

#include <linux/init.h>
#include <linux/kd.h> /* For KDSETLED */
#include <linux/module.h>
#include <linux/tty.h> /* For tty_struct */
#include <linux/vt.h> /* For MAX_NR_CONSOLES */
#include <linux/vt_kern.h> /* for fg_console */
#include <linux/console_struct.h> /* For vc_cons */

MODULE_DESCRIPTION("Example module illustrating the use of Keyboard LEDs.");

static struct timer_list my_timer;
static struct tty_driver *my_driver;
static unsigned long kbledstatus = 0;

#define BLINK_DELAY HZ / 5
#define ALL_LEDS_ON 0x07
#define RESTORE_LEDS 0xFF

/* Function my_timer_func blinks the keyboard LEDs periodically by invoking
 * command KDSETLED of ioctl() on the keyboard driver. To learn more on virtual
 * terminal ioctl operations, please see file:
 *   drivers/tty/vt/vt_ioctl.c, function vt_ioctl().
 *
 * The argument to KDSETLED is alternatively set to 7 (thus causing the led
 * mode to be set to LED_SHOW_IOCTL, and all the leds are lit) and to 0xFF
 * (any value above 7 switches back the led mode to LED_SHOW_FLAGS, thus
 * the LEDs reflect the actual keyboard status).  To learn more on this,
 * please see file: drivers/tty/vt/keyboard.c, function setledstate().
 */
static void my_timer_func(struct timer_list *unused)
{
    struct tty_struct *t = vc_cons[fg_console].d->port.tty;

    if (kbledstatus == ALL_LEDS_ON)
        kbledstatus = RESTORE_LEDS;
    else
        kbledstatus = ALL_LEDS_ON;

    (my_driver->ops->ioctl)(t, KDSETLED, kbledstatus);

    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);
}

static int __init kbleds_init(void)
{
    int i;

    pr_info("kbleds: loading\n");
    pr_info("kbleds: fgconsole is %x\n", fg_console);
    for (i = 0; i < MAX_NR_CONSOLES; i++) {
        if (!vc_cons[i].d)
            break;
        pr_info("poet_atkm: console[%i/%i] #%i, tty %p\n", i, MAX_NR_CONSOLES,
                vc_cons[i].d->vc_num, (void *)vc_cons[i].d->port.tty);
    }
    pr_info("kbleds: finished scanning consoles\n");

    my_driver = vc_cons[fg_console].d->port.tty->driver;
    pr_info("kbleds: tty driver name %s\n", my_driver->driver_name);

    /* Set up the LED blink timer the first time. */
    timer_setup(&my_timer, my_timer_func, 0);
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);

    return 0;
}

static void __exit kbleds_cleanup(void)
{
    pr_info("kbleds: unloading...\n");
    del_timer(&my_timer);
    (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED,
                            RESTORE_LEDS);
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);

MODULE_LICENSE("GPL");
```

If none of the examples in this chapter fit your debugging needs, there might yet be some other tricks to try. Ever wondered what `CONFIG_LL_DEBUG` in `make menuconfig` is good for? If you activate that you get low level access to the serial port. While this might not sound very powerful by itself, you can patch [kernel/printk.c](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/kernel/printk.c) or any other essential syscall to print ASCII characters, thus making it possible to trace virtually everything what your code does over a serial line. If you find yourself porting the kernel to some new and former unsupported architecture, this is usually amongst the first things that should be implemented. Logging over a netconsole might also be worth a try.  
如果本章中的示例都不符合您的调试需求，可能还可以尝试一些其他技巧。有没有想过 `make menuconfig` 中的 `CONFIG_LL_DEBUG` 有什么用处？如果激活它，您将获得对串行端口的低级别访问权限。虽然这本身听起来不是很强大，但您可以修补 kernel/printk.c 或任何其他必要的系统调用来打印 ASCII 字符，从而可以跟踪代码通过串行线路执行的几乎所有操作。如果您发现自己将内核移植到一些新的和以前不受支持的体系结构，这通常是首先应该实现的事情之一。通过网络控制台登录也可能值得一试。

While you have seen lots of stuff that can be used to aid debugging here, there are some things to be aware of. Debugging is almost always intrusive. Adding debug code can change the situation enough to make the bug seem to disappear. Thus, you should keep debug code to a minimum and make sure it does not show up in production code.  
虽然您在这里看到了很多可用于帮助调试的内容，但仍有一些事情需要注意。调试几乎总是侵入性的。添加调试代码可以改变情况，足以使错误看起来消失。因此，您应该将调试代码保持在最低限度，并确保它不会出现在生产代码中。

### 14 Scheduling Tasks 14 调度任务

There are two main ways of running tasks: tasklets and work queues. Tasklets are a quick and easy way of scheduling a single function to be run. For example, when triggered from an interrupt, whereas work queues are more complicated but also better suited to running multiple things in a sequence.  
运行任务有两种主要方式：tasklet 和工作队列。 Tasklet 是一种安排单个函数运行的快速而简单的方法。例如，当从中断触发时，工作队列更复杂，但也更适合按顺序运行多个事物。

#### 14.1 Tasklets 14.1 小任务

Here is an example tasklet module. The `tasklet_fn` function runs for a few seconds. In the meantime, execution of the `example_tasklet_init` function may continue to the exit point, depending on whether it is interrupted by softirq.  
这是一个示例 tasklet 模块。 `tasklet_fn` 函数运行几秒钟。同时， `example_tasklet_init` 函数的执行可能会继续到退出点，具体取决于它是否被 softirq 中断。

```c
/*
 * example_tasklet.c
 */
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/printk.h>

/* Macro DECLARE_TASKLET_OLD exists for compatibility.
 * See https://lwn.net/Articles/830964/
 */
#ifndef DECLARE_TASKLET_OLD
#define DECLARE_TASKLET_OLD(arg1, arg2) DECLARE_TASKLET(arg1, arg2, 0L)
#endif

static void tasklet_fn(unsigned long data)
{
    pr_info("Example tasklet starts\n");
    mdelay(5000);
    pr_info("Example tasklet ends\n");
}

static DECLARE_TASKLET_OLD(mytask, tasklet_fn);

static int __init example_tasklet_init(void)
{
    pr_info("tasklet example init\n");
    tasklet_schedule(&mytask);
    mdelay(200);
    pr_info("Example tasklet init continues...\n");
    return 0;
}

static void __exit example_tasklet_exit(void)
{
    pr_info("tasklet example exit\n");
    tasklet_kill(&mytask);
}

module_init(example_tasklet_init);
module_exit(example_tasklet_exit);

MODULE_DESCRIPTION("Tasklet example");
MODULE_LICENSE("GPL");
```

So with this example loaded `dmesg` should show:  
因此，加载此示例后 `dmesg` 应显示：

```bash
tasklet example init
Example tasklet starts
Example tasklet init continues...
Example tasklet ends
```

Although tasklet is easy to use, it comes with several defators, and developers are discussing about getting rid of tasklet in linux kernel. The tasklet callback runs in atomic context, inside a software interrupt, meaning that it cannot sleep or access user-space data, so not all work can be done in a tasklet handler. Also, the kernel only allows one instance of any given tasklet to be running at any given time; multiple different tasklet callbacks can run in parallel.  
虽然 tasklet 很容易使用，但它带有几个 defators，开发人员正在讨论在 linux 内核中摆脱 tasklet。小任务回调在软件中断内的原子上下文中运行，这意味着它无法休眠或访问用户空间数据，因此并非所有工作都可以在小任务处理程序中完成。此外，内核只允许任何给定微线程的一个实例在任何给定时间运行；多个不同的 tasklet 回调可以并行运行。

In recent kernels, tasklets can be replaced by workqueues, timers, or threaded interrupts.[<sup data-immersive-translate-effect="1" data-immersive_translate_walked="8ab93f9b-58ac-4b89-b455-647ae1c1af85">1</sup>](https://sysprog21.github.io/lkmpg/#fn1x0) While the removal of tasklets remains a longer-term goal, the current kernel contains more than a hundred uses of tasklets. Now developers are proceeding with the API changes and the macro `DECLARE_TASKLET_OLD` exists for compatibility. For further information, see [https://lwn.net/Articles/830964/](https://lwn.net/Articles/830964/).  
在最近的内核中，tasklet 可以被工作队列、定时器或线程中断所取代。 <sup data-immersive-translate-effect="1" data-immersive_translate_walked="8ab93f9b-58ac-4b89-b455-647ae1c1af85">1</sup> 虽然删除微线程仍然是一个长期目标，但当前内核包含一百多种微线程的用途。现在开发人员正在继续进行 API 更改，宏 `DECLARE_TASKLET_OLD` 的存在是为了兼容。有关更多信息，请参阅 https://lwn.net/Articles/830964/。

#### 14.2 Work queues 14.2 工作队列

To add a task to the scheduler we can use a workqueue. The kernel then uses the Completely Fair Scheduler (CFS) to execute work within the queue.  
要将任务添加到调度程序，我们可以使用工作队列。然后，内核使用完全公平调度程序 (CFS) 来执行队列中的工作。

```c
/*
 * sched.c
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>

static struct workqueue_struct *queue = NULL;
static struct work_struct work;

static void work_handler(struct work_struct *data)
{
    pr_info("work handler function.\n");
}

static int __init sched_init(void)
{
    queue = alloc_workqueue("HELLOWORLD", WQ_UNBOUND, 1);
    INIT_WORK(&work, work_handler);
    schedule_work(&work);
    return 0;
}

static void __exit sched_exit(void)
{
    destroy_workqueue(queue);
}

module_init(sched_init);
module_exit(sched_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Workqueue example");
```

### 15 Interrupt Handlers 15 个中断处理程序

#### 15.1 Interrupt Handlers 15.1 中断处理程序

Except for the last chapter, everything we did in the kernel so far we have done as a response to a process asking for it, either by dealing with a special file, sending an `ioctl()` , or issuing a system call. But the job of the kernel is not just to respond to process requests. Another job, which is every bit as important, is to speak to the hardware connected to the machine.  
除了最后一章之外，到目前为止我们在内核中所做的一切都是为了响应进程的请求，无论是处理特殊文件，发送 `ioctl()` ，还是发出系统调用。但内核的工作不仅仅是响应进程请求。另一项同样重要的工作是与连接到机器的硬件进行通信。

There are two types of interaction between the CPU and the rest of the computer’s hardware. The first type is when the CPU gives orders to the hardware, the other is when the hardware needs to tell the CPU something. The second, called interrupts, is much harder to implement because it has to be dealt with when convenient for the hardware, not the CPU. Hardware devices typically have a very small amount of RAM, and if you do not read their information when available, it is lost.  
CPU 和计算机硬件的其余部分之间有两种类型的交互。第一种是 CPU 向硬件发出命令，另一种是硬件需要告诉 CPU 一些事情。第二种称为中断，实现起来要困难得多，因为它必须在硬件（而不是 CPU）方便的时候进行处理。硬件设备通常具有非常少量的 RAM，如果您不读取可用的信息，这些信息就会丢失。

Under Linux, hardware interrupts are called IRQ’s (Interrupt ReQuests). There are two types of IRQ’s, short and long. A short IRQ is one which is expected to take a very short period of time, during which the rest of the machine will be blocked and no other interrupts will be handled. A long IRQ is one which can take longer, and during which other interrupts may occur (but not interrupts from the same device). If at all possible, it is better to declare an interrupt handler to be long.  
在 Linux 下，硬件中断称为 IRQ（中断请求）。 IRQ 有两种类型：短中断和长中断。短 IRQ 是指预计需要很短时间的中断，在此期间机器的其余部分将被阻塞，并且不会处理其他中断。长 IRQ 可能需要更长的时间，并且在此期间可能会发生其他中断（但不是来自同一设备的中断）。如果可能的话，最好将中断处理程序声明得较长。

When the CPU receives an interrupt, it stops whatever it is doing (unless it is processing a more important interrupt, in which case it will deal with this one only when the more important one is done), saves certain parameters on the stack and calls the interrupt handler. This means that certain things are not allowed in the interrupt handler itself, because the system is in an unknown state. Linux kernel solves the problem by splitting interrupt handling into two parts. The first part executes right away and masks the interrupt line. Hardware interrupts must be handled quickly, and that is why we need the second part to handle the heavy work deferred from an interrupt handler. Historically, BH (Linux naming for Bottom Halves) statistically book-keeps the deferred functions. Softirq and its higher level abstraction, Tasklet, replace BH since Linux 2.3.  
当 CPU 收到一个中断时，它会停止正在做的任何事情（除非它正在处理一个更重要的中断，在这种情况下，只有当更重要的中断完成时，它才会处理这个中断），将某些参数保存在堆栈上并调用中断处理程序。这意味着中断处理程序本身不允许执行某些操作，因为系统处于未知状态。 Linux 内核通过将中断处理分为两部分来解决这个问题。第一部分立即执行并屏蔽中断线。硬件中断必须快速处理，这就是为什么我们需要第二部分来处理中断处理程序推迟的繁重工作。从历史上看，BH（Linux 下半部分的命名）以统计方式记录延迟函数。自 Linux 2.3 起，Softirq 及其更高级别的抽象 Tasklet 取代了 BH。

The way to implement this is to call `request_irq()` to get your interrupt handler called when the relevant IRQ is received.  
实现此目的的方法是调用 `request_irq()` 以在收到相关 IRQ 时调用中断处理程序。

In practice IRQ handling can be a bit more complex. Hardware is often designed in a way that chains two interrupt controllers, so that all the IRQs from interrupt controller B are cascaded to a certain IRQ from interrupt controller A. Of course, that requires that the kernel finds out which IRQ it really was afterwards and that adds overhead. Other architectures offer some special, very low overhead, so called "fast IRQ" or FIQs. To take advantage of them requires handlers to be written in assembly language, so they do not really fit into the kernel. They can be made to work similar to the others, but after that procedure, they are no longer any faster than "common" IRQs. SMP enabled kernels running on systems with more than one processor need to solve another truckload of problems. It is not enough to know if a certain IRQs has happened, it’s also important to know what CPU(s) it was for. People still interested in more details, might want to refer to "APIC" now.  
实际上，IRQ 处理可能有点复杂。硬件通常设计为链接两个中断控制器，以便中断控制器 B 的所有 IRQ 都级联到中断控制器 A 的某个 IRQ。当然，这需要内核随后找出它到底是哪个 IRQ，并这会增加开销。其他架构提供一些特殊的、非常低的开销，即所谓的“快速 IRQ”或 FIQ。要利用它们，需要用汇编语言编写处理程序，因此它们并不真正适合内核。它们可以与其他 IRQ 类似地工作，但在该过程之后，它们不再比“常见”IRQ 快。在具有多个处理器的系统上运行的支持 SMP 的内核需要解决另一堆问题。仅仅知道某个 IRQ 是否发生还不够，了解它用于哪些 CPU 也很重要。人们仍然对更多细节感兴趣，现在可能想参考“APIC”。

This function receives the IRQ number, the name of the function, flags, a name for /proc/interrupts and a parameter to be passed to the interrupt handler. Usually there is a certain number of IRQs available. How many IRQs there are is hardware-dependent. The flags can include `SA_SHIRQ` to indicate you are willing to share the IRQ with other interrupt handlers (usually because a number of hardware devices sit on the same IRQ) and `SA_INTERRUPT` to indicate this is a fast interrupt. This function will only succeed if there is not already a handler on this IRQ, or if you are both willing to share.  
该函数接收 IRQ 号、函数名称、标志、/proc/interrupts 的名称以及要传递给中断处理程序的参数。通常有一定数量的可用 IRQ。有多少个 IRQ 取决于硬件。这些标志可以包含 `SA_SHIRQ` 来指示您愿意与其他中断处理程序共享 IRQ（通常是因为许多硬件设备位于同一 IRQ）和 `SA_INTERRUPT` 来指示这是快速中断。仅当此 IRQ 上尚无处理程序或者双方都愿意共享时，此函数才会成功。

#### 15.2 Detecting button presses

15.2 检测按钮按下

Many popular single board computers, such as Raspberry Pi or Beagleboards, have a bunch of GPIO pins. Attaching buttons to those and then having a button press do something is a classic case in which you might need to use interrupts, so that instead of having the CPU waste time and battery power polling for a change in input state, it is better for the input to trigger the CPU to then run a particular handling function.  
许多流行的单板计算机，例如 Raspberry Pi 或 Beagleboard，都有一堆 GPIO 引脚。将按钮附加到这些按钮上，然后按下按钮执行某些操作是一种典型的情况，在这种情况下，您可能需要使用中断，这样就不必让 CPU 浪费时间和电池电量来轮询输入状态的变化，而是更好地执行操作。输入触发 CPU 然后运行特定的处理函数。

Here is an example where buttons are connected to GPIO numbers 17 and 18 and an LED is connected to GPIO 4. You can change those numbers to whatever is appropriate for your board.  
下面是一个示例，其中按钮连接到 GPIO 编号 17 和 18，LED 连接到 GPIO 4。您可以将这些编号更改为适合您的主板的编号。

```c
/*
 * intrpt.c - Handling GPIO with interrupts
 *
 * Based upon the RPi example by Stefan Wendler (devnull@kaltpost.de)
 * from:
 *   https://github.com/wendlers/rpi-kmod-samples
 *
 * Press one button to turn on a LED and another to turn it off.
 */

#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/kernel.h> /* for ARRAY_SIZE() */
#include <linux/module.h>
#include <linux/printk.h>

static int button_irqs[] = { -1, -1 };

/* Define GPIOs for LEDs.
 * TODO: Change the numbers for the GPIO on your board.
 */
static struct gpio leds[] = { { 4, GPIOF_OUT_INIT_LOW, "LED 1" } };

/* Define GPIOs for BUTTONS
 * TODO: Change the numbers for the GPIO on your board.
 */
static struct gpio buttons[] = { { 17, GPIOF_IN, "LED 1 ON BUTTON" },
                                 { 18, GPIOF_IN, "LED 1 OFF BUTTON" } };

/* interrupt function triggered when a button is pressed. */
static irqreturn_t button_isr(int irq, void *data)
{
    /* first button */
    if (irq == button_irqs[0] && !gpio_get_value(leds[0].gpio))
        gpio_set_value(leds[0].gpio, 1);
    /* second button */
    else if (irq == button_irqs[1] && gpio_get_value(leds[0].gpio))
        gpio_set_value(leds[0].gpio, 0);

    return IRQ_HANDLED;
}

static int __init intrpt_init(void)
{
    int ret = 0;

    pr_info("%s\n", __func__);

    /* register LED gpios */
    ret = gpio_request_array(leds, ARRAY_SIZE(leds));

    if (ret) {
        pr_err("Unable to request GPIOs for LEDs: %d\n", ret);
        return ret;
    }

    /* register BUTTON gpios */
    ret = gpio_request_array(buttons, ARRAY_SIZE(buttons));

    if (ret) {
        pr_err("Unable to request GPIOs for BUTTONs: %d\n", ret);
        goto fail1;
    }

    pr_info("Current button1 value: %d\n", gpio_get_value(buttons[0].gpio));

    ret = gpio_to_irq(buttons[0].gpio);

    if (ret < 0) {
        pr_err("Unable to request IRQ: %d\n", ret);
        goto fail2;
    }

    button_irqs[0] = ret;

    pr_info("Successfully requested BUTTON1 IRQ # %d\n", button_irqs[0]);

    ret = request_irq(button_irqs[0], button_isr,
                      IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                      "gpiomod#button1", NULL);

    if (ret) {
        pr_err("Unable to request IRQ: %d\n", ret);
        goto fail2;
    }

    ret = gpio_to_irq(buttons[1].gpio);

    if (ret < 0) {
        pr_err("Unable to request IRQ: %d\n", ret);
        goto fail2;
    }

    button_irqs[1] = ret;

    pr_info("Successfully requested BUTTON2 IRQ # %d\n", button_irqs[1]);

    ret = request_irq(button_irqs[1], button_isr,
                      IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                      "gpiomod#button2", NULL);

    if (ret) {
        pr_err("Unable to request IRQ: %d\n", ret);
        goto fail3;
    }

    return 0;

/* cleanup what has been setup so far */
fail3:
    free_irq(button_irqs[0], NULL);

fail2:
    gpio_free_array(buttons, ARRAY_SIZE(leds));

fail1:
    gpio_free_array(leds, ARRAY_SIZE(leds));

    return ret;
}

static void __exit intrpt_exit(void)
{
    int i;

    pr_info("%s\n", __func__);

    /* free irqs */
    free_irq(button_irqs[0], NULL);
    free_irq(button_irqs[1], NULL);

    /* turn all LEDs off */
    for (i = 0; i < ARRAY_SIZE(leds); i++)
        gpio_set_value(leds[i].gpio, 0);

    /* unregister */
    gpio_free_array(leds, ARRAY_SIZE(leds));
    gpio_free_array(buttons, ARRAY_SIZE(buttons));
}

module_init(intrpt_init);
module_exit(intrpt_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Handle some GPIO interrupts");
```

#### 15.3 Bottom Half 15.3 下半部分

Suppose you want to do a bunch of stuff inside of an interrupt routine. A common way to do that without rendering the interrupt unavailable for a significant duration is to combine it with a tasklet. This pushes the bulk of the work off into the scheduler.  
假设您想在中断例程内执行一系列操作。在不使中断长时间不可用的情况下实现此目的的一种常见方法是将其与微线程结合起来。这将大部分工作推入调度程序。

The example below modifies the previous example to also run an additional task when an interrupt is triggered.  
下面的示例修改了前面的示例，以便在触发中断时也运行附加任务。

```c
/*
 * bottomhalf.c - Top and bottom half interrupt handling
 *
 * Based upon the RPi example by Stefan Wendler (devnull@kaltpost.de)
 * from:
 *    https://github.com/wendlers/rpi-kmod-samples
 *
 * Press one button to turn on an LED and another to turn it off
 */

#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/init.h>

/* Macro DECLARE_TASKLET_OLD exists for compatibiity.
 * See https://lwn.net/Articles/830964/
 */
#ifndef DECLARE_TASKLET_OLD
#define DECLARE_TASKLET_OLD(arg1, arg2) DECLARE_TASKLET(arg1, arg2, 0L)
#endif

static int button_irqs[] = { -1, -1 };

/* Define GPIOs for LEDs.
 * TODO: Change the numbers for the GPIO on your board.
 */
static struct gpio leds[] = { { 4, GPIOF_OUT_INIT_LOW, "LED 1" } };

/* Define GPIOs for BUTTONS
 * TODO: Change the numbers for the GPIO on your board.
 */
static struct gpio buttons[] = {
    { 17, GPIOF_IN, "LED 1 ON BUTTON" },
    { 18, GPIOF_IN, "LED 1 OFF BUTTON" },
};

/* Tasklet containing some non-trivial amount of processing */
static void bottomhalf_tasklet_fn(unsigned long data)
{
    pr_info("Bottom half tasklet starts\n");
    /* do something which takes a while */
    mdelay(500);
    pr_info("Bottom half tasklet ends\n");
}

static DECLARE_TASKLET_OLD(buttontask, bottomhalf_tasklet_fn);

/* interrupt function triggered when a button is pressed */
static irqreturn_t button_isr(int irq, void *data)
{
    /* Do something quickly right now */
    if (irq == button_irqs[0] && !gpio_get_value(leds[0].gpio))
        gpio_set_value(leds[0].gpio, 1);
    else if (irq == button_irqs[1] && gpio_get_value(leds[0].gpio))
        gpio_set_value(leds[0].gpio, 0);

    /* Do the rest at leisure via the scheduler */
    tasklet_schedule(&buttontask);

    return IRQ_HANDLED;
}

static int __init bottomhalf_init(void)
{
    int ret = 0;

    pr_info("%s\n", __func__);

    /* register LED gpios */
    ret = gpio_request_array(leds, ARRAY_SIZE(leds));

    if (ret) {
        pr_err("Unable to request GPIOs for LEDs: %d\n", ret);
        return ret;
    }

    /* register BUTTON gpios */
    ret = gpio_request_array(buttons, ARRAY_SIZE(buttons));

    if (ret) {
        pr_err("Unable to request GPIOs for BUTTONs: %d\n", ret);
        goto fail1;
    }

    pr_info("Current button1 value: %d\n", gpio_get_value(buttons[0].gpio));

    ret = gpio_to_irq(buttons[0].gpio);

    if (ret < 0) {
        pr_err("Unable to request IRQ: %d\n", ret);
        goto fail2;
    }

    button_irqs[0] = ret;

    pr_info("Successfully requested BUTTON1 IRQ # %d\n", button_irqs[0]);

    ret = request_irq(button_irqs[0], button_isr,
                      IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                      "gpiomod#button1", NULL);

    if (ret) {
        pr_err("Unable to request IRQ: %d\n", ret);
        goto fail2;
    }

    ret = gpio_to_irq(buttons[1].gpio);

    if (ret < 0) {
        pr_err("Unable to request IRQ: %d\n", ret);
        goto fail2;
    }

    button_irqs[1] = ret;

    pr_info("Successfully requested BUTTON2 IRQ # %d\n", button_irqs[1]);

    ret = request_irq(button_irqs[1], button_isr,
                      IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                      "gpiomod#button2", NULL);

    if (ret) {
        pr_err("Unable to request IRQ: %d\n", ret);
        goto fail3;
    }

    return 0;

/* cleanup what has been setup so far */
fail3:
    free_irq(button_irqs[0], NULL);

fail2:
    gpio_free_array(buttons, ARRAY_SIZE(leds));

fail1:
    gpio_free_array(leds, ARRAY_SIZE(leds));

    return ret;
}

static void __exit bottomhalf_exit(void)
{
    int i;

    pr_info("%s\n", __func__);

    /* free irqs */
    free_irq(button_irqs[0], NULL);
    free_irq(button_irqs[1], NULL);

    /* turn all LEDs off */
    for (i = 0; i < ARRAY_SIZE(leds); i++)
        gpio_set_value(leds[i].gpio, 0);

    /* unregister */
    gpio_free_array(leds, ARRAY_SIZE(leds));
    gpio_free_array(buttons, ARRAY_SIZE(buttons));
}

module_init(bottomhalf_init);
module_exit(bottomhalf_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Interrupt with top and bottom half");
```

### 16 Crypto 16 加密货币

At the dawn of the internet, everybody trusted everybody completely…but that did not work out so well. When this guide was originally written, it was a more innocent era in which almost nobody actually gave a damn about crypto - least of all kernel developers. That is certainly no longer the case now. To handle crypto stuff, the kernel has its own API enabling common methods of encryption, decryption and your favourite hash functions.  
在互联网诞生之初，每个人都完全信任每个人……但效果并不那么好。当本指南最初编写时，那是一个更加天真的时代，几乎没有人真正关心加密——尤其是所有内核开发人员。现在情况肯定不再是这样了。为了处理加密内容，内核有自己的 API，支持常见的加密、解密方法和您最喜欢的哈希函数。

#### 16.1 Hash functions 16.1 哈希函数

Calculating and checking the hashes of things is a common operation. Here is a demonstration of how to calculate a sha256 hash within a kernel module. To provide the sha256 algorithm support, make sure `CONFIG_CRYPTO_SHA256` is enabled in kernel.  
计算和检查事物的哈希值是一种常见的操作。以下演示了如何在内核模块内计算 sha256 哈希值。要提供 sha256 算法支持，请确保在内核中启用 `CONFIG_CRYPTO_SHA256` 。

```c
/*
 * cryptosha256.c
 */
#include <crypto/internal/hash.h>
#include <linux/module.h>

#define SHA256_LENGTH 32

static void show_hash_result(char *plaintext, char *hash_sha256)
{
    int i;
    char str[SHA256_LENGTH * 2 + 1];

    pr_info("sha256 test for string: \"%s\"\n", plaintext);
    for (i = 0; i < SHA256_LENGTH; i++)
        sprintf(&str[i * 2], "%02x", (unsigned char)hash_sha256[i]);
    str[i * 2] = 0;
    pr_info("%s\n", str);
}

static int __init cryptosha256_init(void)
{
    char *plaintext = "This is a test";
    char hash_sha256[SHA256_LENGTH];
    struct crypto_shash *sha256;
    struct shash_desc *shash;

    sha256 = crypto_alloc_shash("sha256", 0, 0);
    if (IS_ERR(sha256)) {
        pr_err(
            "%s(): Failed to allocate sha256 algorithm, enable CONFIG_CRYPTO_SHA256 and try again.\n",
            __func__);
        return -1;
    }

    shash = kmalloc(sizeof(struct shash_desc) + crypto_shash_descsize(sha256),
                    GFP_KERNEL);
    if (!shash)
        return -ENOMEM;

    shash->tfm = sha256;

    if (crypto_shash_init(shash))
        return -1;

    if (crypto_shash_update(shash, plaintext, strlen(plaintext)))
        return -1;

    if (crypto_shash_final(shash, hash_sha256))
        return -1;

    kfree(shash);
    crypto_free_shash(sha256);

    show_hash_result(plaintext, hash_sha256);

    return 0;
}

static void __exit cryptosha256_exit(void)
{
}

module_init(cryptosha256_init);
module_exit(cryptosha256_exit);

MODULE_DESCRIPTION("sha256 hash test");
MODULE_LICENSE("GPL");
```

Install the module:  
安装模块：

```bash
sudo insmod cryptosha256.ko
sudo dmesg
```

And you should see that the hash was calculated for the test string.  
您应该看到为测试字符串计算了哈希值。

Finally, remove the test module:  
最后，删除测试模块：

```bash
sudo rmmod cryptosha256
```

#### 16.2 Symmetric key encryption

16.2 对称密钥加密

Here is an example of symmetrically encrypting a string using the AES algorithm and a password.  
以下是使用 AES 算法和密码对字符串进行对称加密的示例。

```c
/*
 * cryptosk.c
 */
#include <crypto/internal/skcipher.h>
#include <linux/crypto.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/scatterlist.h>

#define SYMMETRIC_KEY_LENGTH 32
#define CIPHER_BLOCK_SIZE 16

struct tcrypt_result {
    struct completion completion;
    int err;
};

struct skcipher_def {
    struct scatterlist sg;
    struct crypto_skcipher *tfm;
    struct skcipher_request *req;
    struct tcrypt_result result;
    char *scratchpad;
    char *ciphertext;
    char *ivdata;
};

static struct skcipher_def sk;

static void test_skcipher_finish(struct skcipher_def *sk)
{
    if (sk->tfm)
        crypto_free_skcipher(sk->tfm);
    if (sk->req)
        skcipher_request_free(sk->req);
    if (sk->ivdata)
        kfree(sk->ivdata);
    if (sk->scratchpad)
        kfree(sk->scratchpad);
    if (sk->ciphertext)
        kfree(sk->ciphertext);
}

static int test_skcipher_result(struct skcipher_def *sk, int rc)
{
    switch (rc) {
    case 0:
        break;
    case -EINPROGRESS || -EBUSY:
        rc = wait_for_completion_interruptible(&sk->result.completion);
        if (!rc && !sk->result.err) {
            reinit_completion(&sk->result.completion);
            break;
        }
        pr_info("skcipher encrypt returned with %d result %d\n", rc,
                sk->result.err);
        break;
    default:
        pr_info("skcipher encrypt returned with %d result %d\n", rc,
                sk->result.err);
        break;
    }

    init_completion(&sk->result.completion);

    return rc;
}

static void test_skcipher_callback(struct crypto_async_request *req, int error)
{
    struct tcrypt_result *result = req->data;

    if (error == -EINPROGRESS)
        return;

    result->err = error;
    complete(&result->completion);
    pr_info("Encryption finished successfully\n");

    /* decrypt data */
#if 0
    memset((void*)sk.scratchpad, '-', CIPHER_BLOCK_SIZE);
    ret = crypto_skcipher_decrypt(sk.req);
    ret = test_skcipher_result(&sk, ret);
    if (ret)
        return;

    sg_copy_from_buffer(&sk.sg, 1, sk.scratchpad, CIPHER_BLOCK_SIZE);
    sk.scratchpad[CIPHER_BLOCK_SIZE-1] = 0;

    pr_info("Decryption request successful\n");
    pr_info("Decrypted: %s\n", sk.scratchpad);
#endif
}

static int test_skcipher_encrypt(char *plaintext, char *password,
                                 struct skcipher_def *sk)
{
    int ret = -EFAULT;
    unsigned char key[SYMMETRIC_KEY_LENGTH];

    if (!sk->tfm) {
        sk->tfm = crypto_alloc_skcipher("cbc-aes-aesni", 0, 0);
        if (IS_ERR(sk->tfm)) {
            pr_info("could not allocate skcipher handle\n");
            return PTR_ERR(sk->tfm);
        }
    }

    if (!sk->req) {
        sk->req = skcipher_request_alloc(sk->tfm, GFP_KERNEL);
        if (!sk->req) {
            pr_info("could not allocate skcipher request\n");
            ret = -ENOMEM;
            goto out;
        }
    }

    skcipher_request_set_callback(sk->req, CRYPTO_TFM_REQ_MAY_BACKLOG,
                                  test_skcipher_callback, &sk->result);

    /* clear the key */
    memset((void *)key, '\0', SYMMETRIC_KEY_LENGTH);

    /* Use the world's favourite password */
    sprintf((char *)key, "%s", password);

    /* AES 256 with given symmetric key */
    if (crypto_skcipher_setkey(sk->tfm, key, SYMMETRIC_KEY_LENGTH)) {
        pr_info("key could not be set\n");
        ret = -EAGAIN;
        goto out;
    }
    pr_info("Symmetric key: %s\n", key);
    pr_info("Plaintext: %s\n", plaintext);

    if (!sk->ivdata) {
        /* see https://en.wikipedia.org/wiki/Initialization_vector */
        sk->ivdata = kmalloc(CIPHER_BLOCK_SIZE, GFP_KERNEL);
        if (!sk->ivdata) {
            pr_info("could not allocate ivdata\n");
            goto out;
        }
        get_random_bytes(sk->ivdata, CIPHER_BLOCK_SIZE);
    }

    if (!sk->scratchpad) {
        /* The text to be encrypted */
        sk->scratchpad = kmalloc(CIPHER_BLOCK_SIZE, GFP_KERNEL);
        if (!sk->scratchpad) {
            pr_info("could not allocate scratchpad\n");
            goto out;
        }
    }
    sprintf((char *)sk->scratchpad, "%s", plaintext);

    sg_init_one(&sk->sg, sk->scratchpad, CIPHER_BLOCK_SIZE);
    skcipher_request_set_crypt(sk->req, &sk->sg, &sk->sg, CIPHER_BLOCK_SIZE,
                               sk->ivdata);
    init_completion(&sk->result.completion);

    /* encrypt data */
    ret = crypto_skcipher_encrypt(sk->req);
    ret = test_skcipher_result(sk, ret);
    if (ret)
        goto out;

    pr_info("Encryption request successful\n");

out:
    return ret;
}

static int __init cryptoapi_init(void)
{
    /* The world's favorite password */
    char *password = "password123";

    sk.tfm = NULL;
    sk.req = NULL;
    sk.scratchpad = NULL;
    sk.ciphertext = NULL;
    sk.ivdata = NULL;

    test_skcipher_encrypt("Testing", password, &sk);
    return 0;
}

static void __exit cryptoapi_exit(void)
{
    test_skcipher_finish(&sk);
}

module_init(cryptoapi_init);
module_exit(cryptoapi_exit);

MODULE_DESCRIPTION("Symmetric key encryption example");
MODULE_LICENSE("GPL");
```

### 17 Virtual Input Device Driver

17 虚拟输入设备驱动

The input device driver is a module that provides a way to communicate with the interaction device via the event. For example, the keyboard can send the press or release event to tell the kernel what we want to do. The input device driver will allocate a new input structure with `input_allocate_device()` and sets up input bitfields, device id, version, etc. After that, registers it by calling `input_register_device()` .  
输入设备驱动程序是提供通过事件与交互设备进行通信的方式的模块。例如，键盘可以发送按下或释放事件来告诉内核我们想要做什么。输入设备驱动程序将使用 `input_allocate_device()` 分配一个新的输入结构，并设置输入位字段、设备 ID、版本等。之后，通过调用 `input_register_device()` 注册它。

Here is an example, vinput, It is an API to allow easy development of virtual input drivers. The drivers needs to export a `vinput_device()` that contains the virtual device name and `vinput_ops` structure that describes:  
这是一个例子，vinput，它是一个 API，可以轻松开发虚拟输入驱动程序。驱动程序需要导出包含虚拟设备名称的 `vinput_device()` 和描述以下内容的 `vinput_ops` 结构：

- the init function: `init()`  
  初始化函数： `init()`
- the input event injection function: `send()`  
  输入事件注入函数： `send()`
- the readback function: `read()`  
  回读功能： `read()`

Then using `vinput_register_device()` and `vinput_unregister_device()` will add a new device to the list of support virtual input devices.  
然后使用 `vinput_register_device()` 和 `vinput_unregister_device()` 将新设备添加到支持虚拟输入设备列表中。

```c
int init(struct vinput *);
```

This function is passed a `struct vinput` already initialized with an allocated `struct input_dev` . The `init()` function is responsible for initializing the capabilities of the input device and register it.  
该函数传递一个已经用分配的 `struct input_dev` 初始化的 `struct vinput` 。 `init()` 函数负责初始化输入设备的能力并注册。

```c
int send(struct vinput *, char *, int);
```

This function will receive a user string to interpret and inject the event using the `input_report_XXXX` or `input_event` call. The string is already copied from user.  
此函数将接收用户字符串，以使用 `input_report_XXXX` 或 `input_event` 调用解释和注入事件。该字符串已从用户复制。

```c
int read(struct vinput *, char *, int);
```

This function is used for debugging and should fill the buffer parameter with the last event sent in the virtual input device format. The buffer will then be copied to user.  
此函数用于调试，应使用以虚拟输入设备格式发送的最后一个事件填充缓冲区参数。然后缓冲区将被复制给用户。

vinput devices are created and destroyed using sysfs. And, event injection is done through a /dev node. The device name will be used by the userland to export a new virtual input device.  
vinput 设备是使用 sysfs 创建和销毁的。并且，事件注入是通过 /dev 节点完成的。用户空间将使用设备名称来导出新的虚拟输入设备。

The `class_attribute` structure is similar to other attribute types we talked about in section [8](https://sysprog21.github.io/lkmpg/#sysfs-interacting-with-your-module):  
`class_attribute` 结构与我们在第 8 节中讨论的其他属性类型类似：

```c
struct class_attribute {
    struct attribute attr;
    ssize_t (*show)(struct class *class, struct class_attribute *attr,
                    char *buf);
    ssize_t (*store)(struct class *class, struct class_attribute *attr,
                    const char *buf, size_t count);
};
```

In vinput.c, the macro `CLASS_ATTR_WO(export/unexport)` defined in [include/linux/device.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/device.h) (in this case, device.h is included in [include/linux/input.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/input.h)) will generate the `class_attribute` structures which are named class_attr_export/unexport. Then, put them into `vinput_class_attrs` array and the macro `ATTRIBUTE_GROUPS(vinput_class)` will generate the `struct attribute_group vinput_class_group` that should be assigned in `vinput_class` . Finally, call `class_register(&vinput_class)` to create attributes in sysfs.  
在 vinput.c 中，include/linux/device.h 中定义的宏 `CLASS_ATTR_WO(export/unexport)` （在本例中，device.h 包含在 include/linux/input.h 中）将生成 `class_attribute` 数组中，宏 `ATTRIBUTE_GROUPS(vinput_class)` 将生成应在 `vinput_class` 中分配的 `struct attribute_group vinput_class_group` 。最后调用 `class_register(&vinput_class)` 在 sysfs 中创建属性。

To create a vinputX sysfs entry and /dev node.  
创建 vinputX sysfs 条目和 /dev 节点。

```bash
echo "vkbd" | sudo tee /sys/class/vinput/export
```

To unexport the device, just echo its id in unexport:  
要取消导出设备，只需在 unexport 中回显其 ID：

```bash
echo "0" | sudo tee /sys/class/vinput/unexport
```

```c
/*
 * vinput.h
 */

#ifndef VINPUT_H
#define VINPUT_H

#include <linux/input.h>
#include <linux/spinlock.h>

#define VINPUT_MAX_LEN 128
#define MAX_VINPUT 32
#define VINPUT_MINORS MAX_VINPUT

#define dev_to_vinput(dev) container_of(dev, struct vinput, dev)

struct vinput_device;

struct vinput {
    long id;
    long devno;
    long last_entry;
    spinlock_t lock;

    void *priv_data;

    struct device dev;
    struct list_head list;
    struct input_dev *input;
    struct vinput_device *type;
};

struct vinput_ops {
    int (*init)(struct vinput *);
    int (*kill)(struct vinput *);
    int (*send)(struct vinput *, char *, int);
    int (*read)(struct vinput *, char *, int);
};

struct vinput_device {
    char name[16];
    struct list_head list;
    struct vinput_ops *ops;
};

int vinput_register(struct vinput_device *dev);
void vinput_unregister(struct vinput_device *dev);

#endif
```

```c
/*
 * vinput.c
 */

#include <linux/cdev.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include <asm/uaccess.h>

#include "vinput.h"

#define DRIVER_NAME "vinput"

#define dev_to_vinput(dev) container_of(dev, struct vinput, dev)

static DECLARE_BITMAP(vinput_ids, VINPUT_MINORS);

static LIST_HEAD(vinput_devices);
static LIST_HEAD(vinput_vdevices);

static int vinput_dev;
static struct spinlock vinput_lock;
static struct class vinput_class;

/* Search the name of vinput device in the vinput_devices linked list,
 * which added at vinput_register().
 */
static struct vinput_device *vinput_get_device_by_type(const char *type)
{
    int found = 0;
    struct vinput_device *vinput;
    struct list_head *curr;

    spin_lock(&vinput_lock);
    list_for_each (curr, &vinput_devices) {
        vinput = list_entry(curr, struct vinput_device, list);
        if (vinput && strncmp(type, vinput->name, strlen(vinput->name)) == 0) {
            found = 1;
            break;
        }
    }
    spin_unlock(&vinput_lock);

    if (found)
        return vinput;
    return ERR_PTR(-ENODEV);
}

/* Search the id of virtual device in the vinput_vdevices linked list,
 * which added at vinput_alloc_vdevice().
 */
static struct vinput *vinput_get_vdevice_by_id(long id)
{
    struct vinput *vinput = NULL;
    struct list_head *curr;

    spin_lock(&vinput_lock);
    list_for_each (curr, &vinput_vdevices) {
        vinput = list_entry(curr, struct vinput, list);
        if (vinput && vinput->id == id)
            break;
    }
    spin_unlock(&vinput_lock);

    if (vinput && vinput->id == id)
        return vinput;
    return ERR_PTR(-ENODEV);
}

static int vinput_open(struct inode *inode, struct file *file)
{
    int err = 0;
    struct vinput *vinput = NULL;

    vinput = vinput_get_vdevice_by_id(iminor(inode));

    if (IS_ERR(vinput))
        err = PTR_ERR(vinput);
    else
        file->private_data = vinput;

    return err;
}

static int vinput_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t vinput_read(struct file *file, char __user *buffer, size_t count,
                           loff_t *offset)
{
    int len;
    char buff[VINPUT_MAX_LEN + 1];
    struct vinput *vinput = file->private_data;

    len = vinput->type->ops->read(vinput, buff, count);

    if (*offset > len)
        count = 0;
    else if (count + *offset > VINPUT_MAX_LEN)
        count = len - *offset;

    if (raw_copy_to_user(buffer, buff + *offset, count))
        count = -EFAULT;

    *offset += count;

    return count;
}

static ssize_t vinput_write(struct file *file, const char __user *buffer,
                            size_t count, loff_t *offset)
{
    char buff[VINPUT_MAX_LEN + 1];
    struct vinput *vinput = file->private_data;

    memset(buff, 0, sizeof(char) * (VINPUT_MAX_LEN + 1));

    if (count > VINPUT_MAX_LEN) {
        dev_warn(&vinput->dev, "Too long. %d bytes allowed\n", VINPUT_MAX_LEN);
        return -EINVAL;
    }

    if (raw_copy_from_user(buff, buffer, count))
        return -EFAULT;

    return vinput->type->ops->send(vinput, buff, count);
}

static const struct file_operations vinput_fops = {
    .owner = THIS_MODULE,
    .open = vinput_open,
    .release = vinput_release,
    .read = vinput_read,
    .write = vinput_write,
};

static void vinput_unregister_vdevice(struct vinput *vinput)
{
    input_unregister_device(vinput->input);
    if (vinput->type->ops->kill)
        vinput->type->ops->kill(vinput);
}

static void vinput_destroy_vdevice(struct vinput *vinput)
{
    /* Remove from the list first */
    spin_lock(&vinput_lock);
    list_del(&vinput->list);
    clear_bit(vinput->id, vinput_ids);
    spin_unlock(&vinput_lock);

    module_put(THIS_MODULE);

    kfree(vinput);
}

static void vinput_release_dev(struct device *dev)
{
    struct vinput *vinput = dev_to_vinput(dev);
    int id = vinput->id;

    vinput_destroy_vdevice(vinput);

    pr_debug("released vinput%d.\n", id);
}

static struct vinput *vinput_alloc_vdevice(void)
{
    int err;
    struct vinput *vinput = kzalloc(sizeof(struct vinput), GFP_KERNEL);

    try_module_get(THIS_MODULE);

    memset(vinput, 0, sizeof(struct vinput));

    spin_lock_init(&vinput->lock);

    spin_lock(&vinput_lock);
    vinput->id = find_first_zero_bit(vinput_ids, VINPUT_MINORS);
    if (vinput->id >= VINPUT_MINORS) {
        err = -ENOBUFS;
        goto fail_id;
    }
    set_bit(vinput->id, vinput_ids);
    list_add(&vinput->list, &vinput_vdevices);
    spin_unlock(&vinput_lock);

    /* allocate the input device */
    vinput->input = input_allocate_device();
    if (vinput->input == NULL) {
        pr_err("vinput: Cannot allocate vinput input device\n");
        err = -ENOMEM;
        goto fail_input_dev;
    }

    /* initialize device */
    vinput->dev.class = &vinput_class;
    vinput->dev.release = vinput_release_dev;
    vinput->dev.devt = MKDEV(vinput_dev, vinput->id);
    dev_set_name(&vinput->dev, DRIVER_NAME "%lu", vinput->id);

    return vinput;

fail_input_dev:
    spin_lock(&vinput_lock);
    list_del(&vinput->list);
fail_id:
    spin_unlock(&vinput_lock);
    module_put(THIS_MODULE);
    kfree(vinput);

    return ERR_PTR(err);
}

static int vinput_register_vdevice(struct vinput *vinput)
{
    int err = 0;

    /* register the input device */
    vinput->input->name = vinput->type->name;
    vinput->input->phys = "vinput";
    vinput->input->dev.parent = &vinput->dev;

    vinput->input->id.bustype = BUS_VIRTUAL;
    vinput->input->id.product = 0x0000;
    vinput->input->id.vendor = 0x0000;
    vinput->input->id.version = 0x0000;

    err = vinput->type->ops->init(vinput);

    if (err == 0)
        dev_info(&vinput->dev, "Registered virtual input %s %ld\n",
                 vinput->type->name, vinput->id);

    return err;
}

static ssize_t export_store(struct class *class, struct class_attribute *attr,
                            const char *buf, size_t len)
{
    int err;
    struct vinput *vinput;
    struct vinput_device *device;

    device = vinput_get_device_by_type(buf);
    if (IS_ERR(device)) {
        pr_info("vinput: This virtual device isn't registered\n");
        err = PTR_ERR(device);
        goto fail;
    }

    vinput = vinput_alloc_vdevice();
    if (IS_ERR(vinput)) {
        err = PTR_ERR(vinput);
        goto fail;
    }

    vinput->type = device;
    err = device_register(&vinput->dev);
    if (err < 0)
        goto fail_register;

    err = vinput_register_vdevice(vinput);
    if (err < 0)
        goto fail_register_vinput;

    return len;

fail_register_vinput:
    device_unregister(&vinput->dev);
fail_register:
    vinput_destroy_vdevice(vinput);
fail:
    return err;
}
/* This macro generates class_attr_export structure and export_store() */
static CLASS_ATTR_WO(export);

static ssize_t unexport_store(struct class *class, struct class_attribute *attr,
                              const char *buf, size_t len)
{
    int err;
    unsigned long id;
    struct vinput *vinput;

    err = kstrtol(buf, 10, &id);
    if (err) {
        err = -EINVAL;
        goto failed;
    }

    vinput = vinput_get_vdevice_by_id(id);
    if (IS_ERR(vinput)) {
        pr_err("vinput: No such vinput device %ld\n", id);
        err = PTR_ERR(vinput);
        goto failed;
    }

    vinput_unregister_vdevice(vinput);
    device_unregister(&vinput->dev);

    return len;
failed:
    return err;
}
/* This macro generates class_attr_unexport structure and unexport_store() */
static CLASS_ATTR_WO(unexport);

static struct attribute *vinput_class_attrs[] = {
    &class_attr_export.attr,
    &class_attr_unexport.attr,
    NULL,
};

/* This macro generates vinput_class_groups structure */
ATTRIBUTE_GROUPS(vinput_class);

static struct class vinput_class = {
    .name = "vinput",
    .owner = THIS_MODULE,
    .class_groups = vinput_class_groups,
};

int vinput_register(struct vinput_device *dev)
{
    spin_lock(&vinput_lock);
    list_add(&dev->list, &vinput_devices);
    spin_unlock(&vinput_lock);

    pr_info("vinput: registered new virtual input device '%s'\n", dev->name);

    return 0;
}
EXPORT_SYMBOL(vinput_register);

void vinput_unregister(struct vinput_device *dev)
{
    struct list_head *curr, *next;

    /* Remove from the list first */
    spin_lock(&vinput_lock);
    list_del(&dev->list);
    spin_unlock(&vinput_lock);

    /* unregister all devices of this type */
    list_for_each_safe (curr, next, &vinput_vdevices) {
        struct vinput *vinput = list_entry(curr, struct vinput, list);
        if (vinput && vinput->type == dev) {
            vinput_unregister_vdevice(vinput);
            device_unregister(&vinput->dev);
        }
    }

    pr_info("vinput: unregistered virtual input device '%s'\n", dev->name);
}
EXPORT_SYMBOL(vinput_unregister);

static int __init vinput_init(void)
{
    int err = 0;

    pr_info("vinput: Loading virtual input driver\n");

    vinput_dev = register_chrdev(0, DRIVER_NAME, &vinput_fops);
    if (vinput_dev < 0) {
        pr_err("vinput: Unable to allocate char dev region\n");
        err = vinput_dev;
        goto failed_alloc;
    }

    spin_lock_init(&vinput_lock);

    err = class_register(&vinput_class);
    if (err < 0) {
        pr_err("vinput: Unable to register vinput class\n");
        goto failed_class;
    }

    return 0;
failed_class:
    class_unregister(&vinput_class);
failed_alloc:
    return err;
}

static void __exit vinput_end(void)
{
    pr_info("vinput: Unloading virtual input driver\n");

    unregister_chrdev(vinput_dev, DRIVER_NAME);
    class_unregister(&vinput_class);
}

module_init(vinput_init);
module_exit(vinput_end);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Emulate input events");
```

Here the virtual keyboard is one of example to use vinput. It supports all `KEY_MAX` keycodes. The injection format is the `KEY_CODE` such as defined in [include/linux/input.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/input.h). A positive value means `KEY_PRESS` while a negative value is a `KEY_RELEASE` . The keyboard supports repetition when the key stays pressed for too long. The following demonstrates how simulation work.  
这里虚拟键盘是使用 vinput 的示例之一。它支持所有 `KEY_MAX` 键码。注入格式是 `KEY_CODE` ，例如在 include/linux/input.h 中定义的。正值表示 `KEY_PRESS` ，负值表示 `KEY_RELEASE` 。当按键按下时间过长时，键盘支持重复。下面演示了模拟是如何工作的。

Simulate a key press on "g" ( `KEY_G` = 34):  
模拟按下“g”键（ `KEY_G` = 34）：

```bash
echo "+34" | sudo tee /dev/vinput0
```

Simulate a key release on "g" ( `KEY_G` = 34):  
模拟“g”上的按键释放 ( `KEY_G` = 34)：

```bash
echo "-34" | sudo tee /dev/vinput0
```

```c
/*
 * vkbd.c
 */

#include <linux/init.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/spinlock.h>

#include "vinput.h"

#define VINPUT_KBD "vkbd"
#define VINPUT_RELEASE 0
#define VINPUT_PRESS 1

static unsigned short vkeymap[KEY_MAX];

static int vinput_vkbd_init(struct vinput *vinput)
{
    int i;

    /* Set up the input bitfield */
    vinput->input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
    vinput->input->keycodesize = sizeof(unsigned short);
    vinput->input->keycodemax = KEY_MAX;
    vinput->input->keycode = vkeymap;

    for (i = 0; i < KEY_MAX; i++)
        set_bit(vkeymap[i], vinput->input->keybit);

    /* vinput will help us allocate new input device structure via
     * input_allocate_device(). So, we can register it straightforwardly.
     */
    return input_register_device(vinput->input);
}

static int vinput_vkbd_read(struct vinput *vinput, char *buff, int len)
{
    spin_lock(&vinput->lock);
    len = snprintf(buff, len, "%+ld\n", vinput->last_entry);
    spin_unlock(&vinput->lock);

    return len;
}

static int vinput_vkbd_send(struct vinput *vinput, char *buff, int len)
{
    int ret;
    long key = 0;
    short type = VINPUT_PRESS;

    /* Determine which event was received (press or release)
     * and store the state.
     */
    if (buff[0] == '+')
        ret = kstrtol(buff + 1, 10, &key);
    else
        ret = kstrtol(buff, 10, &key);
    if (ret)
        dev_err(&vinput->dev, "error during kstrtol: -%d\n", ret);
    spin_lock(&vinput->lock);
    vinput->last_entry = key;
    spin_unlock(&vinput->lock);

    if (key < 0) {
        type = VINPUT_RELEASE;
        key = -key;
    }

    dev_info(&vinput->dev, "Event %s code %ld\n",
             (type == VINPUT_RELEASE) ? "VINPUT_RELEASE" : "VINPUT_PRESS", key);

    /* Report the state received to input subsystem. */
    input_report_key(vinput->input, key, type);
    /* Tell input subsystem that it finished the report. */
    input_sync(vinput->input);

    return len;
}

static struct vinput_ops vkbd_ops = {
    .init = vinput_vkbd_init,
    .send = vinput_vkbd_send,
    .read = vinput_vkbd_read,
};

static struct vinput_device vkbd_dev = {
    .name = VINPUT_KBD,
    .ops = &vkbd_ops,
};

static int __init vkbd_init(void)
{
    int i;

    for (i = 0; i < KEY_MAX; i++)
        vkeymap[i] = i;
    return vinput_register(&vkbd_dev);
}

static void __exit vkbd_end(void)
{
    vinput_unregister(&vkbd_dev);
}

module_init(vkbd_init);
module_exit(vkbd_end);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Emulate keyboard input events through /dev/vinput");
```

### 18 Standardizing the interfaces: The Device Model

18 标准化接口：设备模型

Up to this point we have seen all kinds of modules doing all kinds of things, but there was no consistency in their interfaces with the rest of the kernel. To impose some consistency such that there is at minimum a standardized way to start, suspend and resume a device model was added. An example is shown below, and you can use this as a template to add your own suspend, resume or other interface functions.  
到目前为止，我们已经看到各种各样的模块在做各种各样的事情，但是它们的接口与内核的其余部分没有一致性。为了强加一定的一致性，至少添加了一种启动、暂停和恢复设备模型的标准化方法。下面是一个示例，您可以以此为模板添加自己的挂起、恢复或其他接口函数。

```c
/*
 * devicemodel.c
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>

struct devicemodel_data {
    char *greeting;
    int number;
};

static int devicemodel_probe(struct platform_device *dev)
{
    struct devicemodel_data *pd =
        (struct devicemodel_data *)(dev->dev.platform_data);

    pr_info("devicemodel probe\n");
    pr_info("devicemodel greeting: %s; %d\n", pd->greeting, pd->number);

    /* Your device initialization code */

    return 0;
}

static int devicemodel_remove(struct platform_device *dev)
{
    pr_info("devicemodel example removed\n");

    /* Your device removal code */

    return 0;
}

static int devicemodel_suspend(struct device *dev)
{
    pr_info("devicemodel example suspend\n");

    /* Your device suspend code */

    return 0;
}

static int devicemodel_resume(struct device *dev)
{
    pr_info("devicemodel example resume\n");

    /* Your device resume code */

    return 0;
}

static const struct dev_pm_ops devicemodel_pm_ops = {
    .suspend = devicemodel_suspend,
    .resume = devicemodel_resume,
    .poweroff = devicemodel_suspend,
    .freeze = devicemodel_suspend,
    .thaw = devicemodel_resume,
    .restore = devicemodel_resume,
};

static struct platform_driver devicemodel_driver = {
    .driver =
        {
            .name = "devicemodel_example",
            .pm = &devicemodel_pm_ops,
        },
    .probe = devicemodel_probe,
    .remove = devicemodel_remove,
};

static int __init devicemodel_init(void)
{
    int ret;

    pr_info("devicemodel init\n");

    ret = platform_driver_register(&devicemodel_driver);

    if (ret) {
        pr_err("Unable to register driver\n");
        return ret;
    }

    return 0;
}

static void __exit devicemodel_exit(void)
{
    pr_info("devicemodel exit\n");
    platform_driver_unregister(&devicemodel_driver);
}

module_init(devicemodel_init);
module_exit(devicemodel_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linux Device Model example");
```

### 19 Optimizations 19 优化

#### 19.1 Likely and Unlikely conditions

19.1 可能和不可能的情况

Sometimes you might want your code to run as quickly as possible, especially if it is handling an interrupt or doing something which might cause noticeable latency. If your code contains boolean conditions and if you know that the conditions are almost always likely to evaluate as either `true` or `false` , then you can allow the compiler to optimize for this using the `likely` and `unlikely` macros. For example, when allocating memory you are almost always expecting this to succeed.  
有时您可能希望代码尽可能快地运行，特别是当它正在处理中断或执行可能导致明显延迟的操作时。如果您的代码包含布尔条件，并且您知道这些条件几乎总是可能计算为 `true` 或 `false` ，那么您可以允许编译器使用 < b2> 和 `unlikely` 宏。例如，当分配内存时，您几乎总是期望它会成功。

```c
bvl = bvec_alloc(gfp_mask, nr_iovecs, &idx);
if (unlikely(!bvl)) {
    mempool_free(bio, bio_pool);
    bio = NULL;
    goto out;
}
```

When the `unlikely` macro is used, the compiler alters its machine instruction output, so that it continues along the false branch and only jumps if the condition is true. That avoids flushing the processor pipeline. The opposite happens if you use the `likely` macro.  
当使用 `unlikely` 宏时，编译器会更改其机器指令输出，以便它沿着 false 分支继续，并且仅在条件为 true 时才跳转。这可以避免刷新处理器管道。如果您使用 `likely` 宏，则会发生相反的情况。

#### 19.2 Static keys 19.2 静态键

Static keys allow us to enable or disable kernel code paths based on the runtime state of key. Its APIs have been available since 2010 (most architectures are already supported), use self-modifying code to eliminate the overhead of cache and branch prediction. The most typical use case of static keys is for performance-sensitive kernel code, such as tracepoints, context switching, networking, etc. These hot paths of the kernel often contain branches and can be optimized easily using this technique. Before we can use static keys in the kernel, we need to make sure that gcc supports `asm goto` inline assembly, and the following kernel configurations are set:  
静态键允许我们根据键的运行时状态启用或禁用内核代码路径。它的 API 自 2010 年以来就已经可用（大多数架构已经支持），使用自修改代码来消除缓存和分支预测的开销。静态键最典型的用例是性能敏感的内核代码，例如跟踪点、上下文切换、网络等。内核的这些热路径通常包含分支，并且可以使用此技术轻松优化。在内核中使用静态键之前，我们需要确保 gcc 支持 `asm goto` 内联汇编，并设置以下内核配置：

```makefile
CONFIG_JUMP_LABEL=y
CONFIG_HAVE_ARCH_JUMP_LABEL=y
CONFIG_HAVE_ARCH_JUMP_LABEL_RELATIVE=y
```

To declare a static key, we need to define a global variable using the `DEFINE_STATIC_KEY_FALSE` or `DEFINE_STATIC_KEY_TRUE` macro defined in [include/linux/jump_label.h](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/include/linux/jump_label.h). This macro initializes the key with the given initial value, which is either false or true, respectively. For example, to declare a static key with an initial value of false, we can use the following code:  
要声明静态键，我们需要使用 include/linux/jump_label.h 中定义的 `DEFINE_STATIC_KEY_FALSE` 或 `DEFINE_STATIC_KEY_TRUE` 宏定义一个全局变量。该宏使用给定的初始值初始化键，该初始值分别为 false 或 true。例如，要声明一个初始值为 false 的静态键，我们可以使用以下代码：

```c
DEFINE_STATIC_KEY_FALSE(fkey);
```

Once the static key has been declared, we need to add branching code to the module that uses the static key. For example, the code includes a fastpath, where a no-op instruction will be generated at compile time as the key is initialized to false and the branch is unlikely to be taken.  
声明静态密钥后，我们需要向使用静态密钥的模块添加分支代码。例如，代码包含快速路径，其中将在编译时生成无操作指令，因为密钥被初始化为 false 并且不太可能采用分支。

```c
pr_info("fastpath 1\n");
if (static_branch_unlikely(&fkey))
    pr_alert("do unlikely thing\n");
pr_info("fastpath 2\n");
```

If the key is enabled at runtime by calling `static_branch_enable(&fkey)` , the fastpath will be patched with an unconditional jump instruction to the slowpath code `pr_alert` , so the branch will always be taken until the key is disabled again.  
如果在运行时通过调用 `static_branch_enable(&fkey)` 启用该键，则快速路径将使用无条件跳转指令修补到慢速路径代码 `pr_alert` ，因此将始终采用分支，直到该键被调用为止。再次禁用。

The following kernel module derived from chardev.c, demonstrates how the static key works.  
以下源自 chardev.c 的内核模块演示了静态密钥的工作原理。

```c
/*
 * static_key.c
 */

#include <linux/atomic.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kernel.h> /* for sprintf() */
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h> /* for get_user and put_user */
#include <linux/jump_label.h> /* for static key macros */

#include <asm/errno.h>

static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *file, char __user *buf, size_t count,
                           loff_t *ppos);
static ssize_t device_write(struct file *file, const char __user *buf,
                            size_t count, loff_t *ppos);

#define SUCCESS 0
#define DEVICE_NAME "key_state"
#define BUF_LEN 10

static int major;

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

static char msg[BUF_LEN + 1];

static struct class *cls;

static DEFINE_STATIC_KEY_FALSE(fkey);

static struct file_operations chardev_fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
};

static int __init chardev_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops);
    if (major < 0) {
        pr_alert("Registering char device failed with %d\n", major);
        return major;
    }

    pr_info("I was assigned major number %d\n", major);

    cls = class_create(THIS_MODULE, DEVICE_NAME);

    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    pr_info("Device created on /dev/%s\n", DEVICE_NAME);

    return SUCCESS;
}

static void __exit chardev_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);

    /* Unregister the device */
    unregister_chrdev(major, DEVICE_NAME);
}

/* Methods */

/**
 * Called when a process tried to open the device file, like
 * cat /dev/key_state
 */
static int device_open(struct inode *inode, struct file *file)
{
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;

    sprintf(msg, static_key_enabled(&fkey) ? "enabled\n" : "disabled\n");

    pr_info("fastpath 1\n");
    if (static_branch_unlikely(&fkey))
        pr_alert("do unlikely thing\n");
    pr_info("fastpath 2\n");

    try_module_get(THIS_MODULE);

    return SUCCESS;
}

/**
 * Called when a process closes the device file
 */
static int device_release(struct inode *inode, struct file *file)
{
    /* We are now ready for our next caller. */
    atomic_set(&already_open, CDEV_NOT_USED);

    /**
     * Decrement the usage count, or else once you opened the file, you will
     * never get rid of the module.
     */
    module_put(THIS_MODULE);

    return SUCCESS;
}

/**
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp, /* see include/linux/fs.h */
                           char __user *buffer, /* buffer to fill with data */
                           size_t length, /* length of the buffer */
                           loff_t *offset)
{
    /* Number of the bytes actually written to the buffer */
    int bytes_read = 0;
    const char *msg_ptr = msg;

    if (!*(msg_ptr + *offset)) { /* We are at the end of the message */
        *offset = 0; /* reset the offset */
        return 0; /* signify end of file */
    }

    msg_ptr += *offset;

    /* Actually put the data into the buffer */
    while (length && *msg_ptr) {
        /**
         * The buffer is in the user data segment, not the kernel
         * segment so "*" assignment won't work. We have to use
         * put_user which copies data from the kernel data segment to
         * the user data segment.
         */
        put_user(*(msg_ptr++), buffer++);
        length--;
        bytes_read++;
    }

    *offset += bytes_read;

    /* Most read functions return the number of bytes put into the buffer. */
    return bytes_read;
}

/* Called when a process writes to dev file; echo "enable" > /dev/key_state */
static ssize_t device_write(struct file *filp, const char __user *buffer,
                            size_t length, loff_t *offset)
{
    char command[10];

    if (length > 10) {
        pr_err("command exceeded 10 char\n");
        return -EINVAL;
    }

    if (copy_from_user(command, buffer, length))
        return -EFAULT;

    if (strncmp(command, "enable", strlen("enable")) == 0)
        static_branch_enable(&fkey);
    else if (strncmp(command, "disable", strlen("disable")) == 0)
        static_branch_disable(&fkey);
    else {
        pr_err("Invalid command: %s\n", command);
        return -EINVAL;
    }

    /* Again, return the number of input characters used. */
    return length;
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
```

To check the state of the static key, we can use the /dev/key_state interface.  
要检查静态密钥的状态，我们可以使用 /dev/key_state 接口。

```bash
cat /dev/key_state
```

This will display the current state of the key, which is disabled by default.  
这将显示该键的当前状态，默认情况下该状态是禁用的。

To change the state of the static key, we can perform a write operation on the file:  
要更改静态密钥的状态，我们可以对文件执行写操作：

```bash
echo enable > /dev/key_state
```

This will enable the static key, causing the code path to switch from the fastpath to the slowpath.  
这将启用静态密钥，导致代码路径从快速路径切换到慢速路径。

In some cases, the key is enabled or disabled at initialization and never changed, we can declare a static key as read-only, which means that it can only be toggled in the module init function. To declare a read-only static key, we can use the `DEFINE_STATIC_KEY_FALSE_RO` or `DEFINE_STATIC_KEY_TRUE_RO` macro instead. Attempts to change the key at runtime will result in a page fault. For more information, see [Static keys](https://www.kernel.org/doc/Documentation/static-keys.txt)  
在某些情况下，密钥在初始化时启用或禁用并且从未更改，我们可以将静态密钥声明为只读，这意味着它只能在模块初始化函数中切换。要声明只读静态键，我们可以使用 `DEFINE_STATIC_KEY_FALSE_RO` 或 `DEFINE_STATIC_KEY_TRUE_RO` 宏。尝试在运行时更改密钥将导致页面错误。有关详细信息，请参阅静态键

### 20 Common Pitfalls 20 个常见陷阱

#### 20.1 Using standard libraries

20.1 使用标准库

You can not do that. In a kernel module, you can only use kernel functions which are the functions you can see in /proc/kallsyms.  
你不能这样做。在内核模块中，您只能使用内核函数，这些函数可以在 /proc/kallsyms 中看到。

#### 20.2 Disabling interrupts

20.2 禁用中断

You might need to do this for a short time and that is OK, but if you do not enable them afterwards, your system will be stuck and you will have to power it off.  
您可能需要在短时间内执行此操作，这没关系，但如果您之后不启用它们，您的系统将被卡住，您将不得不将其关闭。

### 21 Where To Go From Here?

21 下一步该去哪里？

For people seriously interested in kernel programming, I recommend [kernelnewbies.org](https://kernelnewbies.org/) and the [Documentation](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/Documentation) subdirectory within the kernel source code which is not always easy to understand but can be a starting point for further investigation. Also, as Linus Torvalds said, the best way to learn the kernel is to read the source code yourself.  
对于对内核编程非常感兴趣的人，我推荐 kernelnewbies.org 和内核源代码中的 Documentation 子目录，它并不总是很容易理解，但可以作为进一步研究的起点。另外，正如 Linus Torvalds 所说，学习内核的最好方法就是自己阅读源代码。

If you would like to contribute to this guide or notice anything glaringly wrong, please create an issue at [https://github.com/sysprog21/lkmpg](https://github.com/sysprog21/lkmpg). Your pull requests will be appreciated.  
如果您想为本指南做出贡献或发现任何明显错误，请在 https://github.com/sysprog21/lkmpg 上创建问题。您的拉取请求将不胜感激。

Happy hacking!   快乐黑客！

[<sup data-immersive-translate-effect="1" data-immersive_translate_walked="8ab93f9b-58ac-4b89-b455-647ae1c1af85">1</sup>](https://sysprog21.github.io/lkmpg/#fn1x0-bk)The goal of threaded interrupts is to push more of the work to separate threads, so that the minimum needed for acknowledging an interrupt is reduced, and therefore the time spent handling the interrupt (where it can’t handle any other interrupts at the same time) is reduced. See [https://lwn.net/Articles/302043/](https://lwn.net/Articles/302043/)  
<sup data-immersive-translate-effect="1" data-immersive_translate_walked="8ab93f9b-58ac-4b89-b455-647ae1c1af85">1</sup> 线程中断的目标是将更多的工作推送到单独的线程，从而减少确认中断所需的最少时间，从而减少处理中断所花费的时间（在它无法处理的情况下）同时减少任何其他中断。参见https://lwn.net/Articles/302043/.
