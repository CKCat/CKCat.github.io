---
title: ARMv8学习记录10
date: 2023-07-04 13:23:00
tags: C++逆向
category: ARMv8汇编
---

# 虚函数和继承

## 虚函数

例子：

```cpp
#include <stdio.h>

class Point
{
public:
    int cx;
    int cy;
public:
    Point(int x):cx(x), cy(0){};
    ~Point(){
        cx = 0;
        cy = 0;
    };

    virtual void setx(int cx){
        this->cx = cx;
    }
    virtual void sety(int cy){
        this->cy = cy;
    }
};

int main(int argc, char const *argv[])
{
    Point point(12);
    point.sety(34);
    printf("cx = %d, cy = %d\n", point.cx, point.cy);
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -lstdc++  virtual.cpp -o virtual
```

在 IDA 中对应的反汇编代码。

```armasm
.text:00001530 PUSH            {R11,LR}
.text:00001534 MOV             R11, SP
.text:00001538 SUB             SP, SP, #0x28
.text:0000153C MOV             R2, #0
.text:00001540 STR             R2, [R11,#var_4]
.text:00001544 STR             R0, [R11,#var_8]
.text:00001548 STR             R1, [R11,#var_C]
.text:0000154C ADD             R0, SP, #0x28+var_18    ; this
.text:00001550 STR             R0, [SP,#0x28+var_24]
.text:00001554 MOV             R1, #12                 ; int
.text:00001558 BL              _ZN5PointC2Ei           ; Point::Point(int)
.text:00001558
.text:0000155C LDR             R0, [SP,#0x28+var_24]   ; this
.text:00001560 ;   try {
.text:00001560 MOV             R1, #34                 ; int
.text:00001564 BL              _ZN5Point4setyEi        ; Point::sety(int)
.text:00001564
.text:00001568 B               loc_156C
.text:00001568
.text:0000156C ; ---------------------------------------------------------------------------
.text:0000156C
.text:0000156C loc_156C                                ; CODE XREF: main+38↑j
.text:0000156C LDR             R1, [SP,#0x28+var_14]
.text:00001570 LDR             R2, [SP,#0x28+var_10]
.text:00001574 LDR             R0, =(aCxDCyD - 0x1580) ; "cx = %d, cy = %d\n"
.text:00001578 ADD             R0, PC, R0              ; "cx = %d, cy = %d\n"
.text:0000157C BL              printf
.text:0000157C ;   } // starts at 1560
.text:0000157C
.text:00001580 B               loc_1584
.text:00001580
.text:00001584 ; ---------------------------------------------------------------------------
.text:00001584
.text:00001584 loc_1584                                ; CODE XREF: main+50↑j
.text:00001584 MOV             R0, #0
.text:00001588 STR             R0, [R11,#var_4]
.text:0000158C ADD             R0, SP, #0x28+var_18    ; this
.text:00001590 BL              _ZN5PointD2Ev           ; Point::~Point()
.text:00001590
.text:00001594 LDR             R0, [R11,#var_4]
.text:00001598 MOV             SP, R11
.text:0000159C POP             {R11,LR}
.text:000015A0 BX              LR


.text:000015C0 _ZN5PointC2Ei                           ; CODE XREF: main+28↑p
.text:000015C0
.text:000015C0 var_8= -8
.text:000015C0 var_4= -4
.text:000015C0
.text:000015C0 SUB             SP, SP, #8
.text:000015C4 STR             R0, [SP,#8+var_4]       ; this
.text:000015C8 STR             R1, [SP,#8+var_8]
.text:000015CC LDR             R0, [SP,#8+var_4]
.text:000015D0 LDR             R1, =(_ZTV5Point - 0x15DC) ; `vtable for'Point
.text:000015D4 ADD             R1, PC, R1              ; `vtable for'Point
.text:000015D8 ADD             R1, R1, #8
.text:000015DC STR             R1, [R0]                ; 保存虚表指针
.text:000015E0 LDR             R1, [SP,#8+var_8]
.text:000015E4 STR             R1, [R0,#4]
.text:000015E8 MOV             R1, #0
.text:000015EC STR             R1, [R0,#8]
.text:000015F0 ADD             SP, SP, #8
.text:000015F4 BX              LR


.data.rel.ro:000026F8 _ZTV5Point DCD 0                        ; DATA XREF: Point::Point(int)+10↑o
.data.rel.ro:000026F8                                         ; Point::Point(int)+14↑o
.data.rel.ro:000026F8                                         ; .text:off_15F8↑o
.data.rel.ro:000026F8                                         ; Point::~Point()+C↑o
.data.rel.ro:000026F8                                         ; Point::~Point()+10↑o
.data.rel.ro:000026F8                                         ; .text:off_164C↑o
.data.rel.ro:000026F8                                         ; offset to this
.data.rel.ro:000026FC DCD _ZTI5Point                          ; `typeinfo for'Point
.data.rel.ro:00002700 DCD _ZN5Point4setxEi                    ; Point::setx(int)
.data.rel.ro:00002704 DCD _ZN5Point4setyEi                    ; Point::sety(int)
```

对象在调用构造函数时，会将虚表指针保存到对象的首地址空间，其内存结构如下：

```
偏移    0    vtable        |  vtable 指向的内存结构：
偏移    4    cx            |                  偏移    0    Point::setx(int)
偏移    8    cy            |                  偏移    4    Point::sety(int)
```

ARMv8 下对象的内存布局与 ARMv7 的内存布局类似。

## 继承

例子：

```cpp
// inherit.cpp
#include <cstdio>

class CAnimal{
public:
   int weight;
   int age;
public:
    CAnimal(){
        weight = 0;
        age = 0;
    }
    virtual ~CAnimal(){
        weight = 0;
        age = 0;
    }
    virtual void eat(){
        printf("Animal Eat food.\n");
    }
    virtual void sleep(){
        printf("Animal Sleep.\n");
    }
    virtual void setweight(int weight){
        this->weight = weight;
    }
    virtual void setage(int age){
        this->age = age;
    }
    void show(){
        printf("weight=%d, age=%d\n", weight, age);
    }
};

class CDog: public CAnimal{
public:
    int nType;
public:
    CDog(){
        nType = 0;
    }
    ~CDog(){
        nType = 0;
    }
    void setType(int nType){
        this->nType = nType;
    }
    virtual void eat(){
        printf("Dog Eat food.\n");
    }
};

class CCat: public CAnimal{
public:
    int nType;
public:
    CCat(){
        nType = 1;
    }
    ~CCat(){
        nType = 1;
    }
    void setType(int nType){
        this->nType = nType;
    }
    virtual void sleep(){
        printf("Cat Sleep.\n");
    }
};

int main(int argc, char* argv[])
{
    CDog dog;
    CAnimal* pdog = &dog;
    pdog->eat();
	CAnimal* pcat = new CCat;
    pcat->sleep();
    delete pcat;
	return 0;
}


```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -lstdc++  inherit.cpp -o inherit
```

在 IDA 中对应的反汇编代码。

```armasm
.text:000016FC PUSH            {R11,LR}
.text:00001700 MOV             R11, SP
.text:00001704 SUB             SP, SP, #0x40
.text:00001708 MOV             R2, #0
.text:0000170C STR             R2, [R11,#var_4]
.text:00001710 STR             R0, [R11,#var_8]
.text:00001714 STR             R1, [R11,#var_C]
.text:00001718 SUB             R0, R11, #-var_1C       ; this
.text:0000171C STR             R0, [SP,#0x40+var_30]   ; 保存this指针
.text:00001720 BL              _ZN4CDogC2Ev            ; CDog::CDog(void)
.text:00001720
.text:00001724 LDR             R0, [SP,#0x40+var_30]
.text:00001728 STR             R0, [SP,#0x40+var_20]
.text:0000172C LDR             R0, [SP,#0x40+var_20]
.text:00001730 LDR             R1, [R0]                ; 获取CDog的虚表地址
.text:00001734 LDR             R1, [R1,#8]             ; 通过虚表地址+偏移获取对应的虚函数CDog::eat(void)
.text:00001738 ;   try {
.text:00001738 MOV             LR, PC
.text:0000173C BX              R1                      ; 调用虚函数CDog::eat(void)
.text:0000173C
.text:00001740 B               loc_1744
.text:00001740
.text:00001744 ; ---------------------------------------------------------------------------
.text:00001744
.text:00001744 loc_1744                                ; CODE XREF: main+44↑j
.text:00001744 MOV             R0, #0x10               ; 申请空间
.text:00001748 BL              _Znwj                   ; operator new(uint)
.text:00001748 ;   } // starts at 1738
.text:00001748
.text:0000174C STR             R0, [SP,#0x40+var_34]
.text:00001750 B               loc_1754
.text:00001750
.text:00001754 ; ---------------------------------------------------------------------------
.text:00001754
.text:00001754 loc_1754                                ; CODE XREF: main+54↑j
.text:00001754 LDR             R0, [SP,#0x40+var_34]   ; this
.text:00001758 MOV             R1, R0
.text:0000175C STR             R1, [SP,#0x40+var_38]
.text:00001760 ;   try {
.text:00001760 BL              _ZN4CCatC2Ev            ; 调用CCat构造函数CCat::CCat(void)
.text:00001760 ;   } // starts at 1760
.text:00001760
.text:00001764 B               loc_1768
.text:00001764
.text:00001768 ; ---------------------------------------------------------------------------
.text:00001768
.text:00001768 loc_1768                                ; CODE XREF: main+68↑j
.text:00001768 LDR             R0, [SP,#0x40+var_38]
.text:0000176C STR             R0, [SP,#0x40+var_2C]
.text:00001770 LDR             R0, [SP,#0x40+var_2C]
.text:00001774 LDR             R1, [R0]                ; 获取CCat的虚表指针
.text:00001778 LDR             R1, [R1,#0xC]           ; 通过虚表指针+偏移获取虚函数 CCat::sleep(void)
.text:0000177C ;   try {
.text:0000177C MOV             LR, PC
.text:00001780 BX              R1                      ; 调用虚函数 CCat::sleep(void)
.text:00001780 ;   } // starts at 177C
.text:00001780
.text:00001784 B               loc_1788
.text:00001784
.text:00001788 ; ---------------------------------------------------------------------------
.text:00001788
.text:00001788 loc_1788                                ; CODE XREF: main+88↑j
.text:00001788 LDR             R0, [SP,#0x40+var_2C]
.text:0000178C STR             R0, [SP,#0x40+var_3C]
.text:00001790 CMP             R0, #0                  ; 判断地址是否为NULL
.text:00001794 BEQ             loc_17B4
.text:00001794
.text:00001798 B               loc_179C
.text:00001798
.text:0000179C ; ---------------------------------------------------------------------------
.text:0000179C
.text:0000179C loc_179C                                ; CODE XREF: main+9C↑j
.text:0000179C LDR             R0, [SP,#0x40+var_3C]
.text:000017A0 LDR             R1, [R0]                ; 获取CCat的虚表指针
.text:000017A4 LDR             R1, [R1,#4]             ; 通过虚表指针+偏移获取虚函数 CCat::~CKCat(void)
.text:000017A8 MOV             LR, PC
.text:000017AC BX              R1                      ; 调用虚函数 CCat::~CCat()
.text:000017AC
.text:000017B0 B               loc_17B4
.text:000017B0
.text:000017B4 ; ---------------------------------------------------------------------------
.text:000017B4
.text:000017B4 loc_17B4                                ; CODE XREF: main+98↑j
.text:000017B4                                         ; main+B4↑j
.text:000017B4 MOV             R0, #0
.text:000017B8 STR             R0, [R11,#var_4]
.text:000017BC SUB             R0, R11, #-var_1C       ; this
.text:000017C0 BL              _ZN4CDogD2Ev            ; CDog::~CDog()
.text:000017C0
.text:000017C4 LDR             R0, [R11,#var_4]
.text:000017C8 MOV             SP, R11
.text:000017CC POP             {R11,LR}
.text:000017D0 BX              LR


; CDog 构造函数
.text:00001808 _ZN4CDogC2Ev                            ; CODE XREF: main+24↑p
.text:00001808
.text:00001808 var_8= -8
.text:00001808 var_4= -4
.text:00001808
.text:00001808 ; __unwind {
.text:00001808 PUSH            {R11,LR}
.text:0000180C MOV             R11, SP
.text:00001810 SUB             SP, SP, #8
.text:00001814 STR             R0, [SP,#8+var_4]
.text:00001818 LDR             R0, [SP,#8+var_4]       ; this
.text:0000181C STR             R0, [SP,#8+var_8]       ; 调用父类的构造函数
.text:00001820 BL              _ZN7CAnimalC2Ev         ; CAnimal::CAnimal(void)
.text:00001820
.text:00001824 LDR             R0, [SP,#8+var_8]
.text:00001828 LDR             R1, =(_ZTV4CDog - 0x1834) ; `vtable for'CDog
.text:0000182C ADD             R1, PC, R1              ; `vtable for'CDog
.text:00001830 ADD             R1, R1, #8
.text:00001834 STR             R1, [R0]                ; 覆盖父类中的虚表
.text:00001838 MOV             R1, #0
.text:0000183C STR             R1, [R0,#0xC]
.text:00001840 MOV             SP, R11
.text:00001844 POP             {R11,LR}
.text:00001848 BX              LR


; CAnimal 构造函数
.text:000018E0 _ZN7CAnimalC2Ev                         ; CODE XREF: CDog::CDog(void)+18↑p
.text:000018E0                                         ; CCat::CCat(void)+18↑p
.text:000018E0
.text:000018E0 var_4= -4
.text:000018E0
.text:000018E0 SUB             SP, SP, #4
.text:000018E4 STR             R0, [SP,#4+var_4]
.text:000018E8 LDR             R0, [SP,#4+var_4]
.text:000018EC LDR             R1, =(_ZTV7CAnimal - 0x18F8) ; `vtable for'CAnimal
.text:000018F0 ADD             R1, PC, R1              ; `vtable for'CAnimal
.text:000018F4 ADD             R1, R1, #8
.text:000018F8 STR             R1, [R0]                ; 保存虚表指针
.text:000018FC MOV             R1, #0
.text:00001900 STR             R1, [R0,#4]
.text:00001904 STR             R1, [R0,#8]             ; 初始化成员变量
.text:00001908 ADD             SP, SP, #4
.text:0000190C BX              LR


; CCat 构造函数
.text:00001850 _ZN4CCatC2Ev                            ; CODE XREF: main+64↑p
.text:00001850
.text:00001850 var_8= -8
.text:00001850 var_4= -4
.text:00001850
.text:00001850 PUSH            {R11,LR}
.text:00001854 MOV             R11, SP
.text:00001858 SUB             SP, SP, #8
.text:0000185C STR             R0, [SP,#8+var_4]
.text:00001860 LDR             R0, [SP,#8+var_4]       ; this
.text:00001864 STR             R0, [SP,#8+var_8]
.text:00001868 BL              _ZN7CAnimalC2Ev         ; CAnimal::CAnimal(void)
.text:00001868
.text:0000186C LDR             R0, [SP,#8+var_8]
.text:00001870 LDR             R1, =(_ZTV4CCat - 0x187C) ; `vtable for'CCat
.text:00001874 ADD             R1, PC, R1              ; `vtable for'CCat
.text:00001878 ADD             R1, R1, #8
.text:0000187C STR             R1, [R0]
.text:00001880 MOV             R1, #1
.text:00001884 STR             R1, [R0,#0xC]
.text:00001888 MOV             SP, R11
.text:0000188C POP             {R11,LR}
.text:00001890 BX              LR


; CCat 代理析构函数 父类指针指向基类对象，delete父类指针时调用该析构函数
.text:00001AB4 _ZN4CCatD0Ev                            ; DATA XREF: .data.rel.ro:00002C18↓o
.text:00001AB4
.text:00001AB4 var_8= -8
.text:00001AB4 var_4= -4
.text:00001AB4
.text:00001AB4 PUSH            {R11,LR}
.text:00001AB8 MOV             R11, SP
.text:00001ABC SUB             SP, SP, #8
.text:00001AC0 STR             R0, [SP,#8+var_4]
.text:00001AC4 LDR             R0, [SP,#8+var_4]       ; this
.text:00001AC8 STR             R0, [SP,#8+var_8]
.text:00001ACC BL              _ZN4CCatD2Ev            ; CCat::~CCat()
.text:00001ACC
.text:00001AD0 LDR             R0, [SP,#8+var_8]       ; void *
.text:00001AD4 BL              _ZdlPv                  ; operator delete(void *)
.text:00001AD4
.text:00001AD8 MOV             SP, R11
.text:00001ADC POP             {R11,LR}
.text:00001AE0 BX              LR


; CCat 析构函数
.text:00001A6C PUSH            {R11,LR}
.text:00001A70 MOV             R11, SP
.text:00001A74 SUB             SP, SP, #8
.text:00001A78 STR             R0, [SP,#8+var_4]
.text:00001A7C LDR             R0, [SP,#8+var_4]       ; this
.text:00001A80 STR             R0, [SP,#8+var_8]
.text:00001A84 LDR             R1, =(_ZTV4CCat - 0x1A90) ; `vtable for'CCat
.text:00001A88 ADD             R1, PC, R1              ; `vtable for'CCat
.text:00001A8C ADD             R1, R1, #8
.text:00001A90 STR             R1, [R0]                ; 保存虚表指针
.text:00001A94 MOV             R1, #1
.text:00001A98 STR             R1, [R0,#0xC]
.text:00001A9C BL              _ZN7CAnimalD2Ev         ; 调用父类析构函数 CAnimal::~CAnimal()
.text:00001A9C
.text:00001AA0 LDR             R0, [SP,#8+var_8]
.text:00001AA4 MOV             SP, R11
.text:00001AA8 POP             {R11,LR}
.text:00001AAC BX              LR


; CAnimal 析构函数
.text:000019DC _ZN7CAnimalD2Ev                         ; CODE XREF: CDog::~CDog()+30↑p
.text:000019DC                                         ; CAnimal::~CAnimal()+18↓p
.text:000019DC                                         ; CCat::~CCat()+30↓p
.text:000019DC                                         ; DATA XREF: .data.rel.ro:00002BF4↓o
.text:000019DC
.text:000019DC var_4= -4
.text:000019DC
.text:000019DC SUB             SP, SP, #4
.text:000019E0 STR             R0, [SP,#4+var_4]
.text:000019E4 LDR             R0, [SP,#4+var_4]
.text:000019E8 LDR             R1, =(_ZTV7CAnimal - 0x19F4) ; `vtable for'CAnimal
.text:000019EC ADD             R1, PC, R1              ; `vtable for'CAnimal
.text:000019F0 ADD             R1, R1, #8
.text:000019F4 STR             R1, [R0]                ; 保存虚表指针
.text:000019F8 MOV             R1, #0
.text:000019FC STR             R1, [R0,#4]
.text:00001A00 STR             R1, [R0,#8]
.text:00001A04 ADD             SP, SP, #4
.text:00001A08 BX              LR


.data.rel.ro:00002BEC _ZTV7CAnimal DCD 0                      ; DATA XREF: CAnimal::CAnimal(void)+C↑o
.data.rel.ro:00002BEC                                         ; CAnimal::CAnimal(void)+10↑o
.data.rel.ro:00002BEC                                         ; .text:off_1910↑o
.data.rel.ro:00002BF0 DCD _ZTI7CAnimal                        ; `typeinfo for'CAnimal
.data.rel.ro:00002BF4 DCD _ZN7CAnimalD2Ev                     ; CAnimal::~CAnimal()   <--- CAnimal 虚表指针
.data.rel.ro:00002BF8 DCD _ZN7CAnimalD0Ev                     ; CAnimal::~CAnimal()
.data.rel.ro:00002BFC DCD _ZN7CAnimal3eatEv                   ; CAnimal::eat(void)
.data.rel.ro:00002C00 DCD _ZN7CAnimal5sleepEv                 ; CAnimal::sleep(void)
.data.rel.ro:00002C04 DCD _ZN7CAnimal9setweightEi             ; CAnimal::setweight(int)
.data.rel.ro:00002C08 DCD _ZN7CAnimal6setageEi                ; CAnimal::setage(int)


.data.rel.ro:00002BB8 _ZTV4CDog DCD 0                         ; DATA XREF: CDog::CDog(void)+20↑o
.data.rel.ro:00002BB8                                         ; .text:off_18DC↑o
.data.rel.ro:00002BB8                                         ; offset to this
.data.rel.ro:00002BBC DCD _ZTI4CDog                           ; `typeinfo for'CDog
.data.rel.ro:00002BC0 DCD _ZN4CDogD2Ev                        ; CDog::~CDog()   <--- CDog 虚表指针
.data.rel.ro:00002BC4 DCD _ZN4CDogD0Ev                        ; CDog::~CDog()
.data.rel.ro:00002BC8 DCD _ZN4CDog3eatEv                      ; CDog::eat(void)
.data.rel.ro:00002BCC DCD _ZN7CAnimal5sleepEv                 ; CAnimal::sleep(void)
.data.rel.ro:00002BD0 DCD _ZN7CAnimal9setweightEi             ; CAnimal::setweight(int)
.data.rel.ro:00002BD4 DCD _ZN7CAnimal6setageEi                ; CAnimal::setage(int)


.data.rel.ro:00002C0C _ZTV4CCat DCD 0                         ; DATA XREF: CCat::CCat(void)+20↑o
.data.rel.ro:00002C0C                                         ; .text:off_1AB0↑o
.data.rel.ro:00002C0C                                         ; offset to this
.data.rel.ro:00002C10 DCD _ZTI4CCat                           ; `typeinfo for'CCat
.data.rel.ro:00002C14 DCD _ZN4CCatD2Ev                        ; CCat::~CCat()   <--- CCat 虚表指针
.data.rel.ro:00002C18 DCD _ZN4CCatD0Ev                        ; CCat::~CCat()
.data.rel.ro:00002C1C DCD _ZN7CAnimal3eatEv                   ; CAnimal::eat(void)
.data.rel.ro:00002C20 DCD _ZN4CCat5sleepEv                    ; CCat::sleep(void)
.data.rel.ro:00002C24 DCD _ZN7CAnimal9setweightEi             ; CAnimal::setweight(int)
.data.rel.ro:00002C28 DCD _ZN7CAnimal6setageEi                ; CAnimal::setage(int)
```

这里需要注意的是基类的析构函数是虚函数，此时虚表中会产生两个函数指针，分别指向析构函数和代理析构函数，这样当父类指针指向子类，delete 父类指针时，会调用代理析构函数，在代理虚构函数中调用子类的虚构函数，子类析构函数调用父类析构函数，最后调用 delete 释放空间。

## 多重继承

例子：

```cpp
// multiple_inherit.cpp
#include <cstdio>

class CSofa{
public:
    CSofa(){
        m_nColor = 2;
    }
    virtual ~CSofa(){ // 沙发类虚析构函数
        printf("virtua ~CSofa()\n");
    }
    virtual int GetColor(){ // 获取沙发颜色
        return m_nColor;
    }
    virtual int SitDown(){ // 沙发可以坐下休息
        return printf("Sit down and rest yourlegs\n");
    }

protected:
    int m_nColor; // 沙发类成员变量
};

// 定义床类
class CBed{
public:
    CBed(){
        m_nLength = 4;
        m_nWidth = 5;
    }
    virtual ~CBed(){ // 床类虚析构函数
        printf("virtual~CBed()\r\n");
    }
    virtual int GetArea(){ // 获取床面积
        return m_nLength * m_nWidth;
    }
    virtual int Sleep(){ // 床可以用来睡觉
        return printf("go to sleep\r\n");
    }

protected:
    int m_nLength; // 床类成员变量
    int m_nWidth;
};
// 子类沙发床定义，派生自CSofa类和CBed类
class CSofaBed : public CSofa, public CBed
{
public:
    CSofaBed(){
        m_nHeight = 6;
    }
    virtual ~CSofaBed(){ // 沙发床类的虚析构函数
        printf("virtual~CSofaBed()\r\n");
    }
    virtual int SitDown(){ // 沙发可以坐下休息
        return printf("Sit down on the sofabed\r\n");
    }
    virtual int Sleep(){ // 床可以用来睡觉
        return printf("go to sleep on the sofabed\r\n");
    }
    virtual int GetHeight(){
        return m_nHeight;
    }

protected:
    int m_nHeight; // 沙发类的成员变量
};

int main(int argc, char const *argv[]){
    CSofaBed SofaBed;
    CSofa* pSofa=&SofaBed;
    pSofa->SitDown();
    CBed* pBed=&SofaBed;
    pBed->Sleep();

    pSofa = new CSofaBed;
    pSofa->SitDown();
    pBed = dynamic_cast<CBed*>(pSofa);
    pBed->Sleep();

    delete pSofa;

    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -lstdc++  multiple_inherit.cpp -o multiple_inherit
```

在 IDA 中对应的反汇编代码。

```armasm
; 构造函数会根据继承顺序分别调用父类的构造函数
.text:00001A48 PUSH            {R11,LR}
.text:00001A4C MOV             R11, SP
.text:00001A50 SUB             SP, SP, #0x10
.text:00001A54 STR             R0, [R11,#var_4]
.text:00001A58 LDR             R0, [R11,#var_4]        ; this
.text:00001A5C STR             R0, [SP,#0x10+var_10]   ; 调用CSofa构造函数
.text:00001A60 BL              _ZN5CSofaC2Ev           ; CSofa::CSofa(void)
.text:00001A60
.text:00001A64 LDR             R0, [SP,#0x10+var_10]
.text:00001A68 ADD             R0, R0, #8              ; this
.text:00001A6C ;   try {
.text:00001A6C BL              _ZN4CBedC2Ev            ; CBed::CBed(void)
.text:00001A6C ;   } // starts at 1A6C
.text:00001A6C
.text:00001A70 B               loc_1A74
.text:00001A70
.text:00001A74 ; ---------------------------------------------------------------------------
.text:00001A74
.text:00001A74 loc_1A74                                ; CODE XREF: CSofaBed::CSofaBed(void)+28↑j
.text:00001A74 LDR             R0, [SP,#0x10+var_10]
.text:00001A78 LDR             R1, =(_ZTV8CSofaBed - 0x1A84) ; `vtable for'CSofaBed
.text:00001A7C ADD             R1, PC, R1              ; `vtable for'CSofaBed
.text:00001A80 ADD             R2, R1, #8
.text:00001A84 STR             R2, [R0]
.text:00001A88 ADD             R1, R1, #0x28 ; '('
.text:00001A8C STR             R1, [R0,#8]
.text:00001A90 MOV             R1, #6
.text:00001A94 STR             R1, [R0,#0x14]
.text:00001A98 MOV             SP, R11
.text:00001A9C POP             {R11,LR}
.text:00001AA0 BX              LR

; 析构函数会根据继承顺序反向调用父类析构函数
.text:00001AC4 PUSH            {R11,LR}
.text:00001AC8 MOV             R11, SP
.text:00001ACC SUB             SP, SP, #0x10
.text:00001AD0 STR             R0, [R11,#var_4]
.text:00001AD4 LDR             R1, [R11,#var_4]
.text:00001AD8 STR             R1, [SP,#0x10+var_10]
.text:00001ADC LDR             R0, =(_ZTV8CSofaBed - 0x1AE8) ; `vtable for'CSofaBed
.text:00001AE0 ADD             R0, PC, R0              ; `vtable for'CSofaBed
.text:00001AE4 ADD             R2, R0, #8
.text:00001AE8 STR             R2, [R1]
.text:00001AEC ADD             R0, R0, #0x28 ; '('
.text:00001AF0 STR             R0, [R1,#8]
.text:00001AF4 ;   try {
.text:00001AF4 LDR             R0, =(aVirtualCsofabe - 0x1B00) ; "virtual~CSofaBed()\r\n"
.text:00001AF8 ADD             R0, PC, R0              ; "virtual~CSofaBed()\r\n"
.text:00001AFC BL              printf
.text:00001AFC ;   } // starts at 1AF4
.text:00001AFC
.text:00001B00 B               loc_1B04
.text:00001B00
.text:00001B04 ; ---------------------------------------------------------------------------
.text:00001B04
.text:00001B04 loc_1B04                                ; CODE XREF: CSofaBed::~CSofaBed()+3C↑j
.text:00001B04 LDR             R0, [SP,#0x10+var_10]
.text:00001B08 ADD             R0, R0, #8              ; this
.text:00001B0C BL              _ZN4CBedD2Ev            ; CBed::~CBed()
.text:00001B0C
.text:00001B10 LDR             R0, [SP,#0x10+var_10]   ; this
.text:00001B14 BL              _ZN5CSofaD2Ev           ; CSofa::~CSofa()
.text:00001B14
.text:00001B18 LDR             R0, [SP,#0x10+var_10]
.text:00001B1C MOV             SP, R11
.text:00001B20 POP             {R11,LR}
.text:00001B24 BX              LR
```

创建 CSofaBed 对象时，调用 CSofaBed 的构造函数，在 CSofaBed 的构造函数中先调用 CSofa 的构造函数，然后调用 CBed 构造函数，其内存布局变化如下：

```
    CSofa构造函数                  CBed构造函数             CSofaBed构造函数

偏移         内存
+0        vtable for CSofa                                 vtable for CSofaBed
+4        m_nColor
+8                                 vtable for CBed         vtable for CSofaBed + 0x20
+12                                m_nLength
+16                                m_nWidth
+20                                                        m_nHeight

对应的虚表

vtable for CSofa
+0x0                 DCD _ZN5CSofaD2Ev                       ; CSofa::~CSofa()
+0x4                 DCD _ZN5CSofaD0Ev                       ; CSofa::~CSofa()
+0x8                 DCD _ZN5CSofa8GetColorEv                ; CSofa::GetColor(void)
+0xC                 DCD _ZN5CSofa7SitDownEv                 ; CSofa::SitDown(void)

vtable for CBed
+0x0                 DCD _ZN4CBedD2Ev                        ; CBed::~CBed()
+0x4                 DCD _ZN4CBedD0Ev                        ; CBed::~CBed()
+0x8                 DCD _ZN4CBed7GetAreaEv                  ; CBed::GetArea(void)
+0xC                 DCD _ZN4CBed5SleepEv                    ; CBed::Sleep(void)

vtable for CSofaBed
+0x0                 DCD _ZN8CSofaBedD2Ev                    ; CSofaBed::~CSofaBed()
+0x4                 DCD _ZN8CSofaBedD0Ev                    ; CSofaBed::~CSofaBed()
+0x8                 DCD _ZN5CSofa8GetColorEv                ; CSofa::GetColor(void)
+0xC                 DCD _ZN8CSofaBed7SitDownEv              ; CSofaBed::SitDown(void)
+0x10                DCD _ZN8CSofaBed5SleepEv                ; CSofaBed::Sleep(void)
+0x14                DCD _ZN8CSofaBed9GetHeightEv            ; CSofaBed::GetHeight(void)
+0x18                DCD -8                                  ; offset to this
+0x1C                DCD _ZTI8CSofaBed                       ; `typeinfo for'CSofaBed
+0x20                DCD _ZThn8_N8CSofaBedD1Ev               ; `non-virtual thunk to'CSofaBed::~CSofaBed()
+0x20 + 0x4          DCD _ZThn8_N8CSofaBedD0Ev               ; `non-virtual thunk to'CSofaBed::~CSofaBed()
+0x20 + 0x8          DCD _ZN4CBed7GetAreaEv                  ; CBed::GetArea(void)
+0x20 + 0xC          DCD _ZThn8_N8CSofaBed5SleepEv           ; `non-virtual thunk to'CSofaBed::Sleep(void)
```

使用 O2 编译选项进行优化：

```bash
➜  clang -target arm-linux-android21 -lstdc++ -O2 multiple_inherit.cpp -o multiple_inherit
```

在 IDA 中对应的反汇编代码。

```armasm
.text:0000187C PUSH            {R4-R11,LR}
.text:00001880 ADD             R11, SP, #0x1C
.text:00001884 SUB             SP, SP, #0x1C
.text:00001888 MOV             R8, #5
.text:0000188C MOV             R9, #4
.text:00001890 MOV             R5, #2
.text:00001894 MOV             R10, #6
.text:00001898 STR             R8, [SP,#0x38+var_24]
.text:0000189C STR             R9, [SP,#0x38+var_28]
.text:000018A0 STR             R5, [SP,#0x38+var_30]
.text:000018A4 STR             R10, [SP,#0x38+var_20]
.text:000018A8 LDR             R0, =(_ZTV8CSofaBed - 0x18B4) ; `vtable for'CSofaBed
.text:000018AC ADD             R0, PC, R0              ; `vtable for'CSofaBed
.text:000018B0 ADD             R6, R0, #0x28 ; '('
.text:000018B4 ADD             R7, R0, #8
.text:000018B8 STR             R6, [SP,#0x38+var_2C]
.text:000018BC STR             R7, [SP,#0x38+var_34]
.text:000018C0 LDR             R0, =(aSitDownOnTheSo - 0x18CC) ; "Sit down on the sofabed\r"
.text:000018C4 ADD             R0, PC, R0              ; "Sit down on the sofabed\r"
.text:000018C8 BL              puts
.text:000018C8
.text:000018CC LDR             R0, =(aGoToSleepOnThe - 0x18D8) ; "go to sleep on the sofabed\r"
.text:000018D0 ADD             R0, PC, R0              ; "go to sleep on the sofabed\r"
.text:000018D4 BL              puts
.text:000018D4
.text:000018D8 ;   try {
.text:000018D8 MOV             R0, #0x18               ; unsigned int
.text:000018DC BL              _Znwj                   ; operator new(uint)
.text:000018DC
.text:000018E0 STR             R7, [R0]
.text:000018E4 STMIB           R0, {R5,R6,R9}
.text:000018E8 STR             R8, [R0,#0x10]
.text:000018EC STR             R10, [R0,#0x14]
.text:000018F0 MOV             R4, R0
.text:000018F4 LDR             R0, =(aSitDownOnTheSo - 0x1900) ; "Sit down on the sofabed\r"
.text:000018F8 ADD             R0, PC, R0              ; "Sit down on the sofabed\r"
.text:000018FC BL              puts
.text:000018FC
.text:00001900 LDR             R1, =(_ZTI5CSofa - 0x1918) ; `typeinfo for'CSofa
.text:00001904 LDR             R2, =(_ZTI4CBed - 0x191C) ; `typeinfo for'CBed
.text:00001908 MOV             R0, R4                  ; lpsrc
.text:0000190C MOV             R3, #0xFFFFFFFE         ; s2d
.text:00001910 ADD             R1, PC, R1              ; lpstype
.text:00001914 ADD             R2, PC, R2              ; lpdtype
.text:00001918 BL              __dynamic_cast
.text:00001918
.text:0000191C LDR             R1, [R0]
.text:00001920 LDR             R1, [R1,#0xC]
.text:00001924 MOV             LR, PC
.text:00001928 BX              R1
.text:00001928 ;   } // starts at 18D8
.text:00001928
.text:0000192C LDR             R0, [R4]
.text:00001930 LDR             R1, [R0,#4]
.text:00001934 MOV             R0, R4
.text:00001938 MOV             LR, PC
.text:0000193C BX              R1
.text:0000193C
.text:00001940 LDR             R0, =(aVirtualCsofabe - 0x194C) ; "virtual~CSofaBed()\r"
.text:00001944 ADD             R0, PC, R0              ; "virtual~CSofaBed()\r"
.text:00001948 BL              puts
.text:00001948
.text:0000194C LDR             R0, =(aVirtualCbed - 0x1958) ; "virtual~CBed()\r"
.text:00001950 ADD             R0, PC, R0              ; "virtual~CBed()\r"
.text:00001954 BL              puts
.text:00001954
.text:00001958 LDR             R0, =(aVirtuaCsofa - 0x1964) ; "virtua ~CSofa()"
.text:0000195C ADD             R0, PC, R0              ; "virtua ~CSofa()"
.text:00001960 BL              puts
.text:00001960
.text:00001964 MOV             R0, #0
.text:00001968 SUB             SP, R11, #0x1C
.text:0000196C POP             {R4-R11,LR}
.text:00001970 BX              LR
```

经过优化后，成员函数都内联了，只能通过虚表指针判断多继承关系。

## 虚基类

例子：

```cpp
// pure_virtual.cpp
#include <cstdio>

class CVirtualBase{
public:
    virtual void Show() = 0; // 定义纯虚函数
};
class CVirtualChild : public CVirtualBase { // 定义继承虚基类的子类
public:
    virtual void Show(){ // 实现纯虚函数
        printf("虚基类分析\r\n");
    }
};
int main(int argc, char *argv[])
{
    CVirtualChild VirtualChild;
    VirtualChild.Show();
    return 0;
}
```

#### ARMv7

使用下列命令生成 ARMv7 可执行文件。

```bash
➜  clang -target arm-linux-android21 -lstdc++  pure_virtual.cpp -o pure_virtual
```

对应大构造函数以及虚基类的虚表：

```armasm
.text:000015AC 00 48 2D E9 PUSH            {R11,LR}
.text:000015B0 0D B0 A0 E1 MOV             R11, SP
.text:000015B4 08 D0 4D E2 SUB             SP, SP, #8
.text:000015B8 04 00 8D E5 STR             R0, [SP,#8+var_4]
.text:000015BC 04 00 9D E5 LDR             R0, [SP,#8+var_4]       ; this
.text:000015C0 00 00 8D E5 STR             R0, [SP,#8+var_8]
.text:000015C4 13 00 00 EB BL              _ZN12CVirtualBaseC2Ev   ; CVirtualBase::CVirtualBase(void)
.text:000015C4
.text:000015C8 00 00 9D E5 LDR             R0, [SP,#8+var_8]
.text:000015CC 14 10 9F E5 LDR             R1, =(_ZTV13CVirtualChild - 0x15D8) ; `vtable for'CVirtualChild
.text:000015D0 01 10 8F E0 ADD             R1, PC, R1              ; `vtable for'CVirtualChild
.text:000015D4 08 10 81 E2 ADD             R1, R1, #8
.text:000015D8 00 10 80 E5 STR             R1, [R0]
.text:000015DC 0B D0 A0 E1 MOV             SP, R11
.text:000015E0 00 48 BD E8 POP             {R11,LR}
.text:000015E4 1E FF 2F E1 BX              LR
.text:000015E4


.text:00001618             _ZN12CVirtualBaseC2Ev                   ; CODE XREF: CVirtualChild::CVirtualChild(void)+18↑p
.text:00001618
.text:00001618             var_4= -4
.text:00001618
.text:00001618 04 D0 4D E2 SUB             SP, SP, #4
.text:0000161C 00 00 8D E5 STR             R0, [SP,#4+var_4]
.text:00001620 00 00 9D E5 LDR             R0, [SP,#4+var_4]
.text:00001624 10 10 9F E5 LDR             R1, =(_ZTV12CVirtualBase - 0x1630) ; `vtable for'CVirtualBase
.text:00001628 01 10 8F E0 ADD             R1, PC, R1              ; `vtable for'CVirtualBase
.text:0000162C 08 10 81 E2 ADD             R1, R1, #8
.text:00001630 00 10 80 E5 STR             R1, [R0]
.text:00001634 04 D0 8D E2 ADD             SP, SP, #4
.text:00001638 1E FF 2F E1 BX              LR



.data.rel.ro:000026C8 00 00 00 00 _ZTV12CVirtualBase DCD 0                ; DATA XREF: CVirtualBase::CVirtualBase(void)+C↑o
.data.rel.ro:000026C8                                                     ; CVirtualBase::CVirtualBase(void)+10↑o
.data.rel.ro:000026C8                                                     ; .text:off_163C↑o
.data.rel.ro:000026C8                                                     ; offset to this
.data.rel.ro:000026CC B4 26 00 00 DCD _ZTI12CVirtualBase                  ; `typeinfo for'CVirtualBase
.data.rel.ro:000026D0 00 38 00 00 DCD __cxa_pure_virtual
```

## 菱形继承

例子：

```cpp
#include <cstdio>

class CFurniture
{
public:
    CFurniture(){
        m_nPrice = 0;
    }
    virtual ~CFurniture(){ // 家具类的虚析构函数
        printf("virtual ~CFurniture()\r\n");
    }
    virtual int GetPrice(){ // 获取家具价格
        return m_nPrice;
    };

protected:
    int m_nPrice; // 家具类的成员变量
};

class CSofa : virtual public CFurniture
{
public:
    CSofa(){
        m_nPrice = 1;
        m_nColor = 2;
    }
    virtual ~CSofa(){ // 沙发类虚析构函数
        printf("virtual ~CSofa()\r\n");
    }
    virtual int GetColor(){ // 获取沙发颜色
        return m_nColor;
    }
    virtual int SitDown(){ // 沙发可以坐下休息
        return printf("Sit down and rest your legs\r\n");
    }
protected:
    int m_nColor; // 沙发类成员变量
};

class CBed : virtual public CFurniture
{
public:
    CBed(){
        m_nPrice = 3;
        m_nLength = 4;
        m_nWidth = 5;
    }
    virtual ~CBed(){ // 床类的虚析构函数
        printf("virtual ~CBed()\r\n");
    }
    virtual int GetArea(){ // 获取床面积
        return m_nLength * m_nWidth;
    }
    virtual int Sleep(){ // 床可以用来睡觉
        return printf("go to sleep\r\n");
    }

protected:
    int m_nLength; // 床类成员变量
    int m_nWidth;
};

class CSofaBed : public CSofa, public CBed
{
public:
    CSofaBed(){
        m_nHeight = 6;
    }
    virtual ~CSofaBed(){ // 沙发床类的虚析构函数
        printf("virtual ~CSofaBed()\r\n");
    }
    virtual int SitDown(){ // 沙发可以坐下休息
        return printf("Sit down on the sofabed\r\n");
    }
    virtual int Sleep(){ // 床可以用来睡觉
        return printf("go to sleep on the sofabed\r\n");
    }
    virtual int GetHeight()
    {
return m_nHeight;
    }

protected:
    int m_nHeight; // 沙发类的成员变量
};
int main(int argc, char *argv[])
{
    CSofaBed SofaBed;
    return 0;
}
```

对应的构造函数：

```armasm
.text:00001A3C PUSH            {R11,LR}
.text:00001A40 MOV             R11, SP
.text:00001A44 SUB             SP, SP, #0x10
.text:00001A48 STR             R0, [R11,#var_4]
.text:00001A4C LDR             R0, [R11,#var_4]
.text:00001A50 STR             R0, [SP,#0x10+var_10]
.text:00001A54 ADD             R0, R0, #0x18           ; this
.text:00001A58 BL              _ZN10CFurnitureC2Ev     ; CFurniture::CFurniture(void)
.text:00001A58
.text:00001A5C LDR             R0, [SP,#0x10+var_10]   ; this
.text:00001A60 ;   try {
.text:00001A60 LDR             R1, =(_ZTT8CSofaBed - 0x1A6C) ; `VTT for'CSofaBed
.text:00001A64 ADD             R1, PC, R1              ; `VTT for'CSofaBed
.text:00001A68 ADD             R1, R1, #4
.text:00001A6C BL              _ZN5CSofaC2Ev           ; CSofa::CSofa(void)
.text:00001A6C ;   } // starts at 1A60
.text:00001A6C
.text:00001A70 B               loc_1A74
.text:00001A70
.text:00001A74 ; ---------------------------------------------------------------------------
.text:00001A74
.text:00001A74 loc_1A74                                ; CODE XREF: CSofaBed::CSofaBed(void)+34↑j
.text:00001A74 LDR             R0, [SP,#0x10+var_10]
.text:00001A78 ADD             R0, R0, #8              ; this
.text:00001A7C ;   try {
.text:00001A7C LDR             R1, =(_ZTT8CSofaBed - 0x1A88) ; `VTT for'CSofaBed
.text:00001A80 ADD             R1, PC, R1              ; `VTT for'CSofaBed
.text:00001A84 ADD             R1, R1, #0xC
.text:00001A88 BL              _ZN4CBedC2Ev            ; CBed::CBed(void)
.text:00001A88 ;   } // starts at 1A7C
.text:00001A88
.text:00001A8C B               loc_1A90
.text:00001A8C
.text:00001A90 ; ---------------------------------------------------------------------------
.text:00001A90
.text:00001A90 loc_1A90                                ; CODE XREF: CSofaBed::CSofaBed(void)+50↑j
.text:00001A90 LDR             R0, [SP,#0x10+var_10]
.text:00001A94 LDR             R1, =(_ZTV8CSofaBed - 0x1AA0) ; `vtable for'CSofaBed
.text:00001A98 ADD             R1, PC, R1              ; `vtable for'CSofaBed
.text:00001A9C ADD             R2, R1, #0xC
.text:00001AA0 STR             R2, [R0]
.text:00001AA4 ADD             R2, R1, #0x50 ; 'P'
.text:00001AA8 STR             R2, [R0,#0x18]
.text:00001AAC ADD             R1, R1, #0x30 ; '0'
.text:00001AB0 STR             R1, [R0,#8]
.text:00001AB4 MOV             R1, #6
.text:00001AB8 STR             R1, [R0,#0x14]
.text:00001ABC MOV             SP, R11
.text:00001AC0 POP             {R11,LR}
.text:00001AC4 BX              LR

.text:00001B60 _ZN10CFurnitureC2Ev                     ; CODE XREF: CSofaBed::CSofaBed(void)+1C↑p
.text:00001B60
.text:00001B60 var_4= -4
.text:00001B60
.text:00001B60 SUB             SP, SP, #4
.text:00001B64 STR             R0, [SP,#4+var_4]
.text:00001B68 LDR             R0, [SP,#4+var_4]
.text:00001B6C LDR             R1, =(_ZTV10CFurniture - 0x1B78) ; `vtable for'CFurniture
.text:00001B70 ADD             R1, PC, R1              ; `vtable for'CFurniture
.text:00001B74 ADD             R1, R1, #8
.text:00001B78 STR             R1, [R0]
.text:00001B7C MOV             R1, #0
.text:00001B80 STR             R1, [R0,#4]
.text:00001B84 ADD             SP, SP, #4
.text:00001B88 BX              LR

.text:00001B90 _ZN5CSofaC2Ev                           ; CODE XREF: CSofaBed::CSofaBed(void)+30↑p
.text:00001B90
.text:00001B90 var_8= -8
.text:00001B90 var_4= -4
.text:00001B90
.text:00001B90 SUB             SP, SP, #8
.text:00001B94 STR             R0, [SP,#8+var_4]
.text:00001B98 STR             R1, [SP,#8+var_8]
.text:00001B9C LDR             R0, [SP,#8+var_4]
.text:00001BA0 LDR             R1, [SP,#8+var_8]
.text:00001BA4 LDR             R2, [R1]
.text:00001BA8 STR             R2, [R0]
.text:00001BAC LDR             R1, [R1,#4]
.text:00001BB0 LDR             R2, [R0]
.text:00001BB4 LDR             R2, [R2,#-0xC]
.text:00001BB8 STR             R1, [R0,R2]
.text:00001BBC LDR             R1, [R0]
.text:00001BC0 LDR             R1, [R1,#-0xC]
.text:00001BC4 ADD             R2, R0, R1
.text:00001BC8 MOV             R1, #1
.text:00001BCC STR             R1, [R2,#4]
.text:00001BD0 MOV             R1, #2
.text:00001BD4 STR             R1, [R0,#4]
.text:00001BD8 ADD             SP, SP, #8
.text:00001BDC BX              LR

.text:00001BE0 _ZN4CBedC2Ev                            ; CODE XREF: CSofaBed::CSofaBed(void)+4C↑p
.text:00001BE0
.text:00001BE0 var_8= -8
.text:00001BE0 var_4= -4
.text:00001BE0
.text:00001BE0 SUB             SP, SP, #8
.text:00001BE4 STR             R0, [SP,#8+var_4]
.text:00001BE8 STR             R1, [SP,#8+var_8]
.text:00001BEC LDR             R0, [SP,#8+var_4]
.text:00001BF0 LDR             R1, [SP,#8+var_8]
.text:00001BF4 LDR             R2, [R1]
.text:00001BF8 STR             R2, [R0]
.text:00001BFC LDR             R1, [R1,#4]
.text:00001C00 LDR             R2, [R0]
.text:00001C04 LDR             R2, [R2,#-0xC]
.text:00001C08 STR             R1, [R0,R2]
.text:00001C0C LDR             R1, [R0]
.text:00001C10 LDR             R1, [R1,#-0xC]
.text:00001C14 ADD             R2, R0, R1
.text:00001C18 MOV             R1, #3
.text:00001C1C STR             R1, [R2,#4]
.text:00001C20 MOV             R1, #4
.text:00001C24 STR             R1, [R0,#4]
.text:00001C28 MOV             R1, #5
.text:00001C2C STR             R1, [R0,#8]
.text:00001C30 ADD             SP, SP, #8
.text:00001C34 BX              LR
```

CSofaBed 对应的内存布局：

```
        CFurniture构造     CSofa构造       CBed构造            CSofaBed构造
偏移    内存
+0x0                       vt_CSofa                           vtable for CSofaBed
+0x4                       m_nColor
+0x8                                      vt_CBed             vt_CBed
+0xC                                      m_nLength
+0x10                                     m_nWidth
+0x14                                                         m_nHeight
+0x18  vt_CFurniture                                          vt_CFurniture
+0x1C  m_nPrice
```
