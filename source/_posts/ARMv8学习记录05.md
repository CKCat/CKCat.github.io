---
title: ARMv8学习记录05
date: 2021-04-10 13:23:52
tags: 汇编
category: ARMv8汇编
---

# 运算符

## 加法和减法

例子：

```c
// add_sub.c
#include <stdio.h>

int main(int argc, char const *argv[])
{
    int n = 123;
    // 使用 argc 防止编译器优化
    printf("n + argc = %d\n", n + argc);
    printf("n + (argc++) = %d\n", n + (argc++));
    printf("argc = %d\n", argc);
    printf("n + (++argc) = %d\n", n + (++argc));
    printf("argc = %d\n", argc);
    printf("(argc--) = %d\n", (argc--));
    printf("argc = %d\n", argc);
    printf("(--argc) = %d\n", (--argc));
    printf("argc = %d\n", argc);

    float pi = 3.14;
    printf("pi + argc = %f\n", pi + argc);
    printf("pi - argc = %f\n", pi - argc);

    return 0;
}
```

### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  add_sub.c -o add_sub
```

IDA 对应的反汇编代码。

```armasm
.text:000014E0 PUSH            {R4-R6,R10,R11,LR}
.text:000014E4 ADD             R11, SP, #0x10
.text:000014E8 MOV             R4, R0
.text:000014EC ADD             R1, R0, #123            ; 直接使用ADD指令
.text:000014F0 LDR             R0, =(aNArgcD - 0x14FC) ; "n + argc = %d\n"
.text:000014F4 ADD             R0, PC, R0              ; "n + argc = %d\n"
.text:000014F8 BL              printf
.text:000014F8
.text:000014FC LDR             R0, =(aNArgcD_0 - 0x150C) ; "n + (argc++) = %d\n"
.text:00001500 MOV             R1, R4                  ; (argc++) = argc
.text:00001504 ADD             R0, PC, R0              ; "n + (argc++) = %d\n"
.text:00001508 BL              printf
.text:00001508
.text:0000150C LDR             R6, =(aArgcD - 0x151C)  ; "argc = %d\n"
.text:00001510 ADD             R1, R4, #1              ; argc = argc++ = argc+1
.text:00001514 ADD             R6, PC, R6              ; "argc = %d\n"
.text:00001518 MOV             R0, R6                  ; format
.text:0000151C BL              printf
.text:0000151C
.text:00001520 LDR             R0, =(aNArgcD_1 - 0x1534) ; "n + (++argc) = %d\n"
.text:00001524 ADD             R5, R4, #2              ; (++argc) = argc + 1 由于前面已经加1了，所以编译器优化为: (++argc) = argc + 2 。
.text:00001528 MOV             R1, R5
.text:0000152C ADD             R0, PC, R0              ; "n + (++argc) = %d\n"
.text:00001530 BL              printf
.text:00001530
.text:00001534 MOV             R0, R6                  ; format
.text:00001538 MOV             R1, R5                  ; argc = ++argc = argc + 1
.text:0000153C BL              printf
.text:0000153C
.text:00001540 LDR             R0, =(aArgcD_0 - 0x1550) ; "(argc--) = %d\n"
.text:00001544 SUB             R1, R4, #3              ; (argc--)=argc-5+2=argc-3
.text:00001548 ADD             R0, PC, R0              ; "(argc--) = %d\n"
.text:0000154C BL              printf
.text:0000154C
.text:00001550 SUB             R1, R4, #4              ; argc=argc-5+2-1=argc-4
.text:00001554 MOV             R0, R6                  ; format
.text:00001558 BL              printf
.text:00001558
.text:0000155C LDR             R0, =(aArgcD_1 - 0x1570) ; "(--argc) = %d\n"
.text:00001560 SUB             R5, R4, #5              ; (--argc)= argc-4-1=agrc-5
.text:00001564 MOV             R1, R5
.text:00001568 ADD             R0, PC, R0              ; "(--argc) = %d\n"
.text:0000156C BL              printf
.text:0000156C
.text:00001570 MOV             R0, R6                  ; format
.text:00001574 MOV             R1, R5                  ; argc=argc-5
.text:00001578 BL              printf
.text:00001578
.text:0000157C LDR             R1, =0x98765432
.text:00001580 LDR             R0, =0x12345678
.text:00001584 MOV             R6, R5,ASR#31           ; argc右移31位，实际获取argc的符号位
.text:00001588 ADDS            R2, R5, R1              ; 低32位 R2=R5+R1
.text:0000158C ADC             R3, R6, R0              ; 高32位 R3=R6+R0+C
.text:00001590 LDR             R0, =(aLlArgc0xLlx - 0x159C) ; "ll + argc = 0x%llx\n"
.text:00001594 ADD             R0, PC, R0              ; "ll + argc = 0x%llx\n"
.text:00001598 BL              printf
.text:00001598
.text:0000159C LDR             R1, =0x6789ABCE
.text:000015A0 LDR             R0, =0xEDCBA987
.text:000015A4 ADDS            R2, R5, R1              ; 将减法变为加法，加上负数
.text:000015A8 ADC             R3, R6, R0
.text:000015AC LDR             R0, =(aArgcLl0xLlx - 0x15B8) ; "argc - ll = 0x%llx\n"
.text:000015B0 ADD             R0, PC, R0              ; "argc - ll = 0x%llx\n"
.text:000015B4 BL              printf
.text:000015B4
.text:000015B8 ADD             R0, R4, #0xFA
.text:000015BC ADD             R1, R0, #0xFF00         ; 无符号数加法直接使用Add指令
.text:000015C0 LDR             R0, =(aU1Argc0xX - 0x15CC) ; "u1 + argc = 0x%x\n"
.text:000015C4 ADD             R0, PC, R0              ; "u1 + argc = 0x%x\n"
.text:000015C8 BL              printf
.text:000015C8
.text:000015CC SUB             R0, R4, #4              ; argc-0xffff-5 =argc-4-0x10000
.text:000015D0 SUB             R1, R0, #0x10000        ; 无符号数减法直接使用Sub指令
.text:000015D4 LDR             R0, =(aArgcU10xX - 0x15E0) ; "argc - u1 = 0x%x\n"
.text:000015D8 ADD             R0, PC, R0              ; "argc - u1 = 0x%x\n"
.text:000015DC BL              printf
.text:000015DC
.text:000015E0 MOV             R0, R5
.text:000015E4 BL              __floatsisf
.text:000015E4
.text:000015E8 LDR             R1, =0x4048F5C3
.text:000015EC MOV             R4, R0
.text:000015F0 BL              __aeabi_fadd            ; 浮点数相加直接调用了相应的浮点函数
.text:000015F0
.text:000015F4 BL              __extendsfdf2
.text:000015F4
.text:000015F8 MOV             R2, R0
.text:000015FC LDR             R0, =(aPiArgcF - 0x160C) ; "pi + argc = %f\n"
.text:00001600 MOV             R3, R1
.text:00001604 ADD             R0, PC, R0              ; "pi + argc = %f\n"
.text:00001608 BL              printf
.text:00001608
.text:0000160C LDR             R1, =0xC048F5C3
.text:00001610 MOV             R0, R4
.text:00001614 BL              __aeabi_fadd            ; 浮点数相减也转变为加法
.text:00001614
.text:00001618 BL              __extendsfdf2
.text:00001618
.text:0000161C MOV             R2, R0
.text:00001620 LDR             R0, =(aArgcPiF - 0x1630) ; "argc - pi = %f\n"
.text:00001624 MOV             R3, R1
.text:00001628 ADD             R0, PC, R0              ; "argc - pi = %f\n"
.text:0000162C BL              printf
.text:0000162C
.text:00001630 MOV             R0, #0
.text:00001634 POP             {R4-R6,R10,R11,LR}
.text:00001638 BX              LR
```

整数相加比价简单，直接调用 `ADD` 指令，其中可以发现 `argc++` 整体返回加 1 之前的值，`++argc` 整体返回加 1 后的值，两者不存在效率高低问题；减法和加法类似。

其中 `long long` 类型相加时比较巧妙：

```armasm
.text:B190957C LDR             R1, =0x98765432
.text:B1909580 LDR             R0, =0x12345678
.text:B1909584 MOV             R6, R5,ASR#31           ; argc右移31位，实际获取argc的符号位
.text:B1909588 ADDS            R2, R5, R1              ; 低32位 R2=R5+R1
.text:B190958C ADC             R3, R6, R0              ; 高32位 R3=R6+R0+C
```

当 R5 为负数时，R6=-1，如果 R1+R5 产生进位，则 C=1,最终 R2 的结果不变；如果没有产生进位，则 C=0，最终 R2=R2-1，相当于借了高位给 R2。
当 R5 为非负数时，R6=0，如果 R1+R5 产生进位，则 C=1,最终 R2=R2+1；如果没有产生进位，则 C=0，最终 R2 的结果不变。

浮点数相加，我们可以看到调用了 `__floatsisf` 、 `__aeabi_fadd` 、 `__extendsfdf2` 函数，通过名字可以猜测与浮点数运算相关。通过搜索后发现了相关文档：https://www.segger.com/doc/UM12008_FloatingPointLibrary.html ，通过该文档可以查看各种浮点运算相关的库函数。

而实际逆向过程中，各种函数的名称都经过处理了，不会有这么多符号，我们使用 `strip` 工具进行处理一下：

```bash
➜  arm-linux-androideabi-strip  add_sub
# 或者
➜  llvm-strip  add_sub
```

结果处理后的 IDA 反汇编结果如下：

```armasm
.text:000015E0 MOV             R0, R5
.text:000015E4 BL              sub_1A3C
.text:000015E4
.text:000015E8 LDR             R1, =0x4048F5C3
.text:000015EC MOV             R4, R0
.text:000015F0 BL              sub_1684
.text:000015F0
.text:000015F4 BL              sub_1974
.text:000015F4
.text:000015F8 MOV             R2, R0
.text:000015FC LDR             R0, =(aPiArgcF - 0x160C) ; "pi + argc = %f\n"
.text:00001600 MOV             R3, R1
.text:00001604 ADD             R0, PC, R0              ; "pi + argc = %f\n"
.text:00001608 BL              printf
.text:00001608
.text:0000160C LDR             R1, =0xC048F5C3
.text:00001610 MOV             R0, R4
.text:00001614 BL              sub_1684
.text:00001614
.text:00001618 BL              sub_1974
.text:00001618
.text:0000161C MOV             R2, R0
.text:00001620 LDR             R0, =(aArgcPiF - 0x1630) ; "argc - pi = %f\n"
.text:00001624 MOV             R3, R1
.text:00001628 ADD             R0, PC, R0              ; "argc - pi = %f\n"
.text:0000162C BL              printf
```

可以发现浮点数相关的函数名没有了，这时就需要进一步跟进函数进行或者制作 sig 文件确认函数的功能。

### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  add.c -o add
```

IDA 对应的反汇编代码。

```armasm
.text:000000000000171C STR             D8, [SP,#-0x28+var_8]!
.text:0000000000001720 STP             X29, X30, [SP,#8+var_s0]
.text:0000000000001724 ADD             X29, SP, #8
.text:0000000000001728 STR             X21, [SP,#8+var_s10]
.text:000000000000172C STP             X20, X19, [SP,#8+var_s18]
.text:0000000000001730 MOV             W19, W0                 ; W19=W0=argc
.text:0000000000001734 ADD             W1, W0, #123
.text:0000000000001738 ADRL            X0, aNArgcD             ; "n + argc = %d\n"
.text:0000000000001740 BL              .printf
.text:0000000000001740
.text:0000000000001744 ADRP            X0, #aNArgcD_1@PAGE     ; "n + (argc++) = %d\n"
.text:0000000000001748 MOV             W1, W19
.text:000000000000174C ADD             X0, X0, #aNArgcD_1@PAGEOFF ; "n + (argc++) = %d\n"
.text:0000000000001750 ADD             W20, W19, #1
.text:0000000000001754 BL              .printf
.text:0000000000001754
.text:0000000000001758 ADRP            X21, #aArgcD_0@PAGE     ; "argc = %d\n"
.text:000000000000175C MOV             W1, W20
.text:0000000000001760 ADD             X21, X21, #aArgcD_0@PAGEOFF ; "argc = %d\n"
.text:0000000000001764 MOV             X0, X21                 ; format
.text:0000000000001768 BL              .printf
.text:0000000000001768
.text:000000000000176C ADD             W20, W19, #2
.text:0000000000001770 ADRL            X0, aNArgcD_0           ; "n + (++argc) = %d\n"
.text:0000000000001778 MOV             W1, W20
.text:000000000000177C BL              .printf
.text:000000000000177C
.text:0000000000001780 MOV             X0, X21                 ; format
.text:0000000000001784 MOV             W1, W20
.text:0000000000001788 BL              .printf
.text:0000000000001788
.text:000000000000178C ADRP            X0, #aArgcD_1@PAGE      ; "(argc--) = %d\n"
.text:0000000000001790 SUB             W1, W19, #3
.text:0000000000001794 ADD             X0, X0, #aArgcD_1@PAGEOFF ; "(argc--) = %d\n"
.text:0000000000001798 SUB             W20, W19, #4
.text:000000000000179C BL              .printf
.text:000000000000179C
.text:00000000000017A0 MOV             X0, X21                 ; format
.text:00000000000017A4 MOV             W1, W20
.text:00000000000017A8 BL              .printf
.text:00000000000017A8
.text:00000000000017AC SUB             W20, W19, #5
.text:00000000000017B0 ADRL            X0, aArgcD              ; "(--argc) = %d\n"
.text:00000000000017B8 MOV             W1, W20
.text:00000000000017BC BL              .printf
.text:00000000000017BC
.text:00000000000017C0 MOV             X0, X21                 ; format
.text:00000000000017C4 MOV             W1, W20
.text:00000000000017C8 BL              .printf
.text:00000000000017C8
.text:00000000000017CC MOV             X8, #0x5432
.text:00000000000017D0 SXTW            X21, W20
.text:00000000000017D4 MOVK            X8, #0x9876,LSL#16
.text:00000000000017D8 ADRP            X0, #aLlArgc0xLlx@PAGE  ; "ll + argc = 0x%llx\n"
.text:00000000000017DC MOVK            X8, #0x5678,LSL#32
.text:00000000000017E0 ADD             X0, X0, #aLlArgc0xLlx@PAGEOFF ; "ll + argc = 0x%llx\n"
.text:00000000000017E4 MOVK            X8, #0x1234,LSL#48      ; X8=0x1234567898765432
.text:00000000000017E8 ADD             X1, X21, X8             ; 64位数直接相加
.text:00000000000017EC BL              .printf
.text:00000000000017EC
.text:00000000000017F0 MOV             X8, #0xABCE
.text:00000000000017F4 ADRP            X0, #aArgcLl0xLlx@PAGE  ; "argc - ll = 0x%llx\n"
.text:00000000000017F8 MOVK            X8, #0x6789,LSL#16
.text:00000000000017FC ADD             X0, X0, #aArgcLl0xLlx@PAGEOFF ; "argc - ll = 0x%llx\n"
.text:0000000000001800 MOVK            X8, #0xA987,LSL#32
.text:0000000000001804 MOVK            X8, #0xEDCB,LSL#48      ; X8=-0x1234567898765432
.text:0000000000001808 ADD             X1, X21, X8             ; 减法变加法
.text:000000000000180C BL              .printf
.text:000000000000180C
.text:0000000000001810 MOV             W8, #0xFFFA
.text:0000000000001814 ADRP            X0, #aU1Argc0xX@PAGE    ; "u1 + argc = 0x%x\n"
.text:0000000000001818 ADD             W1, W19, W8
.text:000000000000181C ADD             X0, X0, #aU1Argc0xX@PAGEOFF ; "u1 + argc = 0x%x\n"
.text:0000000000001820 BL              .printf
.text:0000000000001820
.text:0000000000001824 SUB             W8, W19, #0x10,LSL#12
.text:0000000000001828 ADRP            X0, #aArgcU10xX@PAGE    ; "argc - u1 = 0x%x\n"
.text:000000000000182C SUB             W1, W8, #4
.text:0000000000001830 ADD             X0, X0, #aArgcU10xX@PAGEOFF ; "argc - u1 = 0x%x\n"
.text:0000000000001834 BL              .printf
.text:0000000000001834
.text:0000000000001838 MOV             W8, #0xF5C3
.text:000000000000183C SCVTF           S8, W20
.text:0000000000001840 MOVK            W8, #0x4048,LSL#16
.text:0000000000001844 ADRL            X0, aPiArgcF            ; "pi + argc = %f\n"
.text:000000000000184C FMOV            S0, W8
.text:0000000000001850 FADD            S0, S8, S0              ; 直接使用浮点运算符
.text:0000000000001854 FCVT            D0, S0
.text:0000000000001858 BL              .printf
.text:0000000000001858
.text:000000000000185C MOV             W8, #0xF5C3
.text:0000000000001860 ADRP            X0, #aArgcPiF@PAGE      ; "argc - pi = %f\n"
.text:0000000000001864 MOVK            W8, #0xC048,LSL#16
.text:0000000000001868 ADD             X0, X0, #aArgcPiF@PAGEOFF ; "argc - pi = %f\n"
.text:000000000000186C FMOV            S0, W8
.text:0000000000001870 FADD            S0, S8, S0
.text:0000000000001874 FCVT            D0, S0
.text:0000000000001878 BL              .printf
.text:0000000000001878
.text:000000000000187C LDP             X20, X19, [SP,#8+var_s18]
.text:0000000000001880 MOV             W0, WZR
.text:0000000000001884 LDP             X29, X30, [SP,#8+var_s0]
.text:0000000000001888 LDR             X21, [SP,#8+var_s10]
.text:000000000000188C LDR             D8, [SP+8+var_8],#0x30
.text:0000000000001890 RET
```

ARMv8 反汇编中的 64 位加法更加简单了，而且浮点运算也有相应的运算指令，不用识别函数了。

## 乘法

```c
#include <stdio.h>
int main(int argc, char const *argv[])
{

    printf("argc * 7 = %d\n", argc * 7);
    printf("argc * 8 = %d\n", argc * 8);
    printf("argc * -3 = %d\n", argc * -3);
    printf("argc * -4 = %d\n", argc * -4);

    unsigned int n = 5;
    printf("n * argc = %u\n", n * argc);
    unsigned long long ll = 0x1234567898765432;
    printf("ll * argc = 0x%llx\n", ll * argc);

    printf("argc * 3.14 = %lf\n", argc * 3.14);
    printf("argc * 1.2 = %lf\n", argc * -1.2);

    return 0;
}
```

### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  mul.c -o mul
```

IDA 对应的反汇编代码。

```armasm
.text:00001490 PUSH            {R4,R5,R11,LR}
.text:00001494 ADD             R11, SP, #8
.text:00001498 MOV             R4, R0                  ; R4=R0=argc
.text:0000149C RSB             R1, R0, R0,LSL#3        ; argc<<3 -argc = argc*2^3 -argc = argc*7
.text:000014A0 LDR             R0, =(aArgc7D - 0x14AC) ; "argc * 7 = %d\n"
.text:000014A4 ADD             R0, PC, R0              ; "argc * 7 = %d\n"
.text:000014A8 BL              printf
.text:000014A8
.text:000014AC LDR             R0, =(aArgc8D - 0x14BC) ; "argc * 8 = %d\n"
.text:000014B0 MOV             R1, R4,LSL#3            ; argc<<3 = argc*2^3 = argc*8
.text:000014B4 ADD             R0, PC, R0              ; "argc * 8 = %d\n"
.text:000014B8 BL              printf
.text:000014B8
.text:000014BC LDR             R0, =(aArgc3D - 0x14CC) ; "argc * -3 = %d\n"
.text:000014C0 SUB             R1, R4, R4,LSL#2        ; argc - (argc<<2) = -(argc*3)
.text:000014C4 ADD             R0, PC, R0              ; "argc * -3 = %d\n"
.text:000014C8 BL              printf
.text:000014C8
.text:000014CC MOV             R0, #0
.text:000014D0 SUB             R1, R0, R4,LSL#2        ; 0 - (argc<<2) = -argc*4
.text:000014D4 LDR             R0, =(aArgc4D - 0x14E0) ; "argc * -4 = %d\n"
.text:000014D8 ADD             R0, PC, R0              ; "argc * -4 = %d\n"
.text:000014DC BL              printf
.text:000014DC
.text:000014E0 LDR             R0, =(aNArgcU - 0x14F0) ; "n * argc = %u\n"
.text:000014E4 ADD             R1, R4, R4,LSL#2        ; argc<<2 + argc = argc*5
.text:000014E8 ADD             R0, PC, R0              ; "n * argc = %u\n"
.text:000014EC BL              printf
.text:000014EC
.text:000014F0 LDR             R0, =0x98765432
.text:000014F4 LDR             R3, =0x12345678
.text:000014F8 UMULL           R2, R1, R4, R0          ; R1:R2 = R4*R0; R2保存低32位
.text:000014FC MLA             R5, R4, R3, R1          ; R5=R3*R4+R1
.text:00001500 MOV             R1, R4,ASR#31           ; 获取符号位,R1=0或R1=-1
.text:00001504 MLA             R3, R1, R0, R5          ; R3=R1*R0+R5,这里R1=0或R1=-1
.text:00001508 LDR             R0, =(aLlArgc0xLlx - 0x1514) ; "ll * argc = 0x%llx\n"
.text:0000150C ADD             R0, PC, R0              ; "ll * argc = 0x%llx\n"
.text:00001510 BL              printf
.text:00001510
.text:00001514 MOV             R0, R4
.text:00001518 BL              __floatsidf             ; 浮点数运算直接调用相应的函数
.text:00001518
.text:0000151C LDR             R2, =0x51EB851F
.text:00001520 LDR             R3, =0x40091EB8
.text:00001524 MOV             R4, R0
.text:00001528 MOV             R5, R1
.text:0000152C BL              __muldf3
.text:0000152C
.text:00001530 MOV             R2, R0
.text:00001534 LDR             R0, =(aArgc314Lf - 0x1544) ; "argc * 3.14 = %lf\n"
.text:00001538 MOV             R3, R1
.text:0000153C ADD             R0, PC, R0              ; "argc * 3.14 = %lf\n"
.text:00001540 BL              printf
.text:00001540
.text:00001544 LDR             R2, =0x33333333
.text:00001548 LDR             R3, =0xBFF33333
.text:0000154C MOV             R0, R4
.text:00001550 MOV             R1, R5
.text:00001554 BL              __muldf3
.text:00001554
.text:00001558 MOV             R2, R0
.text:0000155C LDR             R0, =(aArgc12Lf - 0x156C) ; "argc * 1.2 = %lf\n"
.text:00001560 MOV             R3, R1
.text:00001564 ADD             R0, PC, R0              ; "argc * 1.2 = %lf\n"
.text:00001568 BL              printf
.text:00001568
.text:0000156C MOV             R0, #0
.text:00001570 POP             {R4,R5,R11,LR}
.text:00001574 BX              LR
```

可以看到乘法运算编译后变成了位运算，这是因为位运算使用的指令周期比较短，速度更快。

其中 `long long` 类型乘以 `int` 类型需要解释一下：

```armasm
.text:000014F0 LDR             R0, =0x98765432
.text:000014F4 LDR             R3, =0x12345678
.text:000014F8 UMULL           R2, R1, R4, R0          ; R1:R2 = R4*R0; R2保存低32位
.text:000014FC MLA             R5, R4, R3, R1          ; R5=R3*R4+R1
.text:00001500 MOV             R1, R4,ASR#31           ; 获取符号位,R1=0或R1=-1
.text:00001504 MLA             R3, R1, R0, R5          ; R3=R1*R0+R5,这里R1=0或R1=-1
```

`R1, R4,ASR#31` 实际上是获取 `R4` 高 32 为的值，由于 `R4` 为有符号数，所以其高 32 为可能为-1 或者 0。
`R3, R1, R0, R5` 这里就比较好理解了 `R1*R0` 的值就是 `R4` 的高 32 位乘以 `R0` ，然后再加上原来的高 32 位就是最终结果。

### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  mul.c -o mul64
```

IDA 对应的反汇编代码。

```armasm
.text:00000000000016E4 STR             D8, [SP,#-0x20+var_10]!
.text:00000000000016E8 STP             X29, X30, [SP,#0x10+var_s0]
.text:00000000000016EC ADD             X29, SP, #0x10
.text:00000000000016F0 STP             X20, X19, [SP,#0x10+var_s10]
.text:00000000000016F4 LSL             W20, W0, #3
.text:00000000000016F8 MOV             W19, W0
.text:00000000000016FC SUB             W1, W20, W0
.text:0000000000001700 ADRL            X0, aArgc7D             ; "argc * 7 = %d\n"
.text:0000000000001708 BL              .printf
.text:0000000000001708
.text:000000000000170C ADRP            X0, #aArgc8D@PAGE       ; "argc * 8 = %d\n"
.text:0000000000001710 MOV             W1, W20
.text:0000000000001714 ADD             X0, X0, #aArgc8D@PAGEOFF ; "argc * 8 = %d\n"
.text:0000000000001718 BL              .printf
.text:0000000000001718
.text:000000000000171C LSL             W20, W19, #2
.text:0000000000001720 ADRP            X0, #aArgc3D@PAGE       ; "argc * -3 = %d\n"
.text:0000000000001724 SUB             W1, W19, W20
.text:0000000000001728 ADD             X0, X0, #aArgc3D@PAGEOFF ; "argc * -3 = %d\n"
.text:000000000000172C BL              .printf
.text:000000000000172C
.text:0000000000001730 ADRP            X0, #aArgc4D@PAGE       ; "argc * -4 = %d\n"
.text:0000000000001734 NEG             W1, W20
.text:0000000000001738 ADD             X0, X0, #aArgc4D@PAGEOFF ; "argc * -4 = %d\n"
.text:000000000000173C BL              .printf
.text:000000000000173C
.text:0000000000001740 ADRP            X0, #aNArgcU@PAGE       ; "n * argc = %u\n"
.text:0000000000001744 ADD             W1, W20, W19
.text:0000000000001748 ADD             X0, X0, #aNArgcU@PAGEOFF ; "n * argc = %u\n"
.text:000000000000174C BL              .printf
.text:000000000000174C
.text:0000000000001750 MOV             X9, #0x5432
.text:0000000000001754 SXTW            X8, W19
.text:0000000000001758 MOVK            X9, #0x9876,LSL#16
.text:000000000000175C ADRP            X0, #aLlArgc0xLlx@PAGE  ; "ll * argc = 0x%llx\n"
.text:0000000000001760 MOVK            X9, #0x5678,LSL#32
.text:0000000000001764 ADD             X0, X0, #aLlArgc0xLlx@PAGEOFF ; "ll * argc = 0x%llx\n"
.text:0000000000001768 MOVK            X9, #0x1234,LSL#48
.text:000000000000176C MUL             X1, X8, X9
.text:0000000000001770 BL              .printf
.text:0000000000001770
.text:0000000000001774 ADRP            X8, #qword_508@PAGE
.text:0000000000001778 SCVTF           D8, W19
.text:000000000000177C ADRL            X0, aArgc314Lf          ; "argc * 3.14 = %lf\n"
.text:0000000000001784 LDR             D0, [X8,#qword_508@PAGEOFF]
.text:0000000000001788 FMUL            D0, D8, D0
.text:000000000000178C BL              .printf
.text:000000000000178C
.text:0000000000001790 MOV             X8, #0x3333333333333333
.text:0000000000001794 ADRP            X0, #aArgc12Lf@PAGE     ; "argc * 1.2 = %lf\n"
.text:0000000000001798 MOVK            X8, #0xBFF3,LSL#48
.text:000000000000179C ADD             X0, X0, #aArgc12Lf@PAGEOFF ; "argc * 1.2 = %lf\n"
.text:00000000000017A0 FMOV            D0, X8
.text:00000000000017A4 FMUL            D0, D8, D0
.text:00000000000017A8 BL              .printf
.text:00000000000017A8
.text:00000000000017AC LDP             X20, X19, [SP,#0x10+var_s10]
.text:00000000000017B0 MOV             W0, WZR
.text:00000000000017B4 LDP             X29, X30, [SP,#0x10+var_s0]
.text:00000000000017B8 LDR             D8, [SP+0x10+var_10],#0x30
.text:00000000000017BC RET
```

ARMv8 反汇编的内容和 ARMv7 的类似，不过新增了浮点运算指令。

## 位运算

```c
#include <stdio.h>
int main(int argc, char const *argv[])
{
    unsigned int n = argc;
    printf("argc << 3 = %d\n", argc << 3);  // 逻辑左移
    printf("argc >> 3 = %d\n", argc >> 4);  // 算术右移
    printf("n << 3 = %u\n", n << 7);     // 逻辑左移
    printf("n >> 3 = %u\n", n >> 9);     // 逻辑右移
    printf("~argc = %d\n", ~argc);          // 按位取反（求补码）
    printf("argc & 1 = %d\n", argc & 1);    // 按位与 AND
    printf("argc | 2 = %d\n", argc | 2);    // 按位或 OR
    printf("argc ^ 3 = %d\n", argc ^ 3);    // 异或 XOR

    return 0;
}
```

### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  bit.c -o bit
```

IDA 对应的反汇编代码。

```armasm
.text:0000147C PUSH            {R4,R10,R11,LR}
.text:00001480 ADD             R11, SP, #8
.text:00001484 MOV             R4, R0
.text:00001488 MOV             R1, R0,LSL#3
.text:0000148C LDR             R0, =(aArgc3D - 0x1498) ; "argc << 3 = %d\n"
.text:00001490 ADD             R0, PC, R0              ; "argc << 3 = %d\n"
.text:00001494 BL              printf
.text:00001494
.text:00001498 LDR             R0, =(aArgc3D_0 - 0x14A8) ; "argc >> 3 = %d\n"
.text:0000149C MOV             R1, R4,ASR#4
.text:000014A0 ADD             R0, PC, R0              ; "argc >> 3 = %d\n"
.text:000014A4 BL              printf
.text:000014A4
.text:000014A8 LDR             R0, =(aN3U - 0x14B8)    ; "n << 3 = %u\n"
.text:000014AC MOV             R1, R4,LSL#7
.text:000014B0 ADD             R0, PC, R0              ; "n << 3 = %u\n"
.text:000014B4 BL              printf
.text:000014B4
.text:000014B8 LDR             R0, =(aN3U_0 - 0x14C8)  ; "n >> 3 = %u\n"
.text:000014BC MOV             R1, R4,LSR#9
.text:000014C0 ADD             R0, PC, R0              ; "n >> 3 = %u\n"
.text:000014C4 BL              printf
.text:000014C4
.text:000014C8 LDR             R0, =(aArgcD - 0x14D8)  ; "~argc = %d\n"
.text:000014CC MVN             R1, R4
.text:000014D0 ADD             R0, PC, R0              ; "~argc = %d\n"
.text:000014D4 BL              printf
.text:000014D4
.text:000014D8 LDR             R0, =(aArgc1D - 0x14E8) ; "argc & 1 = %d\n"
.text:000014DC AND             R1, R4, #1
.text:000014E0 ADD             R0, PC, R0              ; "argc & 1 = %d\n"
.text:000014E4 BL              printf
.text:000014E4
.text:000014E8 LDR             R0, =(aArgc2D - 0x14F8) ; "argc | 2 = %d\n"
.text:000014EC ORR             R1, R4, #2
.text:000014F0 ADD             R0, PC, R0              ; "argc | 2 = %d\n"
.text:000014F4 BL              printf
.text:000014F4
.text:000014F8 LDR             R0, =(aArgc3D_1 - 0x1508) ; "argc ^ 3 = %d\n"
.text:000014FC EOR             R1, R4, #3
.text:00001500 ADD             R0, PC, R0              ; "argc ^ 3 = %d\n"
.text:00001504 BL              printf
.text:00001504
.text:00001508 MOV             R0, #0
.text:0000150C POP             {R4,R10,R11,LR}
.text:00001510 BX              LR
```

可以看到位运算基本都有对应的汇编指令。

### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  bit.c -o bit64
```

IDA 对应的反汇编代码。

```armasm
.text:00000000000016BC STP             X29, X30, [SP,#var_20]!
.text:00000000000016C0 STR             X19, [SP,#0x20+var_10]
.text:00000000000016C4 MOV             X29, SP
.text:00000000000016C8 MOV             W19, W0
.text:00000000000016CC LSL             W1, W0, #3
.text:00000000000016D0 ADRL            X0, aArgc3D             ; "argc << 3 = %d\n"
.text:00000000000016D8 BL              .printf
.text:00000000000016D8
.text:00000000000016DC ADRP            X0, #aArgc3D_0@PAGE     ; "argc >> 3 = %d\n"
.text:00000000000016E0 ASR             W1, W19, #4
.text:00000000000016E4 ADD             X0, X0, #aArgc3D_0@PAGEOFF ; "argc >> 3 = %d\n"
.text:00000000000016E8 BL              .printf
.text:00000000000016E8
.text:00000000000016EC ADRP            X0, #aN3U@PAGE          ; "n << 3 = %u\n"
.text:00000000000016F0 LSL             W1, W19, #7
.text:00000000000016F4 ADD             X0, X0, #aN3U@PAGEOFF   ; "n << 3 = %u\n"
.text:00000000000016F8 BL              .printf
.text:00000000000016F8
.text:00000000000016FC ADRP            X0, #aN3U_0@PAGE        ; "n >> 3 = %u\n"
.text:0000000000001700 LSR             W1, W19, #9
.text:0000000000001704 ADD             X0, X0, #aN3U_0@PAGEOFF ; "n >> 3 = %u\n"
.text:0000000000001708 BL              .printf
.text:0000000000001708
.text:000000000000170C ADRP            X0, #aArgcD@PAGE        ; "~argc = %d\n"
.text:0000000000001710 MVN             W1, W19
.text:0000000000001714 ADD             X0, X0, #aArgcD@PAGEOFF ; "~argc = %d\n"
.text:0000000000001718 BL              .printf
.text:0000000000001718
.text:000000000000171C ADRP            X0, #aArgc1D@PAGE       ; "argc & 1 = %d\n"
.text:0000000000001720 AND             W1, W19, #1
.text:0000000000001724 ADD             X0, X0, #aArgc1D@PAGEOFF ; "argc & 1 = %d\n"
.text:0000000000001728 BL              .printf
.text:0000000000001728
.text:000000000000172C ADRP            X0, #aArgc2D@PAGE       ; "argc | 2 = %d\n"
.text:0000000000001730 ORR             W1, W19, #2
.text:0000000000001734 ADD             X0, X0, #aArgc2D@PAGEOFF ; "argc | 2 = %d\n"
.text:0000000000001738 BL              .printf
.text:0000000000001738
.text:000000000000173C ADRP            X0, #aArgc3D_1@PAGE     ; "argc ^ 3 = %d\n"
.text:0000000000001740 EOR             W1, W19, #3
.text:0000000000001744 ADD             X0, X0, #aArgc3D_1@PAGEOFF ; "argc ^ 3 = %d\n"
.text:0000000000001748 BL              .printf
.text:0000000000001748
.text:000000000000174C LDR             X19, [SP,#0x20+var_10]
.text:0000000000001750 MOV             W0, WZR
.text:0000000000001754 LDP             X29, X30, [SP+0x20+var_20],#0x20
.text:0000000000001758 RET
```

ARMv8 同样有对应的汇编指令。

# 除法和模运算

除法运算会转换位乘法和位运算，在转换过程中比较复杂，有多种转换公式，下面将分多种情况进行讲解。

## 除数为无符号 2 的幂

`x` 为被除数，`y` 为除数，其中 `y=2^n` 且为无符号数，`q` 为结果，`r` 为余数，对应的算法为：

```c
q = x >> n

r = x & (2^n - 1)
```

例如：

```c
// div1.c
#include <stdio.h>

int main(int argc, char const *argv[])
{
    unsigned int n = 4;
    unsigned int j = 8;
    printf("argc / 4 = %u\n", argc / n);
    printf("argc %% 8 = %d\n", argc % j);
    return 0;
}
```

### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  div1.c -o div1
```

IDA 对应的反汇编代码。

```armasm
.text:00001428 PUSH            {R4,R10,R11,LR}
.text:0000142C ADD             R11, SP, #8
.text:00001430 MOV             R4, R0
.text:00001434 MOV             R1, R0,LSR#2            ; 直接右移2位
.text:00001438 LDR             R0, =(aArgc4U - 0x1444) ; "argc / 4 = %u\n"
.text:0000143C ADD             R0, PC, R0              ; "argc / 4 = %u\n"
.text:00001440 BL              printf
.text:00001440
.text:00001444 LDR             R0, =(aArgc8D - 0x1454) ; "argc %% 8 = %d\n"
.text:00001448 AND             R1, R4, #7              ; 直接与(2^n-1)
.text:0000144C ADD             R0, PC, R0              ; "argc %% 8 = %d\n"
.text:00001450 BL              printf
.text:00001450
.text:00001454 MOV             R0, #0
.text:00001458 POP             {R4,R10,R11,LR}
.text:0000145C BX              LR
```

### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  div1.c -o div164
```

IDA 对应的反汇编代码。

```armasm
.text:000000000000166C STP             X29, X30, [SP,#var_20]!
.text:0000000000001670 STR             X19, [SP,#0x20+var_10]
.text:0000000000001674 MOV             X29, SP
.text:0000000000001678 MOV             W19, W0
.text:000000000000167C LSR             W1, W0, #2
.text:0000000000001680 ADRL            X0, aArgc4U             ; "argc / 4 = %u\n"
.text:0000000000001688 BL              .printf
.text:0000000000001688
.text:000000000000168C ADRP            X0, #aArgc8D@PAGE       ; "argc %% 8 = %d\n"
.text:0000000000001690 AND             W1, W19, #7
.text:0000000000001694 ADD             X0, X0, #aArgc8D@PAGEOFF ; "argc %% 8 = %d\n"
.text:0000000000001698 BL              .printf
.text:0000000000001698
.text:000000000000169C LDR             X19, [SP,#0x20+var_10]
.text:00000000000016A0 MOV             W0, WZR
.text:00000000000016A4 LDP             X29, X30, [SP+0x20+var_20],#0x20
.text:00000000000016A8 RET
```

## 除数为无符号非 2 的幂

### 第一种情况

`x` 为被除数，`y` 为除数，其中 `y!=2^n` 且为无符号数，`q` 为结果，`r` 为余数，`MAGIC_NUM` 为魔数，`i` 为位移，对应的算法为：

```c
q = (x * MAGIC_NUM)>>(32+i)

y = 2^(32+i) / MAGIC_NUM

r = x - q * y
```

例如：

```c
// div2.c
#include <stdio.h>

int main(int argc, char const *argv[])
{
    unsigned int n = 3;
    unsigned int j = 6;
    printf("argc / 4 = %u\n", argc / n);
    printf("argc %% 8 = %d\n", argc % j);
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  div2.c -o div2
```

IDA 对应的反汇编代码。

```armasm
.text:00001428 PUSH            {R4,R5,R11,LR}
.text:0000142C ADD             R11, SP, #8
.text:00001430 MOV             R4, R0
.text:00001434 LDR             R0, =0xAAAAAAAB
.text:00001438 UMULL           R1, R5, R4, R0
.text:0000143C LDR             R0, =(aArgc4U - 0x144C) ; "argc / 3 = %u\n"
.text:00001440 MOV             R1, R5,LSR#1            ; 除数 y= 2^(32+1) /0xAAAAAAAB = 2.99
.text:00001444 ADD             R0, PC, R0              ; "argc / 3 = %u\n"
.text:00001448 BL              printf
.text:00001448
.text:0000144C MOV             R0, R5,LSR#2            ; 除数 y= 2^(32+2) /0xAAAAAAAB = 5.99
.text:00001450 ADD             R0, R0, R0,LSL#1        ; R0 = R0 + R0*2
.text:00001454 SUB             R1, R4, R0,LSL#1        ; R1 = argc - R0*2 = argc - o*6
.text:00001458 LDR             R0, =(aArgc8D - 0x1464) ; "argc %% 6 = %d\n"
.text:0000145C ADD             R0, PC, R0              ; "argc %% 6 = %d\n"
.text:00001460 BL              printf
.text:00001460
.text:00001464 MOV             R0, #0
.text:00001468 POP             {R4,R5,R11,LR}
.text:0000146C BX              LR
```

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  div2.c -o div264
```

IDA 对应的反汇编代码。

```armasm
.text:0000000000001674 STP             X29, X30, [SP,#-0x20+var_s0]!
.text:0000000000001678 STP             X20, X19, [SP,#var_s10]
.text:000000000000167C MOV             X29, SP
.text:0000000000001680 MOV             W8, #0xAAAB
.text:0000000000001684 MOV             W19, W0
.text:0000000000001688 MOVK            W8, #0xAAAA,LSL#16
.text:000000000000168C UMULL           X20, W0, W8
.text:0000000000001690 ADRL            X0, aArgc4U             ; "argc / 3 = %u\n"
.text:0000000000001698 LSR             X1, X20, #33            ; 除数 y= 2^(32+1) /0xAAAAAAAB = 2.99
.text:000000000000169C BL              .printf
.text:000000000000169C
.text:00000000000016A0 LSR             X8, X20, #34
.text:00000000000016A4 MOV             W9, #6
.text:00000000000016A8 ADRP            X0, #aArgc8D@PAGE       ; "argc %% 6 = %d\n"
.text:00000000000016AC MSUB            W1, W8, W9, W19
.text:00000000000016B0 ADD             X0, X0, #aArgc8D@PAGEOFF ; "argc %% 6 = %d\n"
.text:00000000000016B4 BL              .printf
.text:00000000000016B4
.text:00000000000016B8 LDP             X20, X19, [SP,#var_s10]
.text:00000000000016BC MOV             W0, WZR
.text:00000000000016C0 LDP             X29, X30, [SP+var_s0],#0x20
.text:00000000000016C4 RET
```

### 第二种情况

`x` 为被除数，`y` 为除数，其中 `y!=2^n` 且为无符号数，`q` 为结果，`r` 为余数，`MAGIC_NUM` 为魔数，`i`、`j` 为位移，对应的算法为：

```c
q = ( ( c(x - (x*MAGIC_NUM) >> 32) >> i ) + (x*MAGIC_NUM) >> 32 ) >> j

y = (2^(32 + i +j)) / (2^32 + MAGIC_NUM);

r = x - q * y
```

例如：

```c
// div3.c
#include <stdio.h>

int main(int argc, char const *argv[])
{
    unsigned int n = 7;
    printf("argc / 7 = %u\n", argc / n);
    printf("argc %% 7 = %d\n", argc % n);
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  div3.c -o div3
```

IDA 对应的反汇编代码。

```armasm
.text:00001428 PUSH            {R4-R6,R10,R11,LR}
.text:0000142C ADD             R11, SP, #0x10
.text:00001430 MOV             R4, R0
.text:00001434 LDR             R0, =0x24924925
.text:00001438 UMULL           R1, R2, R4, R0          ; R2=(argc*0x24924925)>>32
.text:0000143C SUB             R0, R4, R2              ; R0 = argc - R2
.text:00001440 ADD             R6, R2, R0,LSR#1        ; R6 = R2 + R0>>1
.text:00001444 LDR             R0, =(aArgc7U - 0x1458) ; "argc / 7 = %u\n"
.text:00001448 MOV             R5, R6,LSR#2            ; R5 = R6 >> 2 = (R2+R0>>1)>>2 = (R2 + (argc-R2)>>1) >> 2
.text:0000144C MOV             R1, R5
.text:00001450 ADD             R0, PC, R0              ; "argc / 7 = %u\n"
.text:00001454 BL              printf
.text:00001454
.text:00001458 MOV             R0, R5,LSL#3
.text:0000145C SUB             R0, R0, R6,LSR#2        ; R6,LSR#2 == R5; R0 = R5*7
.text:00001460 SUB             R1, R4, R0              ; r = argc-(y*7)
.text:00001464 LDR             R0, =(aArgc7D - 0x1470) ; "argc %% 7 = %d\n"
.text:00001468 ADD             R0, PC, R0              ; "argc %% 7 = %d\n"
.text:0000146C BL              printf
.text:0000146C
.text:00001470 MOV             R0, #0
.text:00001474 POP             {R4-R6,R10,R11,LR}
.text:00001478 BX              LR
```

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  div3.c -o div364
```

IDA 对应的反汇编代码。

```armasm
.text:0000000000001674 STP             X29, X30, [SP,#-0x20+var_s0]!
.text:0000000000001678 STP             X20, X19, [SP,#var_s10]
.text:000000000000167C MOV             X29, SP
.text:0000000000001680 MOV             W8, #0x4925
.text:0000000000001684 MOV             W19, W0
.text:0000000000001688 MOVK            W8, #0x2492,LSL#16
.text:000000000000168C UMULL           X8, W0, W8
.text:0000000000001690 LSR             X8, X8, #32
.text:0000000000001694 SUB             W9, W0, W8
.text:0000000000001698 ADRL            X0, aArgc7U             ; "argc / 7 = %u\n"
.text:00000000000016A0 ADD             W8, W8, W9,LSR#1
.text:00000000000016A4 LSR             W20, W8, #2
.text:00000000000016A8 MOV             W1, W20
.text:00000000000016AC BL              .printf
.text:00000000000016AC
.text:00000000000016B0 SUB             W8, W20, W20,LSL#3
.text:00000000000016B4 ADRP            X0, #aArgc7D@PAGE       ; "argc %% 7 = %d\n"
.text:00000000000016B8 ADD             W1, W19, W8
.text:00000000000016BC ADD             X0, X0, #aArgc7D@PAGEOFF ; "argc %% 7 = %d\n"
.text:00000000000016C0 BL              .printf
.text:00000000000016C0
.text:00000000000016C4 LDP             X20, X19, [SP,#var_s10]
.text:00000000000016C8 MOV             W0, WZR
.text:00000000000016CC LDP             X29, X30, [SP+var_s0],#0x20
.text:00000000000016D0 RET
```

## 除数为有符号 2 的幂

`x` 为被除数，`y` 为除数，其中 `y=2^n` 且为有符号数，`q` 为结果，`r` 为余数，对应的算法为：

```c
if(x >= 0){
    q = x >> n;
    r = x - (x and -(2^n-1))
}else{
    q = (x + 2^n -1) >> n;
    r = x - ((x + 2^n -1) and -(2^n-1))
}
```

例如：

```c
// div4.c
#include <stdio.h>

int main(int argc, char const *argv[])
{
    unsigned int n = 4;
    unsigned int j = 8;
    printf("argc / 4 = %u\n", argc / n);
    printf("argc %% 8 = %d\n", argc % j);
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  div4.c -o div4
```

IDA 对应的反汇编代码。

```armasm
.text:00001428 PUSH            {R4,R5,R11,LR}
.text:0000142C ADD             R11, SP, #8
.text:00001430 MOV             R5, R0,ASR#31           ; 获取符号位R5=0xFFFFFFFF或R5=0
.text:00001434 MOV             R4, R0
.text:00001438 ADD             R0, R0, R5,LSR#30       ; if R0<0; R0=(R0+2^n-1)=R0+3; else R0=R0
.text:0000143C MOV             R1, R0,ASR#2            ; 右移n位
.text:00001440 LDR             R0, =(aArgc4U - 0x144C) ; "argc / 4 = %d\n"
.text:00001444 ADD             R0, PC, R0              ; "argc / 4 = %d\n"
.text:00001448 BL              printf
.text:00001448
.text:0000144C ADD             R0, R4, R5,LSR#29       ; if R0<0; R0=(R0+2^n-1)=R0+7; else R0=R0
.text:00001450 BIC             R0, R0, #7              ; 按位清除
.text:00001454 SUB             R1, R4, R0
.text:00001458 LDR             R0, =(aArgc8D - 0x1464) ; "argc %% 8 = %d\n"
.text:0000145C ADD             R0, PC, R0              ; "argc %% 8 = %d\n"
.text:00001460 BL              printf
.text:00001460
.text:00001464 MOV             R0, #0
.text:00001468 POP             {R4,R5,R11,LR}
.text:0000146C BX              LR
```

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  div4.c -o div464
```

IDA 对应的反汇编代码。

```armasm
.text:0000000000001674 STP             X29, X30, [SP,#-0x20+var_s0]!
.text:0000000000001678 STP             X20, X19, [SP,#var_s10]
.text:000000000000167C MOV             X29, SP
.text:0000000000001680 ADD             W8, W0, #3
.text:0000000000001684 CMP             W0, #0
.text:0000000000001688 MOV             W19, W0
.text:000000000000168C CSEL            W8, W8, W0, LT          ; if LT;W8=W8; else W8=W0
.text:0000000000001690 ADRP            X0, #aArgc4U@PAGE       ; "argc / 4 = %d\n"
.text:0000000000001694 ASR             W1, W8, #2
.text:0000000000001698 ADD             X0, X0, #aArgc4U@PAGEOFF ; "argc / 4 = %d\n"
.text:000000000000169C ADD             W8, W19, #7
.text:00000000000016A0 CSEL            W20, W8, W19, LT
.text:00000000000016A4 BL              .printf
.text:00000000000016A4
.text:00000000000016A8 AND             W8, W20, #0xFFFFFFF8
.text:00000000000016AC ADRP            X0, #aArgc8D@PAGE       ; "argc %% 8 = %d\n"
.text:00000000000016B0 SUB             W1, W19, W8
.text:00000000000016B4 ADD             X0, X0, #aArgc8D@PAGEOFF ; "argc %% 8 = %d\n"
.text:00000000000016B8 BL              .printf
.text:00000000000016B8
.text:00000000000016BC LDP             X20, X19, [SP,#var_s10]
.text:00000000000016C0 MOV             W0, WZR
.text:00000000000016C4 LDP             X29, X30, [SP+var_s0],#0x20
.text:00000000000016C8 RET
```

## 除数为有符号非 2 的幂

### 第一种情况，MAGIC_NUM > 0

`x` 为被除数，`y` 为除数，其中 `y!=2^n` 且为有符号数，`q` 为结果，`r` 为余数，`MAGIC_NUM` 为魔数，`i` 位位移，对应的算法为：

```c
if(x >=0)
    q = (x * MAGIC_NUM) >> (32 + i);
else
    q = ( (x * MAGIC_NUM) >> (32 + i) ) + 1;

y = 2^(32+i) / MAGIC_NUM;
```

例如：

```c
// div5.c
#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("argc / 9 = %u\n", argc / 9);
    printf("argc %% 9 = %u\n", argc % 9);
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  div5.c -o div5
```

IDA 对应的反汇编代码。

```armasm
.text:00001428 PUSH            {R4,R5,R11,LR}
.text:0000142C ADD             R11, SP, #8
.text:00001430 MOV             R4, R0
.text:00001434 LDR             R0, =954437177
.text:00001438 SMULL           R1, R2, R4, R0          ; R2为高32位
.text:0000143C MOV             R0, R2,ASR#1            ; R2>>1 等价 ((R0*R4)>>32)>>1
.text:00001440 ADD             R5, R0, R2,LSR#31       ; R2,LSR#31获取最高位
.text:00001444 LDR             R0, =(aArgc9U - 0x1454) ; "argc / 9 = %u\n"
.text:00001448 MOV             R1, R5
.text:0000144C ADD             R0, PC, R0              ; "argc / 9 = %u\n"
.text:00001450 BL              printf
.text:00001450
.text:00001454 ADD             R0, R5, R5,LSL#3        ; R5, R5,LSL#3 = R5*9
.text:00001458 SUB             R1, R4, R0
.text:0000145C LDR             R0, =(aArgc9U_0 - 0x1468) ; "argc %% 9 = %u\n"
.text:00001460 ADD             R0, PC, R0              ; "argc %% 9 = %u\n"
.text:00001464 BL              printf
.text:00001464
.text:00001468 MOV             R0, #0
.text:0000146C POP             {R4,R5,R11,LR}
.text:00001470 BX              LR
```

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  div5.c -o div564
```

IDA 对应的反汇编代码。

```armasm
.text:0000000000001674 STP             X29, X30, [SP,#-0x20+var_s0]!
.text:0000000000001678 STP             X20, X19, [SP,#var_s10]
.text:000000000000167C MOV             X29, SP
.text:0000000000001680 MOV             W8, #0x8E39
.text:0000000000001684 MOV             W19, W0
.text:0000000000001688 MOVK            W8, #0x38E3,LSL#16
.text:000000000000168C SMULL           X8, W0, W8
.text:0000000000001690 ADRL            X0, aArgc9U             ; "argc / 9 = %u\n"
.text:0000000000001698 LSR             X9, X8, #63             ; 获取符号位
.text:000000000000169C ASR             X8, X8, #33             ; 右移 32 + i
.text:00000000000016A0 ADD             W20, W8, W9
.text:00000000000016A4 MOV             W1, W20
.text:00000000000016A8 BL              .printf
.text:00000000000016A8
.text:00000000000016AC ADD             W8, W20, W20,LSL#3      ; W8=W20+w20*8=W20*9
.text:00000000000016B0 ADRP            X0, #aArgc9U_0@PAGE     ; "argc %% 9 = %u\n"
.text:00000000000016B4 SUB             W1, W19, W8
.text:00000000000016B8 ADD             X0, X0, #aArgc9U_0@PAGEOFF ; "argc %% 9 = %u\n"
.text:00000000000016BC BL              .printf
.text:00000000000016BC
.text:00000000000016C0 LDP             X20, X19, [SP,#var_s10]
.text:00000000000016C4 MOV             W0, WZR
.text:00000000000016C8 LDP             X29, X30, [SP+var_s0],#0x20
.text:00000000000016CC RET
```

### 第二种情况，MAGIC_NUM < 0

`x` 为被除数，`y` 为除数，其中 `y!=2^n` 且为有符号数，`q` 为结果，`r` 为余数，`MAGIC_NUM` 为魔数，`i` 位位移，对应的算法为：

```c
if (x >=0)
    ((x * MAGIC_NUM) >> 32 + x) >> i
if (x < 0)
    (((x * MAGIC_NUM) >> 32 + x) >> i) + 1

y = = 2^(32+i) / MAGIC_NUM;
```

例如：

```c
// div6.c
#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("argc / 7 = %u\n", argc / 7);
    printf("argc %% 7 = %u\n", argc % 7);
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  div6.c -o div6
```

IDA 对应的反汇编代码。

```armasm
.text:00001428 PUSH            {R4,R5,R11,LR}
.text:0000142C ADD             R11, SP, #8
.text:00001430 MOV             R4, R0
.text:00001434 LDR             R0, =0x92492493
.text:00001438 SMULL           R1, R2, R4, R0
.text:0000143C ADD             R0, R2, R4              ; R0=R2+R4=(R4*MAGIC_NUM)>>32 + R4
.text:00001440 MOV             R1, R0,ASR#2
.text:00001444 ADD             R5, R1, R0,LSR#31
.text:00001448 LDR             R0, =(aArgc9U - 0x1458) ; "argc / 7 = %u\n"
.text:0000144C MOV             R1, R5
.text:00001450 ADD             R0, PC, R0              ; "argc / 7 = %u\n"
.text:00001454 BL              printf
.text:00001454
.text:00001458 RSB             R0, R5, R5,LSL#3        ; R0=R5*8-R5=R5*7
.text:0000145C SUB             R1, R4, R0
.text:00001460 LDR             R0, =(aArgc9U_0 - 0x146C) ; "argc %% 7 = %u\n"
.text:00001464 ADD             R0, PC, R0              ; "argc %% 7 = %u\n"
.text:00001468 BL              printf
.text:00001468
.text:0000146C MOV             R0, #0
.text:00001470 POP             {R4,R5,R11,LR}
.text:00001474 BX              LR
```

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  div6.c -o div664
```

IDA 对应的反汇编代码。

```armasm
.text:0000000000001674 STP             X29, X30, [SP,#-0x20+var_s0]!
.text:0000000000001678 STP             X20, X19, [SP,#var_s10]
.text:000000000000167C MOV             X29, SP
.text:0000000000001680 MOV             W8, #0x2493
.text:0000000000001684 MOV             W19, W0
.text:0000000000001688 MOVK            W8, #0x9249,LSL#16
.text:000000000000168C SMULL           X8, W0, W8
.text:0000000000001690 LSR             X8, X8, #32             ; X8=(R0*MAGIC_NUM)>>32
.text:0000000000001694 ADD             W8, W8, W0
.text:0000000000001698 ADRP            X0, #aArgc7U@PAGE       ; "argc / 7 = %u\n"
.text:000000000000169C ASR             W9, W8, #2
.text:00000000000016A0 ADD             X0, X0, #aArgc7U@PAGEOFF ; "argc / 7 = %u\n"
.text:00000000000016A4 ADD             W20, W9, W8,LSR#31      ; 获取符号位
.text:00000000000016A8 MOV             W1, W20
.text:00000000000016AC BL              .printf
.text:00000000000016AC
.text:00000000000016B0 SUB             W8, W20, W20,LSL#3
.text:00000000000016B4 ADRP            X0, #aArgc7U_0@PAGE     ; "argc %% 7 = %u\n"
.text:00000000000016B8 ADD             W1, W19, W8
.text:00000000000016BC ADD             X0, X0, #aArgc7U_0@PAGEOFF ; "argc %% 7 = %u\n"
.text:00000000000016C0 BL              .printf
.text:00000000000016C0
.text:00000000000016C4 LDP             X20, X19, [SP,#var_s10]
.text:00000000000016C8 MOV             W0, WZR
.text:00000000000016CC LDP             X29, X30, [SP+var_s0],#0x20
.text:00000000000016D0 RET
```

## 除数为有符号负 2 的幂

`x` 为被除数，`y` 为除数，其中 `y=-2^n` 且为有符号数，`q` 为结果，`r` 为余数，对应的算法为：

```c
if(x >= 0)
    q = -(x >> n);
else
    q = -((x + 2^n -1) >> n);


```

例如：

```c
// div7.c
#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("argc / 9 = %u\n", argc / 9);
    printf("argc %% 9 = %u\n", argc % 9);
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  div7.c -o div7
```

IDA 对应的反汇编代码。

```armasm
.text:0000142C PUSH            {R4,R5,R11,LR}
.text:00001430 ADD             R11, SP, #8
.text:00001434 MOV             R4, R0
.text:00001438 MOV             R0, R0,ASR#31           ; 获取符号位
.text:0000143C ADD             R5, R4, R0,LSR#30       ; if R0>=0; R5=R4; else R5= R4+3
.text:00001440 MOV             R0, #0
.text:00001444 SUB             R1, R0, R5,ASR#2        ; R1 = -(R5>>2)
.text:00001448 LDR             R0, =(aArgc4U - 0x1454) ; "argc / -4 = %u\n"
.text:0000144C ADD             R0, PC, R0              ; "argc / -4 = %u\n"
.text:00001450 BL              printf
.text:00001450
.text:00001454 BIC             R0, R5, #3              ; R0 = R5& 0xFFFFFFFC
.text:00001458 SUB             R1, R4, R0
.text:0000145C LDR             R0, =(aArgc4U_0 - 0x1468) ; "argc %% -4 = %u\n"
.text:00001460 ADD             R0, PC, R0              ; "argc %% -4 = %u\n"
.text:00001464 BL              printf
.text:00001464
.text:00001468 MOV             R0, #0
.text:0000146C POP             {R4,R5,R11,LR}
.text:00001470 BX              LR
```

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  div7.c -o div764
```

IDA 对应的反汇编代码。

```armasm
.text:0000000000001674 STP             X29, X30, [SP,#-0x20+var_s0]!
.text:0000000000001678 STP             X20, X19, [SP,#var_s10]
.text:000000000000167C MOV             X29, SP
.text:0000000000001680 ADD             W8, W0, #3
.text:0000000000001684 CMP             W0, #0
.text:0000000000001688 CSEL            W20, W8, W0, LT         ; if W0>=0: W20=W0; else: W20=W8
.text:000000000000168C MOV             W19, W0
.text:0000000000001690 ADRP            X0, #aArgc4U@PAGE       ; "argc / -4 = %u\n"
.text:0000000000001694 NEG             W1, W20,ASR#2
.text:0000000000001698 ADD             X0, X0, #aArgc4U@PAGEOFF ; "argc / -4 = %u\n"
.text:000000000000169C BL              .printf
.text:000000000000169C
.text:00000000000016A0 AND             W8, W20, #0xFFFFFFFC
.text:00000000000016A4 ADRP            X0, #aArgc4U_0@PAGE     ; "argc %% -4 = %u\n"
.text:00000000000016A8 SUB             W1, W19, W8
.text:00000000000016AC ADD             X0, X0, #aArgc4U_0@PAGEOFF ; "argc %% -4 = %u\n"
.text:00000000000016B0 BL              .printf
.text:00000000000016B0
.text:00000000000016B4 LDP             X20, X19, [SP,#var_s10]
.text:00000000000016B8 MOV             W0, WZR
.text:00000000000016BC LDP             X29, X30, [SP+var_s0],#0x20
.text:00000000000016C0 RET
```

## 除数为有符号负非 2 的幂

### 第一种情况， MAGIC_NUM > 0

`x` 为被除数，`y` 为除数，其中 `y!=2^n` 且为有符号负数，`q` 为结果，`r` 为余数，`MAGIC_NUM` 为魔数，`i` 位位移，对应的算法为：

```c
if(x >=0)
    q = ((x * MAGIC_NUM)>>32 - x)>>i;
else
    q = ((x * MAGIC_NUM)>>32 - x)>>i + 1;

y = -2^(32 + i) /(2^32-MAGIC_NUM)
```

例如：

```c
// div8.c
#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("argc / 9 = %u\n", argc / 9);
    printf("argc %% 9 = %u\n", argc % 9);
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  div7.c -o div7
```

IDA 对应的反汇编代码。

```armasm
.text:0000142C PUSH            {R4,R10,R11,LR}
.text:00001430 ADD             R11, SP, #8
.text:00001434 MOV             R4, R0
.text:00001438 LDR             R0, =0x6DB6DB6D
.text:0000143C SMULL           R1, R2, R4, R0
.text:00001440 SUB             R0, R2, R4              ; R0=(R4*MAGIC_NUM)>>32 - R4
.text:00001444 MOV             R1, R0,ASR#2
.text:00001448 ADD             R1, R1, R0,LSR#31       ; 获取符号位
.text:0000144C LDR             R0, =(aArgc7U - 0x1458) ; "argc / -7 = %u\n"
.text:00001450 ADD             R0, PC, R0              ; "argc / -7 = %u\n"
.text:00001454 BL              printf
.text:00001454
.text:00001458 LDR             R0, =0x92492493
.text:0000145C SMULL           R1, R2, R4, R0
.text:00001460 ADD             R0, R2, R4
.text:00001464 MOV             R1, R0,ASR#2
.text:00001468 ADD             R0, R1, R0,LSR#31
.text:0000146C RSB             R0, R0, R0,LSL#3
.text:00001470 SUB             R1, R4, R0
.text:00001474 LDR             R0, =(aArgc7U_0 - 0x1480) ; "argc %% -7 = %u\n"
.text:00001478 ADD             R0, PC, R0              ; "argc %% -7 = %u\n"
.text:0000147C BL              printf
.text:0000147C
.text:00001480 MOV             R0, #0
.text:00001484 POP             {R4,R10,R11,LR}
.text:00001488 BX              LR
.text:00001488
```

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  div8.c -o div864
```

IDA 对应的反汇编代码。

```armasm
.text:000000000000166C STP             X29, X30, [SP,#var_20]!
.text:0000000000001670 STR             X19, [SP,#0x20+var_10]
.text:0000000000001674 MOV             X29, SP
.text:0000000000001678 MOV             W8, #0xDB6D
.text:000000000000167C MOV             W19, W0
.text:0000000000001680 MOVK            W8, #0x6DB6,LSL#16
.text:0000000000001684 SMULL           X8, W0, W8
.text:0000000000001688 LSR             X8, X8, #32
.text:000000000000168C SUB             W8, W8, W0              ; W8=(W0*MAGIC_NUM)>>32 - W0
.text:0000000000001690 ADRP            X0, #aArgc7U@PAGE       ; "argc / -7 = %u\n"
.text:0000000000001694 ASR             W9, W8, #2
.text:0000000000001698 ADD             X0, X0, #aArgc7U@PAGEOFF ; "argc / -7 = %u\n"
.text:000000000000169C ADD             W1, W9, W8,LSR#31
.text:00000000000016A0 BL              .printf
.text:00000000000016A0
.text:00000000000016A4 MOV             W8, #0x2493
.text:00000000000016A8 ADRP            X0, #aArgc7U_0@PAGE     ; "argc %% -7 = %u\n"
.text:00000000000016AC MOVK            W8, #0x9249,LSL#16
.text:00000000000016B0 ADD             X0, X0, #aArgc7U_0@PAGEOFF ; "argc %% -7 = %u\n"
.text:00000000000016B4 SMULL           X8, W19, W8
.text:00000000000016B8 LSR             X8, X8, #32
.text:00000000000016BC ADD             W8, W8, W19
.text:00000000000016C0 ASR             W9, W8, #2
.text:00000000000016C4 ADD             W8, W9, W8,LSR#31
.text:00000000000016C8 SUB             W8, W8, W8,LSL#3
.text:00000000000016CC ADD             W1, W19, W8
.text:00000000000016D0 BL              .printf
.text:00000000000016D0
.text:00000000000016D4 LDR             X19, [SP,#0x20+var_10]
.text:00000000000016D8 MOV             W0, WZR
.text:00000000000016DC LDP             X29, X30, [SP+0x20+var_20],#0x20
.text:00000000000016E0 RET
```

### 第二种情况，MAGIC_NUM < 0

`x` 为被除数，`y` 为除数，其中 `y!=2^n` 且为有符号负数，`q` 为结果，`r` 为余数，`MAGIC_NUM` 为魔数，`i` 位位移，对应的算法为：

```c
if(x >=0)
    q = (x * MAGIC_NUM) >> (32 + i);
else
    q = ((x * MAGIC_NUM) >> (32 + i)) + 1;

y = -2^(32+i) / (2^32-MAGIC_NUM)
```

例如：

```c
// div9.c
#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("argc / -5 = %u\n", argc / -5);
    printf("argc %% -5 = %u\n", argc % -5);
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  div9.c -o div9
```

IDA 对应的反汇编代码。

```armasm
.text:0000142C PUSH            {R4,R10,R11,LR}
.text:00001430 ADD             R11, SP, #8
.text:00001434 MOV             R4, R0
.text:00001438 LDR             R0, =0x99999999
.text:0000143C SMULL           R1, R2, R4, R0
.text:00001440 MOV             R0, R2,ASR#1            ; R0=(R0*MAGIC_NUM)>>(32+1)
.text:00001444 ADD             R1, R0, R2,LSR#31
.text:00001448 LDR             R0, =(aArgc5U - 0x1454) ; "argc / -5 = %u\n"
.text:0000144C ADD             R0, PC, R0              ; "argc / -5 = %u\n"
.text:00001450 BL              printf
.text:00001450
.text:00001454 LDR             R0, =0x66666667
.text:00001458 SMULL           R1, R2, R4, R0
.text:0000145C MOV             R0, R2,ASR#1
.text:00001460 ADD             R0, R0, R2,LSR#31
.text:00001464 ADD             R0, R0, R0,LSL#2
.text:00001468 SUB             R1, R4, R0
.text:0000146C LDR             R0, =(aArgc5U_0 - 0x1478) ; "argc %% -5 = %u\n"
.text:00001470 ADD             R0, PC, R0              ; "argc %% -5 = %u\n"
.text:00001474 BL              printf
.text:00001474
.text:00001478 MOV             R0, #0
.text:0000147C POP             {R4,R10,R11,LR}
.text:00001480 BX              LR
```

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  div9.c -o div964
```

IDA 对应的反汇编代码。

```armasm
.text:000000000000166C STP             X29, X30, [SP,#var_20]!
.text:0000000000001670 STR             X19, [SP,#0x20+var_10]
.text:0000000000001674 MOV             X29, SP
.text:0000000000001678 MOV             W8, #0x99999999
.text:000000000000167C MOV             W19, W0
.text:0000000000001680 SMULL           X8, W0, W8
.text:0000000000001684 ADRL            X0, aArgc5U             ; "argc / -5 = %u\n"
.text:000000000000168C LSR             X9, X8, #63
.text:0000000000001690 ASR             X8, X8, #33             ; X8=(W0*MAGIC_NUM)>>(32+1)
.text:0000000000001694 ADD             W1, W8, W9
.text:0000000000001698 BL              .printf
.text:0000000000001698
.text:000000000000169C MOV             W8, #0x6667
.text:00000000000016A0 ADRP            X0, #aArgc5U_0@PAGE     ; "argc %% -5 = %u\n"
.text:00000000000016A4 MOVK            W8, #0x6666,LSL#16
.text:00000000000016A8 ADD             X0, X0, #aArgc5U_0@PAGEOFF ; "argc %% -5 = %u\n"
.text:00000000000016AC SMULL           X8, W19, W8
.text:00000000000016B0 LSR             X9, X8, #0x3F ; '?'
.text:00000000000016B4 ASR             X8, X8, #0x21 ; '!'
.text:00000000000016B8 ADD             W8, W8, W9
.text:00000000000016BC ADD             W8, W8, W8,LSL#2
.text:00000000000016C0 SUB             W1, W19, W8
.text:00000000000016C4 BL              .printf
.text:00000000000016C4
.text:00000000000016C8 LDR             X19, [SP,#0x20+var_10]
.text:00000000000016CC MOV             W0, WZR
.text:00000000000016D0 LDP             X29, X30, [SP+0x20+var_20],#0x20
.text:00000000000016D4 RET
```

除法运算主要就是以上几种模式，看到类似的 MAGIC_NUM 可以初步判断为除法。
