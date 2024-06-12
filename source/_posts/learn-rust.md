---
title: learn-rust
date: 2023-09-22 14:49:12
tags: rust
category: rust
---

# Rust 环境搭建

## 安装

### Windows 平台

1. 首先需要安装 [Visual C++ 生成工具](https://visualstudio.microsoft.com/zh-hans/downloads/) 。
2. 然后下载并运行 [rustup‑init.exe](https://static.rust-lang.org/rustup/dist/x86_64-pc-windows-msvc/rustup-init.exe) ，一路默认即可。

所有工具都安装到 `%USERPROFILE%\.cargo\bin` 目录， 并且您能够在这里找到 Rust 工具链，包括 rustc、cargo 及 rustup。

安装完成后，运行 `rustc --version` 检查安装是否成功

```bash
$ rustc --version
rustc 1.59.0 (9d1b2106e 2022-02-23)
```

### 编译运行 Rust 程序

创建一个 hello_world 目录，并在其中创建 `main.rs` 文件，其中的内容如下：

```rust
fn mian(){
    println!("Hello, world!");
}
```

然后使用 rustc 编译，并运行。

```bash
$ rustc main.rs
$ ./main.exe
Hello, world!
```

## 开发环境配置

Rust 开发工具可以使用 [VSCode](https://code.visualstudio.com/) 以及下列插件。

- [rust-analyzer](https://marketplace.visualstudio.com/items?itemName=rust-lang.rust-analyzer) : Rust language support for Visual Studio Code.
- [Even Better](https://marketplace.visualstudio.com/items?itemName=tamasfe.even-better-toml) : TOML，支持 .toml 文件完整特性。
- [Error Lens](https://marketplace.visualstudio.com/items?itemName=usernamehw.errorlens) : 更好的获得错误展示。
- [One Dark Pro](https://marketplace.visualstudio.com/items?itemName=zhuangtongfa.Material-theme) : 非常好看的 VSCode 主题。
- [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb) : Debugger 程序。

## cargo

cargo 是 Rust 语言的包管理工具，提供了一系列的工具，从项目的建立、构建到测试、运行直至部署，为 Rust 项目的管理提供尽可能完整的手段。

### 创建一个项目

可以使用使用 `cargo new` 创建一个项目，默认参数 `--bin` 为应用程序类型的项目，也可以使用 `--lib` 创建一个依赖库项目。

创建一个应用程序项目。

```bash
cargo new hello          # 创建一个应用程序项目。
```

创建一个依赖库项目。

```bash
cargo new hellolib --lib    # 创建一个依赖库项目。
```

### 运行项目

1. cargo run

进入源码的根目录，直接运行 `cargo run` 即可自动编译运行。

2. 手动编译运行

进入源码的根目录，首先运行 `cargo build` 编译项目，然后运行编译好的应用。

以上默认编译为 debug 版，可以使用 `--release` 命令编译 release 版。

### cargo check

`cargo check` 是我们在代码开发过程中最常用的命令，它的作用很简单：快速的检查一下代码能否编译通过。因此该命令速度会非常快，能节省大量的编译时间。

### Cargo.toml 和 Cargo.lock

- `Cargo.toml` 是 cargo 特有的项目数据描述文件。它存储了项目的所有元配置信息，如果 Rust 开发者希望 Rust 项目能够按照期望的方式进行构建、测试和运行，那么，必须按照合理的方式构建 `Cargo.toml`。

- `Cargo.lock` 文件是 cargo 工具根据同一项目的 toml 文件生成的项目依赖详细清单，因此我们一般不用修改它，只需要对着 `Cargo.toml` 文件撸就行了。

#### Cargo.toml 文件

1. package 配置段落

package 中记录了项目的描述信息，典型的如下：

```toml
[package]
name = "hello"
version = "0.1.0"
edition = "2021"
```

name 字段定义了项目名称，version 字段定义当前版本，新项目默认是 0.1.0，edition 字段定义了我们使用的 Rust 大版本。

2. dependencies 项目依赖

使用 cargo 工具的最大优势就在于，能够对该项目的各种依赖项进行方便、统一和灵活的管理。

在 `Cargo.toml` 中，主要通过各种依赖段落来描述该项目的各种依赖项：

- 基于 Rust 官方仓库 `crates.io`，通过版本说明来描述
- 基于项目源代码的 git 仓库地址，通过 URL 来描述
- 基于本地项目的绝对路径或者相对路径，通过类 Unix 模式的路径来描述

这三种形式具体写法如下：

```toml
[dependencies]
rand = "0.3"
hammer = { version = "0.5.0"}
color = { git = "https://github.com/bjz/color-rs" }
geometry = { path = "crates/geometry" }
```

### 标准的 Package 目录结构

一个典型的 Package 目录结构如下：

```bash
.
├── Cargo.lock
├── Cargo.toml
├── src/
│   ├── lib.rs
│   ├── main.rs
│   └── bin/
│       ├── named-executable.rs
│       ├── another-executable.rs
│       └── multi-file-executable/
│           ├── main.rs
│           └── some_module.rs
├── benches/
│   ├── large-input.rs
│   └── multi-file-bench/
│       ├── main.rs
│       └── bench_module.rs
├── examples/
│   ├── simple.rs
│   └── multi-file-example/
│       ├── main.rs
│       └── ex_module.rs
└── tests/
    ├── some-integration-tests.rs
    └── multi-file-test/
        ├── main.rs
        └── test_module.rs

```

这也是 Cargo 推荐的目录结构，解释如下：

- `Cargo.toml` 和 `Cargo.lock` 保存在 package 根目录下
- 源代码放在 src 目录下
- 默认的 lib 包根是 `src/lib.rs`
- 默认的二进制包根是 `src/main.rs`
- 其它二进制包根放在 `src/bin/` 目录下
- 基准测试 benchmark 放在 benches 目录下
- 示例代码放在 examples 目录下
- 集成测试代码放在 tests 目录下

# Rust 基础入门

## 变量绑定与解构

### 变量绑定

在 Rust 中，我们这样写： `let a = "hello world"` ，同时给这个过程起了另一个名字：变量绑定。

### 变量可变性

Rust 的变量在默认情况下是不可变的。可以通过 mut 关键字让变量变为可变的。

### 使用下划线开头忽略未使用的变量

创建一个未使用的变量，Rust 通常会给你一个警告。如果希望 Rust 不要警告未使用的变量，为此可以用下划线作为变量名的开头。

在函数头添加 `#[warn(unused_variables)]` 也可以忽略未使用的变量。

### 变量解构

`let` 表达式不仅仅用于变量的绑定，还能进行复杂变量的解构：从一个相对复杂的变量中，匹配出该变量的一部分内容。

```rust
let (a, b) = (true, false);
```

### 解构式赋值

在 Rust 1.59 版本后，我们可以在赋值语句的左式中使用元组、切片和结构体模式了。

```rust
// 解构式赋值
let (a, b, c, d, e);
// 对之前绑定的变量进行再赋值。
(a, b) = (1, 2);

[c, .., d, _] = [1, 2, 3, 4, 5];

Struct{e, ..} = Struct{e:5};

println!("a = {}, b = {}, c = {}, d = {}, e = {}.", a, b, c, d, e);
```

### 变量和常量之间的差异

- 常量不允许使用 mut。常量不仅仅默认不可变，而且自始至终不可变，因为常量在编译完成后，已经确定它的值。
- 常量使用 const 关键字而不是 let 关键字来声明，并且值的类型必须标注。
- 在声明它的作用域之中，常量在整个程序生命周期中都有效。

> Rust 常量的命名约定是全部字母都使用大写，并使用下划线分隔单词，另外对数字字面量可插入下划线以提高可读性。

### 变量遮蔽

Rust 允许声明相同的变量名，在后面声明的变量会遮蔽掉前面声明的。

> 常量是无法声明相同的名称。

### 示例

```rust
struct Struct {
    e: i32
}


#[allow(unused_variables)]
fn main() {
    // 通过 mut 关键字让变量变为可变的
    let mut x = 1;
    println!("x = {}.", x);
    x = 2;
    println!("x = {}.", x);


    // 用下划线作为变量名的开头告诉 Rust 不要警告未使用的变量
    let _x = 1;
    let y = 0; // #[allow(unused_variables)]
    println!("Hello, world!");

    // 变量解构
    // 解构元组
    let (a, b) = (true, false);
    println!("a = {:?}, b = {:?}", a, b);

    // 解构式赋值
    let (a, b, c, d, e);
    // 对之前绑定的变量进行再赋值。
    (a, b) = (1, 2);
    // .. 表示忽略 0 或多个，_ 表示忽略一个
    [c, .., d, _] = [1, 2, 3, 4, 5];

    Struct{e, ..} = Struct{e:5};

    println!("a = {}, b = {}, c = {}, d = {}, e = {}.", a, b, c, d, e);

    // 定义常量，常量无法重复声明
    const ONE:u32 = 1;
    println!("The value of ONE is: {}", ONE);

    // 变量遮蔽
    let spaces = "  ";
    let spaces = spaces.len();
    {
        // 在内部作用域内，隐藏变量只在作用域内生效。
        let spaces = 10;
        println!("spaces = {}.", spaces);
    }
    println!("spaces = {}.", spaces);
}
```

## 基本类型

Rust 每个值都有其确切的数据类型，总的来说可以分为两类：基本类型和复合类型。

基本类型意味着它们往往是一个最小化原子类型，无法解构为其它类型，由以下组成：

- 数值类型: 有符号整数 (i8, i16, i32, i64, isize)、 无符号整数 (u8, u16, u32, u64, usize) 、浮点数 (f32, f64)、以及有理数、复数
- 字符串：字符串字面量和字符串切片 `&str`
- 布尔类型： true 和 false
- 字符类型: 表示单个 Unicode 字符，存储为 4 个字节
- 单元类型: 即 () ，其唯一的值也是 ()。

### 类型推导与标注

Rust 编译器可以根据变量的值和上下文中的使用方式来自动推导出变量的类型，但是在某些情况下，它无法推导出变量类型，需要手动去给予一个类型标注。

### 数值类型

#### 整数类型

整数是没有小数部分的数字。下表显示了 Rust 中的内置的整数类型：
| 长度 | 有符号类型 | 无符号类型 |
| ---------- | ---------- | ---------- |
| 8 位 | i8 | u8 |
| 16 位 | i16 | u16 |
| 32 位 | i32 | u32 |
| 64 位 | i64 | u64 |
| 128-位 | i128 | u128 |
| 视架构而定 | isize | usize |

整形字面量可以用下表的形式书写：
| 数字字面量 | 示例 |
| ---------------- | ----------- |
| 十进制 | 98_222 |
| 十六进制 | 0xff |
| 八进制 | 0o77 |
| 二进制 | 0b1111_0000 |
| 字节 (仅限于 u8) | b'A' |

显式处理可能的溢出，可以使用标准库针对原始数字类型提供的这些方法：

- 使用 `wrapping_*` 方法在所有模式下都按照补码循环溢出规则处理，例如 wrapping_add
- 如果使用 `checked_*` 方法时发生溢出，则返回 None 值
- 使用 `overflowing_*` 方法返回该值和一个指示是否存在溢出的布尔值
- 使用 `saturating_*` 方法使值达到最小值或最大值

### 浮点类型

浮点类型数字 是带有小数点的数字，在 Rust 中浮点类型数字也有两种基本类型： f32 和 f64，分别为 32 位和 64 位大小。

使用浮点数需要遵守以下准则：

- 避免在浮点数上测试相等性
- 当结果在数学上可能存在未定义时，需要格外的小心

Rust 的浮点数类型使用 NaN (not a number)来处理数学上未定义的结果。

所有跟 NaN 交互的操作，都会返回一个 NaN，而且 NaN 不能用来比较。

### 数字运算

Rust 支持所有数字类型的基本数学运算：加法、减法、乘法、除法和取模运算。

### 位运算

Rust 的运算基本上和其他语言一样
| 运算符 | 说明 |
| ------- | -------------------------------------- |
| & 位与 | 相同位置均为 1 时则为 1，否则为 0 |
| \| 位或 | 相同位置只要有 1 时则为 1，否则为 0 |
| ^ 异或 | 相同位置不相同则为 1，相同则为 0 |
| ! 位非 | 把位中的 0 和 1 相互取反，即 0 置为 1，1 置为 0 |
| << 左移 | 所有位向左移动指定位数，右位补零 |
| >> 右移 | 所有位向右移动指定位数，左位补零 |

### 序列

Rust 提供了一个非常简洁的方式，用来生成连续的数值，例如 1..5，生成从 1 到 4 的连续数字，不包含 5 ；1..=5，生成从 1 到 5 的连续数字，包含 5 。

序列只允许用于数字或字符类型，原因是：它们可以连续，同时编译器在编译期可以检查该序列是否为空，字符和数字值是 Rust 中仅有的可以用于判断是否为空的类型。

### 有理数和复数

Rust 的标准库并未包含有理数和复数，需要引入第三方库 [num](https://crates.io/crates/num).

引入 num 库的方式：

1. 在 `Cargo.toml` 中的 `[dependencies]` 下添加一行 `num = "0.4.0"` 。
2. 将 `src/main.rs` 文件中使用 `use num::complex::Complex;` 引入对应的库。

### 示例

```rust
fn main() {
    // 给 guess 变量一个显式的类型标注
    let guess:i32 = "42".parse().expect("not a number!");
    println!("guess = {}.", guess);
    let guess = "42".parse::<i32>().expect("not a number!");
    println!("guess = {}.", guess);
}
```

## 所有权和借用

## 复合类型

## 流程控制

## 模式匹配

## 方法 Method

## 泛型和特征

## 集合类型

## 类型转换

## 返回值和错误处理

## 包和模块

- 包（Packages）： Cargo 的一个功能，它允许你构建、测试和分享 crate。
- Crates ：一个模块的树形结构，它形成了库或二进制项目。
- 模块（Modules）和 use： 允许你控制作用域和路径的私有性。
- 路径（path）：一个命名例如结构体、函数或模块等项的方式。

crate 是一个二进制项或者库。包（package） 是提供一系列功能的一个或者多个 crate。一个包会包含有一个` Cargo.toml` 文件，阐述如何去构建这些 crate。

cargo new 命令创建包。

`src/main.rs` 就是一个与包同名的二进制 crate 的 crate 根。如果包目录中包含 `src/lib.rs`，则包带有与其同名的库 crate，且 src/lib.rs 是 crate 根。

模块 让我们可以将一个 crate 中的代码进行分组，以提高可读性与重用性。

cargo new --lib 创建模块。

路径有两种形式：

- 绝对路径（absolute path）从 crate 根开始，以 crate 名或者字面值 crate 开头。
- 相对路径（relative path）从当前模块开始，以 self、super 或当前模块的标识符开头。

## 注释和文档

## 格式化输出

# Rust 高级进阶

## 生命周期

## 函数式编程：闭包、迭代器

## 深入类型

## 智能指针

## 循环引用与自引用

## 多线程并发编程

## 全局变量

## 错误处理

## Unsafe Rust

## Macro 宏编程

# 异步编程

## async/wait 异步编程

## Tokio 使用指南

# 参考

[Rust 语言圣经](https://course.rs)

[Rust 程序设计语言](https://kaisery.github.io)
