---
title: ARMv8学习记录09
date: 2023-07-04 13:22:10
tags: C++逆向
category: ARMv8汇编
---

# 构造函数和析构函数

## 构造函数和析构函数的出现时机

在何种情况下,有以下两种情况编译器会提供默认的构造函数。

1. 本类和本类中定义的成员对象或者父类中存在虚函数
   因为需要初始化虚表，且这个工作理应在构造函数中隐式完成，所以在没有定义构造函数的情况下，编译器会添加默认的构造函数，用于隐式完成虚表的初始化工作。

2. 父类或本类中定义的成员对象带有构造函数
   在对象被定义时，因为对象本身为派生类，所以构造顺序是先构造父类再构造自身。当父类中带有构造函数时，将会调用父类构造函数，而这个调用过程需要在构造函数内完成，因此编译器添加了默认的构造函数来完成这个调用过程。成员对象带有构造函数的情况与此相同。

对象生成时会自动调用构造函数。

析构函数的触发时机也需要视情况而定，主要分如下几种情况：

- 局部对象：作用域结束前调用析构函数。
- 堆对象：释放堆空间前调用析构函数。
- 参数对象：退出函数前，调用参数对象的析构函数。
- 返回对象：如无对象引用定义，退出函数后，调用返回对象的析构函数，否则与对象引用的作用域一致。
- 全局对象：main 函数退出后调用析构函数。
- 静态对象：main 函数退出后调用析构函数。

### 局部对象

例子：

```cpp
#include <cstdio>

class Point{
public:
    int cx;
    int cy;
public:
    Point(int cx, int cy){
        this->cx = cx;
        this->cy = cy;
    }

    ~Point(){
        this->cx = 0;
        this->cy = 0;
    }
};

int main(int argc, char const *argv[])
{
    Point point(123, 456);
    printf("cx = %d, cy = %d\n", point.cx, point.cy);
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -lstdc++  local.cpp -o local
```

在 IDA 中对应的反汇编代码。

```armasm
.text:000014BC PUSH            {R11,LR}
.text:000014C0 MOV             R11, SP
.text:000014C4 SUB             SP, SP, #0x20
.text:000014C8 MOV             R2, #0
.text:000014CC STR             R2, [R11,#var_4]
.text:000014D0 STR             R0, [R11,#var_8]
.text:000014D4 STR             R1, [R11,#var_C]
.text:000014D8 ADD             R0, SP, #0x20+var_14    ; this
.text:000014DC MOV             R1, #0x7B ; '{'         ; int
.text:000014E0 MOV             R2, #0x1C8              ; int
.text:000014E4 BL              _ZN5PointC2Eii          ; Point::Point(int,int) 构造函数
.text:000014E4
.text:000014E8 LDR             R1, [SP,#0x20+var_14]
.text:000014EC LDR             R2, [SP,#0x20+var_10]
.text:000014F0 ;   try {
.text:000014F0 LDR             R0, =(aCxDCyD - 0x14FC) ; "cx = %d, cy = %d\n"
.text:000014F4 ADD             R0, PC, R0              ; "cx = %d, cy = %d\n"
.text:000014F8 BL              printf
.text:000014F8 ;   } // starts at 14F0
.text:000014F8
.text:000014FC B               loc_1500
.text:000014FC
.text:00001500 ; ---------------------------------------------------------------------------
.text:00001500
.text:00001500 loc_1500                                ; CODE XREF: main+40↑j
.text:00001500 MOV             R0, #0
.text:00001504 STR             R0, [R11,#var_4]
.text:00001508 ADD             R0, SP, #0x20+var_14    ; this
.text:0000150C BL              _ZN5PointD2Ev           ; Point::~Point() 析构函数
.text:0000150C
.text:00001510 LDR             R0, [R11,#var_4]
.text:00001514 MOV             SP, R11
.text:00001518 POP             {R11,LR}
.text:0000151C BX              LR
```

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -lstdc++ local.cpp -o local64
```

在 IDA 中对应的反汇编代码。

```armasm
.text:0000000000003500 STP             X29, X30, [SP,#0x30+var_s0]
.text:0000000000003504 ADD             X29, SP, #0x30
.text:0000000000003508 STUR            WZR, [X29,#var_4]
.text:000000000000350C STUR            W0, [X29,#var_8]
.text:0000000000003510 STUR            X1, [X29,#var_10]
.text:0000000000003514 ADD             X0, SP, #0x30+var_18    ; this
.text:0000000000003518 MOV             W1, #0x7B ; '{'         ; int
.text:000000000000351C MOV             W2, #0x1C8              ; int
.text:0000000000003520 BL              _ZN5PointC2Eii          ; Point::Point(int,int)
.text:0000000000003520
.text:0000000000003524 LDR             W1, [SP,#0x30+var_18]
.text:0000000000003528 LDR             W2, [SP,#0x30+var_14]
.text:000000000000352C ;   try {
.text:000000000000352C ADRL            X0, aCxDCyD             ; "cx = %d, cy = %d\n"
.text:0000000000003534 BL              .printf
.text:0000000000003534 ;   } // starts at 352C
.text:0000000000003534
.text:0000000000003538 B               loc_353C
.text:0000000000003538
.text:000000000000353C ; ---------------------------------------------------------------------------
.text:000000000000353C
.text:000000000000353C loc_353C                                ; CODE XREF: main+3C↑j
.text:000000000000353C STUR            WZR, [X29,#var_4]
.text:0000000000003540 ADD             X0, SP, #0x30+var_18    ; this
.text:0000000000003544 BL              _ZN5PointD2Ev           ; Point::~Point()
.text:0000000000003544
.text:0000000000003548 LDUR            W0, [X29,#var_4]
.text:000000000000354C LDP             X29, X30, [SP,#0x30+var_s0]
.text:0000000000003550 ADD             SP, SP, #0x40 ; '@'
.text:0000000000003554 RET
.text:0000000000003554
.text:0000000000003558 ; ---------------------------------------------------------------------------
.text:0000000000003558 ;   cleanup() // owned by 352C
.text:0000000000003558 MOV             W8, W1
.text:000000000000355C STR             X0, [SP,#-0x10+arg_10]
.text:0000000000003560 STR             W8, [SP,#-0x10+arg_C]
.text:0000000000003564 ADD             X0, SP, #-0x10+arg_18   ; this
.text:0000000000003568 BL              _ZN5PointD2Ev           ; Point::~Point()
.text:0000000000003568
.text:000000000000356C B               loc_3570
.text:000000000000356C
.text:0000000000003570 ; ---------------------------------------------------------------------------
.text:0000000000003570
.text:0000000000003570 loc_3570                                ; CODE XREF: main+70↑j
.text:0000000000003570 LDR             X0, [SP,#-0x10+arg_10]
.text:0000000000003574 BL              _Unwind_Resume
```

### 堆对象

例子：

```cpp
#include <cstdio>

class Point{
public:
    int cx;
    int cy;
public:
    Point(int cx, int cy){
        this->cx = cx;
        this->cy = cy;
    }

    ~Point(){
        this->cx = 0;
        this->cy = 0;
    }
};

int main(int argc, char const *argv[])
{
    Point *p = new Point(123, 456);
    printf("cx = %d, cy = %d\n", p->cx, p->cy);
    // 需要手动释放
    delete p;
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -lstdc++  heap.cpp -o heap
```

在 IDA 中对应的反汇编代码。

```armasm
.text:00001514 PUSH            {R11,LR}
.text:00001518 MOV             R11, SP
.text:0000151C SUB             SP, SP, #0x28
.text:00001520 MOV             R2, #0
.text:00001524 STR             R2, [R11,#var_4]
.text:00001528 STR             R0, [R11,#var_8]
.text:0000152C STR             R1, [R11,#var_C]
.text:00001530 MOV             R0, #8                  ; unsigned int
.text:00001534 BL              _Znwj                   ; operator new(uint)
.text:00001534
.text:00001538 MOV             R1, R0
.text:0000153C STR             R1, [SP,#0x28+var_20]
.text:00001540 MOV             R1, R0
.text:00001544 STR             R1, [SP,#0x28+var_1C]
.text:00001548 ;   try {
.text:00001548 MOV             R1, #0x7B ; '{'         ; int
.text:0000154C MOV             R2, #0x1C8              ; int
.text:00001550 BL              _ZN5PointC2Eii          ; Point::Point(int,int)
.text:00001550 ;   } // starts at 1548
.text:00001550
.text:00001554 B               loc_1558
.text:00001554
.text:00001558 ; ---------------------------------------------------------------------------
.text:00001558
.text:00001558 loc_1558                                ; CODE XREF: main+40↑j
.text:00001558 LDR             R0, [SP,#0x28+var_1C]
.text:0000155C STR             R0, [R11,#var_10]
.text:00001560 LDR             R0, [R11,#var_10]
.text:00001564 LDR             R1, [R0]
.text:00001568 LDR             R2, [R0,#4]
.text:0000156C LDR             R0, =(aCxDCyD - 0x1578) ; "cx = %d, cy = %d\n"
.text:00001570 ADD             R0, PC, R0              ; "cx = %d, cy = %d\n"
.text:00001574 BL              printf
.text:00001574
.text:00001578 LDR             R0, [R11,#var_10]
.text:0000157C STR             R0, [SP,#0x28+var_24]
.text:00001580 CMP             R0, #0
.text:00001584 BEQ             loc_15A0
.text:00001584
.text:00001588 B               loc_158C
.text:00001588
.text:0000158C ; ---------------------------------------------------------------------------
.text:0000158C
.text:0000158C loc_158C                                ; CODE XREF: main+74↑j
.text:0000158C LDR             R0, [SP,#0x28+var_24]   ; this
.text:00001590 BL              _ZN5PointD2Ev           ; Point::~Point()
.text:00001590
.text:00001594 LDR             R0, [SP,#0x28+var_24]   ; void *
.text:00001598 BL              _ZdlPv                  ; operator delete(void *)
.text:00001598
.text:0000159C B               loc_15A0
.text:0000159C
.text:000015A0 ; ---------------------------------------------------------------------------
.text:000015A0
.text:000015A0 loc_15A0                                ; CODE XREF: main+70↑j
.text:000015A0                                         ; main+88↑j
.text:000015A0 MOV             R0, #0
.text:000015A4 MOV             SP, R11
.text:000015A8 POP             {R11,LR}
.text:000015AC BX              LR
```

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -lstdc++ heap.cpp -o heap64
```

在 IDA 中对应的反汇编代码。

```armasm
.text:0000000000003584 SUB             SP, SP, #0x50
.text:0000000000003588 STP             X29, X30, [SP,#0x40+var_s0]
.text:000000000000358C ADD             X29, SP, #0x40
.text:0000000000003590 STUR            WZR, [X29,#var_4]
.text:0000000000003594 STUR            W0, [X29,#var_8]
.text:0000000000003598 STUR            X1, [X29,#var_10]
.text:000000000000359C MOV             X0, #8                  ; unsigned __int64
.text:00000000000035A0 BL              ._Znwm                  ; operator new(ulong)
.text:00000000000035A0
.text:00000000000035A4 STR             X0, [SP,#0x40+var_30]
.text:00000000000035A8 ;   try {
.text:00000000000035A8 MOV             W1, #0x7B ; '{'         ; int
.text:00000000000035AC MOV             W2, #0x1C8              ; int
.text:00000000000035B0 BL              _ZN5PointC2Eii          ; Point::Point(int,int)
.text:00000000000035B0 ;   } // starts at 35A8
.text:00000000000035B0
.text:00000000000035B4 B               loc_35B8
.text:00000000000035B4
.text:00000000000035B8 ; ---------------------------------------------------------------------------
.text:00000000000035B8
.text:00000000000035B8 loc_35B8                                ; CODE XREF: main+30↑j
.text:00000000000035B8 LDR             X8, [SP,#0x40+var_30]
.text:00000000000035BC STUR            X8, [X29,#var_18]
.text:00000000000035C0 LDUR            X8, [X29,#var_18]
.text:00000000000035C4 LDR             W1, [X8]
.text:00000000000035C8 LDUR            X8, [X29,#var_18]
.text:00000000000035CC LDR             W2, [X8,#4]
.text:00000000000035D0 ADRL            X0, aCxDCyD             ; "cx = %d, cy = %d\n"
.text:00000000000035D8 BL              .printf
.text:00000000000035D8
.text:00000000000035DC LDUR            X8, [X29,#var_18]
.text:00000000000035E0 STR             X8, [SP,#0x40+var_38]
.text:00000000000035E4 CBZ             X8, loc_3600
.text:00000000000035E4
.text:00000000000035E8 B               loc_35EC
.text:00000000000035E8
.text:00000000000035EC ; ---------------------------------------------------------------------------
.text:00000000000035EC
.text:00000000000035EC loc_35EC                                ; CODE XREF: main+64↑j
.text:00000000000035EC LDR             X0, [SP,#0x40+var_38]   ; this
.text:00000000000035F0 BL              _ZN5PointD2Ev           ; Point::~Point()
.text:00000000000035F0
.text:00000000000035F4 LDR             X0, [SP,#0x40+var_38]   ; void *
.text:00000000000035F8 BL              ._ZdlPv                 ; operator delete(void *)
.text:00000000000035F8
.text:00000000000035FC B               loc_3600
.text:00000000000035FC
.text:0000000000003600 ; ---------------------------------------------------------------------------
.text:0000000000003600
.text:0000000000003600 loc_3600                                ; CODE XREF: main+60↑j
.text:0000000000003600                                         ; main+78↑j
.text:0000000000003600 MOV             W0, WZR
.text:0000000000003604 LDP             X29, X30, [SP,#0x40+var_s0]
.text:0000000000003608 ADD             SP, SP, #0x50 ; 'P'
.text:000000000000360C RET
.text:000000000000360C
.text:0000000000003610 ; ---------------------------------------------------------------------------
.text:0000000000003610 ;   cleanup() // owned by 35A8
.text:0000000000003610 MOV             X9, X0
.text:0000000000003614 LDR             X0, [SP,#-0x10+arg_10]  ; void *
.text:0000000000003618 MOV             W8, W1
.text:000000000000361C STR             X9, [SP,#-0x10+arg_20]
.text:0000000000003620 STR             W8, [SP,#-0x10+arg_1C]
.text:0000000000003624 BL              ._ZdlPv                 ; operator delete(void *)
.text:0000000000003624
.text:0000000000003628 B               loc_362C
.text:0000000000003628
.text:000000000000362C ; ---------------------------------------------------------------------------
.text:000000000000362C
.text:000000000000362C loc_362C                                ; CODE XREF: main+A4↑j
.text:000000000000362C LDR             X0, [SP,#-0x10+arg_20]
.text:0000000000003630 BL              _Unwind_Resume
```

### 参数对象与返回对象

```cpp
#include <cstdio>
#include <cstring>

class Person{
public:
    int age;
    char* name;
public:
    Person(int age, const char* name){
        this->age = age;
        this->name = new char[strlen(name) + 1];
        strcpy(this->name, name);
    }
    // 拷贝构造函
    Person(const Person& person){
        this->age = person.age;
        this->name = new char[strlen(person.name) + 1];
        strcpy(this->name, person.name);
    }
    Person& operator= (const Person& person){
        this->age = person.age;
        this->name = new char[strlen(person.name) + 1];
        strcpy(this->name, person.name);
        return *this;
    }

    ~Person(){
        this->age = 0;
        delete[] this->name;
        this->name = nullptr;
    }
};

void show(const Person& person){
    printf("age=%d, name=%s\n", person.age, person.name);
}

Person create(int age, const char* name){
    return Person(age, name);
}

int main(int argc, char const *argv[])
{
    Person person = create(18, "zhangsan");
    printf("age=%d, name=%s\n", person.age, person.name);
    show(person);
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -lstdc++  params.cpp -o params
```

在 IDA 中对应的反汇编代码。

```armasm
.text:000015EC PUSH            {R11,LR}
.text:000015F0 MOV             R11, SP
.text:000015F4 SUB             SP, SP, #0x20
.text:000015F8 MOV             R2, #0
.text:000015FC STR             R2, [R11,#var_4]
.text:00001600 STR             R0, [R11,#var_8]
.text:00001604 STR             R1, [R11,#var_C]
.text:00001608 LDR             R2, =(aZhangsan - 0x1614) ; "zhangsan"
.text:0000160C ADD             R2, PC, R2              ; "zhangsan"
.text:00001610 ADD             R0, SP, #0x20+var_14    ; 传递返回对象的地址
.text:00001614 MOV             R1, #18                 ; char *
.text:00001618 BL              _Z6createiPKc           ; create(int,char const*)
.text:00001618
.text:0000161C LDR             R1, [SP,#0x20+var_14]
.text:00001620 LDR             R2, [SP,#0x20+var_10]
.text:00001624 ;   try {
.text:00001624 LDR             R0, =(aAgeDNameS - 0x1630) ; "age=%d, name=%s\n"
.text:00001628 ADD             R0, PC, R0              ; "age=%d, name=%s\n"
.text:0000162C BL              printf
.text:0000162C
.text:00001630 B               loc_1634
.text:00001630
.text:00001634 ; ---------------------------------------------------------------------------
.text:00001634
.text:00001634 loc_1634                                ; CODE XREF: main+44↑j
.text:00001634 ADD             R0, SP, #0x20+var_14    ; 传递对象的地址
.text:00001638 BL              _Z4showRK6Person        ; show(Person const&)
.text:00001638 ;   } // starts at 1624
.text:00001638
.text:0000163C B               loc_1640
.text:0000163C
.text:00001640 ; ---------------------------------------------------------------------------
.text:00001640
.text:00001640 loc_1640                                ; CODE XREF: main+50↑j
.text:00001640 MOV             R0, #0
.text:00001644 STR             R0, [R11,#var_4]
.text:00001648 ADD             R0, SP, #0x20+var_14    ; this
.text:0000164C BL              _ZN6PersonD2Ev          ; Person::~Person()
.text:0000164C
.text:00001650 LDR             R0, [R11,#var_4]
.text:00001654 MOV             SP, R11
.text:00001658 POP             {R11,LR}
.text:0000165C BX              LR
```

可以发现参数对象和返回对象都会传递一个 this 指针给函数。

#### ARMv8

使用下列命令生成 ARMv8 可执行文件。

```bash
➜  clang -target aarch64-linux-android21 -lstdc++ params.cpp -o params64
```

在 IDA 中对应的反汇编代码。

```armasm
.text:00000000000036E4 SUB             SP, SP, #0x40
.text:00000000000036E8 STP             X29, X30, [SP,#0x30+var_s0]
.text:00000000000036EC ADD             X29, SP, #0x30
.text:00000000000036F0 STUR            WZR, [X29,#var_4]
.text:00000000000036F4 STUR            W0, [X29,#var_8]
.text:00000000000036F8 STUR            X1, [X29,#var_10]
.text:00000000000036FC ADD             X8, SP, #0x30+var_20    ; X8保存返回对象的地址
.text:0000000000003700 MOV             W0, #0x12               ; int
.text:0000000000003704 ADRL            X1, aZhangsan           ; "zhangsan"
.text:000000000000370C BL              _Z6createiPKc           ; create(int,char const*)
.text:000000000000370C
.text:0000000000003710 LDR             W1, [SP,#0x30+var_20]
.text:0000000000003714 LDR             X2, [SP,#0x30+var_18]
.text:0000000000003718 ;   try {
.text:0000000000003718 ADRL            X0, aAgeDNameS          ; "age=%d, name=%s\n"
.text:0000000000003720 BL              .printf
.text:0000000000003720
.text:0000000000003724 B               loc_3728                ; X0传递对象的地址
.text:0000000000003724
.text:0000000000003728 ; ---------------------------------------------------------------------------
.text:0000000000003728
.text:0000000000003728 loc_3728                                ; CODE XREF: main+40↑j
.text:0000000000003728 ADD             X0, SP, #0x30+var_20    ; X0传递对象的地址
.text:000000000000372C BL              _Z4showRK6Person        ; show(Person const&)
.text:000000000000372C ;   } // starts at 3718
.text:000000000000372C
.text:0000000000003730 B               loc_3734
.text:0000000000003730
.text:0000000000003734 ; ---------------------------------------------------------------------------
.text:0000000000003734
.text:0000000000003734 loc_3734                                ; CODE XREF: main+4C↑j
.text:0000000000003734 STUR            WZR, [X29,#var_4]
.text:0000000000003738 ADD             X0, SP, #0x30+var_20    ; this
.text:000000000000373C BL              _ZN6PersonD2Ev          ; Person::~Person()
.text:000000000000373C
.text:0000000000003740 LDUR            W0, [X29,#var_4]
.text:0000000000003744 LDP             X29, X30, [SP,#0x30+var_s0]
.text:0000000000003748 ADD             SP, SP, #0x40 ; '@'
.text:000000000000374C RET
```

### 全局对象与静态对象

例子：

```cpp
// jni/static.cpp
#include <cstdio>
#include <cstring>

class Person{
public:
    int age;
    char* name;
public:
    Person(int age, const char* name){
        this->age = age;
        this->name = new char[strlen(name) + 1];
        strcpy(this->name, name);
    }
    ~Person(){
        this->age = 0;
        delete[] this->name;
        this->name = nullptr;
    }
};

void fun(){
    static Person p(38, "lisi");
    printf("age=%d, name=%s\n", p.age, p.name);
}

Person person1(18, "zhangsan");
static Person person2(28, "wangwu");

int main(int argc, char const *argv[])
{
    printf("person1.age=%d, person1.name=%s\n", person1.age, person1.name);
    printf("person2.age=%d, person2.name=%s\n", person2.age, person2.name);
    fun();
    return 0;
}
```

对应的 mk 文件内容:

```makefile
// jni/Android.mk
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := static.cpp
LOCAL_MODULE := static
include $(BUILD_EXECUTABLE)

// Application.mk
APP_BUILD_SCRIPT := Android.mk
APP_PLATFORM := android-21
APP_ABI := armeabi-v7a arm64-v8a
```

进入 jni 目录使用 `ndk-build` 编译，对应的二进制文件在 `jni/../libs` 目录中。

#### ARMv7

通过 start 函数找到 main 函数 sub_6E4：

```armasm
.text:000006E4 PUSH            {R7,LR}
.text:000006E6 MOV             R7, SP
.text:000006E8 LDR             R0, =(off_1988 - 0x6EE)
.text:000006EA ADD             R0, PC                  ; off_1988
.text:000006EC LDR             R0, [R0]                ; unk_29C0 全局变量 person1
.text:000006EE LDRD.W          R1, R2, [R0]
.text:000006F2 LDR             R0, =(aPerson1AgeDPer - 0x6F8) ; "person1.age=%d, person1.name=%s\n"
.text:000006F4 ADD             R0, PC                  ; "person1.age=%d, person1.name=%s\n"
.text:000006F6 BLX             printf
.text:000006F6
.text:000006FA LDR             R0, =(unk_29C8 - 0x700)
.text:000006FC ADD             R0, PC                  ; unk_29C8 全局变量 person2
.text:000006FE LDRD.W          R1, R2, [R0]
.text:00000702 LDR             R0, =(aPerson2AgeDPer - 0x708) ; "person2.age=%d, person2.name=%s\n"
.text:00000704 ADD             R0, PC                  ; "person2.age=%d, person2.name=%s\n"
.text:00000706 BLX             printf
.text:00000706
.text:0000070A BL              sub_630                 ; fun 函数
.text:0000070A
.text:0000070E MOVS            R0, #0
.text:00000710 POP             {R7,PC}
```

在 mian 函数中并没有发现 person1 和 person2 的初始化，其实全局对象的初始化在 `.init_array` 中。

```armasm
.init_array:00001868 AREA .init_array, DATA
.init_array:00001868 ; ORG 0x1868
.init_array:00001868 dword_1868 DCD 0xFFFFFFFF               ; DATA XREF: .text:000005D0↑o
.init_array:00001868                                         ; .got:off_1978↓o
.init_array:0000186C DCD sub_724+1
.init_array:00001870 DCD 0
.init_array:00001870 ; .init_array ends


.text:00000724 sub_724                                 ; DATA XREF: .init_array:0000186C↓o
.text:00000724 ; __unwind {
.text:00000724 PUSH            {R4-R7,LR}
.text:00000726 ADD             R7, SP, #0xC
.text:00000728 PUSH.W          {R11}
.text:0000072C LDR             R0, =(off_1988 - 0x736)
.text:0000072E MOVS            R1, #0x12               ; int
.text:00000730 LDR             R2, =(aZhangsan - 0x738) ; "zhangsan"
.text:00000732 ADD             R0, PC                  ; off_1988
.text:00000734 ADD             R2, PC                  ; "zhangsan"
.text:00000736 LDR             R5, [R0]                ; unk_29C0
.text:00000738 MOV             R0, R5                  ; int
.text:0000073A BL              sub_6A8
.text:0000073A
.text:0000073E LDR             R0, =(off_1984 - 0x748)
.text:00000740 MOV             R1, R5                  ; obj
.text:00000742 LDR             R4, =(unk_29BC - 0x74A)
.text:00000744 ADD             R0, PC                  ; off_1984
.text:00000746 ADD             R4, PC                  ; unk_29BC
.text:00000748 LDR             R6, [R0]                ; sub_6CA
.text:0000074A MOV             R2, R4                  ; lpdso_handle
.text:0000074C MOV             R0, R6                  ; lpfunc
.text:0000074E BLX             __cxa_atexit
.text:0000074E
.text:00000752 LDR             R5, =(unk_29C8 - 0x75C)
.text:00000754 MOVS            R1, #0x1C               ; int
.text:00000756 LDR             R2, =(aWangwu - 0x75E)  ; "wangwu"
.text:00000758 ADD             R5, PC                  ; unk_29C8
.text:0000075A ADD             R2, PC                  ; "wangwu"
.text:0000075C MOV             R0, R5                  ; int
.text:0000075E BL              sub_6A8
.text:0000075E
.text:00000762 MOV             R0, R6
.text:00000764 MOV             R1, R5
.text:00000766 MOV             R2, R4
.text:00000768 POP.W           {R11}
.text:0000076C POP.W           {R4-R7,LR}
.text:00000770 B.W             sub_798
```

当对象调用完构造函数后，将会把析构函数的地址作为参数传递给 `__cxa_atexit` 实现程序退出前调用对应的析构函数。

局部静态对象的初始化是在相应的作用域中进行的。
```armasm
.text:00000630 PUSH            {R4-R7,LR}
.text:00000632 ADD             R7, SP, #0xC
.text:00000634 PUSH.W          {R11}
.text:00000638 LDR             R6, =(byte_29D0 - 0x63E)
.text:0000063A ADD             R6, PC                  ; byte_29D0
.text:0000063C LDRB            R0, [R6]
.text:0000063E DMB.W           ISH
.text:00000642 LSLS            R0, R0, #0x1F
.text:00000644 BEQ             loc_65A
.text:00000644
.text:00000646
.text:00000646 loc_646                                 ; CODE XREF: sub_630+36↓j
.text:00000646                                         ; sub_630+5C↓j
.text:00000646 LDRD.W          R1, R2, [R6,#4]
.text:0000064A LDR             R0, =(aAgeDNameS - 0x650) ; "age=%d, name=%s\n"
.text:0000064C ADD             R0, PC                  ; "age=%d, name=%s\n"
.text:0000064E POP.W           {R11}
.text:00000652 POP.W           {R4-R7,LR}
.text:00000656 B.W             sub_78C
.text:00000656
.text:0000065A ; ---------------------------------------------------------------------------
.text:0000065A
.text:0000065A loc_65A                                 ; CODE XREF: sub_630+14↑j
.text:0000065A LDR             R4, =(byte_29D0 - 0x660)
.text:0000065C ADD             R4, PC                  ; byte_29D0
.text:0000065E MOV             R0, R4                  ; __guard *
.text:00000660 BLX             __cxa_guard_acquire
.text:00000660
.text:00000664 CMP             R0, #0
.text:00000666 BEQ             loc_646
.text:00000666
.text:00000668 LDR             R2, =(aLisi - 0x672)    ; "lisi"
.text:0000066A ADDS            R5, R4, #4
.text:0000066C MOVS            R1, #0x26 ; '&'         ; int
.text:0000066E ADD             R2, PC                  ; "lisi"
.text:00000670 MOV             R0, R5                  ; int
.text:00000672 BL              sub_6A8
.text:00000672
.text:00000676 LDR             R0, =(off_1984 - 0x680)
.text:00000678 MOV             R1, R5                  ; obj
.text:0000067A LDR             R2, =(unk_29BC - 0x682)
.text:0000067C ADD             R0, PC                  ; off_1984
.text:0000067E ADD             R2, PC                  ; unk_29BC ; lpdso_handle
.text:00000680 LDR             R0, [R0]                ; sub_6CA ; lpfunc
.text:00000682 BLX             __cxa_atexit
.text:00000682
.text:00000686 MOV             R0, R4                  ; __guard *
.text:00000688 BLX             __cxa_guard_release
.text:00000688
.text:0000068C B               loc_646
```
可以发现局部静态对象调用完构造函数后，也将析构函数的地址传递给了 `__cxa_atexit` 函数。

#### ARMv8

ARMv8 对应的反汇编与AMRv7的类似，这里就不进行分析了。
