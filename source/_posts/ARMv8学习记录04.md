---
title: ARMv8学习记录04
date: 2021-03-18 09:46:39
tags: C++逆向
category: ARMv8汇编
---

# 基本数据类型

## main 函数

一般情况下，IDA 会自动将 main 函数标记出来，但是还是有部分应用 IDA 无法自动标记，需要我们手动确认 main 函数。首先找到 start 函数，然后查找 `__libc_init` 的第 3 个参数 R2，该参数即为 main 函数的地址。

![](ARMv8学习记录04/2021-04-18-16-23-38.png)

可以通过 Android 源码进行验证，在 http://androidxref.com/ 网站上搜索 `__libc_init` 调用位置，可以发现其第三个参数为 main 函数的地址。

![](ARMv8学习记录04/2021-04-19-13-14-06.png)

## 整型

C++ 的整型分为有无符号整型：`short`、`int`、`long`、`long long` 和无符号整型 `unsigned short`、`unsigned int`、`unsigned long`、`unsigned long long`。

其中 `int` 类型与 `long` 类型都占 4 字节（ARMv8 下，long 占 8 字节），`short` 类型占 2 字节，`long long` 类型占 8 字节。

有符号整型使用最高位来表示符号，最高位为 0 则表示正数，为 1 则表示负数，有符号 `int` 取值范围为 `0x80000000 ～ 0x7FFFFFFF` 。

无符号整型的所有位都用来表示数值，无符号 `int` 取值范围为 `0x00000000 ～ 0xFFFFFFFF` 。

例子：

```c
// integer.c
# include <stdio.h>

int main(int argc, char const *argv[])
{
    unsigned short u2 = 32768;
    unsigned int u4 = 123;
    unsigned long ul4 = 456;
    unsigned long long ul8 =0x8765432198765432;

    short s2 = 32768;
    int i4 = -1;
    long l4 = 0x87654321;
    long long l8 =0x1234567812345678;
    // 防止被优化，使用 printf 输出
    printf("u2 = %hu\n", u2);
    printf("u4 = %u\n", u4);
    printf("ul4 = %lu\n", ul4);
    printf("ul8 = %llu\n", ul8);
    printf("s2 = %hd\n", s2);
    printf("i4 = %d\n", i4);
    printf("l4 = %ld\n", l4);
    printf("l8 = %lld\n", l8);

    return 0;
}
```

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2 integer.c -o integer
```

IDA 对应的反汇编内容如下。

```armasm
.text:0000145C PUSH            {R4,R10,R11,LR}
.text:00001460 ADD             R11, SP, #8
.text:00001464 LDR             R0, =(aU2Hu - 0x1474)   ; "u2 = %hu\n"
.text:00001468 MOV             R1, #32768
.text:0000146C ADD             R0, PC, R0              ; "u2 = %hu\n"
.text:00001470 BL              printf
.text:00001470
.text:00001474 LDR             R0, =(aU4U - 0x1484)    ; "u4 = %u\n"
.text:00001478 MOV             R1, #123
.text:0000147C ADD             R0, PC, R0              ; "u4 = %u\n"
.text:00001480 BL              printf
.text:00001480
.text:00001484 LDR             R0, =(aUl4Lu - 0x1494)  ; "ul4 = %lu\n"
.text:00001488 MOV             R1, #456
.text:0000148C ADD             R0, PC, R0              ; "ul4 = %lu\n"
.text:00001490 BL              printf
.text:00001490
.text:00001494 LDR             R0, =(aUl8Llu - 0x14A8) ; "ul8 = %llu\n"
.text:00001498 LDR             R4, =0x87654321
.text:0000149C LDR             R2, =0x98765432
.text:000014A0 ADD             R0, PC, R0              ; "ul8 = %llu\n"
.text:000014A4 MOV             R3, R4                  ; R3 高32位，R2 低32位作为参数传递给printf
.text:000014A8 BL              printf
.text:000014A8
.text:000014AC LDR             R0, =(aS2Hd - 0x14BC)   ; "s2 = %hd\n"
.text:000014B0 LDR             R1, =0xFFFF8000
.text:000014B4 ADD             R0, PC, R0              ; "s2 = %hd\n"
.text:000014B8 BL              printf
.text:000014B8
.text:000014BC LDR             R0, =(aI4D - 0x14CC)    ; "i4 = %d\n"
.text:000014C0 MOV             R1, #0xFFFFFFFF
.text:000014C4 ADD             R0, PC, R0              ; "i4 = %d\n"
.text:000014C8 BL              printf
.text:000014C8
.text:000014CC LDR             R0, =(aL4Ld - 0x14DC)   ; "l4 = %ld\n"
.text:000014D0 MOV             R1, R4
.text:000014D4 ADD             R0, PC, R0              ; "l4 = %ld\n"
.text:000014D8 BL              printf
.text:000014D8
.text:000014DC LDR             R0, =(aL8Lld - 0x14EC)  ; "l8 = %lld\n"
.text:000014E0 LDR             R2, =0x12345678
.text:000014E4 ADD             R0, PC, R0              ; "l8 = %lld\n"
.text:000014E8 MOV             R3, R2
.text:000014EC BL              printf
.text:000014EC
.text:000014F0 MOV             R0, #0
.text:000014F4 POP             {R4,R10,R11,LR}
.text:000014F8 BX              LR
.text:000014F8
```

分析上面汇编可以发现有符号整型和无符号整型在内存中其实无法分辨的，只有当值被使用的时候才能判断其类型。而且 ARMv7 汇编向函数传递 64 位的参数时，分别使用 R3 和 R2 寄存器存放 64 位的高 32 位和低 32 位。

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2 integer.c -o integer64
```

IDA 对应的反汇编内容如下。

```armasm
.text:000000000000169C STP             X29, X30, [SP,#-0x10+var_s0]!
.text:00000000000016A0 MOV             X29, SP
.text:00000000000016A4 ADRP            X0, #aU2Hu@PAGE         ; "u2 = %hu\n"
.text:00000000000016A8 MOV             W1, #32768
.text:00000000000016AC ADD             X0, X0, #aU2Hu@PAGEOFF  ; "u2 = %hu\n"
.text:00000000000016B0 BL              .printf
.text:00000000000016B0
.text:00000000000016B4 ADRP            X0, #aU4U@PAGE          ; "u4 = %u\n"
.text:00000000000016B8 MOV             W1, #123
.text:00000000000016BC ADD             X0, X0, #aU4U@PAGEOFF   ; "u4 = %u\n"
.text:00000000000016C0 BL              .printf
.text:00000000000016C0
.text:00000000000016C4 ADRP            X0, #aUl4Lu@PAGE        ; "ul4 = %lu\n"
.text:00000000000016C8 MOV             W1, #456
.text:00000000000016CC ADD             X0, X0, #aUl4Lu@PAGEOFF ; "ul4 = %lu\n"
.text:00000000000016D0 BL              .printf
.text:00000000000016D0
.text:00000000000016D4 MOV             X1, #0x5432
.text:00000000000016D8 ADRP            X0, #aUl8Llu@PAGE       ; "ul8 = %llu\n"
.text:00000000000016DC MOVK            X1, #0x9876,LSL#16
.text:00000000000016E0 ADD             X0, X0, #aUl8Llu@PAGEOFF ; "ul8 = %llu\n"
.text:00000000000016E4 MOVK            X1, #0x4321,LSL#32
.text:00000000000016E8 MOVK            X1, #0x8765,LSL#48
.text:00000000000016EC BL              .printf
.text:00000000000016EC
.text:00000000000016F0 ADRP            X0, #aS2Hd@PAGE         ; "s2 = %hd\n"
.text:00000000000016F4 MOV             W1, #0xFFFF8000
.text:00000000000016F8 ADD             X0, X0, #aS2Hd@PAGEOFF  ; "s2 = %hd\n"
.text:00000000000016FC BL              .printf
.text:00000000000016FC
.text:0000000000001700 ADRP            X0, #aI4D@PAGE          ; "i4 = %d\n"
.text:0000000000001704 MOV             W1, #0xFFFFFFFF
.text:0000000000001708 ADD             X0, X0, #aI4D@PAGEOFF   ; "i4 = %d\n"
.text:000000000000170C BL              .printf
.text:000000000000170C
.text:0000000000001710 ADRP            X0, #aL4Ld@PAGE         ; "l4 = %ld\n"
.text:0000000000001714 MOV             W1, #0x4321
.text:0000000000001718 ADD             X0, X0, #aL4Ld@PAGEOFF  ; "l4 = %ld\n"
.text:000000000000171C MOVK            W1, #0x8765,LSL#16
.text:0000000000001720 BL              .printf
.text:0000000000001720
.text:0000000000001724 MOV             X1, #0x5678
.text:0000000000001728 ADRP            X0, #aL8Lld@PAGE        ; "l8 = %lld\n"
.text:000000000000172C MOVK            X1, #0x1234,LSL#16
.text:0000000000001730 ADD             X0, X0, #aL8Lld@PAGEOFF ; "l8 = %lld\n"
.text:0000000000001734 MOVK            X1, #0x5678,LSL#32
.text:0000000000001738 MOVK            X1, #0x1234,LSL#48
.text:000000000000173C BL              .printf
.text:000000000000173C
.text:0000000000001740 MOV             W0, WZR
.text:0000000000001744 LDP             X29, X30, [SP+var_s0],#0x10
.text:0000000000001748 RET
```

ARMv8 汇编处理 64 位数时稍微麻烦了一点，需要使用左移操作。

总体而言整型还是比较容易识别的。

## IEEE 浮点数

通常一个浮点数由符号、尾数、基数和指数组成。尾数的位数用于确定精度；指数的位数用于确定能表示的数的范围。

单精度浮点数为 32 位，具有 24 位有效数字，双精度浮点数位 64 位，具有 53 位有效数字。其记录格式如下：

![](ARMv8学习记录04/2021-03-18-19-48-36.png)

单精度：31 位为符号位，22~30 位为指数位，0~22 位为尾数位。其中指数位为指数值加上 127，尾数位为小数点后对应的二进制数。

双精度：63 位为符号位，51~62 位为指数位，0~51 位为尾数位。其中指数位为指数值加上 1023，尾数位为小鼠点后对应的二进制数。

十进制浮点数转换二进制数：

- 整数部分采用除 2 取余法。
- 小数部分采用乘 2 取整法。

由此可知，对于十进制浮点数，小数部分最后一位是 5 时，才可以用浮点数精确表示。

**例 1： 7.625 浮点数二进制数表示为：**

整数部分转换结果：111

小数部分转换结果：

```
0.625 * 2 = 1.25，	整数位为 1 ==> 0.1
0.25 * 2 = 0.5，	整数位为 0 ==> 0.10
0.5 * 2 = 1，		整数位为 1 ==> 0.101
```

即 `7.625 = 1*2^2 + 1*2^1 + 1*2^0 + 1*2^-1 + 0*2^-2 + 1*2^-3` 。
最终结果为 `111.101 = 1.11101 * 10^2`，对应的单精度浮点格式为

```
0   10000001   11101000000000000000000        ==> 0x40F400
--- ------------ -------------------------------
正数  指数为 127+ 2        尾数（小数部分）
```

对应的双精度浮点格式为

```
0   10000000001    1110100000000000000000000000000000000000000000000000  ==> 0x401E800000000000
--- -------------  ---------------------------------------------------
正数  指数为 1023+2      尾数（小数部分）
```

**例 2： 0.625 的二进制数为 `0.101= 1.01*10^-1` ；对应的单精度浮点格式为**

```
0         01111110         01000000000000000000000    ==> 0x 3F20 0000
----- ----------------    -------------------------------
正数  指数为 127+ (-1)        尾数（小数部分）
```

**例 3：1.3 将无法用二进制表示一个精确值，其小数位 23 位的结果为： `1.01001100110011001100110` 对应的单精度浮点格式为**

```
1   01111111   01001100110011001100110     ==> 0xBFA6 6666
--- ------------ -------------------------------
正数  指数为 127+ 0        尾数（小数部分）
```

例子。

```c
// float.c
# include <stdio.h>

int main(int argc, char const *argv[])
{
    float f1 = 7.625f, f2 = 1.3f, f3 = -7.625f;
    double d1 = 7.625, d2 = 1.3, d3 = -7.625;
    printf("f1 = %f\n", f1);
    printf("f2 = %f\n", f2);
    printf("f3 = %f\n", f3);
    printf("d1 = %lf\n", d1);
    printf("d2 = %lf\n", d2);
    printf("d3 = %lf\n", d3);

    return 0;
}
```

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2 float.c -o float
```

IDA 对应的反汇编代码。

```armasm
.text:00001444 PUSH            {R4-R6,R10,R11,LR}
.text:00001448 ADD             R11, SP, #0x10
.text:0000144C LDR             R0, =(aF1F - 0x1468)    ; "f1 = %f\n"
.text:00001450 MOV             R4, #0x1E8000
.text:00001454 MOV             R2, #0
.text:00001458 ORR             R4, R4, #0x40000000
.text:0000145C MOV             R3, R4                  ; 直接将float转成double类型了，使用R3:R2寄存器分别存放高32位和低32位 401E8000 00000000
.text:00001460 ADD             R0, PC, R0              ; "f1 = %f\n"
.text:00001464 BL              printf
.text:00001464
.text:00001468 LDR             R0, =(aF2F - 0x147C)    ; "f2 = %f\n"
.text:0000146C LDR             R5, =0x3FF4CCCC
.text:00001470 MOV             R2, #0xC0000000
.text:00001474 ADD             R0, PC, R0              ; "f2 = %f\n"
.text:00001478 MOV             R3, R5                  ; R3:R2 = 3FF4CCCCC0000000
.text:0000147C BL              printf
.text:0000147C
.text:00001480 LDR             R0, =(aF3F - 0x149C)    ; "f3 = %f\n"
.text:00001484 MOV             R6, #0x1E8000
.text:00001488 MOV             R2, #0
.text:0000148C ORR             R6, R6, #0xC0000000
.text:00001490 MOV             R3, R6                  ; R3:R2 = C01E8000 00000000
.text:00001494 ADD             R0, PC, R0              ; "f3 = %f\n"
.text:00001498 BL              printf
.text:00001498
.text:0000149C LDR             R0, =(aD1Lf - 0x14B0)   ; "d1 = %lf\n"
.text:000014A0 MOV             R2, #0
.text:000014A4 MOV             R3, R4
.text:000014A8 ADD             R0, PC, R0              ; "d1 = %lf\n"
.text:000014AC BL              printf
.text:000014AC
.text:000014B0 LDR             R0, =(aD2Lf - 0x14C4)   ; "d2 = %lf\n"
.text:000014B4 LDR             R2, =0xCCCCCCCD
.text:000014B8 MOV             R3, R5                  ; R3:R2 = 3FF4CCCCCCCCCCCD
.text:000014BC ADD             R0, PC, R0              ; "d2 = %lf\n"
.text:000014C0 BL              printf
.text:000014C0
.text:000014C4 LDR             R0, =(aD3Lf - 0x14D8)   ; "d3 = %lf\n"
.text:000014C8 MOV             R2, #0
.text:000014CC MOV             R3, R6
.text:000014D0 ADD             R0, PC, R0              ; "d3 = %lf\n"
.text:000014D4 BL              printf
.text:000014D4
.text:000014D8 MOV             R0, #0
.text:000014DC POP             {R4-R6,R10,R11,LR}
.text:000014E0 BX              LR
```

可以发现 float 类型全部被自动转换成了 double 类型了，而且可以发现 f2 和 d2 都是的值也不相同，这是由于浮点数的精度问题导致的。

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2 float.c -o float64
```

IDA 对应的反汇编代码。

```armasm
.text:000000000000169C STP             D9, D8, [SP,#-0x10+var_10]!
.text:00000000000016A0 STP             X29, X30, [SP,#0x10+var_s0]
.text:00000000000016A4 ADD             X29, SP, #0x10
.text:00000000000016A8 MOV             X8, #0x800000000000
.text:00000000000016AC ADRP            X0, #aF1F@PAGE          ; "f1 = %f\n"
.text:00000000000016B0 MOVK            X8, #0x401E,LSL#48      ; R8 = 401E800000000000
.text:00000000000016B4 ADD             X0, X0, #aF1F@PAGEOFF   ; "f1 = %f\n"
.text:00000000000016B8 FMOV            D8, X8
.text:00000000000016BC FMOV            D0, D8
.text:00000000000016C0 BL              .printf
.text:00000000000016C0
.text:00000000000016C4 ADRP            X8, #qword_508@PAGE     ; qword_508 DCQ 0x3FF4CCCCC0000000
.text:00000000000016C8 ADRL            X0, aF2F                ; "f2 = %f\n"
.text:00000000000016D0 LDR             D0, [X8,#qword_508@PAGEOFF] ; D0 = 0x3FF4CCCCC0000000
.text:00000000000016D4 BL              .printf
.text:00000000000016D4
.text:00000000000016D8 MOV             X8, #0x800000000000
.text:00000000000016DC ADRP            X0, #aF3F@PAGE          ; "f3 = %f\n"
.text:00000000000016E0 MOVK            X8, #0xC01E,LSL#48      ; X8 = C01E800000000000
.text:00000000000016E4 ADD             X0, X0, #aF3F@PAGEOFF   ; "f3 = %f\n"
.text:00000000000016E8 FMOV            D9, X8
.text:00000000000016EC FMOV            D0, D9
.text:00000000000016F0 BL              .printf
.text:00000000000016F0
.text:00000000000016F4 ADRP            X0, #aD1Lf@PAGE         ; "d1 = %lf\n"
.text:00000000000016F8 FMOV            D0, D8
.text:00000000000016FC ADD             X0, X0, #aD1Lf@PAGEOFF  ; "d1 = %lf\n"
.text:0000000000001700 BL              .printf
.text:0000000000001700
.text:0000000000001704 ADRP            X8, #qword_510@PAGE     ; qword_510 DCQ 0x3FF4CCCCCCCCCCCD
.text:0000000000001708 ADRL            X0, aD2Lf               ; "d2 = %lf\n"
.text:0000000000001710 LDR             D0, [X8,#qword_510@PAGEOFF] ; D0 = 0x3FF4CCCCCCCCCCCD
.text:0000000000001714 BL              .printf
.text:0000000000001714
.text:0000000000001718 ADRP            X0, #aD3Lf@PAGE         ; "d3 = %lf\n"
.text:000000000000171C FMOV            D0, D9
.text:0000000000001720 ADD             X0, X0, #aD3Lf@PAGEOFF  ; "d3 = %lf\n"
.text:0000000000001724 BL              .printf
.text:0000000000001724
.text:0000000000001728 LDP             X29, X30, [SP,#0x10+var_s0]
.text:000000000000172C MOV             W0, WZR
.text:0000000000001730 LDP             D9, D8, [SP+0x10+var_10],#0x20
.text:0000000000001734 RET
```

可以发现 ARMv8 中浮点数和 ARMv7 中浮点数的存储方式时类似的。

## 字符和字符串

在讲解字符串之前，必须先了解一下字符集和字符编码。

- 字符集（Charset）：是一个系统支持的所有抽象字符的集合。字符是各种文字和符号的总称，包括各国家文字、标点符号、图形符号、数字等。
- 字符编码（Character Encoding）：是一套法则，使用该法则能够对自然语言的字符的一个集合（如字母表或音节表），与其他东西的一个集合（如号码或电脉冲）进行配对。

常见字符集名称：ASCII 字符集、GB2312 字符集、BIG5 字符集、GB18030 字符集、Unicode 字符集等。计算机要准确的处理各种字符集文字，需要进行字符编码，以便计算机能够识别和存储各种文字。

在 C++中，主要使用两种字符集：ASCII 和 Unicode。对应的编码为：ASCII 编码和 UTF-32、UTF-16、UTF-8。

字符串是由多个字符按照一定排列顺序组成的，在 C++中，以'\0'作为字符串结束标记。

例子，下面代码保存为 UT8 编码：

```c
// string.c
# include <stdio.h>

int main(int argc, char const *argv[])
{
    char a = 'a';
    printf("a = %c\n", a);
    char str[] = "hello";
    char utf8[] = "你好";
    printf("str = %s\n", str);
    printf("utf8 = %s\n", utf8);
    return 0;
}
```

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2 string.c -o string
```

IDA 对应的反汇编代码。

```armasm
.text:00001434 PUSH            {R11,LR}
.text:00001438 MOV             R11, SP
.text:0000143C SUB             SP, SP, #0x10
.text:00001440 LDR             R0, =(aAC - 0x1450)     ; "a = %c\n"
.text:00001444 MOV             R1, #0x61 ; 'a'         ; 字符a对应的ASCII码为0x61
.text:00001448 ADD             R0, PC, R0              ; "a = %c\n"
.text:0000144C BL              printf
.text:0000144C
.text:00001450 MOV             R0, #0x6F ; 'o'
.text:00001454 ADD             R1, SP, #0x10+var_8
.text:00001458 STRH            R0, [SP,#0x10+var_4]    ; var_4 = 0x0000006F
.text:0000145C LDR             R0, =0x6C6C6568
.text:00001460 STR             R0, [SP,#0x10+var_8]    ; var_8 = 0x6C6C6568
.text:00001464 MOV             R0, #0
.text:00001468 STRB            R0, [SP,#0x10+var_A]    ; var_A = 0x00000000
.text:0000146C MOV             R0, #0xBDA5
.text:00001474 STRH            R0, [SP,#0x10+var_C]    ; var_C = 0x0000BDA5
.text:00001478 LDR             R0, =0xE5A0BDE4
.text:0000147C STR             R0, [SP,#0x10+var_10]   ; var_10 = 0xE5A0BDE4
.text:00001480 LDR             R0, =(aStrS - 0x148C)   ; "str = %s\n"
.text:00001484 ADD             R0, PC, R0              ; "str = %s\n"
.text:00001488 BL              printf
.text:00001488
.text:0000148C LDR             R0, =(aUtf8S - 0x149C)  ; "utf8 = %s\n"
.text:00001490 MOV             R1, SP
.text:00001494 ADD             R0, PC, R0              ; "utf8 = %s\n"
.text:00001498 BL              printf
.text:00001498
.text:0000149C MOV             R0, #0
.text:000014A0 MOV             SP, R11
.text:000014A4 POP             {R11,LR}
.text:000014A8 BX              LR
```

字符 a 对应的 ASCII 码为 0x61；字符串 hello 对应着 var_8:var_4 的内存，按照大端模式其值为 x068 0x65 0x6C 0x 6C 0x6F 0x00 ，正好对应 hello 的 ASCII 码；utf8 编码的"你好" 对应着 var_10: var_C 的内存，按照大端模式其值为 0xE4 0xBD 0xA0 0xE5 0xA5 0xBD 0x00 0x00 。可以到网站 https://onlineutf8tools.com/convert-utf8-to-hexadecimal 上进行验证。

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2 string.c -o string64
```

IDA 对应的反汇编代码。

```armasm
.text:0000000000001674 SUB             SP, SP, #0x20
.text:0000000000001678 STP             X29, X30, [SP,#0x10+var_s0]
.text:000000000000167C ADD             X29, SP, #0x10
.text:0000000000001680 ADRP            X0, #aAC@PAGE           ; "a = %c\n"
.text:0000000000001684 MOV             W1, #0x61 ; 'a'
.text:0000000000001688 ADD             X0, X0, #aAC@PAGEOFF    ; "a = %c\n"
.text:000000000000168C BL              .printf
.text:000000000000168C
.text:0000000000001690 MOV             W9, #0x6568
.text:0000000000001694 MOV             W10, #0xA5E5
.text:0000000000001698 MOV             W11, #0xBDE4
.text:000000000000169C ADRP            X0, #aStrS@PAGE         ; "str = %s\n"
.text:00000000000016A0 MOV             W8, #0x6F ; 'o'
.text:00000000000016A4 MOVK            W9, #0x6C6C,LSL#16
.text:00000000000016A8 MOVK            W10, #0xBD,LSL#16
.text:00000000000016AC MOVK            W11, #0xE5A0,LSL#16
.text:00000000000016B0 ADD             X0, X0, #aStrS@PAGEOFF  ; "str = %s\n"
.text:00000000000016B4 ADD             X1, SP, #0x10+var_8
.text:00000000000016B8 STRH            W8, [SP,#0x10+var_4]
.text:00000000000016BC STR             W9, [SP,#0x10+var_8]
.text:00000000000016C0 STUR            W10, [SP,#0x10+var_D]
.text:00000000000016C4 STR             W11, [SP]
.text:00000000000016C8 BL              .printf
.text:00000000000016C8
.text:00000000000016CC ADRP            X0, #aUtf8S@PAGE        ; "utf8 = %s\n"
.text:00000000000016D0 MOV             X1, SP
.text:00000000000016D4 ADD             X0, X0, #aUtf8S@PAGEOFF ; "utf8 = %s\n"
.text:00000000000016D8 BL              .printf
.text:00000000000016D8
.text:00000000000016DC LDP             X29, X30, [SP,#0x10+var_s0]
.text:00000000000016E0 MOV             W0, WZR
.text:00000000000016E4 ADD             SP, SP, #0x20 ; ' '
.text:00000000000016E8 RET
```

ARMv8 对应的结果和 ARMv7 类似。

## 指针和引用

### 指针
指针本身是一种数据类型，用于保存各种数据类型在内存中的地址。指针变量同样可以取出地址，所以会出现多级指针。

例子。

```c
// point.c
#include <stdio.h>
int main(int argc, char const *argv[])
{
    char ary [] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
    int *p1 = (int*)ary;
    char *p2 = (char*)ary;
    short *p3 = (short*)ary;
    long *p4 = (long*)ary;
    long long *p5 = (long long*)ary;
    printf("*p1 = 0x%x\n", *p1);
    printf("*p2 = 0x%x\n", *p2);
    printf("*p3 = 0x%x\n", *p3);
    printf("*p4 = 0x%lx\n", *p4);
    printf("*p5 = 0x%llx\n", *p5);
    p1 += 1;
    p2 += 1;
    p3 += 1;
    p4 += 1;
    p5 += 1;
    printf("ary = %p\n", ary);
    printf("&ary = %p\n", &ary);
    printf("p1 = %p\n", p1);
    printf("p2 = %p\n", p2);
    printf("p3 = %p\n", p3);
    printf("p4 = %p\n", p4);
    printf("p5 = %p\n", p5);

    return 0;
}
```

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2 point.c -o point
```

IDA 对应的反汇编代码。

```armasm
.text:0000148C PUSH            {R4,R5,R11,LR}
.text:00001490 ADD             R11, SP, #8
.text:00001494 SUB             SP, SP, #8
.text:00001498 LDR             R5, =0x67452301
.text:0000149C LDR             R4, =0xEFCDAB89
.text:000014A0 STR             R4, [SP,#4]
.text:000014A4 STR             R5, [SP]
.text:000014A8 MOV             R1, R5
.text:000014AC LDR             R0, =(aP10xX - 0x14B8)  ; "*p1 = 0x%x\n"
.text:000014B0 ADD             R0, PC, R0              ; "*p1 = 0x%x\n"
.text:000014B4 BL              printf
.text:000014B4
.text:000014B8 LDR             R0, =(aP20xX - 0x14C8)  ; "*p2 = 0x%x\n"
.text:000014BC MOV             R1, #1
.text:000014C0 ADD             R0, PC, R0              ; "*p2 = 0x%x\n"
.text:000014C4 BL              printf
.text:000014C4
.text:000014C8 LDR             R0, =(aP30xX - 0x14DC)  ; "*p3 = 0x%x\n"
.text:000014CC MOV             R1, #0x2301
.text:000014D4 ADD             R0, PC, R0              ; "*p3 = 0x%x\n"
.text:000014D8 BL              printf
.text:000014D8
.text:000014DC LDR             R0, =(aP40xLx - 0x14EC) ; "*p4 = 0x%lx\n"
.text:000014E0 MOV             R1, R5
.text:000014E4 ADD             R0, PC, R0              ; "*p4 = 0x%lx\n"
.text:000014E8 BL              printf
.text:000014E8
.text:000014EC LDR             R0, =(aP50xLlx - 0x1500) ; "*p5 = 0x%llx\n"
.text:000014F0 MOV             R2, R5
.text:000014F4 MOV             R3, R4
.text:000014F8 ADD             R0, PC, R0              ; "*p5 = 0x%llx\n"
.text:000014FC BL              printf
.text:000014FC
.text:00001500 LDR             R0, =(aAryP - 0x1514)   ; "ary = %p\n"
.text:00001504 MOV             R4, SP
.text:00001508 MOV             R1, R4
.text:0000150C ADD             R0, PC, R0              ; "ary = %p\n"
.text:00001510 BL              printf
.text:00001510
.text:00001514 LDR             R0, =(aAryP_0 - 0x1524) ; "&ary = %p\n"
.text:00001518 MOV             R1, R4
.text:0000151C ADD             R0, PC, R0              ; "&ary = %p\n"
.text:00001520 BL              printf
.text:00001520
.text:00001524 LDR             R0, =(aP1P - 0x1538)    ; "p1 = %p\n"
.text:00001528 ORR             R5, R4, #4
.text:0000152C MOV             R1, R5
.text:00001530 ADD             R0, PC, R0              ; "p1 = %p\n"
.text:00001534 BL              printf
.text:00001534
.text:00001538 LDR             R0, =(aP2P - 0x1548)    ; "p2 = %p\n"
.text:0000153C ORR             R1, R4, #1
.text:00001540 ADD             R0, PC, R0              ; "p2 = %p\n"
.text:00001544 BL              printf
.text:00001544
.text:00001548 LDR             R0, =(aP3P - 0x1558)    ; "p3 = %p\n"
.text:0000154C ORR             R1, R4, #2
.text:00001550 ADD             R0, PC, R0              ; "p3 = %p\n"
.text:00001554 BL              printf
.text:00001554
.text:00001558 LDR             R0, =(aP4P - 0x1568)    ; "p4 = %p\n"
.text:0000155C MOV             R1, R5
.text:00001560 ADD             R0, PC, R0              ; "p4 = %p\n"
.text:00001564 BL              printf
.text:00001564
.text:00001568 LDR             R0, =(aP5P - 0x1578)    ; "p5 = %p\n"
.text:0000156C ADD             R1, R4, #8
.text:00001570 ADD             R0, PC, R0              ; "p5 = %p\n"
.text:00001574 BL              printf
.text:00001574
.text:00001578 MOV             R0, #0
.text:0000157C SUB             SP, R11, #8
.text:00001580 POP             {R4,R5,R11,LR}
.text:00001584 BX              LR
```

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2 point.c -o point64
```

IDA 对应的反汇编代码。

```armasm
.text:00000000000016D4 SUB             SP, SP, #0x40
.text:00000000000016D8 STP             X29, X30, [SP,#0x10]
.text:00000000000016DC ADD             X29, SP, #0x10
.text:00000000000016E0 STP             X22, X21, [SP,#0x20]
.text:00000000000016E4 STP             X20, X19, [SP,#0x30]
.text:00000000000016E8 MOV             X8, #0x2301
.text:00000000000016EC ADRP            X0, #aP10xX@PAGE        ; "*p1 = 0x%x\n"
.text:00000000000016F0 MOVK            X8, #0x6745,LSL#16
.text:00000000000016F4 MOV             W1, #0x2301
.text:00000000000016F8 MOVK            X8, #0xAB89,LSL#32
.text:00000000000016FC ADD             X0, X0, #aP10xX@PAGEOFF ; "*p1 = 0x%x\n"
.text:0000000000001700 MOVK            X8, #0xEFCD,LSL#48
.text:0000000000001704 MOVK            W1, #0x6745,LSL#16
.text:0000000000001708 STR             X8, [SP,#8]
.text:000000000000170C BL              .printf
.text:000000000000170C
.text:0000000000001710 ADRP            X0, #aP20xX@PAGE        ; "*p2 = 0x%x\n"
.text:0000000000001714 MOV             W1, #1
.text:0000000000001718 ADD             X0, X0, #aP20xX@PAGEOFF ; "*p2 = 0x%x\n"
.text:000000000000171C BL              .printf
.text:000000000000171C
.text:0000000000001720 ADRP            X0, #aP30xX@PAGE        ; "*p3 = 0x%x\n"
.text:0000000000001724 MOV             W1, #0x2301
.text:0000000000001728 ADD             X0, X0, #aP30xX@PAGEOFF ; "*p3 = 0x%x\n"
.text:000000000000172C BL              .printf
.text:000000000000172C
.text:0000000000001730 MOV             X1, #0x2301
.text:0000000000001734 ADRP            X0, #aP40xLx@PAGE       ; "*p4 = 0x%lx\n"
.text:0000000000001738 MOVK            X1, #0x6745,LSL#16
.text:000000000000173C ADD             X0, X0, #aP40xLx@PAGEOFF ; "*p4 = 0x%lx\n"
.text:0000000000001740 MOVK            X1, #0xAB89,LSL#32
.text:0000000000001744 MOVK            X1, #0xEFCD,LSL#48
.text:0000000000001748 BL              .printf
.text:0000000000001748
.text:000000000000174C MOV             X1, #0x2301
.text:0000000000001750 ADRP            X0, #aP50xLlx@PAGE      ; "*p5 = 0x%llx\n"
.text:0000000000001754 MOVK            X1, #0x6745,LSL#16
.text:0000000000001758 ADD             X0, X0, #aP50xLlx@PAGEOFF ; "*p5 = 0x%llx\n"
.text:000000000000175C MOVK            X1, #0xAB89,LSL#32
.text:0000000000001760 MOVK            X1, #0xEFCD,LSL#48
.text:0000000000001764 BL              .printf
.text:0000000000001764
.text:0000000000001768 ADRP            X0, #aAryP@PAGE         ; "ary = %p\n"
.text:000000000000176C ADD             X1, SP, #8
.text:0000000000001770 ADD             X0, X0, #aAryP@PAGEOFF  ; "ary = %p\n"
.text:0000000000001774 ADD             X8, SP, #8
.text:0000000000001778 ORR             X19, X8, #4
.text:000000000000177C ORR             X20, X8, #1
.text:0000000000001780 ORR             X21, X8, #2
.text:0000000000001784 ADD             X22, X8, #8
.text:0000000000001788 BL              .printf
.text:0000000000001788
.text:000000000000178C ADRP            X0, #aAryP_0@PAGE       ; "&ary = %p\n"
.text:0000000000001790 ADD             X1, SP, #8
.text:0000000000001794 ADD             X0, X0, #aAryP_0@PAGEOFF ; "&ary = %p\n"
.text:0000000000001798 BL              .printf
.text:0000000000001798
.text:000000000000179C ADRP            X0, #aP1P@PAGE          ; "p1 = %p\n"
.text:00000000000017A0 MOV             X1, X19
.text:00000000000017A4 ADD             X0, X0, #aP1P@PAGEOFF   ; "p1 = %p\n"
.text:00000000000017A8 BL              .printf
.text:00000000000017A8
.text:00000000000017AC ADRP            X0, #aP2P@PAGE          ; "p2 = %p\n"
.text:00000000000017B0 MOV             X1, X20
.text:00000000000017B4 ADD             X0, X0, #aP2P@PAGEOFF   ; "p2 = %p\n"
.text:00000000000017B8 BL              .printf
.text:00000000000017B8
.text:00000000000017BC ADRP            X0, #aP3P@PAGE          ; "p3 = %p\n"
.text:00000000000017C0 MOV             X1, X21
.text:00000000000017C4 ADD             X0, X0, #aP3P@PAGEOFF   ; "p3 = %p\n"
.text:00000000000017C8 BL              .printf
.text:00000000000017C8
.text:00000000000017CC ADRP            X0, #aP4P@PAGE          ; "p4 = %p\n"
.text:00000000000017D0 MOV             X1, X22
.text:00000000000017D4 ADD             X0, X0, #aP4P@PAGEOFF   ; "p4 = %p\n"
.text:00000000000017D8 BL              .printf
.text:00000000000017D8
.text:00000000000017DC ADRP            X0, #aP5P@PAGE          ; "p5 = %p\n"
.text:00000000000017E0 MOV             X1, X22
.text:00000000000017E4 ADD             X0, X0, #aP5P@PAGEOFF   ; "p5 = %p\n"
.text:00000000000017E8 BL              .printf
.text:00000000000017E8
.text:00000000000017EC LDP             X20, X19, [SP,#0x30]
.text:00000000000017F0 MOV             W0, WZR
.text:00000000000017F4 LDP             X22, X21, [SP,#0x20]
.text:00000000000017F8 LDP             X29, X30, [SP,#0x10]
.text:00000000000017FC ADD             SP, SP, #0x40 ; '@'
.text:0000000000001800 RET
```

对比 ARMv7 和 ARMv8 代码可以发现指针类型加 1 实际上等于加上类型所占的字节数。即 addr + 1 = (int)addr + sizeof(TYPE) 。

### 引用

引用在 C++中是不可以单独定义的，并且在定义时就要进行初始化。引用表示一个变量的别名，对它的任何操作本质上都是在操作它所表示的变量。

例子。

```cpp
// reference.cpp
#include <stdio.h>

void foo(int &ref){
    ref++;
    printf("ref = 0x%x\n", ref);
    printf("&ref = %p\n", &ref);
}

int main(int argc, char const *argv[])
{
    int n = 0x12345678;
    int &ref = n;
    printf("&n = %p\n", &n);
    foo(ref);
    printf("n = 0x%x\n", n);
    return 0;
}
```

为了防止 foo 函数被优化，这里去掉 `-O2` 编译选项。

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21  reference.c -o reference
```

IDA 对应的反汇编代码。

```armasm
.text:00001444 _Z3fooRi                                
.text:00001444
.text:00001444 var_4= -4
.text:00001444
.text:00001444 PUSH            {R11,LR}
.text:00001448 MOV             R11, SP
.text:0000144C SUB             SP, SP, #8
.text:00001450 STR             R0, [SP,#8+var_4]
.text:00001454 LDR             R1, [SP,#8+var_4]
.text:00001458 LDR             R0, [R1]
.text:0000145C ADD             R0, R0, #1
.text:00001460 STR             R0, [R1]
.text:00001464 LDR             R0, [SP,#8+var_4]
.text:00001468 LDR             R1, [R0]
.text:0000146C LDR             R0, =(aRef0xX - 0x1478) ; "ref = 0x%x\n"
.text:00001470 ADD             R0, PC, R0              ; "ref = 0x%x\n"
.text:00001474 BL              printf
.text:00001474
.text:00001478 LDR             R1, [SP,#8+var_4]
.text:0000147C LDR             R0, =(aRefP - 0x1488)   ; "&ref = %p\n"
.text:00001480 ADD             R0, PC, R0              ; "&ref = %p\n"
.text:00001484 BL              printf
.text:00001484
.text:00001488 MOV             SP, R11
.text:0000148C POP             {R11,LR}
.text:00001490 BX              LR

.text:0000149C main                                    
.text:0000149C
.text:0000149C var_18= -0x18
.text:0000149C var_14= -0x14
.text:0000149C var_10= -0x10
.text:0000149C var_C= -0xC
.text:0000149C var_8= -8
.text:0000149C var_4= -4
.text:0000149C
.text:0000149C PUSH            {R11,LR}
.text:000014A0 MOV             R11, SP
.text:000014A4 SUB             SP, SP, #0x18
.text:000014A8 MOV             R2, #0
.text:000014AC STR             R2, [SP,#0x18+var_18]
.text:000014B0 STR             R2, [R11,#var_4]
.text:000014B4 STR             R0, [R11,#var_8]
.text:000014B8 STR             R1, [SP,#0x18+var_C]
.text:000014BC LDR             R0, =0x12345678
.text:000014C0 STR             R0, [SP,#0x18+var_10]
.text:000014C4 ADD             R1, SP, #0x18+var_10
.text:000014C8 STR             R1, [SP,#0x18+var_14]
.text:000014CC LDR             R0, =(aNP - 0x14D8)     ; "&n = %p\n"
.text:000014D0 ADD             R0, PC, R0              ; "&n = %p\n"
.text:000014D4 BL              printf
.text:000014D4
.text:000014D8 LDR             R0, [SP,#0x18+var_14]   ; int *
.text:000014DC BL              _Z3fooRi                ; foo(int &)
.text:000014DC
.text:000014E0 LDR             R1, [SP,#0x18+var_10]
.text:000014E4 LDR             R0, =(aN0xX - 0x14F0)   ; "n = 0x%x\n"
.text:000014E8 ADD             R0, PC, R0              ; "n = 0x%x\n"
.text:000014EC BL              printf
.text:000014EC
.text:000014F0 LDR             R0, [SP,#0x18+var_18]
.text:000014F4 MOV             SP, R11
.text:000014F8 POP             {R11,LR}
.text:000014FC BX              LR
```

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21  reference.c -o reference64
```

IDA 对应的反汇编代码。
```armasm
.text:000000000000169C _Z3fooRi                                
.text:000000000000169C
.text:000000000000169C var_8= -8
.text:000000000000169C var_s0=  0
.text:000000000000169C
.text:000000000000169C SUB             SP, SP, #0x20
.text:00000000000016A0 STP             X29, X30, [SP,#0x10+var_s0]
.text:00000000000016A4 ADD             X29, SP, #0x10
.text:00000000000016A8 STR             X0, [SP,#0x10+var_8]
.text:00000000000016AC LDR             X9, [SP,#0x10+var_8]
.text:00000000000016B0 LDR             W8, [X9]
.text:00000000000016B4 ADD             W8, W8, #1
.text:00000000000016B8 STR             W8, [X9]
.text:00000000000016BC LDR             X8, [SP,#0x10+var_8]
.text:00000000000016C0 LDR             W1, [X8]
.text:00000000000016C4 ADRL            X0, aRef0xX             ; "ref = 0x%x\n"
.text:00000000000016CC BL              .printf
.text:00000000000016CC
.text:00000000000016D0 LDR             X1, [SP,#0x10+var_8]
.text:00000000000016D4 ADRL            X0, aRefP               ; "&ref = %p\n"
.text:00000000000016DC BL              .printf
.text:00000000000016DC
.text:00000000000016E0 LDP             X29, X30, [SP,#0x10+var_s0]
.text:00000000000016E4 ADD             SP, SP, #0x20 ; ' '
.text:00000000000016E8 RET


.text:00000000000016EC main                                   
.text:00000000000016EC
.text:00000000000016EC var_24= -0x24
.text:00000000000016EC var_20= -0x20
.text:00000000000016EC var_14= -0x14
.text:00000000000016EC var_10= -0x10
.text:00000000000016EC var_8= -8
.text:00000000000016EC var_4= -4
.text:00000000000016EC var_s0=  0
.text:00000000000016EC
.text:00000000000016EC SUB             SP, SP, #0x40
.text:00000000000016F0 STP             X29, X30, [SP,#0x30+var_s0]
.text:00000000000016F4 ADD             X29, SP, #0x30
.text:00000000000016F8 MOV             W8, WZR
.text:00000000000016FC STR             W8, [SP,#0x30+var_24]
.text:0000000000001700 STUR            WZR, [X29,#var_4]
.text:0000000000001704 STUR            W0, [X29,#var_8]
.text:0000000000001708 STUR            X1, [X29,#var_10]
.text:000000000000170C SUB             X1, X29, #-var_14
.text:0000000000001710 MOV             W8, #0x12345678
.text:0000000000001718 STUR            W8, [X29,#var_14]
.text:000000000000171C MOV             X8, X1
.text:0000000000001720 STR             X8, [SP,#0x30+var_20]
.text:0000000000001724 ADRL            X0, aNP                 ; "&n = %p\n"
.text:000000000000172C BL              .printf
.text:000000000000172C
.text:0000000000001730 LDR             X0, [SP,#0x30+var_20]   ; int *
.text:0000000000001734 BL              _Z3fooRi                ; foo(int &)
.text:0000000000001734
.text:0000000000001738 LDUR            W1, [X29,#var_14]
.text:000000000000173C ADRL            X0, aN0xX               ; "n = 0x%x\n"
.text:0000000000001744 BL              .printf
.text:0000000000001744
.text:0000000000001748 LDR             W0, [SP,#0x30+var_24]
.text:000000000000174C LDP             X29, X30, [SP,#0x30+var_s0]
.text:0000000000001750 ADD             SP, SP, #0x40 ; '@'
.text:0000000000001754 RET
```
可以发现引用在反汇编层面传递的指针，在反汇编下，没有引用这种数据类型。


## 常量
定义常量可以使用 `#define` 和 `const` 关键字。

例子：
```c
// const.c
#include <stdio.h>

#define SIZE 10

int main(int argc, char const *argv[])
{
    const int n = 123;
    printf("SIZE = %d\n", SIZE);
    printf("n = %d\n", n);

    return 0;
}
```
为了防止被优化，这里去掉 `-O2` 编译选项。

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21  const.c -o const
```

IDA 对应的反汇编代码。
```armasm
.text:0000141C PUSH            {R11,LR}
.text:00001420 MOV             R11, SP
.text:00001424 SUB             SP, SP, #0x18
.text:00001428 MOV             R2, #0
.text:0000142C STR             R2, [SP,#0x18+var_14]
.text:00001430 STR             R2, [R11,#var_4]
.text:00001434 STR             R0, [R11,#var_8]
.text:00001438 STR             R1, [SP,#0x18+var_C]
.text:0000143C MOV             R0, #123
.text:00001440 STR             R0, [SP,#0x18+var_18]
.text:00001444 STR             R0, [SP,#0x18+var_10]
.text:00001448 LDR             R0, =(aSizeD - 0x1454)  ; "SIZE = %d\n"
.text:0000144C ADD             R0, PC, R0              ; "SIZE = %d\n"
.text:00001450 MOV             R1, #10
.text:00001454 BL              printf
.text:00001454
.text:00001458 LDR             R1, [SP,#0x18+var_18]
.text:0000145C LDR             R0, =(aND - 0x1468)     ; "n = %d\n"
.text:00001460 ADD             R0, PC, R0              ; "n = %d\n"
.text:00001464 BL              printf
.text:00001464
.text:00001468 LDR             R0, [SP,#0x18+var_14]
.text:0000146C MOV             SP, R11
.text:00001470 POP             {R11,LR}
.text:00001474 BX              LR
```
可以明显的发现 `const` 定义的变量被存放到了内存中，而 `#define` 定义的变量直接存到寄存器中了。


上面例子的所有例子代码位于 [src](https://github.com/CKCat/CKCat.github.io/tree/main/source/_posts/ARMv8%E5%AD%A6%E4%B9%A0%E8%AE%B0%E5%BD%9504/src) 目录。

