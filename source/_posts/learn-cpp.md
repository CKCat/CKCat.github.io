---
title: learn-cpp
date: 2019-12-20 16:19:10
tags: C++
category: C++
---

# 基础

## `const`、引用和函数默认参数的使用及注意事项

1. `const` 的作用
   - 取代无参宏，增加类型检查功能。
   - 对指针类型做一些限定。

`const` 主要是语法层面的限定，实际上可以通过内存修改 `const` 变量。

```cpp
#include <stdio.h>

#define PI 3.14f

int main(int argc, char** argv) {

    printf("PI: %f\r\n", PI);   // 3.140000
    // const 取代无参宏，方便调试，增加类型检查。
    const float CPI = 3.14f;
    printf("CPI: %f\r\n", CPI);  // 3.140000
    *(float*)&CPI = 2.14f;
    // 编译中直接替换为常量了，所以修改了CPI的值输出也是3.140000。
    printf("CPI: %f\r\n", CPI);  // 3.140000

    // const 指针
    // const 在 * 前面，则指针指向的内容无法修改。
    const char* pbuffer = "hello";
    pbuffer = NULL;
    // pbuffer[0] = 'H';  这条语句会出错，表达式必须是可修改的左值。

    // const 在 * 后面，则指针变量的值无法修改。
    char szBuffer[] = "world";  // 等价于  char * const szBuffer = "hello"
    char* const cbuffer = szBuffer;
    // cbuffer = NULL; 表达式必须是可修改的左值。
    cbuffer[0] = 'W';

    // const 在 * 前后都有，则指向的内容和指针的值都无法修改。
    const char* const ccbuffer = szBuffer;
    // ccbuffer = NULL; 表达式必须是可修改的左值。
    // ccbuffer[0] = 'h';表达式必须是可修改的左值。
    szBuffer[1] = 'O'; // 语法上不可修改，实际上可以通过内存修改
    // printf("pbuffer: %s\r\n", pbuffer);
    printf("cbuffer: %s\r\n", cbuffer);
    printf("ccbuffer: %s\r\n", ccbuffer);
}
```

其中 const 和非 const 指针转换问题如下：

```cpp
const TYPE * = TYPE *;    // 正确
TYPE* = const TYPE*;      // 错误
```

2. 引用注意事项
   - 引用必须初始化。
   - 引用不能初始化为常量(存在特例)。
   - 没有二级引用。

```cpp
#include <stdio.h>

void swap_point(int* a, int* b) {
    int n = *a;
    *a = *b;
    *b = n;
}

void swap_ref(int& a, int& b) {
    int n = a;
    a = b;
    b = n;
}

int main(int argc, char** argv) {

    int a = 1, b = 2;
    // 引用必须初始化
    int& ref_a = a;
    int& ref_b = b;
    printf("ref_a = %d, ref_b= %d\r\n", ref_a, ref_b);
    swap_point(&a, &b);
    printf("a = %d, b= %d\r\n", a, b);
    printf("ref_a = %d, ref_b= %d\r\n", ref_a, ref_b);
    swap_ref(ref_a, ref_b);
    printf("a = %d, b= %d\r\n", a, b);
    printf("ref_a = %d, ref_b= %d\r\n", ref_a, ref_b);

    // int& const_value = 1;   // error C2440: 'initializing': cannot convert from 'int' to 'int &'
    // int&& n = ref_a;        // error C2440: 'initializing': cannot convert from 'int' to 'int &&'
    // 下面相当于 int tmp = 2; const int &c_ref = tmp;
    const int& c_ref = 0x12345678;
    printf("address: %p\r\n", &c_ref);  // 这里可以发现输出的为栈地址。
}
```

引用与指针无本质区别，只是语法上的区别，下列为 IDA 中反汇编后的结果。

```asm
mov     [ebp+a], 1
mov     [ebp+b], 2
lea     eax, [ebp+a]
mov     [ebp+ref_a], eax
lea     eax, [ebp+b]
mov     [ebp+ref_b], eax
```

3. 默认参数注意事项
   - 缺省参数只能放在最右边。
   - 声明和实现不能同时定义缺省，只能写一个。
   - 缺省参数通常放在声明文件中。

```cpp
#include <stdio.h>

int add(int a, int b = 1) {
    return a + b;
}

int main(int argc, char** argv) {
    int sum = add(1);
    printf("%d\r\n", sum);
    sum = add(1, 2);
    printf("%d\r\n", sum);
    return 0;
}
```

## 类

1. `class` 类默认 `private`；`struct` 类和 `union` 类默认公有，其中 `union` 类内存共享。
2. 不同的对象，数据成员内存地址不一样。
3. 调用成员函数时会隐藏传递 `this` 指针，默认会用寄存器（ECX）传递。
4. 使用 `__stdcall` 方式的函数调用时， `this` 指针直接入栈传递。
5. 类的声明一般定义在头文件中，实现在另外的 cpp 文件中。

### 内联 inline

在 C++中使用 inline 函数也是直接替换内容，而且有参数类型检查，就解决了宏的类型检查问题，可以在编译期间就避免类型不一致的错误。

内联的一些情况:

- debug 版不内联。
- release 版才会内联，inline 告诉编译器需要内联，编译器会根据函数的复杂度来确定是否直接替换。
- 直接在类中定义并实现的函数默认内联。

**类成员函数内联的定义和实现一般都是写在同一个文件中，主要是为了防止重复定义以及避免编译器在链接过程中直接替换函数时找不到函数定义。**

例如:

```cpp
// CInline.h
#pragma once
class CInline
{
public:
    inline int add(int, int);
};

// CInline.cpp
#include "CInline.h"

int CInline::add(int a, int b) {
    return a + b;
}

// main.cpp
#include "CInline.h"

int main(int argc, char** argv) {
    CInline clinne;
    int sum = clinne.add(1, 2);
    printf("%d\r\n", sum);
}
```

以上代码就会出现下列错误，将 `CInline::add` 的实现写到 `CInline.h` 中就可以编译通过了。

```bash
error LNK2019: unresolved external symbol "public: int __thiscall CInline::add(int,int)" (?add@CInline@@QAEHHH@Z) referenced in function _main
```

### 函数重载

重载实现编译器期的多态。

重载的定义：函数名相同，参数个数不同 或 参数类型不同 或 参数顺序不同即可构成重载。

注意点：

- 返回值不作参考，即返回值不同也不构成重载。
- `TYPE` 和 `const TPYE` 不构成重载，如 `add(int)` 和 `add(const int)` 不构成重载。
- `TYPE*` 和 `const TPYE*` 可以重载，如 `add(int*)` 和 `add(const int*)` 构成重载。
- 引用可以重载，如 `add(int)` 和 `add(int&)` 构成重载。
- `typedef` 无效，如 `typedef int T;` `add(T)` 和 `add(int)` 不构成重载。

### 构造和析构

构造函数是一个特殊的成员函数，主要作用是初始化，申请资源；其注意点如下：

- 不能显示调用，即使用者不能调用。
- 没有返回值。
- 可以重载。

析构函数也是特殊的成员函数，主要作用是释放资源；其注意点如下：

- 可以显示调用。
- 没有返回值。
- 不能有参数，所以不能重载。

编译器一般会自动为一个类生成六个特殊的成员函数，

| 成员函数                | class C 对应的形式       |
| ----------------------- | ------------------------ |
| 默认构造函数            | C::C();                  |
| 析构函数                | C::~C();                 |
| 复制构造函数            | C::C (const C&);         |
| 复制赋值运算符          | C& operator= (const C&); |
| 移动构造函数（C++11）   | C::C (C&&);              |
| 移动赋值运算符（C++11） | C& operator= (C&&);      |

以上六个成员函数如果不用，最好用 `delete` 标记一下，避免出现非预期结果。

复制构造函数和复制赋值运算符默认情况下直接将对象进行整体复制，如果对象数据成员有指针变量时，就会复制指针变量而不是指针对应的数据，就造成了两个对象中的指针变量指向了同一个地址，与预期的结果是不一样的，还有一个问题就是析构函数会两次释放资源，造成错误。

例如：

```cpp
#include <string.h>

class Copy
{
public:
    int m_Number;
    char* m_pszBuff;
public:
    Copy(int number, const char* szbuf = nullptr) {
        init(number, szbuf);
    }
    ~Copy() {
        if (m_pszBuff){
            delete[] m_pszBuff;
            m_pszBuff = nullptr;
        }
    }
    void init(int number, const char* szbuf) {
        m_Number = number;
        if (szbuf == nullptr){
            m_pszBuff = nullptr;
        }else{
            m_pszBuff = new char[strlen(szbuf) + 1];
            strcpy(m_pszBuff, szbuf);
        }
    }
};


int main(int argc, char** argv) {
    Copy p1(1, "Hello");
    Copy p2 = p1;
    Copy p3(p1);
    printf("p1.m_Number address: %p, p1.m_pszBuff address: %p\r\n", &p1.m_Number, p1.m_pszBuff);
    printf("p2.m_Number address: %p, p2.m_pszBuff address: %p\r\n", &p2.m_Number, p2.m_pszBuff);
    printf("p3.m_Number address: %p, p3.m_pszBuff address: %p\r\n", &p3.m_Number, p3.m_pszBuff);
}
```

上面代码运行后可以发现 m_pszBuff 的内存地址为同一个值，然后析构函数会多次释放内存导致错误。

函数参数尽量使用引用作为参数，应为如果使用对象作为参数，将会多执行一次拷贝构造和一次析构，导致性能损失。

一个类的成员包含另一个类的析构和构造函数调用情况：

- 构造顺序：先根据定义的顺序构造成员对象，再构造自己。
- 析构顺序与析构顺序相反：先析构自己，再根据定义的顺序反向析构成员对象。

例如:

```cpp
#include <iostream>
using namespace std;
class ClassA
{
public:
    ClassA(int count = 0){
        mCount = count;
        cout << "ClassA() " << mCount << endl;
    }
    ~ClassA(){
        cout << "~ClassA() " << mCount << endl;
    }
private:
    int mCount;
};

class ClassB
{
private:
    ClassA d1;
    ClassA d2;
public:
    ClassB():d1(1), d2(2) {
        cout << "ClassB() " << endl;
    }
    ~ClassB() {
        cout << "ClassB() " << endl;
    }
};
int main(int argc, char** argv) {
    ClassB b;
}
```

运行结果如下：

```bash
ClassA() 1
ClassA() 2
ClassB()
ClassB()
~ClassA() 2
~ClassA() 1
```

## static 和 const

### namespace

- 命名空间也是为了解决访问权限和命名重复的问题。
- 当两个命名空间名是一样的就会合并为一个。
- 命名空间尽量确保唯一性。
- 可以嵌套使用类名空间，但是尽量不要这样使用。

例子：

```cpp
#include <iostream>
#include <string.h>

using namespace std;

namespace Mystring{
    char *strcpy(char *strDestination, const char *strSource){
        return NULL;
    }
    int g_nCount;
}

int g_nCount = 0;

int main(){
    char szText[10] = { 0 };
    int g_nCount = 2;
    //strcpy(szText, "hello");          //error，因为不知道用标准库函数还是用Mystring中的函数
    Mystring::strcpy(szText, "hello");  //调用的是Mystring里面的strcpy
    Mystring::g_nCount = 1;             //调用的是Mystring里面的strcpy
    ::strcpy(szText, "hello");          //调用的是全局strcpy, ::前面没有命名空间表示全局的
    g_nCount = 10;                      //调用的是main函数中的g_nCount
    ::g_nCount = 20;                    //调用全局的g_nCount
    return 0;
}
```

### static

1. 静态成员变量

- 静态成员变量只限于类里面访问，并且不同的类可以定义相同的变量名，可以避免全局变量的权限范围过大以及命名空间问题。
- 静态成员变量和普通成员变量的定义方式是一样的，并且不占用对象空间。
- 静态数据成员使用前必须要先初始化，初始化方式：类名::static 数据成员名 = 0;

2. 静态成员函数

- 静态成员函数出现的原因和静态成员函数出现的原因一样，也是因为权限和命名空间的问题。
- 静态成员函数中不含 this 指针，所以不能访问普通数据成员，只能访问静态数据成员。
- 调用方式：类名::静态成员函数名 或者 对象.静态成员函数名。

例如：

```cpp
class CTestStatic
{
private:
    int mNumber;
    static int mCount;    //不占用对象的空间
public:
    CTestStatic()
    {
        mNumber = 0;
        mCount++;         //每个对象修改的都是同一个m_nCount
        cout << "CTestStatic() mCount = " << mCount << endl;
    }
    ~CTestStatic()
    {
        cout << "~CTestStatic() mCount = " << mCount<< endl;
    }

    static int getCount()
    {
        //因为静态成员函数没有传递this指针，不能访问普通数据成员
        //只能访问静态数据成员
        return mCount;
    }
};
int CTestStatic::mCount = 0;
    cout << "CTestStatic size: " << sizeof(CTestStatic) << endl;

    CTestStatic t1;
    CTestStatic t2;

    cout << "mCount = " << t1.getCount() << endl;
    cout << "mCount = " << t2.getCount() << endl;
    cout << "mCount = " << CTestStatic::getCount() << endl;

}
```

### cosnt

1. cosnt 数据成员
   cosnt 数据成员的定义和普通 const 变量的定义是一样的，但是由于在类里面定义数据成员是不支持定义的同时初始化的，只能通过构造函数使用初始化表来初始化。

2. const 成员函数
   cosnt 成员函数的使用方法在普通成员函数后面加上 const 即可，本质是改变了 this 指针的属性。主要是为了防止在对象调用成员函数是修改了数据成员，还有就是 const 对象只能调用 const 成员函数，如果没有 const 成员函数，const 对象什么事都干不了。

### friend

友元可以是一个函数，该函数被称为友元函数；友元也可以是一个类，该类被称为友元类，在这种情况下，整个类及其所有成员都是友元。

在类定义中，使用 `friend` 关键字和非成员函数或其他类的名称，以允许其访问类的私有和受保护成员。

```cpp
#pragma once

#include <iostream>
using namespace std;

class Square;
class Test;
class Retangle {
private:
    int width, height;
public:
    Retangle() { width = height = 0; }
    Retangle(int x, int y) :width(x), height(y) {}
    int area() { return width * height; }
    // 友元函数可以访问 Retangle 私有和受保护的成员。
    friend Retangle duplicate(const Retangle& param);
    void convert(Square a);
    void fun(Test& t);
};

class Square {
    // 友元类，其成员可以访问 Square 的私有或受保护成员。
    friend class Retangle;
private:
    int side;
public:
    Square(int a) :side(a) {}
};

class Test {
    int t;
public:
    // 友元类成员函数
    friend void Retangle::fun(Test& t);
};

Retangle duplicate(const Retangle& param) {
    Retangle res;
    res.width = param.width;
    res.height = param.height;
    return res;
}

void Retangle::convert(Square a) {
    width = a.side;
    height = a.side;
}

void Retangle::fun(Test& t) {
    t.t = 1;
}

int main () {
  Rectangle rect;
  Square sqr (4);
  rect.convert(sqr);
  cout << rect.area();
  return 0;
}
```

友元一般用作设计，因为友元后能访问私有成员，破坏封装性，不推荐使用。

### 运算符重载

| 表达式      | 操作符                                                                                 | 成员函数                | 非成员函数         |
| ----------- | -------------------------------------------------------------------------------------- | ----------------------- | ------------------ |
| `@a`        | `+` `-` `*` `&` `!` `~` `++` `--`                                                      | `A::operator@()`        | `operator@(A)`     |
| `a@`        | `++` `--`                                                                              | `A::operator@(int)`     | `operator@(A,int)` |
| `a@b`       | `+` `-` `*` `/` `%` `^` `&` `\|` `<` `>` `==` `!=` `<=` `>=` `<<` `>>` `&&` `\|\|` `,` | `A::operator@(B)`       | `operator@(A,B)`   |
| `a@b`       | `=` `+=` `-=` `*=` `/=` `%=` `^=` `&=` `\| =` `<<=` `>>=` `[]`                         | `A::operator@(B)`       | -                  |
| `a(b,c...)` | `()`                                                                                   | `A::operator()(B,C...)` | -                  |
| `a->b`      | `->`                                                                                   | `A::operator->()`       | -                  |
| `(TYPE) a`  | `TYPE`                                                                                 | `A::operator TYPE()`    | -                  |

## 继承和虚函数

### 继承的基本知识

- 构造函数的调用顺序：构造基类 ==> 构造成员对象(按定义顺序) ==> 自己构造
- 析构函数调用顺序：析构自己 ==> 析构成员对象(按定义顺序反向) ==> 析构基类

**派生类可以在构造函数使用初始化列表指定基类的构造函数显示的调用。**

继承的内存布局：

```bash
基类数据成员
派生类数据成员
```

- 派生类和基类的首地址指向同一块内存空间。
- 派生类指针转换成基类的指针是安全的，不可能越界访问。
- 基类指针转换成派生类指针是不安全的，可能会产生溢出。

### 成员函数指针

```cpp
// 普通函数指针的定义
typedef void (__cdecl *FUNTYPE)();  //typedef 方便以后使用

FUNTYPE pfnfun = fun;   //指针函数的定义
pfnfun();   //指针函数的是用 


//成员函数指针定义与使用
typedef void (A::*CFUNTYPE)();
CFUNTYPE cpfnFun = &A::fun; //成员函数指针的定义与赋值

A theA;
A *pA = &theA;
(theA.*cpfnFun)();  //成员函数指针的调用

pfnFun2 = A::fun2;
(pA->*cpfnFun)();  //成员函数指针的调用
```

## 虚函数

虚函数的定义就是在函数前加上关键字 `virtual`，虚函数的特性：运行时多态。

析构函数必须是虚函数，可以防止一些资源未被释放的问题。

### 重载、隐藏和覆盖

1. 重载：

- 作用域相同。
- 函数名相同，参数个数，参数类型，参数顺序不同构成重载。
- 返回值不做参考。

2. 覆盖：

- 不同作用域(基类和派生类)
- 函数名相同
- 参数个数，参数类型，参数顺序都要相同
- 返回值相同
- 基类函数是虚函数

3. 隐藏(除了覆盖就是隐藏)：

- 作用域不同(基类和派生类)。
- 函数名相同

### 虚函数表

- 在基类的构造函数中拷贝基类的虚表指针到对象的首地址。
- 在派生类的构造函数中拷贝派生类的虚表指针到对象首地址。
  - 有覆盖先覆盖
  - 新增加的虚函数地址加在虚表后面

### 虚表函数的特性

1. 如果一个类至少有一个虚表函数，那么这个类就有一个虚表指针。
2. 虚表存放在全局数据区，即没实例化对象虚表也存在。
3. 相同的类，不同的对象共享虚表，即虚表指针指向同一个位置。
4. 虚表指针存放在对象的首地址处。
5. 虚表项存放成员函数指针(数组)，通常按虚函数的定义顺序排列。
6. 虚表不一定以 NULL 结尾。
7. 不同的类虚表不同。
8. 构造时初始化虚表指针。

### 纯虚函数

写法 virtual fun() = 0;

注意事项

1. 如果一个类至少有一个纯虚函数，那么这个类就是一个抽象类。
2. 抽象类不能实例化对象。
3. 如果派生类没有覆盖全部纯虚函数，那么派生类也是抽象类。

在 VC6.0 中，纯虚析构函数必须实现。

显示调用析构函数，无多态性。

用途

1. 有些类很抽象，无法实现，例如(图形，动物)。
2. 强制要求派生类实现虚函数。
3. 给派生类规范了接口(常用)。

### 普通多重继承

构造顺序

1. 先构造基类(按继承顺序)
2. 对象成员构造函数(定义顺序)
3. 最后执行自己构造函数

析构顺序

1. 先执行自己的析构函数
2. 再执行对象成员析构函数(按定义顺序反向执行)
3. 最后执行基类的析构函数(按继承顺序反向执行)

数据成员在内存中的结构

1. 基类数据成员(按继承顺序排列)
2. 自己的数据成员

多重继承中的基类的函数和数据成员有可能出现同名的情况，使用派生类实例化的对像调用同名函数或使用同名数据成员时，就会出现二义性，此时编译器不知道到底调用哪个基类的，就会报错。如果要调用同名函数或使用同名数据成员时，只要加上作用域就可以使用了。

### 虚继承

虚继承后派生类多了一个指针变量，存放的是基类数据成员的偏移量表，这样的存储方式使得派生类中继承基类的数据成员只存一份，如果要访问直接通过偏移量表来访问就可以了。

构造顺序:

A. 构造基类(先虚基类，普通基类按继承顺序)

B. 对象成员构造

C. 构造自己

析构顺序:与构造顺序相反。

## 异常

异常处理程序是在 `try` 块的右大括号后立即使用 `catch` 关键字声明的。 `catch` 的语法类似于具有一个参数的常规函数 ​​。此参数的类型非常重要，因为会根据它检查 `throw` 表达式传递的参数类型，只有在它们匹配的情况下，异常才会被该处理程序捕获。

`try catch` 可以嵌套处理。
也可以自定义自己的异常。

```cpp
#include <iostream>
using namespace std;
class myexcepton:public exception
{
public:
    myexcepton() {};
    ~myexcepton() {};
    virtual const char* what() const {
        return "my exception happened.";
    }
private:

};

int main(int argc, char** argv) {
    try{
        // 嵌套
        try{
            // 抛出自定义异常
            throw myexcepton();
        }
        catch (int n){
            throw; // 将异常转发给外部try catch.
        }
        catch (const std::exception& e){ // exception将会捕获所有子类异常
            cout << e.what() << endl;
            throw 3; // 抛出异常
        }
    }
    catch (int e){ // 捕获 int 类型的异常
        cout << e << endl;
    }
    catch (char e) { // 捕获 char 类型的异常
        cout << e << endl;
    }
    catch (...){    // 捕获其他的所有类型的异常
    }
}
```

# c++11 新特性

> https://mp.weixin.qq.com/s/sWEBZzm6qmW2-LEw6Rdz3Q

## auto & decltype

关于 C++11 新特性，最先提到的肯定是类型推导，C++11 引入了 `auto` 和 `decltype` 关键字，使用他们可以在编译期就推导出变量或者表达式的类型，方便开发者编码也简化了代码。

- `auto`：让编译器在编译器就推导出变量的类型，可以通过=右边的类型推导出变量的类型。
- `decltype`：相对于 auto 用于推导变量类型，而 `decltype` 则用于推导表达式类型，这里只用于编译器分析表达式的类型，表达式实际不会进行运算。

```cpp
auto a = 10; // 10 是 int 型，可以自动推导出 a 是 int

cont int &i = 1;
int a = 2;
decltype(i) b = 2; // b 是 const int&
```

## 左值右值

众所周知 C++11 新增了右值引用，这里涉及到很多概念：

- 左值：可以取地址并且有名字的东西就是左值。
- 右值：不能取地址的没有名字的东西就是右值。
- 纯右值：运算表达式产生的临时变量、不和对象关联的原始字面量、非引用返回的临时变量、`lambda` 表达式等都是纯右值。
- 将亡值：可以理解为即将要销毁的值。
- 左值引用：对左值进行引用的类型。
- 右值引用：对右值进行引用的类型。
- 移动语义：转移资源所有权，类似于转让或者资源窃取的意思，对于那块资源，转为自己所拥有，别人不再拥有也不会再使用。
- 完美转发：可以写一个接受任意实参的函数模板，并转发到其它函数，目标函数会收到与转发函数完全相同的实参。
- 返回值优化：当函数需要返回一个对象实例时候，就会创建一个临时对象并通过复制构造函数将目标对象复制到临时对象，这里有复制构造函数和析构函数会被多余的调用到，有代价，而通过返回值优化，C++标准允许省略调用这些复制构造函数。

## 列表初始化

在 C++11 中可以直接在变量名后面加上初始化列表来进行对象的初始化。

## std::function & std::bind & lambda 表达式

c++11 新增了 std::function、std::bind、lambda 表达式等封装使函数调用更加方便。

## 模板的改进

C++11 关于模板有一些细节的改进：

- 模板的右尖括号
- 模板的别名
- 函数模板的默认模板参数

## 并发

c++11 关于并发引入了好多好东西，有：

- `std::thread` 相关
- `std::mutex` 相关
- `std::lock` 相关
- `std::atomic` 相关
- `std::call_once` 相关
- `volatile` 相关
- `std::condition_variable` 相关
- `std::future` 相关
- `async` 相关

## 智能指针

c++11 引入了三种智能指针：

- `std::shared_ptr`
- `std::weak_ptr`
- `std::unique_ptr`

## 基于范围的 for 循环

直接看代码

```cpp

vector<int> vec;

for (auto iter = vec.begin(); iter != vec.end(); iter++) { // before c++11
   cout << *iter << endl;
}

for (int i : vec) { // c++11基于范围的for循环
cout << "i" << endl;
}
```

## 委托构造函数

委托构造函数允许在同一个类中一个构造函数调用另外一个构造函数，可以在变量初始化时简化操作，通过代码来感受下委托构造函数的妙处吧：

```cpp
struct A {
   A(){}
   A(int a) { a_ = a; }

   A(int a, int b) : A(a) { b_ = b; }

   A(int a, int b, int c) : A(a, b) { c_ = c; }

   int a_;
   int b_;
   int c_;
};
```

初始化变量是不是方便了许多。

## 继承构造函数

继承构造函数可以让派生类直接使用基类的构造函数，如果有一个派生类，我们希望派生类采用和基类一样的构造方式，可以直接使用基类的构造函数，而不是再重新写一遍构造函数，老规矩，看代码：

```cpp

struct Base {
   Base() {}
   Base(int a) { a_ = a; }

   Base(int a, int b) : Base(a) { b_ = b; }

   Base(int a, int b, int c) : Base(a, b) { c_ = c; }

   int a_;
   int b_;
   int c_;
};

struct Derived : Base {
   using Base::Base;
};

int main() {
   Derived a(1, 2, 3);
   return 0;
}
```

只需要使用 `using Base::Base` 继承构造函数，就免去了很多重写代码的麻烦。

## nullptr

`nullptr` 是 c++11 用来表示空指针新引入的常量值，在 c++中如果表示空指针语义时建议使用 `nullptr` 而不要使用 `NULL`，因为 `NULL` 本质上是个 `int` 型的 0，其实不是个指针。举例：

```cpp
void func(void *ptr) {
   cout << "func ptr" << endl;
}

void func(int i) {
   cout << "func i" << endl;
}

int main() {
   func(NULL); // 编译失败，会产生二义性
   func(nullptr); // 输出func ptr
   return 0;
}
```

## final & override

c++11 关于继承新增了两个关键字，`final` 用于修饰一个类，表示禁止该类进一步派生和虚函数的进一步重载，`override` 用于修饰派生类中的成员函数，标明该函数重写了基类函数，如果一个函数声明了 `override` 但父类却没有这个虚函数，编译报错，使用 `override` 关键字可以避免开发者在重写基类函数时无意产生的错误。

```cpp

struct Base {
   virtual void func() {
       cout << "base" << endl;
  }
};

struct Derived : public Base{
   void func() override { // 确保func被重写
       cout << "derived" << endl;
  }

   void fu() override { // error，基类没有fu()，不可以被重写

  }
};


struct Base final {
   virtual void func() {
       cout << "base" << endl;
  }
};

struct Derived : public Base{ // 编译失败，final修饰的类不可以被继承
   void func() override {
       cout << "derived" << endl;
  }

};
```

## default

c++11 引入 `default` 特性，多数时候用于声明构造函数为默认构造函数，如果类中有了自定义的构造函数，编译器就不会隐式生成默认构造函数，而通过 `default`，程序员只需在函数声明后加上`=default;`，就可将该函数声明为 `defaulted` 函数，编译器将为显式声明的 `defaulted` 函数自动生成函数体，如下：

```cpp

struct A {
   A() = default;
   int a;
   A(int i) { a = i; }
};

int main() {
   A a;
   return 0;
}
```

## delete

c++中，如果开发人员没有定义特殊成员函数，那么编译器在需要特殊成员函数时候会隐式自动生成一个默认的特殊成员函数，例如拷贝构造函数或者拷贝赋值操作符，而我们有时候想禁止对象的拷贝与赋值，可以使用 `delete` 修饰，如下：

```cpp

struct A {
   A() = default;
   A(const A&) = delete;
   A& operator=(const A&) = delete;
   int a;
   A(int i) { a = i; }
};

int main() {
   A a1;
   A a2 = a1;  // 错误，拷贝构造函数被禁用
   A a3;
   a3 = a1;  // 错误，拷贝赋值操作符被禁用
}
```

`delele` 函数在 c++11 中很常用，`std::unique_ptr` 就是通过 `delete` 修饰来禁止对象的拷贝的。

## explicit

`explicit` 专用于修饰构造函数，表示只能显式构造，不可以被隐式转换，根据代码看 `explicit` 的作用：

```cpp
structA {
explicitA(int value){
cout << "value" << endl;
}
};

intmain(){
A a = 1; // error，不可以隐式转换
A aa(2); // ok
return0;
}
```

## const

因为要讲后面的 `constexpr`，所以这里简单介绍下 `const`。

`const` 字面意思为只读，可用于定义变量，表示变量是只读的，不可以更改，如果更改，编译期间就会报错。

主要用法如下：

- 用于定义常量，`const` 的修饰的变量不可更改。
- 指针也可以使用 `const`，这里有个小技巧，从右向左读，即可知道 `const` 究竟修饰的是指针还是指针所指向的内容。
- 在函数参数中使用 `const`，一般会传递类对象时会传递一个 `const` 的引用或者指针，这样可以避免对象的拷贝，也可以防止对象被修改。
- `const` 修饰类的成员变量，表示是成员常量，不能被修改，可以在初始化列表中被赋值。
- 修饰类成员函数，表示在该函数内不可以修改该类的成员变量。
- 修饰类对象，类对象只能调用该对象的 `const` 成员函数。

## constexpr

`constexpr` 是 c++11 新引入的关键字，用于编译时的常量和常量函数，这里直接介绍 `constexpr` 和 `const` 的区别：

两者都代表可读，`const` 只表示 read only 的语义，只保证了运行时不可以被修改，但它修饰的仍然有可能是个动态变量，而 `constexpr` 修饰的才是真正的常量，它会在编译期间就会被计算出来，整个运行过程中都不可以被改变，`constexpr` 可以用于修饰函数，这个函数的返回值会尽可能在编译期间被计算出来当作一个常量，但是如果编译期间此函数不能被计算出来，那它就会当作一个普通函数被处理。

## enum class

c++11 新增有作用域的枚举类型，不带作用域的枚举类型可以自动转换成整形，且不同的枚举可以相互比较；带作用域的枚举类型可以选择底层类型，默认是 `int`，可以改成 `char` 等别的类型。

我们平时编程过程中使用枚举，一定要使用有作用域的枚举取代传统的枚举。

## 非受限联合体

c++11 之前 `union` 中数据成员的类型不允许有非 POD 类型，而这个限制在 c++11 被取消，允许数据成员类型有非 POD 类型。

对于什么是 POD 类型，大家可以自行查下资料，大体上可以理解为对象可以直接 `memcpy` 的类型。

## sizeof

c++11 中 `sizeof` 可以用的类的数据成员上，想知道类中数据成员的大小在 c++11 中方便了许多，而不需要定义一个对象，在计算对象的成员大小。

## assertion

c++11 引入 `static_assert` 声明，用于在编译期间检查，如果第一个参数值为 `false`，则打印 `message`，编译失败。

## 自定义字面量

c++11 可以自定义字面量。

## 内存对齐

什么是内存对齐

理论上计算机对于任何变量的访问都可以从任意位置开始，然而实际上系统会对这些变量的存放地址有限制，通常将变量首地址设为某个数 N 的倍数，这就是内存对齐。

为什么要内存对齐

硬件平台限制，内存以字节为单位，不同硬件平台不一定支持任何内存地址的存取，一般可能以双字节、4 字节等为单位存取内存，为了保证处理器正确存取数据，需要进行内存对齐。
提高 CPU 内存访问速度，一般处理器的内存存取粒度都是 N 的整数倍，假如访问 N 大小的数据，没有进行内存对齐，有可能就需要两次访问才可以读取出数据，而进行内存对齐可以一次性把数据全部读取出来，提高效率。

## thread_local

c++11 引入 `thread_local`，用 `thread_local` 修饰的变量具有 `thread` 周期，每一个线程都拥有并只拥有一个该变量的独立实例，一般用于需要保证线程安全的函数中。

## 基础数值类型

c++11 新增了几种数据类型：`long long`、`char16_t`、`char32_t` 等

## 随机数功能

c++11 关于随机数功能则较之前丰富了很多，典型的可以选择概率分布类型。

## 正则表达式

c++11 引入了 regex 库更好的支持正则表达式。

## chrono

c++11 关于时间引入了 `chrono` 库，源于 boost，功能强大，`chrono` 主要有三个点：

- duration: 表示一段时间，常见的单位有 s、ms 等
- time_point: 表示一个具体时间点，如 2020 年 5 月 10 日 10 点 10 分 10 秒
- clocks:时钟，chrono 里面提供了三种时钟：
  - steady_clock: 稳定的时间间隔，表示相对时间，相对于系统开机启动的时间，无论系统时间如何被更改，后一次调用 now()肯定比前一次调用 now()的数值大，可用于计时。
  - system_clock: 表示当前的系统时钟，可以用于获取当前时间。
  - high_resolution_clock: 表示系统可用的最高精度的时钟。

## 新增数据结构

- `std::forward_list`：单向链表，只可以前进，在特定场景下使用，相比于 std::list 节省了内存，提高了性能
- `std::unordered_set`：基于 hash 表实现的 set，内部不会排序，使用方法和 set 类似
- `std::unordered_map`：基于 hash 表实现的 map，内部不会排序，使用方法和 set 类似
- `std::array`：数组，在越界访问时抛出异常，建议使用 std::array 替代普通的数组
- `std::tuple`：元组类型，类似 pair，但比 pair 扩展性好

## 新增算法

- `all_of`：检测表达式是否对范围 `[first, last)` 中所有元素都返回 true，如果都满足，则返回 true
- `any_of`：检测表达式是否对范围 `[first, last)` 中至少一个元素返回 true，如果满足，则返回 true，否则返回 false，用法和上面一样
- `none_of`：检测表达式是否对范围 `[first, last)` 中所有元素都不返回 true，如果都不满足，则返回 true，否则返回 false，用法和上面一样
- `find_if_not`：找到第一个不符合要求的元素迭代器，和 find_if 相反
- `copy_if`：复制满足条件的元素
- `itoa`：对容器内的元素按序递增
- `minmax_element`：返回容器内最大元素和最小元素位置
- `is_sorted`、`is_sorted_until`：返回容器内元素是否已经排好序。

# C++14 新特性

## 函数返回值类型推导

C++14 对函数返回类型推导规则做了优化，返回值类型推导也可以用在模板中。

## lambda 参数 auto

## 变量模板

## 别名模板

## `[[deprecated]]`标记

C++14 中增加了 deprecated 标记，修饰类、变、函数等，当程序中使用到了被其修饰的代码时，编译时被产生警告，用户提示开发者该标记修饰的内容将来可能会被丢弃，尽量不要使用。

## 二进制字面量与整形字面量分隔符

## 新增下列标准函数：

- `std::make_unique`
- `std::shared_timed_mutex` 与 `std::shared_lock`
- `std::integer_sequence`
- `std::exchange`
- `std::quoted`

# C++17 新特性

## 构造函数模板推导

C++17 就不需要构造一个模板类对象需要指明类型，直接可以推导出类型。

```cpp

pair p(1, 2.2); // c++17 自动推导
vector v = {1, 2, 3}; // c++17
```

## 结构化绑定

通过结构化绑定，对于 tuple、map 等类型，获取相应值会方便很多。

```cpp
std::pair a(1, 2.3f);
auto[i, f] = a;

map<int, string> m = {
{0, "a"},
{1, "b"},
};
for (const auto &[i, s] : m) {
    cout << i << " " << s << endl;
}
```

## if-switch 语句初始化

```cpp
// if (init; condition)

if (int a = GetValue()); a < 101) {
   cout << a;
}

string str = "Hi World";
if (auto [pos, size] = pair(str.find("Hi"), str.size()); pos != string::npos) {
   std::cout << pos << " Hello, size is " << size;
}
```

使用这种方式可以尽可能约束作用域，让代码更简洁，但是可读性略有下降。

## 内联变量

C++17 前只有内联函数，现在有了内联变量，我们印象中 C++类的静态成员变量在头文件中是不能初始化的，但是有了内联变量，就可以达到此目的：

```cpp

// header file
struct A {
   static const int value;
};
inline int const A::value = 10;

// ==========或者========
struct A {
   inline static const int value = 10;
}
```

## 折叠表达式

C++17 引入了折叠表达式使可变参数模板编程更方便：

```cpp
template <typename ... Ts>
auto sum(Ts ... ts) {
   return (ts + ...);
}
int a {sum(1, 2, 3, 4, 5)}; // 15
std::string a{"hello "};
std::string b{"world"};
cout << sum(a, b) << endl; // hello world
```

## constexpr lambda 表达式

C++17 前 lambda 表达式只能在运行时使用，C++17 引入了 constexpr lambda 表达式，可以用于在编译期进行计算。

## namespace 嵌套

```cpp

namespace A {
   namespace B {
       namespace C {
           void func();
      }
  }
}

// c++17，更方便更舒适
namespace A::B::C {
   void func();)
}
```

## `__has_include` 预处理表达式

`__has_include` 预处理表达式可以判断是否有某个头文件，代码可能会在不同编译器下工作，不同编译器的可用头文件有可能不同，所以可以使用此来判断。

```cpp
#if defined __has_include
#if __has_include(<charconv>)
#define has_charconv 1
#include <charconv>
#endif
#endif

std::optional<int> ConvertToInt(const std::string& str) {
   int value{};
#ifdef has_charconv
   const auto last = str.data() + str.size();
   const auto res = std::from_chars(str.data(), last, value);
   if (res.ec == std::errc{} && res.ptr == last) return value;
#else
   // alternative implementation...
   其它方式实现
#endif
   return std::nullopt;
}
```

## 在 lambda 表达式用 `*this` 捕获对象副本

正常情况下，lambda 表达式中访问类的对象成员变量需要捕获 this，但是这里捕获的是 this 指针，指向的是对象的引用，正常情况下可能没问题，但是如果多线程情况下，函数的作用域超过了对象的作用域，对象已经被析构了，还访问了成员变量，就会有问题。

所以 C++17 增加了新特性，捕获\*this，不持有 this 指针，而是持有对象的拷贝，这样生命周期就与对象的生命周期不相关啦。

## 新增 Attribute

我们可能平时在项目中见过 `__declspec__` , `__attribute__` , `#pragma` 指示符，使用它们来给编译器提供一些额外的信息，来产生一些优化或特定的代码，也可以给其它开发者一些提示信息。

```cpp
struct A { short f[3]; } __attribute__((aligned(8)));

void fatal() __attribute__((noreturn));
```

在 C++11 和 C++14 中有更方便的方法：

- `[[carries_dependency]]` 让编译期跳过不必要的内存栅栏指令
- `[[noreturn]]` 函数不会返回
- `[[deprecated]]` 函数将弃用的警告

```cpp
[[noreturn]] void terminate() noexcept;
[[deprecated("use new func instead")]] void func() {}
```

- `[[fallthrough]]`：用在 switch 中提示可以直接落下去，不需要 break，让编译期忽略警告
- `[[nodiscard]]` ：表示修饰的内容不能被忽略，可用于修饰函数，标明返回值一定要被处理
- `[[maybe_unused]]` ：提示编译器修饰的内容可能暂时没有使用，避免产生警告

## 新增下列标准

`file_system`
`std::variant`
`std::optional`
`std::any`
`std::apply`
`std::make_from_tuple`
`as_const`
`std::string_view`
`std::shared_mutex`

# Effective Modern C++

## 条款一：理解模板类型推导

`auto` 是建立在模板类型推导的基础上的，所以理解模板类型推导对于使用 `auto` 非常重要。

在后续进行类型推导时，都是基于下面的伪代码。

```cpp
template<typename T>
void f(ParamType param);

f(expr); // 从 expr 中推导 T 和 ParamType
```

在编译期间，编译器使用 `expr` 进行两个类型推导：一个是针对 `T` 的，另一个是针对 `ParamType` 的。这两个类型通常是不同的，因为 `ParamType` 包含一些修饰，比如 `const` 和引用修饰符。

`T` 的类型推导不仅取决于 `expr` 的类型，也取决于 `ParamType` 的类型。

### ParamType 是一个指针或引用，但不是通用引用

如果 `expr` 的类型是一个引用，忽略引用部分，然后 `expr` 的类型与 `ParamType` 进行模式匹配来决定 `T`。

```cpp
#include <iostream>

template<typename T>
void f(T& param){
    // 下面的代码用来测试 param 的类型是否为 const。如果是，那么就会编译失败。
    param = 10;
    std::cout << param << std::endl;
}

template<typename T>
void f(T* param){
    *param = 10;
    std::cout << *param << std::endl;
}

int main(int argc, char const *argv[]){
    int x=27;                       // x 是 int
    const int cx=x;                 // cx 是 const int
    const int& rx=x;                // rx 是指向作为 const int 的 x 的引用

    f(x);                           // T 是 int，param 的类型是 int&
    f(cx);                          // T 是 const int，param 的类型是 const int&
    f(rx);                          // T 是 const int，param 的类型是 const int&

    f(&x);                           // T 是 int，param 的类型是 int*
    f(&cx);                          // T 是 const int，param 的类型是 const int*
    f(&rx);                          // T 是 const int，param 的类型是 const int*

    return 0;
}
```

### ParamType 是一个通用引用

- 如果 `expr` 是左值，`T` 和 `ParamType` 都会被推导为左值引用。 -**这是模板类型推导中唯一一种 `T` 被推导为引用的情况。**
  - **当通用引用被使用时，类型推导会区分左值实参和右值实参，但是对非通用引用时不会区分。虽然 ParamType 被声明为右值引用类型，但是最后推导的结果是左值引用。**
- 如果 expr 是右值，就使用正常的推导规则。

```cpp
#include <iostream>
void reference(int& v) {
    std::cout << "lelft" << std::endl;
}
void reference(int&& v) {
    std::cout << "right" << std::endl;
}

template<typename T>
void bar(T&& param){    //param现在是一个通用引用类型
    reference(std::forward<T>(param)); //std::forward用来保持param的左值或右值属性
}

int main(int argc, char const *argv[]){
    int x=27;                       // x 是 int
    const int cx=x;                 // cx 是 const int
    const int& rx=x;                // rx 是指向作为 const int 的 x 的引用
    int && rrx = std::move(12);     // rrx 是右值引用变量，所以是一个左值

    bar(x);                           // x 是左值，所以 T 是 int&，param 类型也是 int&
    bar(cx);                          // cx 是左值，所以 T 是 const int&，param 类型也是 const int&
    bar(rx);                          // rx 是左值，所以 T 是 const int&，param 类型也是 const int&
    bar(rrx);                         // rrx 是右值引用，但是同时也是左值，所以 T 是 int&，param 类型也是 int&
    bar(27);                          // 27 是右值，所以 T 是int，param类型就是int&&

    return 0;
}
```

### ParamType 既不是指针也不是引用

当 `ParamType` 既不是指针也不是引用时，我们通过传值（pass-by-value）的方式处理。这意味着无论传递什么 `param` 都会成为它的一份拷贝——一个完整的新对象。事实上 `param` 成为一个新对象这一行为会影响 `T` 如何从 `expr` 中推导出结果。

- 如果 `expr` 的类型是一个引用，忽略这个引用部分。
- 如果忽略 `expr` 的引用性之后，`expr` 是一个 `const`，那就再忽略 `const`。如果它是 `volatile`，也忽略 `volatile`。

```cpp
#include <iostream>

template<typename T>
void foo(T param){    //以传值的方式处理param

}

int main(int argc, char const *argv[]){
    int x=27;                       // x 是 int
    const int cx=x;                 // cx 是 const int
    const int& rx=x;                // rx 是指向作为 const int 的 x 的引用
    const char* const ptr = "Fun with pointers";  //ptr是一个常量指针，指向常量对象

    foo(x);                           // T 和 param 的类型都是 int
    foo(cx);                          // T 和 param 的类型都是 int
    foo(rx);                          // T 和 param 的类型都是 int
    foo(ptr);                         // T 和 param 的类型都是 const char* ，
                                      // ptr 作为实参时，ptr自身的值会被传给形参，拷贝 ptr 来创造一个新指针 param，所以 ptr的常量性将会被忽略。
    return 0;
}
```

### 数组实参和函数实参

在模板类型推导时，数组名或者函数名实参会退化为指针，除非它们被用于初始化引用。

数组被用于初始化为引用时，`T` 被推导为了真正的数组，这个类型包括了数组的大小。

```cpp
#include <iostream>

template<typename T>
void f(T& param){
}

// 利用数组被用于初始化引用的这个特新，将一个函数声明为 constexpr 使得结果在编译期间可用。
template<typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept{
    return N;
}

// fun是一个函数，类型是void(int, double)
void fun(int, double){}

template<typename T>
void f1(T param){}

template<typename T>
void f2(T & param){

int main(int argc, char const *argv[]){
    const char name[] = "CKCat";
    f(name);                           // T 是 const char[6]，param 的类型是 const char(&)[6]

    f1(fun);                           // T 是 void(int, double)，param 的类型是 void(*)(int, double)
    f2(fun);                           // T 是 void(int, double)，param 的类型是 void(&)(int, double)
    return 0;
}
```

### 总结

- 在模板类型推导时，有引用的实参会被视为无引用，他们的引用会被忽略。
- 对于通用引用的推导，**左值实参**会被特殊对待。
- 对于传值类型推导，const 和/或 volatile 实参会被认为是 non-const 的和 non-volatile 的。
- 在模板类型推导时，数组名或者函数名实参会退化为指针，除非它们被用于初始化引用。

## 条款二：理解 auto 类型推导

`auto` 类型推导和模板类型推导有一个直接的映射关系。它们之间可以通过一个非常规范非常系统化的转换流程来转换彼此。在模板类型推到中使用下面这个函数模板和调用来解释。

```cpp
template<typename T>
void f(ParmaType param);

f(expr);                        //使用一些表达式调用f
```

当一个变量使用 `auto` 进行声明时，`auto` 扮演了模板中 `T` 的角色，变量的类型说明符扮演了 `ParamType` 的角色。

在使用 `auto` 作为类型说明符的变量声明中，类型说明符代替了 ParamTyp，因此模板类型推导描述的三个情景稍作修改就能适用于 `auto`：

- 情景一：类型说明符是一个指针或引用但不是通用引用。
- 情景二：类型说明符一个通用引用。
- 情景三：类型说明符既不是指针也不是引用。

```cpp
auto x = 27;                    //情景三（x既不是指针也不是引用）
const auto cx = x;              //情景三（cx也一样）
const auto & rx=cx;             //情景一（rx是非通用引用）

auto&& uref1 = x;               //x是int左值，所以uref1类型为int&
auto&& uref2 = cx;              //cx是const int左值，所以uref2类型为const int&
auto&& uref3 = 27;              //27是int右值，所以uref3类型为int&&

const char name[] = "R. N. Briggs"; //name的类型是const char[13]
auto arr1 = name;               //arr1的类型是const char*
auto& arr2 = name;              //arr2的类型是const char (&)[13]

void someFunc(int, double);     //someFunc是一个函数，类型为void(int, double)

auto func1 = someFunc;          //func1的类型是void (*)(int, double)
auto& func2 = someFunc;         //func2的类型是void (&)(int, double)
```

`auto` 类型推导和模板类型推导几乎一样的工作，它们就像一个硬币的两面。但是当用 `auto` 声明的变量使用花括号进行初始化，`auto` 类型推导推出的类型则为 `std::initializer_list`。

对于花括号的处理是 `auto` 类型推导和模板类型推导唯一不同的地方。当使用 `auto` 声明的变量使用花括号的语法进行初始化的时候，会推导出 `std::initializer_list<T>` 的实例化，但是对于模板类型推导这样就行不通。

```cpp
auto x = { 11, 23, 9 };         //x的类型是std::initializer_list<int>

template<typename T>            //带有与x的声明等价的
void f(T param);                //形参声明的模板

f({ 11, 23, 9 });               //错误！不能推导出T

template<typename T>
void f(std::initializer_list<T> initList);

f({ 11, 23, 9 });               //T被推导为int，initList的类型为
                                //std::initializer_list<int>
```

C++14 允许 `auto` 用于函数返回值并会被推导，而且 C++14 的 `lambda` 函数也允许在形参声明中使用 `auto`。但是在这些情况下 `auto` 实际上使用模板类型推导的那一套规则在工作，而不是 `auto` 类型推导，即不能推导 `std::initializer_list<T>` 的实例化。

```cpp
auto createInitList()
{
    return { 1, 2, 3 };         //错误！不能推导{ 1, 2, 3 }的类型
}

std::vector<int> v;
auto resetV =
    [&v](const auto& newValue){ v = newValue; };        //C++14

resetV({ 1, 2, 3 });            //错误！不能推导{ 1, 2, 3 }的类型
```

### 总结

- `auto` 类型推导通常和模板类型推导相同，但是 `auto` 类型推导假定花括号初始化代表 `std::initializer_list`，而模板类型推导不这样做
- 在 C++14 中 `auto` 允许出现在函数返回值或者 `lambda` 函数形参中，但是它的工作机制是模板类型推导那一套方案，而不是 `auto` 类型推导

## 条款三：理解 decltype

在 C++11 中，`decltype` 最主要的用途就是用于声明函数模板，而这个函数返回类型依赖于形参类型。相比模板类型推导和 `auto` 类型推导，`decltype` 只是简单的返回名字或者表达式的类型。

```cpp
const int i = 0;                //decltype(i)是const int

bool f(const Widget& w);        //decltype(w)是const Widget&
                                //decltype(f)是bool(const Widget&)

struct Point{
    int x,y;                    //decltype(Point::x)是int
};                              //decltype(Point::y)是int

Widget w;                       //decltype(w)是Widget

if (f(w))…                      //decltype(f(w))是bool

template<typename T>            //std::vector的简化版本
class vector{
public:
    T& operator[](std::size_t index);
};

vector<int> v;                  //decltype(v)是vector<int>

if (v[0] == 0)                  //decltype(v[0])是int&
```

C++ 期望在某些情况下当类型被暗示时需要使用 `decltype` 类型推导的规则，C++14 通过使用 `decltype(auto)` 说明符使得这成为可能。`auto` 说明符表示这个类型将会被推导，`decltype` 说明 `decltype` 的规则将会被用到这个推导过程中。

```cpp
template<typename Containter, typename Index>
decltype(auto) authAndAccess(Container&& c, Index i){
    return std::forward<Container>(c)[i];
}
```

将 `decltype` 应用于变量名会产生该变量名的声明类型。虽然变量名都是左值表达式，但这不会影响 `decltype` 的行为。然而，**对于比单纯的变量名更复杂的左值表达式，`decltype` 可以确保报告的类型始终是左值引用**。也就是说，如果一个不是单纯变量名的左值表达式的类型是 `T`，那么 `decltype` 会把这个表达式的类型报告为 `T&`。

```cpp
int x = 0;
decltype((x)) rx = x;    // (x)会产生一个比名字更复杂的表达式，所以 decltype((x)) 是 int&
```

### 总结

- `decltype` 总是不加修改的产生变量或者表达式的类型。
- 对于 `T` 类型的不是单纯的变量名的左值表达式，`decltype` 总是产出 `T` 的引用即 `T&`。
- C++14 支持 `decltype(auto)`，就像 `auto` 一样，推导出类型，但是它使用 `decltype` 的规则进行推导。

## 条款四：学会查看类型推导结果

- IDE 编辑器：在 IDE 中的代码编辑器通常可以显示程序代码中变量，函数，参数的类型。
- 编译器诊断：利用编译器出错时提供的错误消息查看类型推导结果。
- 运行时输出：使用 `typeid(x).name()` 或者 `Boost.TypeIndex` 查看类型。

**上面列举的工具可能既不准确也无帮助，所以理解 C++ 类型推导规则才是最重要的。**

## 条款五：优先考虑 auto 而非显式类型声明

- `auto` 变量必须初始化，通常它可以避免一些移植性和效率性的问题，也使得重构更方便，还能让你少打几个字。
- `auto` 类型的变量可能会踩到一些陷阱。
  - `auto` 类型推导假定花括号表示 `std::initializer_list`。
  - 不可见的代理类可能会使 `auto` 从表达式中推导出“错误的”类型，例如：`std::vector<bool>` 。

```cpp
auto derefUPLess =
    [](const std::unique_ptr<Widget> &p1,       //用于std::unique_ptr
       const std::unique_ptr<Widget> &p2)       //指向的Widget类型的
    { return *p1 < *p2; };                      //比较函数

auto derefLess =                                //C++14版本
    [](const auto& p1,                          //被任何像指针一样的东西
       const auto& p2)                          //指向的值的比较函数
    { return *p1 < *p2; };

std::vector<int> v;
unsigned sz = v.size();  // v.size() 的标准返回类型是 std::vector<int>::size_type ，该值的大小跟平台相关。

std::unordered_map<std::string, int> m;

for(const std::pair<std::string, int>& p : m){ // p 应该声明为 const std::pair<const std::string, int>
    // 用 p 做一些事，但是 p 实际上是一个临时对象，这个临时变量在每次迭代完成时会被销毁。
}   // 使用 auto 声明目标变量你就不必担心这个问题。
```

**`auto` 是可选项，不是命令，在某些情况下如果你的专业判断告诉你使用显式类型声明比 `auto` 要更清晰更易维护，那你就不必再坚持使用 `auto`。**

## 条款六：auto 推导若非己愿，使用显式类型初始化惯用法

不可见的代理类通常不适用于 auto，不可见的代理类可能会使 auto 从表达式中推导出“错误的”类型。

```cpp
#include <iostream>
#include <vector>

std::vector<bool>  features(){
    return {true, false, true, true, false};
}

void process(std::vector<bool>& v, bool & highPriority){
    if (highPriority){
        highPriority = true;
    }
}

int main(int argc, char const *argv[])
{
    std::vector<bool> v = {true, false, true, true, false};
    auto highPriority = features()[1]; // highPriority 行为类似于 bool& 的对象 std::vector<bool>::reference
    // process(v, highPriority); // 这里会报错，因为 highPriority 的类型是 std::vector<bool>::reference
    return 0;
}
```

如果遇到上面的情况，解决方案是强制使用一个不同的类型推导形式，这种方法我通常称之为显式类型初始器惯用法。

```cpp
    auto highPriority = static_cast<bool>(features()[1]);
    process(v, highPriority);
```

显式类型初始器惯用法不限制初始化表达式产生一个代理类。它也可以用于强调你声明了一个变量类型，它的类型不同于初始化表达式的类型。

```cpp
double calcEpsilon(){};
auto ep = static_cast<float>(calcEpsilon()); // 减少函数返回值的精度

auto index = static_cast<int>(ep * size()); // 强制转换为 int
```

### 总结

- 不可见的代理类可能会使 auto 从表达式中推导出“错误的”类型
- 显式类型初始器惯用法强制 auto 推导出你想要的结果

## 条款七：区别使用()和{}创建对象

C++11 对象初始化的语法可能会让你觉得丰富的让人难以选择，亦或是乱的一塌糊涂。“乱的一塌糊涂”是指在初始化中使用"="可能会误导 C++新手，使他们以为这里发生了赋值运算，然而实际并没有。

C++11 使用统一初始化（uniform initialization）来整合这些混乱且不适于所有情景的初始化语法，所谓统一初始化是指在任何涉及初始化的地方都使用单一的初始化语法。它基于花括号，出于这个原因我更喜欢称之为花括号初始化。统一初始化是一个概念上的东西，而花括号初始化是一个具体语法结构。

花括号初始化也能被用于为非静态数据成员指定默认初始值。C++11 允许"="初始化不加花括号也拥有这种能力。

```cpp
class Widget{
    …

private:
    int x{ 0 };                 //没问题，x初始值为0
    int y = 0;                  //也可以
    int z(0);                   //错误！
}
```

不可拷贝的对象可以使用花括号初始化或者圆括号初始化，但是不能使用"="初始化。

```cpp
std::atomic<int> ai1{ 0 };      //没问题
std::atomic<int> ai2(0);        //没问题
std::atomic<int> ai3 = 0;       //错误！
```

在 C++中这三种方式都被看做是初始化表达式，但是只有花括号任何地方都能被使用。

括号表达式还有一个少见的特性，即它不允许内置类型间隐式的变窄转换（narrowing conversion）。

```cpp
double x, y, z;
int sum1{ x + y + z };          //错误！double的和可能不能表示为int
```

C++规定任何可以被解析为一个声明的东西必须被解析为声明。这个规则的副作用是让很多程序员备受折磨：他们可能想创建一个使用默认构造函数构造的对象，却不小心变成了函数声明。

```cpp
Widget w1(10);                  //使用实参10调用Widget的一个构造函数。
Widget w2();                    //尝试使用相似的语法调用Widget无参构造函数，它就会变成函数声明，函数名为w2，返回Widget。
Widget w3{};                    //使用花括号初始化表明你想调用默认构造函数构造对象就没有问题，调用没有参数的构造函数构造对象。
```

编译器一遇到括号初始化就选择带 `std::initializer_list` 的构造函数，普通构造函数和移动构造函数也会受此影响，只有当没办法把括号初始化中实参的类型转化为 `std::initializer_list` 时，编译器才会回到正常的函数决议流程中。

```cpp
class Widget {
public:
    Widget(int i, bool b);                      //同之前一样
    Widget(int i, double d);                    //同之前一样
    Widget(std::initializer_list<bool> il);     //现在元素类型为bool
    …                                           //没有隐式转换函数
};

Widget w{10, 5.0};              //错误！要求变窄转换

class Widget {
public:
    Widget(int i, bool b);                              //同之前一样
    Widget(int i, double d);                            //同之前一样
    //现在std::initializer_list元素类型为std::string
    Widget(std::initializer_list<std::string> il);
    …                                                   //没有隐式转换函数
};

Widget w1(10, true);     // 使用圆括号初始化，调用第一个构造函数
Widget w2{10, true};     // 使用花括号初始化，现在调用第一个构造函数
Widget w3(10, 5.0);      // 使用圆括号初始化，调用第二个构造函数
Widget w4{10, 5.0};      // 使用花括号初始化，现在调用第二个构造函数
```

使用的花括号初始化，最终会调用默认构造函数。空的花括号意味着没有实参，不是一个空的 `std::initializer_list`。

```cpp
class Widget {
public:
    Widget();                                   //默认构造函数
    Widget(std::initializer_list<int> il);      //std::initializer_list构造函数

    …                                           //没有隐式转换函数
};

Widget w1;                      //调用默认构造函数
Widget w2{};                    //也调用默认构造函数
Widget w3();                    //最令人头疼的解析！声明一个函数
```

如果你想用空 `std::initializer` 来调用 `std::initializer_list` 构造函数，你就得创建一个空花括号作为函数实参——把空花括号放在圆括号或者另一个花括号内来界定你想传递的东西。

```cpp
Widget w4({});                  //使用空花括号列表调用std::initializer_list构造函数
Widget w5{{}};                  //同上
```

`std::vector` 有一个非 `std::initializer_list` 构造函数允许你去指定容器的初始大小，以及使用一个值填满你的容器。但它也有一个 `std::initializer_list` 构造函数允许你使用花括号里面的值初始化容器。如果你创建一个数值类型的 `std::vector`（比如 `std::vector<int>` ），然后你传递两个实参，把这两个实参放到圆括号和放到花括号中有天壤之别

```cpp
std::vector<int> v1(10, 20);    //使用非std::initializer_list构造函数
                                //创建一个包含10个元素的std::vector，
                                //所有的元素的值都是20
std::vector<int> v2{10, 20};    //使用std::initializer_list构造函数
                                //创建包含两个元素的std::vector，
                                //元素的值为10和20
```

默认使用花括号初始化的开发者主要被适用面广、禁止变窄转换、免疫 C++最令人头疼的解析这些优点所吸引。这些开发者知道在一些情况下要使用圆括号。
默认使用圆括号初始化的开发者主要被 C++98 语法一致性、避免 `std::initializer_list` 自动类型推导、避免不会不经意间调用 `std::initializer_list` 构造函数。

### 总结

- 花括号初始化是最广泛使用的初始化语法，它防止变窄转换，并且对于 C++最令人头疼的解析有天生的免疫性。
- 在构造函数重载决议中，编译器会尽最大努力将括号初始化与 `std::initializer_list` 参数匹配，即便其他构造函数看起来是更好的选择。
- 对于数值类型的 `std::vector` 来说使用花括号初始化和圆括号初始化会造成巨大的不同。
- 在模板类选择使用圆括号初始化或使用花括号初始化创建对象是一个挑战。

## 条款八：优先考虑 nullptr 而非 0 和 NULL

nullptr 的优点是它不是整型。老实说它也不是一个指针类型，但是你可以把它认为是所有类型的指针。nullptr 的真正类型是 std::nullptr_t，在一个完美的循环定义以后，std::nullptr_t 又被定义为 nullptr。std::nullptr_t 可以隐式转换为指向任何内置类型的指针，这也是为什么 nullptr 表现得像所有类型的指针。

模板类型推导将 0 和 NULL 推导为一个错误的类型（即它们的实际类型，而不是作为空指针的隐含意义），这就导致在当你想要一个空指针时，它们的替代品 nullptr 很吸引人。使用 nullptr，模板不会有什么特殊的转换。另外，使用 nullptr 不会让你受到同重载决议特殊对待 0 和 NULL 一样的待遇。当你想用一个空指针，使用 nullptr，不用 0 或者 NULL。

### 记住

- 优先考虑 nullptr 而非 0 和 NULL
- 避免重载指针和整型

## 条款九：优先考虑别名声明而非 typedef

typedef 是 C++98 的东西。虽然它可以在 C++11 中工作，但是 C++11 也提供了一个别名声明（alias declaration）：

```cpp
typedef
    std::unique_ptr<std::unordered_map<std::string, std::string>>
    UPtrMapSS;

using UPtrMapSS =
    std::unique_ptr<std::unordered_map<std::string, std::string>>;
```

上面使用 typedef 和别名声明做的都是完全一样的事情。但是使用别名声明函数指针时会更容易理解：

```cpp
//FP是一个指向函数的指针的同义词，它指向的函数带有
//int和const std::string&形参，不返回任何东西
typedef void (*FP)(int, const std::string&);    //typedef

//含义同上
using FP = void (*)(int, const std::string&);   //别名声明
```

名声明可以被模板化（这种情况下称为别名模板 alias templates）但是 typedef 不能。这使得 C++11 程序员可以很直接的表达一些 C++98 中只能把 typedef 嵌套进模板化的 struct 才能表达的东西。

```cpp
template<typename T>                            //MyAllocList<T>是
using MyAllocList = std::list<T, MyAlloc<T>>;   //std::list<T, MyAlloc<T>>
                                                //的同义词

MyAllocList<Widget> lw;                         //用户代码

// 使用typedef，你就只能从头开始：
template<typename T>                            //MyAllocList<T>是
struct MyAllocList {                            //std::list<T, MyAlloc<T>>
    typedef std::list<T, MyAlloc<T>> type;      //的同义词
};

MyAllocList<Widget>::type lw;                   //用户代码

```
如果你想使用在一个模板内使用typedef声明一个链表对象，而这个对象又使用了模板形参，你就不得不在typedef前面加上typename：

这里 `MyAllocList<T>::type` 使用了一个类型，这个类型依赖于模板参数T。因此 `MyAllocList<T>::type` 是一个依赖类型（dependent type），在C++很多讨人喜欢的规则中的一个提到必须要在依赖类型名前加上typename。

别名模板一定是一个类型名。因此 `MyAllocList<T>` 就是一个非依赖类型（non-dependent type），就不需要也不允许使用typename修饰符。

当编译器在Widget的模板中看到 `MyAllocList<T>::type`（使用typedef的版本），它不能确定那是一个类型的名称。因为可能存在一个MyAllocList的它们没见到的特化版本，那个版本的 `MyAllocList<T>::type` 指代了一种不是类型的东西。

C++11在type traits（类型特性）中给了你一系列工具去实现类型转换，如果要使用这些模板请包含头文件 `<type_traits>` 。
```cpp
std::remove_const<T>::type          //C++11: const T → T 
std::remove_const_t<T>              //C++14 等价形式

std::remove_reference<T>::type      //C++11: T&/T&& → T 
std::remove_reference_t<T>          //C++14 等价形式

std::add_lvalue_reference<T>::type  //C++11: T → T& 
std::add_lvalue_reference_t<T>      //C++14 等价形式
```

### 总结

请记住：

- typedef 不支持模板化，但是别名声明支持。
- 别名模板避免了使用“::type”后缀，而且在模板中使用 typedef 还需要在前面加上 typename
- C++14 提供了 C++11 所有 type traits 转换的别名声明版本

## 条款十：优先考虑限域enum而非未限域enum
通常来说，在花括号中声明一个名字会限制它的作用域在花括号之内。但这对于C++98风格的enum中声明的枚举名是不成立的。这些枚举名的名字属于包含这个enum的作用域，这意味着作用域内不能含有相同名字的其他东西：
```cpp
enum Color { black, white, red };   //black, white, red在
                                    //Color所在的作用域
auto white = false;                 //错误! white早已在这个作用
                                    //域中声明
```
这种枚举叫做：未限域枚举(unscoped enum)。在C++11中它们有一个相似物，限域枚举(scoped enum)，限域枚举也被称为枚举类，它不会导致枚举名泄漏：
```cpp
enum class Color { black, white, red }; //black, white, red
                                        //限制在Color域内
auto white = false;                     //没问题，域内没有其他“white”

Color c = white;                        //错误，域中没有枚举名叫white

Color c = Color::white;                 //没问题
auto c = Color::white;                  //也没问题（也符合Item5的建议）

```
使用限域enum来减少命名空间污染，这是一个足够合理使用它而不是它的同胞未限域enum的理由，其实限域enum还有第二个吸引人的优点：在它的作用域中，枚举名是强类型。未限域enum中的枚举名会隐式转换为整型。限域enum可以使用类型转换运算符转换类型。
```cpp
enum class Color { black, white, red }; //Color现在是限域enum

Color c = Color::red;                   //和之前一样，只是
                                        //多了一个域修饰符

if (static_cast<double>(c) < 14.5) {    //奇怪的代码，
                                        //但是有效
    auto factors =                                  //有问题，但是
      primeFactors(static_cast<std::size_t>(c));    //能通过编译
}
```
为了高效使用内存，编译器通常在确保能包含所有枚举值的前提下为enum选择一个最小的底层类型。在一些情况下，编译器将会优化速度，舍弃大小，这种情况下它可能不会选择最小的底层类型，而是选择对优化大小有帮助的类型。为此，C++98只支持enum定义（所有枚举名全部列出来）；enum声明是不被允许的。这使得编译器能在使用之前为每一个enum选择一个底层类型。

但是不能前置声明enum也是有缺点的。最大的缺点莫过于它可能增加编译依赖。这种enum很有可能用于整个系统，因此系统中每个包含这个头文件的组件都会依赖它。如果引入一个新状态值，那么可能整个系统都得重新编译，即使只有一个子系统——或者只有一个函数——使用了新添加的枚举名。这是大家都不希望看到的。C++11中的前置声明enums可以解决这个问题。比如这里有一个完全有效的限域enum声明和一个以该限域enum作为形参的函数声明：

限域enum的底层类型总是已知的，不管怎样，编译器都知道限域enum中的枚举名占用多少字节，所以限域enum可以被前置声明。默认情况下，限域枚举的底层类型是int，而对于非限域enum，你可以指定它，为非限域enum指定底层类型，结果就可以前向声明。

### 总结

- C++98的enum即非限域enum。
- 限域enum的枚举名仅在enum内可见。要转换为其它类型只能使用cast。
- 非限域/限域enum都支持底层类型说明语法，限域enum底层类型默认是int。非限域enum没有默认底层类型。
- 限域enum总是可以前置声明。非限域enum仅当指定它们的底层类型时才能前置。

## 条款十一：优先考虑使用deleted函数而非使用未定义的私有声明
在C++98中防止调用这些函数的方法是将它们声明为私有（private）成员函数并且不定义。故意不定义它们意味着假如还是有代码用它们（比如成员函数或者类的友元friend），就会在链接时引发缺少函数定义（missing function definitions）错误。

在C++11中有一种更好的方式达到相同目的：用“= delete”将拷贝构造函数和拷贝赋值运算符标记为deleted函数。deleted函数不能以任何方式被调用，即使你在成员函数或者友元函数里面调用deleted函数也不能通过编译。这是较之C++98行为的一个改进，C++98中不正确的使用这些函数在链接时才被诊断出来。

通常，deleted函数被声明为public而不是private，这样可以让编译器产生更好的错误信息。

deleted函数还有一个重要的优势是任何函数都可以标记为deleted，而只有成员函数可被标记为private。

虽然deleted函数不能被使用，但它们还是存在于你的程序中。也即是说，重载决议会考虑它们。

另一个deleted函数用武之地（private成员函数做不到的地方）是禁止一些模板的实例化。模板特例化必须位于一个命名空间作用域，而不是类作用域。deleted函数不会出现这个问题，因为它不需要一个不同的访问级别，且他们可以在类外被删除。

### 总结

- 比起声明函数为private但不定义，使用deleted函数更好
- 任何函数都能被删除（be deleted），包括非成员函数和模板实例（译注：实例化的函数）

## 条款十二：使用override声明重写函数

要想重写一个函数，必须满足下列要求：

- 基类函数必须是virtual
- 基类和派生类函数名必须完全一样（除非是析构函数)
- 基类和派生类函数形参类型必须完全一样
- 基类和派生类函数常量性constness必须完全一样
- 基类和派生类函数的返回值和异常说明（exception specifications）必须兼容

除了这些C++98就存在的约束外，C++11又添加了一个：

- 函数的引用限定符（reference qualifiers）必须完全一样。成员函数的引用限定符是C++11很少抛头露脸的特性，所以如果你从没听过它无需惊讶。它可以限定成员函数只能用于左值或者右值。成员函数不需要virtual也能使用它们。

如果基类的虚函数有引用限定符，派生类的重写就必须具有相同的引用限定符。如果没有，那么新声明的函数还是属于派生类，但是不会重写父类的任何函数。

由于正确声明派生类的重写函数很重要，但很容易出错，C++11提供一个方法让你可以显式地指定一个派生类函数是基类版本的重写：将它声明为override。

C++既有很多关键字，C++11引入了两个上下文关键字（contextual keywords），override和final（向虚函数添加final可以防止派生类重写。final也能用于类，这时这个类不能用作基类）。这两个关键字的特点是它们是保留的，它们只是位于特定上下文才被视为关键字。对于override，它只在成员函数声明结尾处才被视为关键字。这意味着如果你以前写的代码里面已经用过override这个名字，那么换到C++11标准你也无需修改代码。

### 总结

- 为重写函数加上override
- 成员函数引用限定让我们可以区别对待左值对象和右值对象（即*this)

## 条款十三：优先考虑const_iterator而非iterator

STL const_iterator等价于指向常量的指针（pointer-to-const）。它们都指向不能被修改的值。标准实践是能加上const就加上，这也指示我们需要一个迭代器时只要没必要修改迭代器指向的值，就应当使用const_iterator。


# 其他

## `C++` 名称粉碎使用 `C++filt` 工具恢复

例如：

```bash
$ c++filt _Z3addii
add(int, int)

```

## 判断编译平台宏定义

```cpp
#ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator
    #elif TARGET_OS_IPHONE
        // iOS device
    #elif TARGET_OS_MAC
        // Other kinds of Mac OS
    #else
    #   error "Unknown Apple platform"
    #endif
#elif __ANDROID__
    // android
#elif __linux__
    // linux
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif
```

## error C2027: use of undefined type

当遇到这种问题时， 将实现写到 cpp 文件中。

例如:

```cpp
# builder.h
#pragma once

class Burger;

class BurgerBuilder
{
public:
	float size;
	bool cheese = false;
	bool pepperoni = false;
	bool lettuce = false;
	bool tomato = false;
	BurgerBuilder(float size) : size(size) {}

	Burger* build();
    // 将下列的实现代码写到 cpp 文件中，否则会报 error C2027: use of undefined type 错误。
	// {
	// 	return new Burger(this);
	// }

};

class Burger
{
protected:
	float size;
	bool cheese = false;
	bool pepperoni = false;
	bool lettuce = false;
	bool tomato = false;

public:
	Burger(BurgerBuilder* builder) {
		this->size = builder->size;
		this->cheese = builder->cheese;
		this->pepperoni = builder->pepperoni;
		this->lettuce = builder->lettuce;
		this->tomato = builder->tomato;
	}
};

// builder.cpp
#include "Builder.h"

Burger* BurgerBuilder::build() {
	return new Burger(this);
}
```

> 参考：https://stackoverflow.com/questions/33964994/error-c2027-use-of-undefined-type-how-to-declare-class

## jupyter notebook for C++

这里可以使用 [xeus-cling](https://github.com/jupyter-xeus/xeus-cling) 搭建一个 Jupyter kernel 环境，方便边做笔记边测试。

首先安装 [Miniconda](https://mirrors.tuna.tsinghua.edu.cn/help/anaconda/) 并创建一个新的环境。

```bash
$ conda create -n cling python=3.9
```

然后安装[xeus-cling](https://github.com/jupyter-xeus/xeus-cling) 和 [Jupyter Notebook](https://jupyter.org/install)

```bash
$ conda install jupyter notebook
$ conda install xeus-cling -c conda-forge
```

安装好了之后，运行 `jupyter kernelspec list` 将会出现下列内容。

```bash
$ jupyter kernelspec list
Available kernels:
  python3    /home/ckcat/miniconda3/envs/cling/share/jupyter/kernels/python3
  xcpp11     /home/ckcat/miniconda3/envs/cling/share/jupyter/kernels/xcpp11
  xcpp14     /home/ckcat/miniconda3/envs/cling/share/jupyter/kernels/xcpp14
  xcpp17     /home/ckcat/miniconda3/envs/cling/share/jupyter/kernels/xcpp17
```

之后运行 `jupyter notebook --allow-root --ip=0.0.0.0 --port=2233` 命令即可。

- `--ip=0.0.0.0` 作用是允许外部试用。
- `--port=2233` 表示运行的端口。

后面就可以通过本地浏览器写笔记和代码。
