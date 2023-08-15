---
title: ARMv8学习记录08
date: 2023-07-04 13:19:32
tags: C++逆向
category: ARMv8汇编
---

# 结构体和类

在 C++中，结构体和类都具有构造函数、析构函数和成员函数，两者只有一个区别：结构体的访问控制默认为 public，而类的默认访问控制是 private。

空类：空类中没有任何数据成员，其长度为 1 字节。

内存对齐：一般情况下，内存对齐的规则是结构体大小可以整除数据成员类型最大值，当类型的最大值大于 8 字节时，对齐值为 8 字节。可以使用预编译指令 `#pragma pack(N)` 调整对齐大小。

静态数据成员：虽然静态数据成员是在类中被定义的，但它与局部静态变量类似，存放的位置和全局变量一致。

例子：

```cpp
#include <cstdio>

struct st_null{
};

struct st_size{
    int a;
    char b;
    double d;
};

#pragma pack(1)
struct st_pack{
    int a;
    char b;
    double d;
};

struct st_static{
    static int a;
    double d;
};

int main(int argc, char const *argv[])
{
    printf("sizeof(st_null) = %d\n", sizeof(st_null)); // 1
    printf("sizeof(st_size) = %d\n", sizeof(st_size)); // 16
    printf("sizeof(st_pack) = %d\n", sizeof(st_pack)); // 13
    printf("sizeof(st_static) = %d\n", sizeof(st_static)); // 8
    return 0;
}
```

运行结果如下：

```bash
sizeof(st_null) = 1
sizeof(st_size) = 16
sizeof(st_pack) = 13
sizeof(st_static) = 8
```

## this 指针

this 指针应属于指针类型。this 指针在 32 位环境下占 4 字节空间，在 64 位环境下占 8 字节空间，保存了所属对象的首地址。

例子:

```c
// this.cpp
#include <cstdio>

class Point{
public:
    double cx;
    double cy;
public:
    Point(double cx, double cy){
        this->cx = cx;
        this->cy = cy;
    }

    void setx(double cx){
        this->cx = cx;
    }

    void sety(double cy){
        this->cy = cy;
    }
};

int main(int argc, char const *argv[])
{
    Point point(1.1, 2.2);
    printf("cx = %f, cy = %f\n", point.cx, point.cy);
    point.setx(3.14);
    point.sety(6.18);
    printf("cx = %f, cy = %f\n", point.cx, point.cy);
    return 0;
}
```

### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21  this.cpp -o this
```

IDA 对应的反汇编代码。

```armasm
.text:0000142C PUSH            {R11,LR}
.text:00001430 MOV             R11, SP
.text:00001434 SUB             SP, SP, #0x38
.text:00001438 MOV             R2, #0
.text:0000143C STR             R2, [SP,#0x38+var_24]
.text:00001440 STR             R2, [R11,#var_4]
.text:00001444 STR             R0, [R11,#var_8]
.text:00001448 STR             R1, [R11,#var_C]
.text:0000144C MOV             R0, SP
.text:00001450 LDR             R1, =0x40019999
.text:00001454 STR             R1, [R0,#0x38+var_38+4]
.text:00001458 LDR             R2, =0x9999999A         ; double
.text:0000145C STR             R2, [R0,#0x38+var_38]   ; double
.text:00001460 ADD             R0, SP, #0x38+var_20    ; this
.text:00001464 STR             R0, [SP,#0x38+var_2C]
.text:00001468 LDR             R3, =0x3FF19999         ; 1.1 = 3FF199999999999A
.text:0000146C BL              _ZN5PointC2Edd          ; Point::Point(double,double)
.text:0000146C
.text:00001470 LDR             R2, [SP,#0x38+var_20]   ; this:cx
.text:00001474 LDR             R3, [SP,#0x38+var_1C]
.text:00001478 LDR             R0, [SP,#0x38+var_18]   ; cy
.text:0000147C LDR             R12, [SP,#0x38+var_14]
.text:00001480 MOV             R1, SP
.text:00001484 STR             R12, [R1,#0x38+var_38+4]
.text:00001488 STR             R0, [R1,#0x38+var_38]
.text:0000148C LDR             R0, =(aCxFCyF - 0x1498) ; "cx = %f, cy = %f\n"
.text:00001490 ADD             R0, PC, R0              ; "cx = %f, cy = %f\n"
.text:00001494 STR             R0, [SP,#0x38+format]
.text:00001498 BL              printf
.text:00001498
.text:0000149C LDR             R0, [SP,#0x38+var_2C]   ; this
.text:000014A0 LDR             R2, =0x51EB851F         ; double
.text:000014A4 LDR             R3, =0x40091EB8
.text:000014A8 BL              _ZN5Point4setxEd        ; Point::setx(double)
.text:000014A8
.text:000014AC LDR             R0, [SP,#0x38+var_2C]   ; this
.text:000014B0 LDR             R2, =0xEB851EB8         ; double
.text:000014B4 LDR             R3, =0x4018B851
.text:000014B8 BL              _ZN5Point4setyEd        ; Point::sety(double)
.text:000014B8
.text:000014BC LDR             R0, [SP,#0x38+format]   ; format
.text:000014C0 LDR             R2, [SP,#0x38+var_20]
.text:000014C4 LDR             R3, [SP,#0x38+var_1C]
.text:000014C8 LDR             R1, [SP,#0x38+var_18]
.text:000014CC LDR             LR, [SP,#0x38+var_14]
.text:000014D0 MOV             R12, SP
.text:000014D4 STR             LR, [R12,#0x38+var_38+4]
.text:000014D8 STR             R1, [R12,#0x38+var_38]
.text:000014DC BL              printf
.text:000014DC
.text:000014E0 LDR             R0, [SP,#0x38+var_24]
.text:000014E4 MOV             SP, R11
.text:000014E8 POP             {R11,LR}
.text:000014EC BX              LR


.text:0000156C _ZN5Point4setxEd                        ; CODE XREF: main+7C↑p
.text:0000156C
.text:0000156C SUB             SP, SP, #0x10
.text:00001570 STR             R0, [SP,#0x10+var_4]    ; 保存this指针
.text:00001574 STR             R3, [SP,#0x10+var_C]
.text:00001578 STR             R2, [SP,#0x10+var_10]   ; 保存参数R3:R2
.text:0000157C LDR             R1, [SP,#0x10+var_4]
.text:00001580 LDR             R0, [SP,#0x10+var_10]
.text:00001584 LDR             R2, [SP,#0x10+var_C]
.text:00001588 STR             R2, [R1,#4]             ; 赋值
.text:0000158C STR             R0, [R1]                ; this指针对应的位置为cx
.text:00001590 ADD             SP, SP, #0x10
.text:00001594 BX              LR
```

可以发现当对象调用其成员函数时，会将 this 指针作为参数传递给 R0。

### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 this.cpp -o this64
```

IDA 对应的反汇编代码。

```armasm
.text:00000000000016DC SUB             SP, SP, #0x50
.text:00000000000016E0 STP             X29, X30, [SP,#0x40+var_s0]
.text:00000000000016E4 ADD             X29, SP, #0x40
.text:00000000000016E8 MOV             W8, WZR
.text:00000000000016EC STR             W8, [SP,#0x40+var_24]
.text:00000000000016F0 STUR            WZR, [X29,#var_4]
.text:00000000000016F4 STUR            W0, [X29,#var_8]
.text:00000000000016F8 STUR            X1, [X29,#var_10]
.text:00000000000016FC ADD             X0, SP, #0x40+var_20    ; this
.text:0000000000001700 STR             X0, [SP,#0x40+var_38]   ; 使用X0保存this指针
.text:0000000000001704 ADRP            X8, #qword_518@PAGE
.text:0000000000001708 LDR             D0, [X8,#qword_518@PAGEOFF] ; double
.text:000000000000170C ADRP            X8, #qword_508@PAGE
.text:0000000000001710 LDR             D1, [X8,#qword_508@PAGEOFF] ; double
.text:0000000000001714 BL              _ZN5PointC2Edd          ; Point::Point(double,double)
.text:0000000000001714
.text:0000000000001718 LDR             D0, [SP,#0x40+var_20]
.text:000000000000171C LDR             D1, [SP,#0x40+var_18]
.text:0000000000001720 ADRL            X0, aCxFCyF             ; "cx = %f, cy = %f\n"
.text:0000000000001728 STR             X0, [SP,#0x40+format]
.text:000000000000172C BL              .printf
.text:000000000000172C
.text:0000000000001730 LDR             X0, [SP,#0x40+var_38]   ; this
.text:0000000000001734 ADRP            X8, #qword_520@PAGE
.text:0000000000001738 LDR             D0, [X8,#qword_520@PAGEOFF] ; double
.text:000000000000173C BL              _ZN5Point4setxEd        ; Point::setx(double)
.text:000000000000173C
.text:0000000000001740 LDR             X0, [SP,#0x40+var_38]   ; this
.text:0000000000001744 ADRP            X8, #qword_510@PAGE
.text:0000000000001748 LDR             D0, [X8,#qword_510@PAGEOFF] ; double
.text:000000000000174C BL              _ZN5Point4setyEd        ; Point::sety(double)
.text:000000000000174C
.text:0000000000001750 LDR             X0, [SP,#0x40+format]   ; format
.text:0000000000001754 LDR             D0, [SP,#0x40+var_20]
.text:0000000000001758 LDR             D1, [SP,#0x40+var_18]
.text:000000000000175C BL              .printf
.text:000000000000175C
.text:0000000000001760 LDR             W0, [SP,#0x40+var_24]
.text:0000000000001764 LDP             X29, X30, [SP,#0x40+var_s0]
.text:0000000000001768 ADD             SP, SP, #0x50 ; 'P'
.text:000000000000176C RET


.text:000000000000179C _ZN5Point4setxEd                        ; CODE XREF: main+60↑p
.text:000000000000179C
.text:000000000000179C ; __unwind {
.text:000000000000179C SUB             SP, SP, #0x10
.text:00000000000017A0 STR             X0, [SP,#0x10+var_8]
.text:00000000000017A4 STR             D0, [SP,#0x10+var_10]
.text:00000000000017A8 LDR             X9, [SP,#0x10+var_8]
.text:00000000000017AC LDR             X8, [SP,#0x10+var_10]
.text:00000000000017B0 STR             X8, [X9]                ; this指针偏移0处为cx
.text:00000000000017B4 ADD             SP, SP, #0x10
.text:00000000000017B8 RET
```

## 静态数据成员

静态数据成员和静态变量原理相同（都是含有作用域的特殊全局变量），所以该静态数据成员的初值会被写入编译链接后的执行文件。。静态数据成员仅仅和类相关，和对象无关，不参与对象的长度计算，多个对象可以拥有同一个静态数据成员。静态数据成员是常量地址，可通过立即数间接寻址的方式访问。

例子：

```c
// static.cpp
#include <cstdio>

class Point{
public:
    static int count;
    double cx;
    double cy;
public:
    Point(double cx, double cy){
        this->cx = cx;
        this->cy = cy;
    }
    void show(){
        printf("cx=%lf, cy=%lf, count=%d\n", this->cx, this->cy, count);
    }
};

int Point::count = 1;

int main(int argc, char const *argv[])
{
    Point point(1.1, 2.2);
    point.show();
    return 0;
}
```

### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21  static.cpp -o static
```

show 成员函数在 IDA 中对应的反汇编代码。

```armasm
.text:00001500 PUSH            {R11,LR}
.text:00001504 MOV             R11, SP
.text:00001508 SUB             SP, SP, #0x10
.text:0000150C STR             R0, [R11,#var_4]
.text:00001510 LDR             R1, [R11,#var_4]
.text:00001514 LDR             LR, =(_ZN5Point5countE - 0x1520) ; Point::count
.text:00001518 LDR             LR, [PC,LR]             ; Point::count
.text:0000151C LDR             R2, [R1]
.text:00001520 LDR             R3, [R1,#4]
.text:00001524 LDR             R0, [R1,#8]
.text:00001528 LDR             R12, [R1,#0xC]
.text:0000152C MOV             R1, SP
.text:00001530 STR             LR, [R1,#0x10+var_8]    ; Point::count初始值编译到文件中了
.text:00001534 STR             R12, [R1,#0x10+var_C]
.text:00001538 STR             R0, [R1,#0x10+var_10]
.text:0000153C LDR             R0, =(aCxLfCyLfCountD - 0x1548) ; "cx=%lf, cy=%lf, count=%d\n"
.text:00001540 ADD             R0, PC, R0              ; "cx=%lf, cy=%lf, count=%d\n"
.text:00001544 BL              printf
.text:00001544
.text:00001548 MOV             SP, R11
.text:0000154C POP             {R11,LR}
.text:00001550 BX              LR
```

### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 static.cpp -o static64
```

show 成员函数在 IDA 中对应的反汇编代码。

```armasm
.text:000000000000173C SUB             SP, SP, #0x20
.text:0000000000001740 STP             X29, X30, [SP,#0x10+var_s0]
.text:0000000000001744 ADD             X29, SP, #0x10
.text:0000000000001748 STR             X0, [SP,#0x10+var_8]    ; this
.text:000000000000174C LDR             X8, [SP,#0x10+var_8]
.text:0000000000001750 LDR             D0, [X8]
.text:0000000000001754 LDR             D1, [X8,#8]
.text:0000000000001758 ADRP            X8, #_ZN5Point5countE@PAGE ; Point::count
.text:000000000000175C LDR             W1, [X8,#_ZN5Point5countE@PAGEOFF] ; Point::count
.text:0000000000001760 ADRL            X0, aCxLfCyLfCountD     ; "cx=%lf, cy=%lf, count=%d\n"
.text:0000000000001768 BL              .printf
.text:0000000000001768
.text:000000000000176C LDP             X29, X30, [SP,#0x10+var_s0]
.text:0000000000001770 ADD             SP, SP, #0x20 ; ' '
.text:0000000000001774 RET
```

## 对象作为函数参数
对象作为函数参数时和结构体作为函数参数时一致，当对象的大小不大于32字节时，使用 R0-R3传递，当对象的大小大于32字节时，使用R0-R3和栈传递。这里使用的编译器并没有自动生成构造函数和析构函数。

例子：
```c
#include <cstdio>
#include <cstring>

class Point{
public:
    static int count;
    double cx;
    double cy;
public:
    Point(double cx, double cy){
        this->cx = cx;
        this->cy = cy;
    }
};

void show(Point point){
    printf("cx=%lf, cy=%lf,\n", point.cx, point.cy);
}

class Person{
public:
    int age;
    int height;
    char name[32];    
};

void show(Person person){
    printf("age=%d, height=%d, name=%s\n", person.age, person.height, person.name); 
}

int main(int argc, char const *argv[])
{
    Point point(1.1, 2.2);
    show(point);

    Person person;
    person.age =18;
    person.height = 180;
    strcpy(person.name, "zhangsan");
    show(person);
    return 0;
}
```
### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21  params.cpp -o params
```

在 IDA 中对应的反汇编代码。

```armasm
.text:00001570 PUSH            {R4-R8,R10,R11,LR}
.text:00001574 ADD             R11, SP, #0x18
.text:00001578 SUB             SP, SP, #0xA0
.text:0000157C MOV             R2, #0
.text:00001580 STR             R2, [SP,#0xB8+var_9C]
.text:00001584 STR             R2, [R11,#var_1C]
.text:00001588 STR             R0, [R11,#var_20]
.text:0000158C STR             R1, [R11,#var_24]
.text:00001590 MOV             R0, SP
.text:00001594 LDR             R1, =0x40019999
.text:00001598 STR             R1, [R0,#0xB8+var_B8+4]
.text:0000159C LDR             R2, =0x9999999A         ; double
.text:000015A0 STR             R2, [R0,#0xB8+var_B8]   ; double
.text:000015A4 SUB             R0, R11, #-var_38       ; this
.text:000015A8 LDR             R3, =0x3FF19999
.text:000015AC BL              _ZN5PointC2Edd          ; Point::Point(double,double)
.text:000015AC
.text:000015B0 LDR             R0, [R11,#var_38]
.text:000015B4 LDR             R1, [R11,#var_34]
.text:000015B8 LDR             R2, [R11,#var_30]
.text:000015BC LDR             R3, [R11,#var_2C]
.text:000015C0 STR             R3, [R11,#var_3C]
.text:000015C4 STR             R2, [R11,#var_40]
.text:000015C8 STR             R1, [R11,#var_44]
.text:000015CC STR             R0, [R11,#var_48]
.text:000015D0 LDR             R0, [R11,#var_48]
.text:000015D4 LDR             R1, [R11,#var_44]
.text:000015D8 LDR             R2, [R11,#var_40]
.text:000015DC LDR             R3, [R11,#var_3C]       ; 和结构体一样，直接使用R0-R3传递参数
.text:000015E0 BL              _Z4show5Point           ; show(Point)
.text:000015E0
.text:000015E4 MOV             R0, #18
.text:000015E8 STR             R0, [SP,#0xB8+var_70]
.text:000015EC MOV             R0, #180
.text:000015F0 STR             R0, [SP,#0xB8+var_6C]
.text:000015F4 ADD             R0, SP, #0xB8+var_70
.text:000015F8 STR             R0, [SP,#0xB8+var_A0]
.text:000015FC ADD             R0, R0, #8              ; dest
.text:00001600 LDR             R1, =(aZhangsan - 0x160C) ; "zhangsan"
.text:00001604 ADD             R1, PC, R1              ; "zhangsan"
.text:00001608 BL              strcpy
.text:00001608
.text:0000160C LDR             R1, [SP,#0xB8+var_A0]   ; 对象首地址
.text:00001610 ADD             R0, SP, #0xB8+var_98    ; 将对象的内存复制到var_98
.text:00001614 LDM             R1!, {R2-R4,R12,LR}
.text:00001618 STM             R0!, {R2-R4,R12,LR}
.text:0000161C LDM             R1, {R2-R4,R12,LR}
.text:00001620 STM             R0, {R2-R4,R12,LR}
.text:00001624 LDR             R0, [SP,#0xB8+var_98]
.text:00001628 LDR             R1, [SP,#0xB8+var_94]
.text:0000162C LDR             R2, [SP,#0xB8+var_90]
.text:00001630 LDR             R3, [SP,#0xB8+var_8C]   ; 前4个参数使用R0-R3传递
.text:00001634 LDR             R12, [SP,#0xB8+var_88]
.text:00001638 LDR             R4, [SP,#0xB8+var_84]
.text:0000163C LDR             R5, [SP,#0xB8+var_80]
.text:00001640 LDR             R6, [SP,#0xB8+var_7C]
.text:00001644 LDR             R7, [SP,#0xB8+var_78]
.text:00001648 LDR             R8, [SP,#0xB8+var_74]
.text:0000164C MOV             LR, SP
.text:00001650 STR             R8, [LR,#0xB8+var_A4]   ; 后6个参数使用栈传递
.text:00001654 STR             R7, [LR,#0xB8+var_A8]
.text:00001658 STR             R6, [LR,#0xB8+var_AC]
.text:0000165C STR             R5, [LR,#0xB8+var_B0]
.text:00001660 STR             R4, [LR,#0xB8+var_B8+4]
.text:00001664 STR             R12, [LR,#0xB8+var_B8]
.text:00001668 BL              _Z4show6Person          ; show(Person)
.text:00001668
.text:0000166C LDR             R0, [SP,#0xB8+var_9C]
.text:00001670 SUB             SP, R11, #0x18
.text:00001674 POP             {R4-R8,R10,R11,LR}
.text:00001678 BX              LR
```

### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 params.cpp -o params64
```

在 IDA 中对应的反汇编代码。
```armasm
.text:00000000000017E8 SUB             SP, SP, #0xB0
.text:00000000000017EC STP             X29, X30, [SP,#0xA0+var_s0]
.text:00000000000017F0 ADD             X29, SP, #0xA0
.text:00000000000017F4 MOV             W8, WZR
.text:00000000000017F8 STR             W8, [SP,#0xA0+var_84]
.text:00000000000017FC STUR            WZR, [X29,#var_4]
.text:0000000000001800 STUR            W0, [X29,#var_8]
.text:0000000000001804 STUR            X1, [X29,#var_10]
.text:0000000000001808 SUB             X0, X29, #-var_20       ; this
.text:000000000000180C ADRP            X8, #qword_598@PAGE
.text:0000000000001810 LDR             D0, [X8,#qword_598@PAGEOFF] ; double
.text:0000000000001814 ADRP            X8, #qword_590@PAGE
.text:0000000000001818 LDR             D1, [X8,#qword_590@PAGEOFF] ; double
.text:000000000000181C BL              _ZN5PointC2Edd          ; Point::Point(double,double)
.text:000000000000181C
.text:0000000000001820 LDUR            Q0, [X29,#var_20]
.text:0000000000001824 STUR            Q0, [X29,#var_30]
.text:0000000000001828 LDUR            D0, [X29,#var_30]
.text:000000000000182C LDUR            D1, [X29,#var_30+8]     ; 直接使用寄存器传递
.text:0000000000001830 BL              _Z4show5Point           ; show(Point)
.text:0000000000001830
.text:0000000000001834 ADD             X8, SP, #0xA0+var_58
.text:0000000000001838 STR             X8, [SP,#0xA0+src]
.text:000000000000183C MOV             W9, #18
.text:0000000000001840 STR             W9, [SP,#0xA0+var_58]
.text:0000000000001844 MOV             W9, #180
.text:0000000000001848 STR             W9, [SP,#0xA0+var_54]
.text:000000000000184C ADD             X0, X8, #8              ; dest
.text:0000000000001850 ADRL            X1, aZhangsan           ; "zhangsan"
.text:0000000000001858 BL              .strcpy
.text:0000000000001858
.text:000000000000185C LDR             X1, [SP,#0xA0+src]      ; src
.text:0000000000001860 ADD             X0, SP, #0xA0+dest      ; dest
.text:0000000000001864 STR             X0, [SP,#0xA0+var_90]
.text:0000000000001868 MOV             X2, #0x28 ; '('         ; n
.text:000000000000186C BL              .memcpy
.text:000000000000186C
.text:0000000000001870 LDR             X0, [SP,#0xA0+var_90]   ; 直接整体复制到内存中，传递内存的地址
.text:0000000000001874 BL              _Z4show6Person          ; show(Person)
.text:0000000000001874
.text:0000000000001878 LDR             W0, [SP,#0xA0+var_84]
.text:000000000000187C LDP             X29, X30, [SP,#0xA0+var_s0]
.text:0000000000001880 ADD             SP, SP, #0xB0
.text:0000000000001884 RET
```


## 对象作为返回值
例子：
```c
#include <cstdio>
#include <cstring>

class Point{
public:
    static int count;
    double cx;
    double cy;
};

Point createPoint(double x, double y){
    Point point;
    point.cx = x;
    point.cy = y;
    return point;
}

class Person{
public:
    int age;
    int height;
    char name[32];    
};

Person createPerson(int age, int height, const char *name){
    Person person;
    person.age =age;
    person.height = height;
    strcpy(person.name, name);
    return person;
}

int main(int argc, char const *argv[])
{
    Point point = createPoint(1.1, 2.2);
    printf("cx = %f, cy = %f\n", point.cx, point.cy);
    Person person = createPerson(18, 180, "zhangsan");
    printf("age=%d, height=%d, name=%s\n", person.age, person.height, person.name);
    
    return 0;
}
```
### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21  return.cpp -o return
```

在 IDA 中对应的反汇编代码。
```armasm
.text:AE662520 PUSH            {R11,LR}
.text:AE662524 MOV             R11, SP
.text:AE662528 SUB             SP, SP, #0x58
.text:AE66252C MOV             R2, #0
.text:AE662530 STR             R2, [SP,#0x58+var_4C]
.text:AE662534 STR             R2, [R11,#var_4]
.text:AE662538 STR             R0, [R11,#var_8]
.text:AE66253C STR             R1, [R11,#var_C]
.text:AE662540 MOV             R0, SP
.text:AE662544 LDR             R1, =0x40019999
.text:AE662548 STR             R1, [R0,#0x58+var_54]
.text:AE66254C LDR             R2, =0x9999999A         ; double
.text:AE662550 STR             R2, [R0,#0x58+var_58]
.text:AE662554 SUB             R0, R11, #-var_20       ; R0保存返回值的地址，作为函数参数传递
.text:AE662558 LDR             R3, =0x3FF19999         ; 
.text:AE66255C BL              _Z11createPointdd       ; createPoint(double,double)
.text:AE66255C
.text:AE662560 LDR             R2, [R11,#var_20]
.text:AE662564 LDR             R3, [R11,#var_1C]
.text:AE662568 LDR             R0, [R11,#var_18]
.text:AE66256C LDR             R12, [R11,#var_14]
.text:AE662570 MOV             R1, SP
.text:AE662574 STR             R12, [R1,#0x58+var_54]
.text:AE662578 STR             R0, [R1,#0x58+var_58]
.text:AE66257C LDR             R0, =(aCxFCyF - 0xAE662588) ; "cx = %f, cy = %f\n"
.text:AE662580 ADD             R0, PC, R0              ; "cx = %f, cy = %f\n"
.text:AE662584 BL              printf
.text:AE662584
.text:AE662588 LDR             R3, =(aZhangsan - 0xAE662594) ; "zhangsan"
.text:AE66258C ADD             R3, PC, R3              ; "zhangsan"
.text:AE662590 ADD             R0, SP, #0x58+var_48    ; R0保存返回值的地址，作为函数参数传递
.text:AE662594 STR             R0, [SP,#0x58+var_50]
.text:AE662598 MOV             R1, #18                 ; int
.text:AE66259C MOV             R2, #180                ; char *
.text:AE6625A0 BL              _Z12createPersoniiPKc   ; createPerson(int,int,char const*)
.text:AE6625A0
.text:AE6625A4 LDR             R0, [SP,#0x58+var_50]
.text:AE6625A8 LDR             R1, [SP,#0x58+var_48]
.text:AE6625AC LDR             R2, [SP,#0x58+var_44]
.text:AE6625B0 ADD             R3, R0, #8
.text:AE6625B4 LDR             R0, =(aAgeDHeightDNam - 0xAE6625C0) ; "age=%d, height=%d, name=%s\n"
.text:AE6625B8 ADD             R0, PC, R0              ; "age=%d, height=%d, name=%s\n"
.text:AE6625BC BL              printf
.text:AE6625BC
.text:AE6625C0 LDR             R0, [SP,#0x58+var_4C]
.text:AE6625C4 MOV             SP, R11
.text:AE6625C8 POP             {R11,LR}
.text:AE6625CC BX              LR


.text:AE662484 _Z11createPointdd                       ; CODE XREF: main+3C↓p
.text:AE662484
.text:AE662484
.text:AE662484 PUSH            {R11,LR}
.text:AE662488 MOV             R11, SP
.text:AE66248C SUB             SP, SP, #0x10
.text:AE662490 MOV             R12, R3
.text:AE662494 MOV             R3, R2
.text:AE662498 MOV             R1, R0
.text:AE66249C LDR             R2, [R11,#arg_4]
.text:AE6624A0 LDR             R0, [R11,#arg_0]
.text:AE6624A4 STR             R12, [SP,#0x10+var_4]
.text:AE6624A8 STR             R3, [SP,#0x10+var_8]
.text:AE6624AC STR             R2, [SP,#0x10+var_C]
.text:AE6624B0 STR             R0, [SP,#0x10+var_10]
.text:AE6624B4 LDR             R0, [SP,#0x10+var_8]
.text:AE6624B8 LDR             R2, [SP,#0x10+var_4]
.text:AE6624BC STR             R2, [R1,#4]
.text:AE6624C0 STR             R0, [R1]                ; R0+偏移定位成员变量
.text:AE6624C4 LDR             R0, [SP,#0x10+var_10]
.text:AE6624C8 LDR             R2, [SP,#0x10+var_C]
.text:AE6624CC STR             R2, [R1,#0xC]
.text:AE6624D0 STR             R0, [R1,#8]
.text:AE6624D4 MOV             SP, R11
.text:AE6624D8 POP             {R11,LR}
.text:AE6624DC BX              LR
```

### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 return.cpp -o return64
```

在 IDA 中对应的反汇编代码。
```armasm
.text:0000000000001790 SUB             SP, SP, #0x70
.text:0000000000001794 STP             X29, X30, [SP,#0x60+var_s0]
.text:0000000000001798 ADD             X29, SP, #0x60
.text:000000000000179C MOV             W8, WZR
.text:00000000000017A0 STR             W8, [SP,#0x60+var_4C]
.text:00000000000017A4 STUR            WZR, [X29,#var_4]
.text:00000000000017A8 STUR            W0, [X29,#var_8]
.text:00000000000017AC STUR            X1, [X29,#var_10]
.text:00000000000017B0 ADRP            X8, #qword_558@PAGE
.text:00000000000017B4 LDR             D0, [X8,#qword_558@PAGEOFF] ; double
.text:00000000000017B8 ADRP            X8, #qword_550@PAGE
.text:00000000000017BC LDR             D1, [X8,#qword_550@PAGEOFF] ; double
.text:00000000000017C0 BL              _Z11createPointdd       ; createPoint(double,double)
.text:00000000000017C0
.text:00000000000017C4 STUR            D0, [X29,#var_20]
.text:00000000000017C8 STUR            D1, [X29,#var_18]       ; 直接使用寄存器保存返回值
.text:00000000000017CC LDUR            D0, [X29,#var_20]
.text:00000000000017D0 LDUR            D1, [X29,#var_18]
.text:00000000000017D4 ADRL            X0, aCxFCyF             ; "cx = %f, cy = %f\n"
.text:00000000000017DC BL              .printf
.text:00000000000017DC
.text:00000000000017E0 ADD             X8, SP, #0x60+var_48    ; X8保存返回值首地址
.text:00000000000017E4 STR             X8, [SP,#0x60+var_58]
.text:00000000000017E8 MOV             W0, #0x12               ; int
.text:00000000000017EC MOV             W1, #0xB4               ; int
.text:00000000000017F0 ADRL            X2, aZhangsan           ; "zhangsan"
.text:00000000000017F8 BL              _Z12createPersoniiPKc   ; createPerson(int,int,char const*)
.text:00000000000017F8
.text:00000000000017FC LDR             X8, [SP,#0x60+var_58]
.text:0000000000001800 LDR             W1, [SP,#0x60+var_48]
.text:0000000000001804 LDR             W2, [SP,#0x60+var_44]
.text:0000000000001808 ADD             X3, X8, #8              ; 通过偏移获取对应的成员变量
.text:000000000000180C ADRL            X0, aAgeDHeightDNam     ; "age=%d, height=%d, name=%s\n"
.text:0000000000001814 BL              .printf
.text:0000000000001814
.text:0000000000001818 LDR             W0, [SP,#0x60+var_4C]
.text:000000000000181C LDP             X29, X30, [SP,#0x60+var_s0]
.text:0000000000001820 ADD             SP, SP, #0x70 ; 'p'
.text:0000000000001824 RET
```