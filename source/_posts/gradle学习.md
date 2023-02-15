---
title: gradle学习
date: 2019-11-20 20:54:41
tags: gradle
category: gradle
---
<!-- TOC -->

- [1. 安装`gradle`](#1-安装gradle)
- [2. 入门前奏](#2-入门前奏)
  - [2.1. 实现 `hello world`](#21-实现-hello-world)
  - [2.2. `gradle` 的任务](#22-gradle-的任务)
    - [2.2.1. 创建任务](#221-创建任务)
      - [2.2.1.1. 直接用任务名称创建](#2211-直接用任务名称创建)
      - [2.2.1.2. 任务名称+任务配置创建](#2212-任务名称任务配置创建)
      - [2.2.1.3. TaskContainer的create方法创建](#2213-taskcontainer的create方法创建)
    - [2.2.2. 任务依赖](#222-任务依赖)
      - [2.2.2.1. 动态定义任务](#2221-动态定义任务)
    - [2.2.3. 任务的分组和描述](#223-任务的分组和描述)
  - [2.3. `Gradle` 日志级别](#23-gradle-日志级别)
  - [2.4. `Gradle` 命令行](#24-gradle-命令行)
    - [2.4.1. 获取所有任务信息](#241-获取所有任务信息)
    - [2.4.2. 排除任务](#242-排除任务)
    - [2.4.3. 多任务调用](#243-多任务调用)
    - [2.4.4. 任务名称缩写](#244-任务名称缩写)
- [3. Groovy 快速入门指南](#3-groovy-快速入门指南)
  - [Groovy 概述](#groovy-概述)
  - [Groovy编写和调试](#groovy编写和调试)
  - [变量](#变量)
  - [方法](#方法)
  - [类](#类)

<!-- /TOC -->


# 1. 安装`gradle`
```
➜  ~ sudo apt install gradle

➜  ~ gradle -v
WARNING: An illegal reflective access operation has occurred
WARNING: Illegal reflective access by org.codehaus.groovy.reflection.CachedClass (file:/usr/share/java/groovy-all.jar) to method java.lang.Object.finalize()
WARNING: Please consider reporting this to the maintainers of org.codehaus.groovy.reflection.CachedClass
WARNING: Use --illegal-access=warn to enable warnings of further illegal reflective access operations
WARNING: All illegal access operations will be denied in a future release

------------------------------------------------------------
Gradle 4.4.1
------------------------------------------------------------

Build time:   2012-12-21 00:00:00 UTC
Revision:     none

Groovy:       2.4.16
Ant:          Apache Ant(TM) version 1.10.5 compiled on March 28 2019
JVM:          11.0.4 (Ubuntu 11.0.4+11-post-Ubuntu-1ubuntu218.04.3)
OS:           Linux 5.0.0-36-generic amd64

```

# 2. 入门前奏

## 2.1. 实现 `hello world`

新建一个目录，例如 `gradle01` ,在这个目录中新建一个 `build.gradle` 文件，输入以下内容：
```
task hello {
    doLast{
        println "hello world"
    }
}
```
运行 `gradle -q hello` 命令：
```
➜  gradle01 gradle -q hello
hello world
```
 `task` （任务）和action(动作)是 `Gradle` 的重要元素。上面的代码中， `task` 代表一个独立的原子性操作，比如复制一个文件，编译一次Java代码，这里我们简单的定义一个名为 `hello` 的任务。 `doLast` 代表`task` 执行的最后一个 `action`，通俗来讲就是 `task` 执行完毕后会回调 `doLast` 中的代码，在上面这个例子中就会打印 `Hello world!` 。

上面的例子可以写的更简洁一些：
```
task hello << {
    println "Hello world"
}
```

## 2.2. `gradle` 的任务

为了更好的讲解后面的Gradle 命令行，这里简单的介绍下Gradle的任务，包括创建任务、任务依赖、 动态定义任务和任务的分组和描述。

### 2.2.1. 创建任务

除了前面实现 `Hello World` 的例子采用的创建任务方式，还有其他的3种创建任务方式。
#### 2.2.1.1. 直接用任务名称创建
```
def Task hello = task(hello)
hello.doLast{
    println "hello world"
}
```

#### 2.2.1.2. 任务名称+任务配置创建
```
def Task hello =  task(hello, group:BasePlugin.BUILD_GROUP)
hello.doLast{
    println "hello world"
}
```
其中 `group` 为任务配置项，它代表了分组。

#### 2.2.1.3. TaskContainer的create方法创建
```
tasks.create(name: "hello") << {
    println "hello world"
}
```

### 2.2.2. 任务依赖
任务依赖会决定任务运行的先后顺序，被依赖的任务会在定义依赖的任务之前执行。创建任务间的依赖关系如下所示。
```
task hello << {
    println "hello world"
}

task go(dependsOn: hello) << {
    println "go for it"
}
```
在 `hello` 任务的基础上增加了一个名为 `go` 的任务，通过 `dependsOn` 来指定依赖的任务为 `hello` ，因此 `go` 任务运行在 `hello` 之后。
```
➜  gradle01 gradle -q go   
hello world
go for it
```

#### 2.2.2.1. 动态定义任务
动态定义任务指的是在运行时来定义任务的名称，如下所示。
```
3.times{
    number->task "task$number" << {
        println "task$number"
    }
}
```
这里用到了 `Groovy` 语法，关于 `Groovy` 语法会在本系列后续的文章进行介绍。`times` 是 `Groovy` 在 `java.lang.Number` 中拓展的方法，是一个定时器。`3.times` 中循环创建了三个新任务，隐式变量 `number` 的值为 `0，1，2` ，任务的名称由 `task` 加上 `number` 的值组成，达到了动态定义任务的目的。
```
➜  gradle01 gradle -q task0
task0
➜  gradle01 gradle -q task1
task1
➜  gradle01 gradle -q task2
task2
```

### 2.2.3. 任务的分组和描述

`Gradle` 有任务组的概念，可以为任务配置分组和描述，以便于更好的管理任务，拥有良好的可读性。改造前面的例子，为 `hello` 任务添加分组和描述。
```
task hello {
    group = "build"
    description = "hello world"
    doLast{
        println "任务分组： ${group}"
        println "任务描述： ${description}"
    }
}
task go(dependsOn: hello) << {
    println "go for it"
}
```
或者采用创建任务的方式来为任务添加分组和描述：
```
def Task hello = task(hello)
hello.description = "hello world"
hello.group = BasePlugin.BUILD_GROUP
hello.doLast{
    println "任务分组: ${group}"
    println "任务描述: ${description}"
}

task go(dependsOn: hello){
    println "go for it"
}
```
最终执行结果。
```
➜  gradle01 gradle -q go
go for it
任务分组: build
任务描述: hello world
```

## 2.3. `Gradle` 日志级别
和Android一样，Gradle也定义了日志级别。

| 级别      | 用于           |
| ---------- | -------------- |
| ERROR     | 错误消息       |
| QUIET     | 重要的信息消息 |
| WARNING   | 警告消息       |
| LIFECYCLE | 进度信息消息   |
| INFO      | 信息性消息     |
| DEBUG     | 调试消息       |

前面我们通过 `gradle -q` + 任务名称来运行一个指定的 `task` ，这个 `q` 是命令行开关选项，通过开关选项可以控制输出的日志级别。

| 开关选项       | 输出日志级别        |
| --------------- | ------------------- |
| 无日志选项     | LIFECYCLE及更高级别 |
| -q或者 --quiet | QUIET及更高级别     |
| -i或者 --info  | INFO及更高级别      |
| -d或者 --debug | DEBUG及更高级别     |

## 2.4. `Gradle` 命令行

### 2.4.1. 获取所有任务信息

这一节的命令行前面[2.2.3. 任务的分组和描述](#223-任务的分组和描述)的代码为例，此前我们通过 `gradle -q +` 任务名称来运行一个指定的任务，如果不知道任务的名称，可以通过运行 `gradle -q tasks` 命令来获取所有的任务信息，这样就不需要打开源码了。
```
➜  gradle01 gradle -a tasks
--no-rebuild/-a has been deprecated and is scheduled to be removed in Gradle 5.0.

> Configure project : 
go for it

> Task :tasks 

------------------------------------------------------------
All tasks runnable from root project
------------------------------------------------------------

Build tasks
-----------
hello - hello world

Build Setup tasks
-----------------
init - Initializes a new Gradle build.
wrapper - Generates Gradle wrapper files.

Help tasks
----------
buildEnvironment - Displays all buildscript dependencies declared in root project 'gradle01'.
components - Displays the components produced by root project 'gradle01'. [incubating]
dependencies - Displays all dependencies declared in root project 'gradle01'.
dependencyInsight - Displays the insight into a specific dependency in root project 'gradle01'.
dependentComponents - Displays the dependent components of components in root project 'gradle01'. [incubating]
help - Displays a help message.
model - Displays the configuration model of root project 'gradle01'. [incubating]
projects - Displays the sub-projects of root project 'gradle01'.
properties - Displays the properties of root project 'gradle01'.
tasks - Displays the tasks runnable from root project 'gradle01'.

To see all tasks and more detail, run gradle tasks --all

To see more detail about a task, run gradle help --task <task>


BUILD SUCCESSFUL in 0s
1 actionable task: 1 executed
```
默认情况下，只会显示那些被分组的任务的名称和描述。比如 `Build tasks` （`Build` 任务组）中有我们定义的 `hello` 任务，`Build Setup tasks` 中有 `init` 和 `wrapper` ，`Help tasks` 有`buildEnvironment` 和 `components` 等等。

### 2.4.2. 排除任务

如果我们不想运行go任务，可以运行 `gradle hello -x go` 命令:
```
➜  gradle01 gradle hello -x go

> Configure project : 
go for it

> Task :hello 
任务分组: build
任务描述: hello world


BUILD SUCCESSFUL in 0s
1 actionable task: 1 executed
```

获取任务帮助信息

通过运行 `gradle -q help --task hello` 命令来显示 `hello` 任务的帮助信息。
```
➜  gradle01 gradle -q help --task hello
go for it
Detailed task information for hello

Path
     :hello

Type
     Task (org.gradle.api.Task)

Description
     hello world

Group
     build
```
可以看到hello任务的路径、类型、描述和分组。

### 2.4.3. 多任务调用
```
task helloworld <<{
    println "hello world!"
}

task goforit << {
    println "go for it!"
}
```
通过命令行一次执行多个任务，每个任务通常只会执行一次，无论是在命令行中指定任务还是任务依赖。上面的例子我们运行 `gradle helloWorld goForit` ，会先执行`helloWorld` 任务后执行 `goforit` 任务。
```
➜  gradle01 gradle helloworld goforit

> Configure project : 
The Task.leftShift(Closure) method has been deprecated and is scheduled to be removed in Gradle 5.0. Please use Task.doLast(Action) instead.

> Task :helloworld 
hello world!

> Task :goforit 
go for it!


BUILD SUCCESSFUL in 0s
2 actionable tasks: 2 executed
```
### 2.4.4. 任务名称缩写

可以对使用驼峰命名的任务进行缩写，对于名称特别长的任务这个特性非常有用，比如[2.4.3. 多任务调用](#243-多任务调用)中的例子只需要执行`gradle hW gF` 就可以了，不过需要注意一点，那就是任务名称的缩写必须是唯一的，如果[2.4.3. 多任务调用](#243-多任务调用)中第二个任务的名称为`helloWangshu`，那么就会报错。
```
➜  gradle01 gradle hW gF

> Configure project : 
The Task.leftShift(Closure) method has been deprecated and is scheduled to be removed in Gradle 5.0. Please use Task.doLast(Action) instead.

> Task :helloworld 
hello world!

> Task :goforit 
go for it!


BUILD SUCCESSFUL in 0s
2 actionable tasks: 2 executed
```

# 3. Groovy 快速入门指南

## Groovy 概述

`Groovy` 是 `Apache` 旗下的一种基于 `JVM` 的面向对象编程语言，既可以用于面向对象编程，也可以用作纯粹的脚本语言。在语言的设计上它吸纳了`Python` 、`Ruby` 和 `Smalltalk` 语言的优秀特性，比如动态类型转换、闭包和元编程支持。
`Groovy` 与 `Java` 可以很好的互相调用并结合编程 ，比如在写 `Groovy` 的时候忘记了语法可以直接按 `Java` 的语法继续写，也可以在 `Java`  中调用 `Groovy`  脚本。比起`Java`，`Groovy` 语法更加的灵活和简洁，可以用更少的代码来实现 `Java` 实现的同样功能。

## Groovy编写和调试

`Groovy` 的代码可以在 `Android Studio` 和 `IntelliJ IDEA` 等 `IDE` 中进行编写和调试，缺点是需要配置环境，这里推荐在文本中编写代码并结合命令行进行调试（文本推荐使用`VSCode`）。关于命令行请查看[2. 入门前奏](#2-入门前奏)。

具体的操作步骤就是：在一个目录中新建 `build.gradle` 文件，在 `build.gradle` 中新建一个 `task` ，在 `task` 中编写 `Groovy` 代码，用命令行进入这个 `build.gradle` 文件所在的目录，运行 `gradle task` 名称 等命令行对代码进行调试，本文中的例子都是这样编写和调试的。

## 变量

`Groovy` 中用 `def` 关键字来定义变量，可以不指定变量的类型，默认访问修饰符是 `public` 。
```
def a = 1;
def int b = 1;
def c = "hello world";
```

## 方法

方法使用返回类型或 `def` 关键字定义，方法可以接收任意数量的参数，这些参数可以不申明类型，如果不提供可见性修饰符，则该方法为 `public`。

```
task method <<{
    add(1, 2)
    minus 1, 2  //语句后面的可以省略，方法的括号可以省略。
    def n = mul 1, 2
    println n
    def f = div 2.0, 3
    println f
}
//用def关键字定义方法。
def add(int a, int b){
    println a + b
}
def minus(a, b){//参数类型可以省略
    println a - b
}
//指定了方法返回类型，可以不需要def关键字来定义方法。
int mul(a, b){
    return a * b
}
//如果不使用return ，方法的返回值为最后一行代码的执行结果。
double div(a, b){//return可以省略掉
    a/b 
}
```
运行结果：
```
➜  gradle01 gradle -q method
3
-1
2
0.6666666667
```

## 类

`Groovy` 类非常类似于 `Java` 类。

