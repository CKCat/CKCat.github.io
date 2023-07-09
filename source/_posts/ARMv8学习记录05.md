---
title: ARMv8学习记录05
date: 2021-04-10 13:23:52
tags: 汇编
category: ARMv8汇编
---

# 运算符

# 条件执行
## if...else if...else
c语言代码：
```cpp
if(argc > 1){
    printf("argc > 1");
}else if(argc == 1){
    printf("argc == 1");
}else{
    printf("argc < 1");
}
```
对应的ARMv7代码：
```armasm
                CMP             R4, #2
                BLT             loc_5C0
                LDR             R0, =(aArgc1 - 0x5C0)
                ADD             R0, PC  ; "argc > 1"
                B               loc_5CE
; ---------------------------------------------------------------------------

loc_5C0                                 ; CODE XREF: main+4C↑j
                CMP             R4, #1
                BNE             loc_5CA
                LDR             R0, =(aArgc1_0 - 0x5CA)
                ADD             R0, PC  ; "argc == 1"
                B               loc_5CE
; ---------------------------------------------------------------------------

loc_5CA                                 ; CODE XREF: main+56↑j
                LDR             R0, =(aArgc1_1 - 0x5D0)
                ADD             R0, PC  ; "argc < 1"

loc_5CE                                 ; CODE XREF: main+52↑j
                                        ; main+5C↑j
                BLX             printf
                MOVS            R0, #0
                POP             {R4,R5,R7,PC}
```
在IDA中对应的结构：

![](2021-04-18-17-51-04.png)

## switch...case
### case后的值连续或间隔不大
C语言代码
```cpp
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
对应的ARMv7代码：
```armasm
                PUSH            {R7,LR}
                MOV             R7, SP
                CMP             R0, #9  ; switch 10 cases
                BHI             def_574 ; jumptable 00000574 default case
                TBB.W           [PC,R0] ; switch jump
; ---------------------------------------------------------------------------
;使用做表的方式定位case块
jpt_574         DCB 5                   ; jump table for switch statement
                DCB 8
                DCB 0x18
                DCB 0x18
                DCB 0x18
                DCB 0xB
                DCB 0xE
                DCB 0x18
                DCB 0x11
                DCB 0x14
; ---------------------------------------------------------------------------

loc_582                                 ; CODE XREF: main+8↑j
                LDR             R0, =(aArgc0 - 0x588) ; jumptable 00000574 case 0
                ADD             R0, PC  ; "argc = 0"
                B               loc_5A4
; ---------------------------------------------------------------------------

loc_588                                 ; CODE XREF: main+8↑j
                LDR             R0, =(aArgc1 - 0x58E) ; jumptable 00000574 case 1
                ADD             R0, PC  ; "argc = 1"
                B               loc_5A4
; ---------------------------------------------------------------------------

loc_58E                                 ; CODE XREF: main+8↑j
                LDR             R0, =(aArgc5 - 0x594) ; jumptable 00000574 case 5
                ADD             R0, PC  ; "argc = 5"
                B               loc_5A4
; ---------------------------------------------------------------------------

loc_594                                 ; CODE XREF: main+8↑j
                LDR             R0, =(aArgc6 - 0x59A) ; jumptable 00000574 case 6
                ADD             R0, PC  ; "argc = 6"
                B               loc_5A4
; ---------------------------------------------------------------------------

loc_59A                                 ; CODE XREF: main+8↑j
                LDR             R0, =(aArgc8 - 0x5A0) ; jumptable 00000574 case 8
                ADD             R0, PC  ; "argc = 8"
                B               loc_5A4
; ---------------------------------------------------------------------------

loc_5A0                                 ; CODE XREF: main+8↑j
                LDR             R0, =(aArgc9 - 0x5A6) ; jumptable 00000574 case 9
                ADD             R0, PC  ; "argc = 9"

loc_5A4                                 ; CODE XREF: main+1A↑j
                                        ; main+20↑j ...
                BLX             printf

def_574                                 ; CODE XREF: main+6↑j
                                        ; main+8↑j
                MOVS            R0, #0  ; jumptable 00000574 default case
                POP             {R7,PC}
; End of function main

; ---------------------------------------------------------------------------
off_5AC         DCD aArgc9 - 0x5A6      ; DATA XREF: main:loc_5A0↑r
                                        ; "argc = 9"
off_5B0         DCD aArgc8 - 0x5A0      ; DATA XREF: main:loc_59A↑r
                                        ; "argc = 8"
off_5B4         DCD aArgc6 - 0x59A      ; DATA XREF: main:loc_594↑r
                                        ; "argc = 6"
off_5B8         DCD aArgc5 - 0x594      ; DATA XREF: main:loc_58E↑r
                                        ; "argc = 5"
off_5BC         DCD aArgc1 - 0x58E      ; DATA XREF: main:loc_588↑r
                                        ; "argc = 1"
off_5C0         DCD aArgc0 - 0x588      ; DATA XREF: main:loc_582↑r
; } // starts at 56C                    ; "argc = 0"
``` 
在IDA中对应的结构：

![](2021-04-18-17-52-51.png)

### case后的值间隔较大时
C语言代码：
```cpp
#include<stdio.h>

int main(int argc,char* argv[]){

	switch(argc){
	case 0:
		printf("argc = 0");
		break;
	case 30:
		printf("argc = 30");
		break;
	case 50:
		printf("argc = 50");
		break;
	case 80:
		printf("argc = 80");
		break;
	case 100:
		printf("argc = 100");
		break;
	case 200:
		printf("argc = 200");
		break;
	}
	return 0;
}
```
对应的ARMv7代码：
```armasm
                PUSH            {R7,LR}
                MOV             R7, SP
                CMP             R0, #79
                BGT             loc_584
                CBZ             R0, loc_596
                CMP             R0, #0x1E
                BEQ             loc_59C
                CMP             R0, #50
                BNE             loc_5B0
                LDR             R0, =(aArgc50 - 0x584)
                ADD             R0, PC  ; "argc = 50"
                B               loc_5AC
; ---------------------------------------------------------------------------

loc_584                                 ; CODE XREF: main+6↑j
                CMP             R0, #80
                BEQ             loc_5A2
                CMP             R0, #100
                BEQ             loc_5A8
                CMP             R0, #200
                BNE             loc_5B0
                LDR             R0, =(aArgc200 - 0x596)
                ADD             R0, PC  ; "argc = 200"
                B               loc_5AC
; ---------------------------------------------------------------------------

loc_596                                 ; CODE XREF: main+8↑j
                LDR             R0, =(aArgc0 - 0x59C)
                ADD             R0, PC  ; "argc = 0"
                B               loc_5AC
; ---------------------------------------------------------------------------

loc_59C                                 ; CODE XREF: main+C↑j
                LDR             R0, =(aArgc30 - 0x5A2)
                ADD             R0, PC  ; "argc = 30"
                B               loc_5AC
; ---------------------------------------------------------------------------

loc_5A2                                 ; CODE XREF: main+1A↑j
                LDR             R0, =(aArgc80 - 0x5A8)
                ADD             R0, PC  ; "argc = 80"
                B               loc_5AC
; ---------------------------------------------------------------------------

loc_5A8                                 ; CODE XREF: main+1E↑j
                LDR             R0, =(aArgc100 - 0x5AE)
                ADD             R0, PC  ; "argc = 100"

loc_5AC                                 ; CODE XREF: main+16↑j
                                        ; main+28↑j ...
                BLX             printf

loc_5B0                                 ; CODE XREF: main+10↑j
                                        ; main+22↑j
                MOVS            R0, #0
                POP             {R7,PC}
```
IDA中对应的结构：

![](2021-04-18-17-53-22.png)

## 循环

## 结构体


## 类

# ARMv7 异常




# 基本数据类型
使用下列命令，生产 ARMv8 汇编文件。
```bash
➜ clang -target aarch64-linux-android21 -S basic.c -o basic64.s
➜ clang -target arm-linux-android21 basic64.s  -o basic64
```

使用 IDA 查看对应的汇编代码：
```armasm
.text:00000000000005D8                 SUB             SP, SP, #0x70
.text:00000000000005DC                 MOV             W8, #0
.text:00000000000005E0                 MOV             W9, #0x40F40000
.text:00000000000005E4                 FMOV            S0, W9  ; S0 =  7.725f，ARMv8 中直接使用浮点寄存器存储浮点数。
.text:00000000000005E8                 FMOV            S1, #0.625
.text:00000000000005EC                 MOV             W9, #0xC0F40000
.text:00000000000005F0                 FMOV            S2, W9
.text:00000000000005F4                 MOV             X10, #0x401E800000000000
.text:00000000000005FC                 FMOV            D3, X10
.text:0000000000000600                 FMOV            D4, #0.625
.text:0000000000000604                 MOV             X10, #0xC01E800000000000
.text:000000000000060C                 FMOV            D5, X10
.text:0000000000000610                 MOV             W9, #0x61 ; 'a'
.text:0000000000000614                 MOV             W11, #0x109
.text:0000000000000618                 MOV             W12, #0x7B ; '{'
.text:000000000000061C                 MOV             X10, #0x1C8
.text:0000000000000620                 MOV             X13, #0x315
.text:0000000000000624                 MOV             W14, #0xFEF7
.text:0000000000000628                 MOV             W15, #0xFFFFFF85
.text:000000000000062C                 MOV             X16, #0xFFFFFFFFFFFFFE38
.text:0000000000000630                 MOV             X17, #0xFFFFFFFFFFFFFCEB
.text:0000000000000634                 STR             WZR, [SP,#0x70+var_4]
.text:0000000000000638                 STR             W0, [SP,#0x70+var_8]
.text:000000000000063C                 STR             X1, [SP,#0x70+var_10]
.text:0000000000000640                 STR             S0, [SP,#0x70+var_14]
.text:0000000000000644                 STR             S1, [SP,#0x70+var_18]
.text:0000000000000648                 STR             S2, [SP,#0x70+var_1C]
.text:000000000000064C                 STR             D3, [SP,#0x70+var_28]
.text:0000000000000650                 STR             D4, [SP,#0x70+var_30]
.text:0000000000000654                 STR             D5, [SP,#0x70+var_38]
.text:0000000000000658                 STRB            W9, [SP,#0x70+var_39]
.text:000000000000065C                 STRH            W11, [SP,#0x70+var_3C]
.text:0000000000000660                 STR             W12, [SP,#0x70+var_40]
.text:0000000000000664                 STR             X10, [SP,#0x70+var_48]
.text:0000000000000668                 STR             X13, [SP,#0x70+var_50]
.text:000000000000066C                 STRH            W14, [SP,#0x70+var_52]
.text:0000000000000670                 STR             W15, [SP,#0x70+var_58]
.text:0000000000000674                 STR             X16, [SP,#0x70+var_60]
.text:0000000000000678                 STR             X17, [SP,#0x70+var_68]
.text:000000000000067C                 MOV             W0, W8
.text:0000000000000680                 ADD             SP, SP, #0x70 ; 'p'
.text:0000000000000684                 RET
```