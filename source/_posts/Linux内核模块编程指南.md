---
title: Linux内核模块编程指南
date: 2023-09-18 18:04:17
tags: linux 内核
category: linux
---

# 简介

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

## Authorship 作者身份

The Linux Kernel Module Programming Guide was initially authored by Ori Pomerantz for Linux v2.2. As the Linux kernel evolved, Ori’s availability to maintain the document diminished. Consequently, Peter Jay Salzman assumed the role of maintainer and updated the guide for Linux v2.4. Similar constraints arose for Peter when tracking developments in Linux v2.6, leading to Michael Burian joining as a co-maintainer to bring the guide up to speed with Linux v2.6. Bob Mottram contributed to the guide by updating examples for Linux v3.8 and later. Jim Huang then undertook the task of updating the guide for recent Linux versions (v5.0 and beyond), along with revising the LaTeX document.
Linux 内核模块编程指南最初由 Ori Pomerantz 针对 Linux v2.2 编写。随着 Linux 内核的发展，Ori 维护文档的可用性逐渐减弱。因此，Peter Jay Salzman 承担了维护者的角色并更新了 Linux v2.4 的指南。 Peter 在跟踪 Linux v2.6 的开发时也遇到了类似的限制，导致 Michael Burian 作为共同维护者加入，以加快该指南与 Linux v2.6 的速度。 Bob Mottram 通过更新 Linux v3.8 及更高版本的示例为本指南做出了贡献。 Jim Huang 随后承担了更新最新 Linux 版本（v5.0 及更高版本）指南以及修订 LaTeX 文档的任务。

## Acknowledgements 致谢

The following people have contributed corrections or good suggestions:
以下人员提供了更正或好的建议：

Andy Shevchenko, Arush Sharma, Benno Bielmeier, Bob Lee, Brad Baker, Che-Chia Chang, Chih-En Lin, Chih-Hsuan Yang, Chih-Yu Chen, Ching-Hua (Vivian) Lin, Chin Yik Ming, Cyril Brulebois, Daniele Paolo Scarpazza, David Porter, demonsome, Dimo Velev, Ekang Monyet, Ethan Chan, fennecJ, Francois Audeon, Gilad Reti, Horst Schirmeier, Hsin-Hsiang Peng, Ignacio Martin, Iûnn Kiàn-îng, Jian-Xing Wu, Johan Calle, keytouch, Kohei Otsuka, manbing, Marconi Jiang, mengxinayan, Peter Lin, Roman Lakeev, Sam Erickson, Shao-Tse Hung, Stacy Prowell, Steven Lung, Tristan Lelong, Tse-Wei Lin, Tucker Polomik, Tyler Fanelli, VxTeemo, Wei-Lun Tsai, Xatierlike Lee, Yin-Chiuan Chen, Yi-Wei Lin, Ylowy, Yu-Hsiang Tseng.

## What Is A Kernel Module? 什么是内核模块？

Involvement in the development of Linux kernel modules requires a foundation in the C programming language and a track record of creating conventional programs intended for process execution. This pursuit delves into a domain where an unregulated pointer, if disregarded, may potentially trigger the total elimination of an entire file system, resulting in a scenario that necessitates a complete system reboot.
参与 Linux 内核模块的开发需要具备 C 编程语言基础以及创建用于进程执行的传统程序的记录。这种追求深入研究了一个领域，其中不受监管的指针如果被忽视，可能会触发整个文件系统的彻底消除，从而导致需要完全重新启动系统的情况。

A Linux kernel module is precisely defined as a code segment capable of dynamic loading and unloading within the kernel as needed. These modules enhance kernel capabilities without necessitating a system reboot. A notable example is seen in the device driver module, which facilitates kernel interaction with hardware components linked to the system. In the absence of modules, the prevailing approach leans toward monolithic kernels, requiring direct integration of new functionalities into the kernel image. This approach leads to larger kernels and necessitates kernel rebuilding and subsequent system rebooting when new functionalities are desired.
Linux 内核模块被精确地定义为能够根据需要在内核内动态加载和卸载的代码段。这些模块增强了内核功能，而无需重新启动系统。一个值得注意的例子是设备驱动程序模块，它促进内核与链接到系统的硬件组件的交互。在没有模块的情况下，流行的方法倾向于整体内核，需要将新功能直接集成到内核映像中。这种方法会导致内核更大，并且在需要新功能时需要重建内核并随后重新启动系统。

## Kernel module package

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

## What Modules are in my Kernel? 我的内核中有哪些模块？

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

## Is there a need to download and compile the kernel? 是否需要下载编译内核？

To effectively follow this guide, there is no obligatory requirement for performing such actions. Nonetheless, a prudent approach involves executing the examples within a test distribution on a virtual machine, thus mitigating any potential risk of disrupting the system.
为了有效遵循本指南，执行此类操作没有强制性要求。尽管如此，谨慎的方法是在虚拟机上执行测试发行版中的示例，从而减轻破坏系统的任何潜在风险。

## Before We Begin 开始之前

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

# Headers 头文件

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

# Examples 示例

All the examples from this document are available within the examples subdirectory.
本文档中的所有示例都可以在示例子目录中找到。

If there are any compile errors then you might have a more recent kernel version or need to install the corresponding kernel header files.
如果存在任何编译错误，则您可能拥有更新的内核版本或需要安装相应的内核头文件。

# Hello World 4 你好世界

## The Simplest Module 最简单的模块

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


If there is no `PWD := $(CURDIR)` statement in Makefile, then it may not compile correctly with sudo make. Because some environment variables are specified by the security policy, they can’t be inherited. The default security policy is sudoers. In the sudoers security policy, env_reset is enabled by default, which restricts environment variables. Specifically, path variables are not retained from the user environment, they are set to default values (For more information see: sudoers manual). You can see the environment variable settings by:
如果 Makefile 中没有 `PWD := $(CURDIR)` 语句，则可能无法使用 `sudo make` 正确编译。由于某些环境变量是由安全策略指定的，因此无法继承。默认安全策略是 sudoers。在sudoers安全策略中，env_reset默认是开启的，它限制了环境变量。具体来说，路径变量不会从用户环境中保留，而是设置为默认值（有关更多信息，请参阅：sudoers 手册）。您可以通过以下方式查看环境变量设置：
```bash
$ sudo -s
# sudo -V
```

 Here is a simple Makefile as an example to demonstrate the problem mentioned above.
这里以一个简单的Makefile为例来演示上述问题。
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






https://sysprog21.github.io/lkmpg/
