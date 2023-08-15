---
title: ARMv8学习记录07
date: 2023-07-04 13:15:30
tags: C++逆向
category: ARMv8汇编
---

# 函数

在 windows 上，C++环境下的调用约定有 3 种：`__cdecl `、`__stdcall`、`__fastcall`。这 3 种调用约定的解释如下。

- `__cdecl`：C/C++默认的调用方式，调用方平衡栈，不定参数的函数可以使用这种方式。
- `__stdcall`：被调方平衡栈，不定参数的函数无法使用这种方式。
- `__fastcall`：寄存器方式传参，被调方平衡栈，不定参数的函数无法使用这种方式。

而在 ARM 平台上，使用的是 [ARM Procedure Call Standard](https://github.com/ARM-software/abi-aa/blob/main/aapcs32/aapcs32.rst) 或 [ARM Procedure Call standard for aarch64](https://github.com/ARM-software/abi-aa/blob/main/aapcs64/aapcs64.rst)，分别规定了 ARM 平台 32 位和 64 位的调用标准。

## ARMv7

在 ARM 平台上参数通常使用寄存器传递：

- 不大于 32 位的数据类型使用 R0-R3 这 4 个寄存器用于传递参数，当参数大于 4 个时，使用栈传递。
- 64 位的数据类型，使用 2 个连续的寄存器传递，R0 和 R1 或 R2 和 R3。
- 128 位的容器化向量使用 4 个连续的寄存器传递, R0-R3。

ARM 平台上的返回值：

- 不大于 32 位的基本数据类型和复合类型在 R0 中返回。
- 64 位的基本数据类型和容器化向量在 r0 和 r1 中返回。
- 128 位的容器化向量在 r0-r3 中返回。

### 32 位数据类型作为参数和返回值

例子：

```c
// fun1.c
#include<stdio.h>

int sum1(int a, int b, int c, int d){
    int sum = a + b + c + d;
    return sum;
}

int sum2(int a, int b, int c, int d, int e, int f){
    int sum = a + b + c + d + e + f;
    return sum;
}

float sum3(int a, char b, float c){
    float sum = a + b + c;
    return sum;
}

int main(int argc, char const *argv[])
{
    int sum = 0;
    sum = sum1(1, 2, 3, argc);
    printf("sum1 = %d\n", sum);
    sum = sum2(11, 22, 32, 44, 55, 66);
    printf("sum2 = %d\n", sum);

    float fsum = sum3(12, 'a', 3.14);
    printf("sum3 = %f\n", fsum);
    return 0;
}
```

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21  fun1.c -o fun1
```

IDA 对应的反汇编代码。

```armasm
.text:00001514 PUSH            {R11,LR}
.text:00001518 MOV             R11, SP
.text:0000151C SUB             SP, SP, #0x20
.text:00001520 MOV             R2, R0
.text:00001524 MOV             R0, #0
.text:00001528 STR             R0, [SP,#0x20+var_18]
.text:0000152C STR             R0, [R11,#var_4]
.text:00001530 STR             R2, [R11,#var_8]
.text:00001534 STR             R1, [R11,#var_C]
.text:00001538 STR             R0, [SP,#0x20+var_10]
.text:0000153C LDR             R3, [R11,#var_8]        ; 参数4 argc
.text:00001540 MOV             R0, #1                  ; 参数1
.text:00001544 MOV             R1, #2                  ; 参数2
.text:00001548 MOV             R2, #3                  ; 参数3
.text:0000154C BL              sum1
.text:0000154C
.text:00001550 STR             R0, [SP,#0x20+var_10]   ; 返回值R0
.text:00001554 LDR             R1, [SP,#0x20+var_10]
.text:00001558 LDR             R0, =(aSum1D - 0x1564)  ; "sum1 = %d\n"
.text:0000155C ADD             R0, PC, R0              ; "sum1 = %d\n"
.text:00001560 BL              printf
.text:00001560
.text:00001564 MOV             R1, SP
.text:00001568 MOV             R0, #66
.text:0000156C STR             R0, [R1,#4]             ; 参数6
.text:00001570 MOV             R0, #55
.text:00001574 STR             R0, [R1]                ; 参数5
.text:00001578 MOV             R0, #11
.text:0000157C MOV             R1, #22
.text:00001580 MOV             R2, #32
.text:00001584 MOV             R3, #44
.text:00001588 BL              sum2                    ; sp 参数5 sp+4 参数6
.text:00001588
.text:0000158C STR             R0, [SP,#0x20+var_10]   ; 返回值R0
.text:00001590 LDR             R1, [SP,#0x20+var_10]
.text:00001594 LDR             R0, =(aSum2D - 0x15A0)  ; "sum2 = %d\n"
.text:00001598 ADD             R0, PC, R0              ; "sum2 = %d\n"
.text:0000159C BL              printf
.text:0000159C
.text:000015A0 MOV             R0, #12
.text:000015A4 MOV             R1, #0x61 ; 'a'         ; 参数2
.text:000015A8 LDR             R2, =0x4048F5C3         ; 参数3
.text:000015AC BL              sum3
.text:000015AC
.text:000015B0 STR             R0, [SP,#0x20+var_14]   ; 返回值R0
.text:000015B4 LDR             R0, [SP,#0x20+var_14]
.text:000015B8 BL              __extendsfdf2
.text:000015B8
.text:000015BC MOV             R2, R0
.text:000015C0 MOV             R3, R1
.text:000015C4 LDR             R0, =(aSum3F - 0x15D0)  ; "sum3 = %f\n"
.text:000015C8 ADD             R0, PC, R0              ; "sum3 = %f\n"
.text:000015CC BL              printf
.text:000015CC
.text:000015D0 LDR             R0, [SP,#0x20+var_18]
.text:000015D4 MOV             SP, R11
.text:000015D8 POP             {R11,LR}
.text:000015DC BX              LR
```

可以很清楚的看到，不大于 32 位的数据类型使用 R0-R3 这 4 个寄存器用于传递参数，当参数大于 4 个时，使用栈传递。

### 64 位数据类型作为参数和返回值

例子：

```c
// fun2.c
#include<stdio.h>

long long sum1(long long a, long long b, long long c, long long d){
    return a + b + c + d;
}

double sum2(long long a, double b){
    return a + b;
}

int main(int argc, char const *argv[])
{
    long long sum = 0;
    sum = sum1(0x1234567898765432, 123L, 456L, 789L);
    printf("sum1 = %lld\n", sum);

    double dsum = sum2(12L, 3.4);
    printf("sum2 = %lf\n", dsum);

    return 0;
}
```

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21  fun2.c -o fun2
```

IDA 对应的反汇编代码。

```armasm
.text:00001500 PUSH            {R11,LR}
.text:00001504 MOV             R11, SP
.text:00001508 SUB             SP, SP, #0x38
.text:0000150C MOV             R3, #0
.text:00001510 STR             R3, [SP,#0x38+var_24]
.text:00001514 STR             R3, [R11,#var_4]
.text:00001518 STR             R0, [R11,#var_8]
.text:0000151C STR             R1, [R11,#var_C]
.text:00001520 STR             R3, [R11,#var_14]
.text:00001524 STR             R3, [R11,#var_18]
.text:00001528 MOV             R1, SP
.text:0000152C STR             R3, [R1,#12]
.text:00001530 MOV             R0, #789
.text:00001538 STR             R0, [R1,#8]             ; 参数4，8字节
.text:0000153C STR             R3, [R1,#4]
.text:00001540 MOV             R0, #456
.text:00001544 STR             R0, [R1]                ; 参数3，8字节
.text:00001548 LDR             R0, =0x98765432
.text:0000154C LDR             R1, =0x12345678         ; 参数1，R1:R0
.text:00001550 MOV             R2, #123                ; 参数2，R3:R2
.text:00001554 BL              sum1
.text:00001554
.text:00001558 STR             R1, [R11,#var_14]       ; R1:R0 存储返回值
.text:0000155C STR             R0, [R11,#var_18]
.text:00001560 LDR             R2, [R11,#var_18]
.text:00001564 LDR             R3, [R11,#var_14]
.text:00001568 LDR             R0, =(aSum1Lld - 0x1574) ; "sum1 = %lld\n"
.text:0000156C ADD             R0, PC, R0              ; "sum1 = %lld\n"
.text:00001570 BL              printf
.text:00001570
.text:00001574 LDR             R1, [SP,#0x38+var_24]
.text:00001578 MOV             R0, #12                 ; 参数1，R1:R0
.text:0000157C LDR             R2, =0x33333333
.text:00001580 LDR             R3, =0x400B3333         ; 参数2，R3:R2
.text:00001584 BL              sum2
.text:00001584
.text:00001588 STR             R1, [SP,#0x38+var_1C]   ; ; R1:R0 存储返回值
.text:0000158C STR             R0, [SP,#0x38+var_20]
.text:00001590 LDR             R2, [SP,#0x38+var_20]
.text:00001594 LDR             R3, [SP,#0x38+var_1C]
.text:00001598 LDR             R0, =(aSum2Lf - 0x15A4) ; "sum2 = %lf\n"
.text:0000159C ADD             R0, PC, R0              ; "sum2 = %lf\n"
.text:000015A0 BL              printf
.text:000015A0
.text:000015A4 LDR             R0, [SP,#0x38+var_24]
.text:000015A8 MOV             SP, R11
.text:000015AC POP             {R11,LR}
.text:000015B0 BX              LR
```

64 位的数据类型，使用 2 个连续的寄存器传递，R0 和 R1 或 R2 和 R3，返回值使用 R0 和 R1 传递。

### 结构体作为参数和返回值

例子：

```c
// fun3.c
#include<stdio.h>

typedef struct st_data1{
   int a;
   char b;
   int c;
   int d;
}st_data1;

typedef struct st_data2{
   int a;
   char b;
   int c;
   int d;
   int e;
   int f;
}st_data2;

st_data1 sum1(st_data1 data){
    int sum =  data.a + data.b + data.c + data.d;
    printf("sum = %d\n", sum);
    data.a = sum;
    return data;
}

st_data2 sum2(st_data2 data){
    int sum = data.a + data.b + data.c + data.d + data.e + data.f;
    printf("sum = %d\n", sum);
    data.a = sum;
    return data;
}

int main(int argc, char const *argv[])
{
    st_data1 d1 = {1, 2, 3, 4};
    st_data2 d2 = {12, 23, 34, 45, 56, 78};
    st_data1 st_r1 = sum1(d1);
    printf("a =%d, b=%d, c=%d, d=%d\n", st_r1.a, st_r1.b, st_r1.c, st_r1.d);

    st_data2 st_r2 = sum2(d2);
    printf("a =%d, b=%d, c=%d, d=%d, e=%d, f=%d\n", st_r2.a, st_r2.b, st_r2.c, st_r2.d, st_r2.e, st_r2.f);

    return 0;
}
```

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21  fun3.c -o fun3
```

IDA 对应的反汇编代码。

```armasm
.text:000015A4 PUSH            {R4,R5,R11,LR}
.text:000015A8 ADD             R11, SP, #8
.text:000015AC SUB             SP, SP, #0x70
.text:000015B0 MOV             R2, #0
.text:000015B4 STR             R2, [SP,#0x78+var_68]
.text:000015B8 STR             R2, [R11,#var_C]
.text:000015BC STR             R0, [R11,#var_10]
.text:000015C0 STR             R1, [R11,#var_14]
.text:000015C4 LDR             R2, =(dword_3C0 - 0x15D0)
.text:000015C8 ADD             R2, PC, R2              ; dword_3C0
.text:000015CC LDR             R0, [R2,#(dword_3C4 - 0x3C0)] ; 2
.text:000015D0 LDR             R1, [R2,#(dword_3C8 - 0x3C0)] ; 3
.text:000015D4 LDR             R2, [R2,#(dword_3CC - 0x3C0)] ; 4
.text:000015D8 STR             R2, [R11,#var_18]
.text:000015DC STR             R1, [R11,#var_1C]
.text:000015E0 STR             R0, [R11,#var_20]
.text:000015E4 LDR             R0, =(dword_3C0 - 0x15F0)
.text:000015E8 LDR             R0, [PC,R0]             ; dword_3C0 ; 1
.text:000015EC STR             R0, [R11,#var_24]
.text:000015F0 LDR             R1, =(dword_3D0 - 0x15FC)
.text:000015F4 ADD             R1, PC, R1              ; dword_3D0
.text:000015F8 ADD             R0, SP, #0x78+var_3C
.text:000015FC LDM             R1, {R2-R5,R12,LR}      ; 将R1地址的内存加载到寄存器
.text:00001600 STM             R0, {R2-R5,R12,LR}      ; 保存到栈上
.text:00001604 LDR             R1, [R11,#var_24]       ; st_data1.a
.text:00001608 LDR             R2, [R11,#var_20]       ; st_data1.b
.text:0000160C LDR             R3, [R11,#var_1C]       ; st_data1.c
.text:00001610 LDR             R0, [R11,#var_18]
.text:00001614 MOV             R12, SP
.text:00001618 STR             R0, [R12,#0x78+var_78]  ; st_data1.d
.text:0000161C ADD             R0, SP, #0x78+var_4C    ; R0保存返回地址
.text:00001620 BL              sum1
.text:00001620
.text:00001624 LDR             R1, [SP,#0x78+var_4C]   ; st_data1.a
.text:00001628 LDRB            R2, [SP,#0x78+var_48]   ; st_data1.a
.text:0000162C LDR             R3, [SP,#0x78+var_44]   ; st_data1.a
.text:00001630 LDR             R0, [SP,#0x78+var_40]
.text:00001634 MOV             R12, SP
.text:00001638 STR             R0, [R12,#0x78+var_78]  ; st_data1.d
.text:0000163C LDR             R0, =(aADBDCDDD - 0x1648) ; "a =%d, b=%d, c=%d, d=%d\n"
.text:00001640 ADD             R0, PC, R0              ; "a =%d, b=%d, c=%d, d=%d\n"
.text:00001644 BL              printf
.text:00001644
.text:00001648 LDR             R1, [SP,#0x78+var_3C]   ; st_data2.a
.text:0000164C LDR             R2, [SP,#0x78+var_38]   ; st_data1.b
.text:00001650 LDR             R3, [SP,#0x78+var_34]   ; st_data1.c
.text:00001654 LDR             R0, [SP,#0x78+var_30]
.text:00001658 LDR             LR, [SP,#0x78+var_2C]
.text:0000165C LDR             R4, [SP,#0x78+var_28]
.text:00001660 MOV             R12, SP
.text:00001664 STR             R4, [R12,#0x78+var_70]  ; st_data1.f
.text:00001668 STR             LR, [R12,#0x78+var_74]  ; st_data1.e
.text:0000166C STR             R0, [R12,#0x78+var_78]  ; st_data1.d
.text:00001670 ADD             R0, SP, #0x78+var_64    ; R0保存返回结果的地址
.text:00001674 BL              sum2
.text:00001674
.text:00001678 LDR             R1, [SP,#0x78+var_64]
.text:0000167C LDRB            R2, [SP,#0x78+var_60]
.text:00001680 LDR             R3, [SP,#0x78+var_5C]
.text:00001684 LDR             R0, [SP,#0x78+var_58]
.text:00001688 LDR             LR, [SP,#0x78+var_54]
.text:0000168C LDR             R4, [SP,#0x78+var_50]
.text:00001690 MOV             R12, SP
.text:00001694 STR             R4, [R12,#0x78+var_70]
.text:00001698 STR             LR, [R12,#0x78+var_74]
.text:0000169C STR             R0, [R12,#0x78+var_78]
.text:000016A0 LDR             R0, =(aADBDCDDDEDFD - 0x16AC) ; "a =%d, b=%d, c=%d, d=%d, e=%d, f=%d\n"
.text:000016A4 ADD             R0, PC, R0              ; "a =%d, b=%d, c=%d, d=%d, e=%d, f=%d\n"
.text:000016A8 BL              printf
.text:000016A8
.text:000016AC LDR             R0, [SP,#0x78+var_68]
.text:000016B0 SUB             SP, R11, #8
.text:000016B4 POP             {R4,R5,R11,LR}
.text:000016B8 BX              LR
```

如果传递的参数是结构体，并且结构体的大小不大于 32 字节，那么使用 R0-R3 传递，如果结构体的大小大于 32 字节，则大于 32 字节的部分使用栈传递。如果函数的返回值是结构体，则会将 R0 保存结构体在内存中的地址作为参数传递给函数。

## ARMv8

在 ARMv8 平台上参数通常使用寄存器传递：

- 不大于 64 位的数据类型使用 X0-X7 这 8 个寄存器传递参数，当参数大于 8 个时，使用栈传递。
- 对于浮点数和 128 位的数据类型使用 v0-v7（q0-q7、d0-d7、s0-s7）这 8 个寄存器传递参数，当参数大于 8 个时，使用栈传递。

ARMv8 平台上的返回值：

- 不大于 64 位的数据类型使用 X0 保存返回值。X8保存返回的地址。
- 浮点数和 128 位的数据类型使用 V0 保存返回值。

### 32 位或 64 位数据类型作为参数和返回值

例子：

```c
// fun4.c
#include<stdio.h>

long long sum(int a, int b, long long c, long long d, int e, int g, int h, char i, short j, int k){
    return a + b + c + d + e + g + h + i + j + k;
}

double fsum(float a, double b, int c, short d){
    return a + b + c + d;
}

int main(int argc, char const *argv[])
{
    long long sum_ = sum(1, 2, 3, 4, 5, 6, 7, 'a', 9, 10);
    printf("sum = %lld\n", sum_);
    double fsum_ = fsum(1.1, 2.2, 100, 20);
    printf("fsum = %lf\n", fsum_);
    return 0;
}
```
使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 fun4.c -o fun4
```

IDA 对应的反汇编代码。
```armasm
.text:0000000000001780 SUB             SP, SP, #0x50
.text:0000000000001784 STP             X29, X30, [SP,#0x40+var_s0]
.text:0000000000001788 ADD             X29, SP, #0x40
.text:000000000000178C MOV             W8, WZR
.text:0000000000001790 STR             W8, [SP,#0x40+var_24]
.text:0000000000001794 STUR            WZR, [X29,#var_4]
.text:0000000000001798 STUR            W0, [X29,#var_8]
.text:000000000000179C STUR            X1, [X29,#var_10]
.text:00000000000017A0 MOV             X9, SP
.text:00000000000017A4 MOV             W8, #9
.text:00000000000017A8 STRH            W8, [X9,#0x40+var_40]
.text:00000000000017AC MOV             W8, #0xA
.text:00000000000017B0 STR             W8, [X9,#0x40+var_38]   ; 参数9和参数10使用栈传递
.text:00000000000017B4 MOV             W0, #1
.text:00000000000017B8 MOV             W1, #2
.text:00000000000017BC MOV             X2, #3
.text:00000000000017C0 MOV             X3, #4
.text:00000000000017C4 MOV             W4, #5
.text:00000000000017C8 MOV             W5, #6
.text:00000000000017CC MOV             W6, #7
.text:00000000000017D0 MOV             W7, #0x61 ; 'a'
.text:00000000000017D4 BL              sum
.text:00000000000017D4
.text:00000000000017D8 STUR            X0, [X29,#var_18]       ; X0保存返回值
.text:00000000000017DC LDUR            X1, [X29,#var_18]
.text:00000000000017E0 ADRL            X0, aSumLld             ; "sum = %lld\n"
.text:00000000000017E8 BL              .printf
.text:00000000000017E8
.text:00000000000017EC MOV             W8, #0x3F8CCCCD
.text:00000000000017F4 FMOV            S0, W8
.text:00000000000017F8 ADRP            X8, #qword_508@PAGE
.text:00000000000017FC LDR             D1, [X8,#qword_508@PAGEOFF] ; V0和V1传递浮点数
.text:0000000000001800 MOV             W0, #0x64 ; 'd'
.text:0000000000001804 MOV             W1, #0x14
.text:0000000000001808 BL              fsum
.text:0000000000001808
.text:000000000000180C STR             D0, [SP,#0x40+var_20]   ; V0保存返回值
.text:0000000000001810 LDR             D0, [SP,#0x40+var_20]
.text:0000000000001814 ADRL            X0, aFsumLf             ; "fsum = %lf\n"
.text:000000000000181C BL              .printf
.text:000000000000181C
.text:0000000000001820 LDR             W0, [SP,#0x40+var_24]
.text:0000000000001824 LDP             X29, X30, [SP,#0x40+var_s0]
.text:0000000000001828 ADD             SP, SP, #0x50 ; 'P'
.text:000000000000182C RET
```

### 结构体作为参数和返回值

例子：
```c
// func5.c
#include<stdio.h>

typedef struct st_data
{
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;
    int h;
    int i;
    int j;
    int k;
}st_data;

st_data sum(st_data data){
    int sum = data.a + data.b + data.c + data.d + data.e + data.f + data.g + data.h + data.i + data.j + data.k;
    printf("sum = %d\n", sum);
    data.a = sum;
    return data;
}

int main(int argc, char const *argv[])
{
    st_data data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    st_data ret = sum(data);
    printf("st_data.a = %d\n", ret.a);
    return 0;
}
```
使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 fun5.c -o fun5
```

IDA 对应的反汇编代码。
```armasm
.text:00000000000017A4 SUB             SP, SP, #0xC0
.text:00000000000017A8 STP             X29, X30, [SP,#0xB0+var_s0]
.text:00000000000017AC ADD             X29, SP, #0xB0
.text:00000000000017B0 MOV             W8, WZR
.text:00000000000017B4 STR             W8, [SP,#0xB0+var_98]
.text:00000000000017B8 STUR            WZR, [X29,#var_4]
.text:00000000000017BC STUR            W0, [X29,#var_8]
.text:00000000000017C0 STUR            X1, [X29,#var_10]
.text:00000000000017C4 SUB             X0, X29, #-dest         ; dest
.text:00000000000017C8 STR             X0, [SP,#0xB0+src]
.text:00000000000017CC ADRL            X1, dword_56C           ; src
.text:00000000000017D4 MOV             X2, #0x2C ; ','         ; n
.text:00000000000017D8 STR             X2, [SP,#0xB0+n]
.text:00000000000017DC BL              .memcpy
.text:00000000000017DC
.text:00000000000017E0 LDR             X1, [SP,#0xB0+src]      ; src
.text:00000000000017E4 LDR             X2, [SP,#0xB0+n]        ; n
.text:00000000000017E8 ADD             X0, SP, #0xB0+var_94    ; dest
.text:00000000000017EC STR             X0, [SP,#0xB0+var_A0]
.text:00000000000017F0 BL              .memcpy
.text:00000000000017F0
.text:00000000000017F4 LDR             X0, [SP,#0xB0+var_A0]   ; X0保存结构体地址作为参数传递
.text:00000000000017F8 ADD             X8, SP, #0xB0+var_68    ; X8 保存返回值地址作为参数传递
.text:00000000000017FC BL              sum
.text:00000000000017FC
.text:0000000000001800 LDR             W1, [SP,#0xB0+var_68]
.text:0000000000001804 ADRL            X0, aStDataAD           ; "st_data.a = %d\n"
.text:000000000000180C BL              .printf
.text:000000000000180C
.text:0000000000001810 LDR             W0, [SP,#0xB0+var_98]
.text:0000000000001814 LDP             X29, X30, [SP,#0xB0+var_s0]
.text:0000000000001818 ADD             SP, SP, #0xC0
.text:000000000000181C RET
```

# 参考：

https://github.com/ARM-software/abi-aa/blob/main/aapcs32/aapcs32.rst

https://github.com/ARM-software/abi-aa/blob/main/aapcs64/aapcs64.rst
