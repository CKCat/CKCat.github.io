---
title: ARMv8学习记录06
date: 2023-07-04 13:08:09
tags: 汇编
category: ARMv8汇编
---

# 流程控制

## if...else if...else

例如：

```c
// if.c
#include<stdio.h>

int main(int argc, char const *argv[])
{
    if(argc > 1){
        printf("argc > 1, argc=%d\n", argc);
    }else if(argc == 1){
        printf("argc == 1, argc=%d\n", argc);
    }else{
        printf("argc < 1, argc=%d\n", argc);
    }
    return 0;
}
```

### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  if.c -o if
```

IDA 对应的反汇编代码。

```armasm
.text:00001444 PUSH            {R11,LR}
.text:00001448 MOV             R11, SP
.text:0000144C MOV             R1, R0
.text:00001450 CMP             R0, #2
.text:00001454 BLT             loc_1464
.text:00001454
.text:00001458 LDR             R0, =(aArgc1ArgcD - 0x1464) ; "argc > 1, argc=%d\n"
.text:0000145C ADD             R0, PC, R0              ; "argc > 1, argc=%d\n"
.text:00001460 B               loc_1484                ;
.text:00001460
.text:00001464 ; ---------------------------------------------------------------------------
.text:00001464
.text:00001464 loc_1464                                ; CODE XREF: main+10↑j
.text:00001464 CMP             R1, #1
.text:00001468 BNE             loc_147C
.text:00001468
.text:0000146C LDR             R0, =(aArgc1ArgcD_0 - 0x147C) ; "argc == 1, argc=%d\n"
.text:00001470 MOV             R1, #1
.text:00001474 ADD             R0, PC, R0              ; "argc == 1, argc=%d\n"
.text:00001478 B               loc_1484                ;
.text:00001478
.text:0000147C ; ---------------------------------------------------------------------------
.text:0000147C
.text:0000147C loc_147C                                ; CODE XREF: main+24↑j
.text:0000147C LDR             R0, =(aArgc1ArgcD_1 - 0x1488) ; "argc < 1, argc=%d\n"
.text:00001480 ADD             R0, PC, R0              ; "argc < 1, argc=%d\n"
.text:00001480
.text:00001484
.text:00001484 loc_1484                                ; CODE XREF: main+1C↑j
.text:00001484                                         ; main+34↑j
.text:00001484 BL              printf                  ; printf优化为一个
.text:00001484
.text:00001488 MOV             R0, #0
.text:0000148C POP             {R11,LR}
.text:00001490 BX              LR
```

### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  if.c -o if64
```

IDA 对应的反汇编代码。

```armasm
.text:0000000000001684 STP             X29, X30, [SP,#-0x10+var_s0]!
.text:0000000000001688 MOV             X29, SP
.text:000000000000168C MOV             W1, W0
.text:0000000000001690 CMP             W0, #2
.text:0000000000001694 B.LT            loc_16A4
.text:0000000000001694
.text:0000000000001698 ADRL            X0, aArgc1ArgcD         ; "argc > 1, argc=%d\n"
.text:00000000000016A0 B               loc_16C0
.text:00000000000016A0
.text:00000000000016A4 ; ---------------------------------------------------------------------------
.text:00000000000016A4
.text:00000000000016A4 loc_16A4                                ; CODE XREF: main+10↑j
.text:00000000000016A4 CMP             W1, #1
.text:00000000000016A8 B.NE            loc_16B8
.text:00000000000016A8
.text:00000000000016AC ADRL            X0, aArgc1ArgcD_0       ; "argc == 1, argc=%d\n"
.text:00000000000016B4 B               loc_16C0
.text:00000000000016B4
.text:00000000000016B8 ; ---------------------------------------------------------------------------
.text:00000000000016B8
.text:00000000000016B8 loc_16B8                                ; CODE XREF: main+24↑j
.text:00000000000016B8 ADRL            X0, aArgc1ArgcD_1       ; "argc < 1, argc=%d\n"
.text:00000000000016B8
.text:00000000000016C0
.text:00000000000016C0 loc_16C0                                ; CODE XREF: main+1C↑j
.text:00000000000016C0                                         ; main+30↑j
.text:00000000000016C0 BL              .printf
.text:00000000000016C0
.text:00000000000016C4 MOV             W0, WZR
.text:00000000000016C8 LDP             X29, X30, [SP+var_s0],#0x10
.text:00000000000016CC RET
```

## switch...case

### case 后的值连续或间隔不大

当 case 后的值间隔不大时，编译器会创建一个偏移表，然后通过表中的偏移获取对应的 case 分支。

例如：

```c
// switch1.c
int main(int argc,char* argv[]){
	switch(argc){
	case 0:
		printf("argc = 0");
		break;
	case 1:
		printf("argc = 1");
		break;
	case 5:
		printf("argc = 5");
		break;
	case 6:
		printf("argc = 6");
		break;
	case 8:
		printf("argc = 8");
		break;
	case 9:
		printf("argc = 9");
		break;
	}
	return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  switch1.c -o switch1
```

IDA 对应的反汇编代码。

```armasm
.text:0000147C main                                    ; DATA XREF: .text:00001438↑o
.text:0000147C                                         ; .got:main_ptr↓o
.text:0000147C PUSH            {R11,LR}
.text:00001480 MOV             R11, SP
.text:00001484 CMP             R0, #9
.text:00001488 BHI             loc_1520                ; default
.text:00001488
.text:0000148C ADR             R1, off_1498            ; argc=0;offset=0x28;PC=0x14C0
.text:00001490 LDR             R0, [R1,R0,LSL#2]       ; 获取偏移
.text:00001494 ADD             PC, R1, R0
.text:00001494 ;设置偏移表
.text:00001494 ; ---------------------------------------------------------------------------
.text:00001498 off_1498 DCD dword_28                   ; DATA XREF: main+10↑o
.text:00001498                                         ; argc=0;offset=0x28;PC=0x14C0
.text:0000149C DCD dword_38                            ; argc=1;offset=0x38;PC=0x14D0
.text:000014A0 DCD dword_88                            ; default;offset=0x88;PC=0x1520
.text:000014A4 DCD dword_88
.text:000014A8 DCD dword_88
.text:000014AC DCD dword_48                            ; argc=5;offset=0x48;PC=0x14E0
.text:000014B0 DCD dword_58                            ; argc=6;offset=0x58;PC=0x14F0
.text:000014B4 DCD dword_88
.text:000014B8 DCD dword_68                            ; argc=8;offset=0x68;PC=0x1500
.text:000014BC DCD dword_78                            ; argc=9;offset=0x78;PC=0x1510
.text:000014C0 ; ---------------------------------------------------------------------------
.text:000014C0 LDR             R0, =(aArgc0ArgcD - 0x14D0) ; "argc = 0, argc=%d\n"
.text:000014C4 MOV             R1, #0
.text:000014C8 ADD             R0, PC, R0              ; "argc = 0, argc=%d\n"
.text:000014CC B               loc_151C
.text:000014CC
.text:000014D0 ; ---------------------------------------------------------------------------
.text:000014D0 LDR             R0, =(aArgc1ArgcD - 0x14E0) ; "argc = 1, argc=%d\n"
.text:000014D4 MOV             R1, #1
.text:000014D8 ADD             R0, PC, R0              ; "argc = 1, argc=%d\n"
.text:000014DC B               loc_151C
.text:000014DC
.text:000014E0 ; ---------------------------------------------------------------------------
.text:000014E0 LDR             R0, =(aArgc5ArgcD - 0x14F0) ; "argc = 5, argc=%d\n"
.text:000014E4 MOV             R1, #5
.text:000014E8 ADD             R0, PC, R0              ; "argc = 5, argc=%d\n"
.text:000014EC B               loc_151C
.text:000014EC
.text:000014F0 ; ---------------------------------------------------------------------------
.text:000014F0 LDR             R0, =(aArgc6ArgcD - 0x1500) ; "argc = 6, argc=%d\n"
.text:000014F4 MOV             R1, #6
.text:000014F8 ADD             R0, PC, R0              ; "argc = 6, argc=%d\n"
.text:000014FC B               loc_151C
.text:000014FC
.text:00001500 ; ---------------------------------------------------------------------------
.text:00001500 LDR             R0, =(aArgc8ArgcD - 0x1510) ; "argc = 8, argc=%d\n"
.text:00001504 MOV             R1, #8
.text:00001508 ADD             R0, PC, R0              ; "argc = 8, argc=%d\n"
.text:0000150C B               loc_151C
.text:0000150C
.text:00001510 ; ---------------------------------------------------------------------------
.text:00001510 LDR             R0, =(aArgc9ArgcD - 0x1520) ; "argc = 9, argc=%d\n"
.text:00001514 MOV             R1, #9
.text:00001518 ADD             R0, PC, R0              ; "argc = 9, argc=%d\n"
.text:0000151C
.text:0000151C loc_151C                                ; CODE XREF: main+50↑j
.text:0000151C
.text:0000151C BL              printf
.text:00001520
.text:00001520 loc_1520                                ; CODE XREF: main+C↑j
.text:00001520 MOV             R0, #0
.text:00001524 POP             {R11,LR}
.text:00001528 BX              LR
```

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  switch1.c -o switch164
```

IDA 对应的反汇编代码。

```armasm
.text:00000000000016C4 STP             X29, X30, [SP,#-0x10+var_s0]!
.text:00000000000016C8 MOV             X29, SP
.text:00000000000016CC CMP             W0, #9                  ; switch 10 cases
.text:00000000000016D0 B.HI            def_16EC                ; jumptable 00000000000016EC default case, cases 2-4,7
.text:00000000000016D0
.text:00000000000016D4 ADRP            X9, #jpt_16EC@PAGE
.text:00000000000016D8 MOV             W8, W0
.text:00000000000016DC ADD             X9, X9, #jpt_16EC@PAGEOFF
.text:00000000000016E0 ADR             X10, loc_16F0           ; jumptable 00000000000016EC case 0
.text:00000000000016E4 LDRB            W11, [X9,X8]
.text:00000000000016E8 ADD             X10, X10, X11,LSL#2
.text:00000000000016EC BR              X10                     ; switch jump
.text:00000000000016EC
.text:00000000000016F0 ; ---------------------------------------------------------------------------
.text:00000000000016F0
.text:00000000000016F0 loc_16F0                                ; CODE XREF: main+28↑j
.text:00000000000016F0                                         ; DATA XREF: main+1C↑o
.text:00000000000016F0 ADRP            X0, #aArgc0ArgcD@PAGE   ; jumptable 00000000000016EC case 0
.text:00000000000016F4 MOV             W1, WZR
.text:00000000000016F8 ADD             X0, X0, #aArgc0ArgcD@PAGEOFF ; "argc = 0, argc=%d\n"
.text:00000000000016FC B               loc_174C
.text:00000000000016FC
.text:0000000000001700 ; ---------------------------------------------------------------------------
.text:0000000000001700
.text:0000000000001700 loc_1700                                ; CODE XREF: main+28↑j
.text:0000000000001700 ADRP            X0, #aArgc1ArgcD@PAGE   ; jumptable 00000000000016EC case 1
.text:0000000000001704 MOV             W1, #1
.text:0000000000001708 ADD             X0, X0, #aArgc1ArgcD@PAGEOFF ; "argc = 1, argc=%d\n"
.text:000000000000170C B               loc_174C
.text:000000000000170C
.text:0000000000001710 ; ---------------------------------------------------------------------------
.text:0000000000001710
.text:0000000000001710 loc_1710                                ; CODE XREF: main+28↑j
.text:0000000000001710 ADRP            X0, #aArgc5ArgcD@PAGE   ; jumptable 00000000000016EC case 5
.text:0000000000001714 MOV             W1, #5
.text:0000000000001718 ADD             X0, X0, #aArgc5ArgcD@PAGEOFF ; "argc = 5, argc=%d\n"
.text:000000000000171C B               loc_174C
.text:000000000000171C
.text:0000000000001720 ; ---------------------------------------------------------------------------
.text:0000000000001720
.text:0000000000001720 loc_1720                                ; CODE XREF: main+28↑j
.text:0000000000001720 ADRP            X0, #aArgc6ArgcD@PAGE   ; jumptable 00000000000016EC case 6
.text:0000000000001724 MOV             W1, #6
.text:0000000000001728 ADD             X0, X0, #aArgc6ArgcD@PAGEOFF ; "argc = 6, argc=%d\n"
.text:000000000000172C B               loc_174C
.text:000000000000172C
.text:0000000000001730 ; ---------------------------------------------------------------------------
.text:0000000000001730
.text:0000000000001730 loc_1730                                ; CODE XREF: main+28↑j
.text:0000000000001730 ADRP            X0, #aArgc8ArgcD@PAGE   ; jumptable 00000000000016EC case 8
.text:0000000000001734 MOV             W1, #8
.text:0000000000001738 ADD             X0, X0, #aArgc8ArgcD@PAGEOFF ; "argc = 8, argc=%d\n"
.text:000000000000173C B               loc_174C
.text:000000000000173C
.text:0000000000001740 ; ---------------------------------------------------------------------------
.text:0000000000001740
.text:0000000000001740 loc_1740                                ; CODE XREF: main+28↑j
.text:0000000000001740 ADRP            X0, #aArgc9ArgcD@PAGE   ; jumptable 00000000000016EC case 9
.text:0000000000001744 MOV             W1, #9
.text:0000000000001748 ADD             X0, X0, #aArgc9ArgcD@PAGEOFF ; "argc = 9, argc=%d\n"
.text:0000000000001748
.text:000000000000174C loc_174C                                ; CODE XREF: main+38↑j
.text:000000000000174C
.text:000000000000174C BL              .printf
.text:000000000000174C
.text:0000000000001750
.text:0000000000001750 def_16EC                                ; CODE XREF: main+C↑j
.text:0000000000001750                                         ; main+28↑j
.text:0000000000001750 MOV             W0, WZR                 ; jumptable 00000000000016EC default case, cases 2-4,7
.text:0000000000001754 LDP             X29, X30, [SP+var_s0],#0x10
.text:0000000000001758 RET

.rodata:0000000000000508 jpt_16EC DCB 0                        ; jump table for switch statement
.rodata:0000000000000508                                       ; argc=0;X11=0;X10=0x16F0
.rodata:0000000000000509 DCB 4                                 ; argc=1;X11=4;X10=0x16F0 + 4>>2 =0x1700
.rodata:000000000000050A DCB 0x18                              ; default;X11=0x18;X10=0x16F0 + 0x18>>2 =0x1750
.rodata:000000000000050B DCB 0x18
.rodata:000000000000050C DCB 0x18
.rodata:000000000000050D DCB 8                                 ; argc=5;X11=8;X10=0x16F0 + 8>>2 =0x1710
.rodata:000000000000050E DCB 0xC                               ; argc=6;X11=0xC;X10=0x16F0 + 0xC>>2 =0x1720
.rodata:000000000000050F DCB 0x18
.rodata:0000000000000510 DCB 0x10                              ; argc=8;X11=0x10;X10=0x16F0 + 0x10>>2 =0x1730
.rodata:0000000000000511 DCB 0x14                              ; argc=9;X11=0x14;X10=0x16F0 + 0x14>>2 =0x1740
```

### case 后的值间隔较大时

当 case 后的值间隔不大时，编译器采用二分查找的方式判断 case 值。

例如：

```c
// switch2.c
#include <stdio.h>

int main(int argc, char *argv[])
{
    switch (argc)
    {
    case 0:
        printf("argc = 0, argc=%d\n", argc);
        break;
    case 30:
        printf("argc = 30, argc=%d\n", argc);
        break;
    case 50:
        printf("argc = 50, argc=%d\n", argc);
        break;
    case 80:
        printf("argc = 80, argc=%d\n", argc);
        break;
    case 100:
        printf("argc = 100, argc=%d\n", argc);
        break;
    case 200:
        printf("argc = 200, argc=%d\n", argc);
        break;
    }
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  switch2.c -o switch2
```

IDA 对应的反汇编代码。

```armasm
.text:00001484 PUSH            {R11,LR}
.text:00001488 MOV             R11, SP
.text:0000148C CMP             R0, #79                 ; 先比较一个中间数，类似二分查找
.text:00001490 BGT             loc_14BC
.text:00001490
.text:00001494 CMP             R0, #0
.text:00001498 BEQ             loc_14E4
.text:00001498
.text:0000149C CMP             R0, #30
.text:000014A0 BEQ             loc_14F4
.text:000014A0
.text:000014A4 CMP             R0, #50
.text:000014A8 BNE             loc_1524
.text:000014A8
.text:000014AC LDR             R0, =(aArgc50ArgcD - 0x14BC) ; "argc = 50, argc=%d\n"
.text:000014B0 MOV             R1, #50
.text:000014B4 ADD             R0, PC, R0              ; "argc = 50, argc=%d\n"
.text:000014B8 B               loc_1520
.text:000014B8
.text:000014BC ; ---------------------------------------------------------------------------
.text:000014BC
.text:000014BC loc_14BC                                ; CODE XREF: main+C↑j
.text:000014BC CMP             R0, #80
.text:000014C0 BEQ             loc_1504
.text:000014C0
.text:000014C4 CMP             R0, #100
.text:000014C8 BEQ             loc_1514
.text:000014C8
.text:000014CC CMP             R0, #200
.text:000014D0 BNE             loc_1524
.text:000014D0
.text:000014D4 LDR             R0, =(aArgc200ArgcD - 0x14E4) ; "argc = 200, argc=%d\n"
.text:000014D8 MOV             R1, #200
.text:000014DC ADD             R0, PC, R0              ; "argc = 200, argc=%d\n"
.text:000014E0 B               loc_1520
.text:000014E0
.text:000014E4 ; ---------------------------------------------------------------------------
.text:000014E4
.text:000014E4 loc_14E4                                ; CODE XREF: main+14↑j
.text:000014E4 LDR             R0, =(aArgc0ArgcD - 0x14F4) ; "argc = 0, argc=%d\n"
.text:000014E8 MOV             R1, #0
.text:000014EC ADD             R0, PC, R0              ; "argc = 0, argc=%d\n"
.text:000014F0 B               loc_1520
.text:000014F0
.text:000014F4 ; ---------------------------------------------------------------------------
.text:000014F4
.text:000014F4 loc_14F4                                ; CODE XREF: main+1C↑j
.text:000014F4 LDR             R0, =(aArgc30ArgcD - 0x1504) ; "argc = 30, argc=%d\n"
.text:000014F8 MOV             R1, #30
.text:000014FC ADD             R0, PC, R0              ; "argc = 30, argc=%d\n"
.text:00001500 B               loc_1520
.text:00001500
.text:00001504 ; ---------------------------------------------------------------------------
.text:00001504
.text:00001504 loc_1504                                ; CODE XREF: main+3C↑j
.text:00001504 LDR             R0, =(aArgc80ArgcD - 0x1514) ; "argc = 80, argc=%d\n"
.text:00001508 MOV             R1, #80
.text:0000150C ADD             R0, PC, R0              ; "argc = 80, argc=%d\n"
.text:00001510 B               loc_1520
.text:00001510
.text:00001514 ; ---------------------------------------------------------------------------
.text:00001514
.text:00001514 loc_1514                                ; CODE XREF: main+44↑j
.text:00001514 LDR             R0, =(aArgc100ArgcD - 0x1524) ; "argc = 100, argc=%d\n"
.text:00001518 MOV             R1, #100
.text:0000151C ADD             R0, PC, R0              ; "argc = 100, argc=%d\n"
.text:0000151C
.text:00001520 loc_1520                                ; CODE XREF: main+34↑j
.text:00001520
.text:00001520 BL              printf
.text:00001520
.text:00001524 loc_1524                                ; CODE XREF: main+24↑j
.text:00001524                                         ; main+4C↑j
.text:00001524 MOV             R0, #0
.text:00001528 POP             {R11,LR}
.text:0000152C BX              LR
```

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  switch2.c -o switch264
```

IDA 对应的反汇编代码。

```armasm
.text:00000000000016C4 STP             X29, X30, [SP,#-0x10+var_s0]!
.text:00000000000016C8 MOV             X29, SP
.text:00000000000016CC CMP             W0, #79
.text:00000000000016D0 B.GT            loc_16F8                ; 大于 79 则跳转
.text:00000000000016D0
.text:00000000000016D4 CBZ             W0, loc_1720
.text:00000000000016D4
.text:00000000000016D8 CMP             W0, #30
.text:00000000000016DC B.EQ            loc_1730
.text:00000000000016DC
.text:00000000000016E0 CMP             W0, #0x32 ; '2'
.text:00000000000016E4 B.NE            loc_1760
.text:00000000000016E4
.text:00000000000016E8 ADRP            X0, #aArgc50ArgcD@PAGE  ; "argc = 50, argc=%d\n"
.text:00000000000016EC MOV             W1, #0x32 ; '2'
.text:00000000000016F0 ADD             X0, X0, #aArgc50ArgcD@PAGEOFF ; "argc = 50, argc=%d\n"
.text:00000000000016F4 B               loc_175C
.text:00000000000016F4
.text:00000000000016F8 ; ---------------------------------------------------------------------------
.text:00000000000016F8
.text:00000000000016F8 loc_16F8                                ; CODE XREF: main+C↑j
.text:00000000000016F8 CMP             W0, #80
.text:00000000000016FC B.EQ            loc_1740
.text:00000000000016FC
.text:0000000000001700 CMP             W0, #100
.text:0000000000001704 B.EQ            loc_1750
.text:0000000000001704
.text:0000000000001708 CMP             W0, #200
.text:000000000000170C B.NE            loc_1760
.text:000000000000170C
.text:0000000000001710 ADRP            X0, #aArgc200ArgcD@PAGE ; "argc = 200, argc=%d\n"
.text:0000000000001714 MOV             W1, #200
.text:0000000000001718 ADD             X0, X0, #aArgc200ArgcD@PAGEOFF ; "argc = 200, argc=%d\n"
.text:000000000000171C B               loc_175C
.text:000000000000171C
.text:0000000000001720 ; ---------------------------------------------------------------------------
.text:0000000000001720
.text:0000000000001720 loc_1720                                ; CODE XREF: main+10↑j
.text:0000000000001720 ADRP            X0, #aArgc0ArgcD@PAGE   ; "argc = 0, argc=%d\n"
.text:0000000000001724 MOV             W1, WZR
.text:0000000000001728 ADD             X0, X0, #aArgc0ArgcD@PAGEOFF ; "argc = 0, argc=%d\n"
.text:000000000000172C B               loc_175C
.text:000000000000172C
.text:0000000000001730 ; ---------------------------------------------------------------------------
.text:0000000000001730
.text:0000000000001730 loc_1730                                ; CODE XREF: main+18↑j
.text:0000000000001730 ADRP            X0, #aArgc30ArgcD@PAGE  ; "argc = 30, argc=%d\n"
.text:0000000000001734 MOV             W1, #30
.text:0000000000001738 ADD             X0, X0, #aArgc30ArgcD@PAGEOFF ; "argc = 30, argc=%d\n"
.text:000000000000173C B               loc_175C
.text:000000000000173C
.text:0000000000001740 ; ---------------------------------------------------------------------------
.text:0000000000001740
.text:0000000000001740 loc_1740                                ; CODE XREF: main+38↑j
.text:0000000000001740 ADRP            X0, #aArgc80ArgcD@PAGE  ; "argc = 80, argc=%d\n"
.text:0000000000001744 MOV             W1, #80
.text:0000000000001748 ADD             X0, X0, #aArgc80ArgcD@PAGEOFF ; "argc = 80, argc=%d\n"
.text:000000000000174C B               loc_175C
.text:000000000000174C
.text:0000000000001750 ; ---------------------------------------------------------------------------
.text:0000000000001750
.text:0000000000001750 loc_1750                                ; CODE XREF: main+40↑j
.text:0000000000001750 ADRP            X0, #aArgc100ArgcD@PAGE ; "argc = 100, argc=%d\n"
.text:0000000000001754 MOV             W1, #100
.text:0000000000001758 ADD             X0, X0, #aArgc100ArgcD@PAGEOFF ; "argc = 100, argc=%d\n"
.text:0000000000001758
.text:000000000000175C loc_175C                                ; CODE XREF: main+30↑j
.text:000000000000175C
.text:000000000000175C BL              .printf
.text:000000000000175C
.text:0000000000001760 loc_1760                                ; CODE XREF: main+20↑j
.text:0000000000001760                                         ; main+48↑j
.text:0000000000001760 MOV             W0, WZR
.text:0000000000001764 LDP             X29, X30, [SP+var_s0],#0x10
.text:0000000000001768 RET
```

## do 循环

例如：

```c
// do.c
#include<stdio.h>

int main(int argc, char const *argv[])
{
    int sum = 0;
    int i = 0;
    do{
        sum +=i;
        i++;
        printf("i=%d", i);
    } while (i <= argc);
    printf("sum=%d\n", sum);
    return 0;
}

```

### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  do.c -o do
```

IDA 对应的反汇编代码。

```armasm
.text:00001418 PUSH            {R4-R8,R10,R11,LR}
.text:0000141C ADD             R11, SP, #0x18
.text:00001420 LDR             R8, =(aID - 0x143C)     ; "i=%d"
.text:00001424 BIC             R0, R0, R0,ASR#31
.text:00001428 MOV             R4, #0                  ; sum
.text:0000142C MOV             R6, #0                  ; i
.text:00001430 ADD             R7, R0, #1
.text:00001434 ADD             R8, PC, R8              ; "i=%d"
.text:00001434
.text:00001438
.text:00001438 loc_1438                                ; CODE XREF: main+3C↓j
.text:00001438 ADD             R5, R6, #1
.text:0000143C MOV             R0, R8                  ; format
.text:00001440 MOV             R1, R5
.text:00001444 BL              printf
.text:00001444
.text:00001448 ADD             R4, R6, R4
.text:0000144C CMP             R7, R5
.text:00001450 MOV             R6, R5
.text:00001454 BNE             loc_1438                ; 跳转
.text:00001454
.text:00001458 LDR             R0, =(aSumD - 0x1468)   ; "sum=%d\n"
.text:0000145C MOV             R1, R4
.text:00001460 ADD             R0, PC, R0              ; "sum=%d\n"
.text:00001464 BL              printf
.text:00001464
.text:00001468 MOV             R0, #0
.text:0000146C POP             {R4-R8,R10,R11,LR}
.text:00001470 BX              LR
```

### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  do.c -o do64
```

IDA 对应的反汇编代码。

```armasm
.text:0000000000001664 STP             X29, X30, [SP,#-0x30+var_s0]!
.text:0000000000001668 STP             X22, X21, [SP,#var_s10]
.text:000000000000166C MOV             X29, SP
.text:0000000000001670 STP             X20, X19, [SP,#var_s20]
.text:0000000000001674 CMP             W0, #0
.text:0000000000001678 ADRP            X21, #aID@PAGE          ; "i=%d"
.text:000000000000167C CSEL            W8, W0, WZR, GT
.text:0000000000001680 MOV             W19, WZR                ; sum
.text:0000000000001684 MOV             W20, WZR                ; i
.text:0000000000001688 ADD             W22, W8, #1
.text:000000000000168C ADD             X21, X21, #aID@PAGEOFF  ; "i=%d"
.text:000000000000168C
.text:0000000000001690
.text:0000000000001690 loc_1690                                ; CODE XREF: main+44↓j
.text:0000000000001690 ADD             W19, W20, W19
.text:0000000000001694 ADD             W20, W20, #1
.text:0000000000001698 MOV             X0, X21                 ; format
.text:000000000000169C MOV             W1, W20
.text:00000000000016A0 BL              .printf
.text:00000000000016A0
.text:00000000000016A4 CMP             W22, W20
.text:00000000000016A8 B.NE            loc_1690                ; 跳转
.text:00000000000016A8
.text:00000000000016AC ADRP            X0, #aSumD@PAGE         ; "sum=%d\n"
.text:00000000000016B0 MOV             W1, W19
.text:00000000000016B4 ADD             X0, X0, #aSumD@PAGEOFF  ; "sum=%d\n"
.text:00000000000016B8 BL              .printf
.text:00000000000016B8
.text:00000000000016BC LDP             X20, X19, [SP,#var_s20]
.text:00000000000016C0 MOV             W0, WZR
.text:00000000000016C4 LDP             X22, X21, [SP,#var_s10]
.text:00000000000016C8 LDP             X29, X30, [SP+var_s0],#0x30
.text:00000000000016CC RET
```

## while 循环

例如：

```c
// while.c
#include<stdio.h>

int main(int argc, char const *argv[])
{
    int sum = 0;
    int i = 0;
    while (i <= argc){
        sum +=i;
        i++;
        printf("i=%d", i);
    }
    printf("sum=%d\n", sum);
    return 0;
}
```

### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  while.c -o while
```

IDA 对应的反汇编代码。

```armasm
.text:00001418 PUSH            {R4-R8,R10,R11,LR}
.text:0000141C ADD             R11, SP, #0x18
.text:00001420 CMP             R0, #0                  ; 先比较
.text:00001424 BMI             loc_1460                ; 负则跳转
.text:00001424
.text:00001428 LDR             R8, =(aID - 0x1440)     ; "i=%d"
.text:0000142C ADD             R7, R0, #1
.text:00001430 MOV             R5, #0                  ; i
.text:00001434 MOV             R4, #0                  ; sum
.text:00001438 ADD             R8, PC, R8              ; "i=%d"
.text:00001438
.text:0000143C
.text:0000143C loc_143C                                ; CODE XREF: main+40↓j
.text:0000143C ADD             R6, R5, #1              ; 进入while循环体
.text:00001440 MOV             R0, R8                  ; format
.text:00001444 MOV             R1, R6
.text:00001448 BL              printf
.text:00001448
.text:0000144C ADD             R4, R5, R4
.text:00001450 CMP             R7, R6
.text:00001454 MOV             R5, R6
.text:00001458 BNE             loc_143C                ; 进入while循环体
.text:00001458
.text:0000145C B               loc_1464                ; 第一次比较结果为真
.text:0000145C
.text:00001460 ; ---------------------------------------------------------------------------
.text:00001460
.text:00001460 loc_1460                                ; CODE XREF: main+C↑j
.text:00001460 MOV             R4, #0
.text:00001460
.text:00001464
.text:00001464 loc_1464                                ; CODE XREF: main+44↑j
.text:00001464 LDR             R0, =(aSumD - 0x1474)   ; "sum=%d\n"
.text:00001468 MOV             R1, R4
.text:0000146C ADD             R0, PC, R0              ; "sum=%d\n"
.text:00001470 BL              printf
.text:00001470
.text:00001474 MOV             R0, #0
.text:00001478 POP             {R4-R8,R10,R11,LR}
.text:0000147C BX              LR
```

### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  while.c -o while64
```

IDA 对应的反汇编代码。

```armasm
.text:0000000000001664 STP             X29, X30, [SP,#-0x30+var_s0]!
.text:0000000000001668 STP             X22, X21, [SP,#var_s10]
.text:000000000000166C MOV             X29, SP
.text:0000000000001670 STP             X20, X19, [SP,#var_s20]
.text:0000000000001674 TBNZ            W0, #31, loc_16B4       ; 若W0第31位不为0则跳转
.text:0000000000001674
.text:0000000000001678 SUB             W8, W0, #1
.text:000000000000167C ADRP            X20, #aID@PAGE          ; "i=%d"
.text:0000000000001680 MOV             W19, W0
.text:0000000000001684 MOV             W22, #0xFFFFFFFF
.text:0000000000001688 UMULL           X8, W0, W8
.text:000000000000168C ADD             X20, X20, #aID@PAGEOFF  ; "i=%d"
.text:0000000000001690 LSR             X21, X8, #1             ; 直接使用算法n*(n-1)/2 + n = n*(n+1)/2
.text:0000000000001690
.text:0000000000001694 loc_1694                                ; CODE XREF: main+44↓j
.text:0000000000001694 ADD             W1, W22, #2
.text:0000000000001698 MOV             X0, X20                 ; format
.text:000000000000169C BL              .printf
.text:000000000000169C
.text:00000000000016A0 ADD             W22, W22, #1
.text:00000000000016A4 CMP             W19, W22
.text:00000000000016A8 B.NE            loc_1694                ; 不相等则跳转
.text:00000000000016A8
.text:00000000000016AC ADD             W1, W21, W19
.text:00000000000016B0 B               loc_16B8
.text:00000000000016B0
.text:00000000000016B4 ; ---------------------------------------------------------------------------
.text:00000000000016B4
.text:00000000000016B4 loc_16B4                                ; CODE XREF: main+10↑j
.text:00000000000016B4 MOV             W1, WZR
.text:00000000000016B4
.text:00000000000016B8 loc_16B8                                ; CODE XREF: main+4C↑j
.text:00000000000016B8 ADRL            X0, aSumD               ; "sum=%d\n"
.text:00000000000016C0 BL              .printf
.text:00000000000016C0
.text:00000000000016C4 LDP             X20, X19, [SP,#var_s20]
.text:00000000000016C8 MOV             W0, WZR
.text:00000000000016CC LDP             X22, X21, [SP,#var_s10]
.text:00000000000016D0 LDP             X29, X30, [SP+var_s0],#0x30
.text:00000000000016D4 RET
```

## for 循环

例如：

```c
// for.c
#include<stdio.h>

int main(int argc, char const *argv[])
{
    int sum = 0;
    for (size_t i = 0; i <= argc; i++)
    {
        sum += i;
        printf("i=%d", i);
    }
    printf("sum=%d\n", sum);

    return 0;
}

```

### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -O2  for.c -o for
```

IDA 对应的反汇编代码。

```armasm
.text:00001418 PUSH            {R4-R7,R11,LR}
.text:0000141C ADD             R11, SP, #0x10
.text:00001420 CMP             R0, #0
.text:00001424 BMI             loc_145C                ; 负则跳转
.text:00001424
.text:00001428 LDR             R6, =(aID - 0x1440)     ; "i=%d"
.text:0000142C ADD             R7, R0, #1
.text:00001430 MOV             R5, #0
.text:00001434 MOV             R4, #0
.text:00001438 ADD             R6, PC, R6              ; "i=%d"
.text:00001438
.text:0000143C
.text:0000143C loc_143C                                ; CODE XREF: main+3C↓j
.text:0000143C MOV             R0, R6                  ; format
.text:00001440 MOV             R1, R5
.text:00001444 BL              printf
.text:00001444
.text:00001448 ADD             R4, R5, R4
.text:0000144C ADD             R5, R5, #1
.text:00001450 CMP             R7, R5
.text:00001454 BNE             loc_143C                ; 不相等则跳转
.text:00001454
.text:00001458 B               loc_1460
.text:00001458
.text:0000145C ; ---------------------------------------------------------------------------
.text:0000145C
.text:0000145C loc_145C                                ; CODE XREF: main+C↑j
.text:0000145C MOV             R4, #0
.text:0000145C
.text:00001460
.text:00001460 loc_1460                                ; CODE XREF: main+40↑j
.text:00001460 LDR             R0, =(aSumD - 0x1470)   ; "sum=%d\n"
.text:00001464 MOV             R1, R4
.text:00001468 ADD             R0, PC, R0              ; "sum=%d\n"
.text:0000146C BL              printf
.text:0000146C
.text:00001470 MOV             R0, #0
.text:00001474 POP             {R4-R7,R11,LR}
.text:00001478 BX              LR
```

### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -O2  for.c -o for64
```

IDA 对应的反汇编代码。

```armasm
.text:0000000000001664 STP             X29, X30, [SP,#-0x20+var_20]!
.text:0000000000001668 STR             X23, [SP,#0x20+var_10]
.text:000000000000166C MOV             X29, SP
.text:0000000000001670 STP             X22, X21, [SP,#0x20+var_s0]
.text:0000000000001674 STP             X20, X19, [SP,#0x20+var_s10]
.text:0000000000001678 TBNZ            W0, #0x1F, loc_16BC
.text:0000000000001678
.text:000000000000167C SUB             W8, W0, #1
.text:0000000000001680 ADRP            X21, #aID@PAGE          ; "i=%d"
.text:0000000000001684 MOV             W19, W0
.text:0000000000001688 MOV             W20, WZR
.text:000000000000168C UMULL           X8, W0, W8
.text:0000000000001690 ADD             W23, W0, #1
.text:0000000000001694 ADD             X21, X21, #aID@PAGEOFF  ; "i=%d"
.text:0000000000001698 LSR             X22, X8, #1
.text:0000000000001698
.text:000000000000169C
.text:000000000000169C loc_169C                                ; CODE XREF: main+4C↓j
.text:000000000000169C MOV             X0, X21                 ; format
.text:00000000000016A0 MOV             W1, W20
.text:00000000000016A4 BL              .printf
.text:00000000000016A4
.text:00000000000016A8 ADD             W20, W20, #1
.text:00000000000016AC CMP             W23, W20
.text:00000000000016B0 B.NE            loc_169C
.text:00000000000016B0
.text:00000000000016B4 ADD             W1, W22, W19
.text:00000000000016B8 B               loc_16C0
.text:00000000000016B8
.text:00000000000016BC ; ---------------------------------------------------------------------------
.text:00000000000016BC
.text:00000000000016BC loc_16BC                                ; CODE XREF: main+14↑j
.text:00000000000016BC MOV             W1, WZR
.text:00000000000016BC
.text:00000000000016C0
.text:00000000000016C0 loc_16C0                                ; CODE XREF: main+54↑j
.text:00000000000016C0 ADRL            X0, aSumD               ; "sum=%d\n"
.text:00000000000016C8 BL              .printf
.text:00000000000016C8
.text:00000000000016CC LDP             X20, X19, [SP,#0x20+var_s10]
.text:00000000000016D0 MOV             W0, WZR
.text:00000000000016D4 LDP             X22, X21, [SP,#0x20+var_s0]
.text:00000000000016D8 LDR             X23, [SP,#0x20+var_10]
.text:00000000000016DC LDP             X29, X30, [SP+0x20+var_20],#0x40
.text:00000000000016E0 RET
```

可以发现 for 循环和 while 循环的反汇编代码基本一致。
