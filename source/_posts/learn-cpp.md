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

# 异常

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

关于 C++11 新特性，最先提到的肯定是类型推导，C++11 引入了 auto 和 decltype 关键字，使用他们可以在编译期就推导出变量或者表达式的类型，方便开发者编码也简化了代码。

auto：让编译器在编译器就推导出变量的类型，可以通过=右边的类型推导出变量的类型。

auto 推导规则

在不声明为引用或指针时，auto 会忽略等号右边的引用类型和 cv 限定
在声明为引用或者指针时，auto 会保留等号右边的引用和 cv 属性
这里的 cv 是指 const 和 volatile

auto 的限制：

auto 的使用必须马上初始化，否则无法推导出类型
auto 在一行定义多个变量时，各个变量的推导不能产生二义性，否则编译失败
auto 不能用作函数参数
在类中 auto 不能用作非静态成员变量
auto 不能定义数组，可以定义指针
auto 无法推导出模板参数
decltype：相对于 auto 用于推导变量类型，而 decltype 则用于推导表达式类型，这里只用于编译器分析表达式的类型，表达式实际不会进行运算。

decltype 推导规则

exp 是表达式，decltype(exp)和 exp 类型相同
exp 是函数调用，decltype(exp)和函数返回值类型相同
其它情况，若 exp 是左值，decltype(exp)是 exp 类型的左值引用

直接看代码

```cpp
// auto会忽略等号右边的引用类型和cv限定
int i = 0;
auto *a = &i; // a是int*
auto &b = i; // b是int&
auto c = b; // c是int，忽略了引用

const auto d = i; // d是const int
auto e = d; // e是int

const auto& f = e; // f是const int&
auto &g = f; // g是const int&

// decltype会保留表达式的引用和cv属性
int func() { return 0; }
decltype(func()) i; // i为int类型

int x = 0;
decltype(x) y; // y是int类型
decltype(x + y) z; // z是int类型

const int &i = 1;
int a = 2;
decltype(i) b = 2; // b是const int&

// auto和decltype的配合使用
// 返回值后置类型语法解决了函数返回值类型依赖于参数但却难以确定返回值类型的问题。
template<typename T, typename U>
auto add(T t, U u) -> decltype(t + u) {
    return t + u;
}
```

委托构造函数

## 左值右值

众所周知 C++11 新增了右值引用，这里涉及到很多概念：

左值：可以取地址并且有名字的东西就是左值。
右值：不能取地址的没有名字的东西就是右值。
纯右值：运算表达式产生的临时变量、不和对象关联的原始字面量、非引用返回的临时变量、lambda 表达式等都是纯右值。
将亡值：可以理解为即将要销毁的值。
左值引用：对左值进行引用的类型。
右值引用：对右值进行引用的类型。
移动语义：转移资源所有权，类似于转让或者资源窃取的意思，对于那块资源，转为自己所拥有，别人不再拥有也不会再使用。
完美转发：可以写一个接受任意实参的函数模板，并转发到其它函数，目标函数会收到与转发函数完全相同的实参。
返回值优化：当函数需要返回一个对象实例时候，就会创建一个临时对象并通过复制构造函数将目标对象复制到临时对象，这里有复制构造函数和析构函数会被多余的调用到，有代价，而通过返回值优化，C++标准允许省略调用这些复制构造函数。

这里的详细介绍请看：左值引用、右值引用、移动语义、完美转发，你知道的不知道的都在这里

列表初始化

在 C++11 中可以直接在变量名后面加上初始化列表来进行对象的初始化，详细介绍一定要看这篇文章：学会 C++11 列表初始化

std::function & std::bind & lambda 表达式

c++11 新增了 std::function、std::bind、lambda 表达式等封装使函数调用更加方便，详细介绍请看：搞定 c++11 新特性 std::function 和 lambda 表达式

模板的改进

C++11 关于模板有一些细节的改进：

模板的右尖括号
模板的别名
函数模板的默认模板参数

详细介绍请看：C++11 的模板改进

并发

c++11 关于并发引入了好多好东西，有：

std::thread 相关
std::mutex 相关
std::lock 相关
std::atomic 相关
std::call_once 相关
volatile 相关
std::condition_variable 相关
std::future 相关
async 相关

详细介绍请看：c++11 新特性之线程相关所有知识点

这里也使用 c++11 来实现的线程池和定时器，可以看：

C++线程池的实现之格式修订版

C++定时器的实现之格式修订版

智能指针

很多人谈到 c++，说它特别难，可能有一部分就是因为 c++的内存管理吧，不像 java 那样有虚拟机动态的管理内存，在程序运行过程中可能就会出现内存泄漏，然而这种问题其实都可以通过 c++11 引入的智能指针来解决，相反我还认为这种内存管理还是 c++语言的优势，因为尽在掌握。

c++11 引入了三种智能指针：

std::shared_ptr
std::weak_ptr
std::unique_ptr

详细介绍请看：c++11 新特性之智能指针

基于范围的 for 循环

直接看代码

vector<int> vec;

for (auto iter = vec.begin(); iter != vec.end(); iter++) { // before c++11
cout << \*iter << endl;
}

for (int i : vec) { // c++11 基于范围的 for 循环
cout << "i" << endl;
}
委托构造函数

委托构造函数允许在同一个类中一个构造函数调用另外一个构造函数，可以在变量初始化时简化操作，通过代码来感受下委托构造函数的妙处吧：

不使用委托构造函数：

structA {
A(){}
A(int a) { a\_ = a; }

A(int a, int b) { // 好麻烦
a* = a;
b* = b;
}

A(int a, int b, int c) { // 好麻烦
a* = a;
b* = b;
c\_ = c;
}

int a*;
int b*;
int c\_;
};

使用委托构造函数：

structA {
A(){}
A(int a) { a\_ = a; }

A(int a, int b) : A(a) { b\_ = b; }

A(int a, int b, int c) : A(a, b) { c\_ = c; }

int a*;
int b*;
int c\_;
};

初始化变量是不是方便了许多。

继承构造函数

继承构造函数可以让派生类直接使用基类的构造函数，如果有一个派生类，我们希望派生类采用和基类一样的构造方式，可以直接使用基类的构造函数，而不是再重新写一遍构造函数，老规矩，看代码：

不使用继承构造函数：

structBase {
Base() {}
Base(int a) { a\_ = a; }

Base(int a, int b) : Base(a) { b\_ = b; }

Base(int a, int b, int c) : Base(a, b) { c\_ = c; }

int a*;
int b*;
int c\_;
};

structDerived : Base {
Derived() {}
Derived(int a) : Base(a) {} // 好麻烦
Derived(int a, int b) : Base(a, b) {} // 好麻烦
Derived(int a, int b, int c) : Base(a, b, c) {} // 好麻烦
};
intmain(){
Derived a(1, 2, 3);
return0;
}

使用继承构造函数：

structBase {
Base() {}
Base(int a) { a\_ = a; }

Base(int a, int b) : Base(a) { b\_ = b; }

Base(int a, int b, int c) : Base(a, b) { c\_ = c; }

int a*;
int b*;
int c\_;
};

structDerived : Base {
using Base::Base;
};

intmain(){
Derived a(1, 2, 3);
return0;
}

只需要使用 using Base::Base 继承构造函数，就免去了很多重写代码的麻烦。

nullptr

nullptr 是 c++11 用来表示空指针新引入的常量值，在 c++中如果表示空指针语义时建议使用 nullptr 而不要使用 NULL，因为 NULL 本质上是个 int 型的 0，其实不是个指针。举例：

voidfunc(void \*ptr){
cout << "func ptr" << endl;
}

voidfunc(int i){
cout << "func i" << endl;
}

intmain(){
func(NULL); // 编译失败，会产生二义性
func(nullptr); // 输出 func ptr
return0;
}
final & override

c++11 关于继承新增了两个关键字，final 用于修饰一个类，表示禁止该类进一步派生和虚函数的进一步重载，override 用于修饰派生类中的成员函数，标明该函数重写了基类函数，如果一个函数声明了 override 但父类却没有这个虚函数，编译报错，使用 override 关键字可以避免开发者在重写基类函数时无意产生的错误。

示例代码 1：

structBase {
virtualvoidfunc(){
cout << "base" << endl;
}
};

structDerived :public Base{
voidfunc() override { // 确保 func 被重写
cout << "derived" << endl;
}

voidfu() override { // error，基类没有 fu()，不可以被重写
}
};

示例代码 2：

structBasefinal {
virtualvoidfunc(){
cout << "base" << endl;
}
};

structDerived :public Base{ // 编译失败，final 修饰的类不可以被继承
voidfunc() override {
cout << "derived" << endl;
}

};
default

c++11 引入 default 特性，多数时候用于声明构造函数为默认构造函数，如果类中有了自定义的构造函数，编译器就不会隐式生成默认构造函数，如下代码：

structA {
int a;
A(int i) { a = i; }
};

intmain(){
A a; // 编译出错
return0;
}

上面代码编译出错，因为没有匹配的构造函数，因为编译器没有生成默认构造函数，而通过 default，程序员只需在函数声明后加上“=default;”，就可将该函数声明为 defaulted 函数，编译器将为显式声明的 defaulted 函数自动生成函数体，如下：

structA {
A() = default;
int a;
A(int i) { a = i; }
};

intmain(){
A a;
return0;
}

编译通过。

delete

c++中，如果开发人员没有定义特殊成员函数，那么编译器在需要特殊成员函数时候会隐式自动生成一个默认的特殊成员函数，例如拷贝构造函数或者拷贝赋值操作符，如下代码：

structA {
A() = default;
int a;
A(int i) { a = i; }
};

intmain(){
A a1;
A a2 = a1; // 正确，调用编译器隐式生成的默认拷贝构造函数
A a3;
a3 = a1; // 正确，调用编译器隐式生成的默认拷贝赋值操作符
}

而我们有时候想禁止对象的拷贝与赋值，可以使用 delete 修饰，如下：

structA {
A() = default;
A(const A&) = delete;
A& operator=(const A&) = delete;
int a;
A(int i) { a = i; }
};

intmain(){
A a1;
A a2 = a1; // 错误，拷贝构造函数被禁用
A a3;
a3 = a1; // 错误，拷贝赋值操作符被禁用
}

delele 函数在 c++11 中很常用，std::unique_ptr 就是通过 delete 修饰来禁止对象的拷贝的。

explicit

explicit 专用于修饰构造函数，表示只能显式构造，不可以被隐式转换，根据代码看 explicit 的作用：

不用 explicit：

structA {
A(int value) { // 没有 explicit 关键字
cout << "value" << endl;
}
};

intmain(){
A a = 1; // 可以隐式转换
return0;
}

使用 explicit:

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
const

因为要讲后面的 constexpr，所以这里简单介绍下 const。

const 字面意思为只读，可用于定义变量，表示变量是只读的，不可以更改，如果更改，编译期间就会报错。

主要用法如下：

用于定义常量，const 的修饰的变量不可更改。
constintvalue = 5;
指针也可以使用 const，这里有个小技巧，从右向左读，即可知道 const 究竟修饰的是指针还是指针所指向的内容。
char _const ptr; // 指针本身是常量
constchar_ ptr; // 指针指向的变量为常量
在函数参数中使用 const，一般会传递类对象时会传递一个 const 的引用或者指针，这样可以避免对象的拷贝，也可以防止对象被修改。
classA{};
voidfunc(const A& a);
const 修饰类的成员变量，表示是成员常量，不能被修改，可以在初始化列表中被赋值。
classA {
constintvalue = 5;
};
classB {
constintvalue;
B(int v) : value(v){}
};
修饰类成员函数，表示在该函数内不可以修改该类的成员变量。
classA{
voidfunc() const;
};
修饰类对象，类对象只能调用该对象的 const 成员函数。
classA {
voidfunc() const;
};
const A a;
a.func();
constexpr

constexpr 是 c++11 新引入的关键字，用于编译时的常量和常量函数，这里直接介绍 constexpr 和 const 的区别：

两者都代表可读，const 只表示 read only 的语义，只保证了运行时不可以被修改，但它修饰的仍然有可能是个动态变量，而 constexpr 修饰的才是真正的常量，它会在编译期间就会被计算出来，整个运行过程中都不可以被改变，constexpr 可以用于修饰函数，这个函数的返回值会尽可能在编译期间被计算出来当作一个常量，但是如果编译期间此函数不能被计算出来，那它就会当作一个普通函数被处理。如下代码：

#include<iostream>
usingnamespacestd;

constexprintfunc(int i){
return i + 1;
}

intmain(){
int i = 2;
func(i);// 普通函数
func(2);// 编译期间就会被计算出来
}
enum class

c++11 新增有作用域的枚举类型，看代码

不带作用域的枚举代码：

enum AColor {
kRed,
kGreen,
kBlue
};

enum BColor {
kWhite,
kBlack,
kYellow
};

intmain(){
if (kRed == kWhite) {
cout << "red == white" << endl;
}
return0;
}

如上代码，不带作用域的枚举类型可以自动转换成整形，且不同的枚举可以相互比较，代码中的红色居然可以和白色比较，这都是潜在的难以调试的 bug，而这种完全可以通过有作用域的枚举来规避。

有作用域的枚举代码：

enumclassAColor {
kRed,
kGreen,
kBlue
};

enumclassBColor {
kWhite,
kBlack,
kYellow
};

intmain(){
if (AColor::kRed == BColor::kWhite) { // 编译失败
cout << "red == white" << endl;
}
return0;
}

使用带有作用域的枚举类型后，对不同的枚举进行比较会导致编译失败，消除潜在 bug，同时带作用域的枚举类型可以选择底层类型，默认是 int，可以改成 char 等别的类型。

enumclassAColor :char {
kRed,
kGreen,
kBlue
};

我们平时编程过程中使用枚举，一定要使用有作用域的枚举取代传统的枚举。

非受限联合体

c++11 之前 union 中数据成员的类型不允许有非 POD 类型，而这个限制在 c++11 被取消，允许数据成员类型有非 POD 类型，看代码：

structA {
int a;
int \*b;
};

union U {
A a; // 非 POD 类型 c++11 之前不可以这样定义联合体
int b;
};

对于什么是 POD 类型，大家可以自行查下资料，大体上可以理解为对象可以直接 memcpy 的类型。

sizeof

c++11 中 sizeof 可以用的类的数据成员上，看代码：

c++11 前：

structA {
int data[10];
int a;
};

intmain(){
A a;
cout << "size " << sizeof(a.data) << endl;
return0;
}

c++11 后：

structA {
int data[10];
int a;
};

intmain(){
cout << "size " << sizeof(A::data) << endl;
return0;
}

想知道类中数据成员的大小在 c++11 中是不是方便了许多，而不需要定义一个对象，在计算对象的成员大小。

assertion
static_assert(true/false, message);

c++11 引入 static_assert 声明，用于在编译期间检查，如果第一个参数值为 false，则打印 message，编译失败。

自定义字面量

c++11 可以自定义字面量，我们平时 c++中都或多或少使用过 chrono 中的时间，例如：

std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100ms
std::this_thread::sleep_for(std::chrono::seconds(100)); // 100s

其实没必要这么麻烦，也可以这么写：

std::this_thread::sleep_for(100ms); // c++14 里可以这么使用，这里只是举个自定义字面量使用的例子
std::this_thread::sleep_for(100s);

这就是自定义字面量的使用，示例如下：

structmytype {
unsignedlonglong value;
};
constexpr mytype operator"" \_mytype ( unsignedlonglong n ) {
return mytype{n};
}
mytype mm = 123_mytype;
cout << mm.value << endl;

关于自定义字面量，可以看下 chrono 的源代码，相信大家会有很大收获，需要源码分析 chrono 的话，可以留言给我。

内存对齐
什么是内存对齐

理论上计算机对于任何变量的访问都可以从任意位置开始，然而实际上系统会对这些变量的存放地址有限制，通常将变量首地址设为某个数 N 的倍数，这就是内存对齐。

为什么要内存对齐
硬件平台限制，内存以字节为单位，不同硬件平台不一定支持任何内存地址的存取，一般可能以双字节、4 字节等为单位存取内存，为了保证处理器正确存取数据，需要进行内存对齐。
提高 CPU 内存访问速度，一般处理器的内存存取粒度都是 N 的整数倍，假如访问 N 大小的数据，没有进行内存对齐，有可能就需要两次访问才可以读取出数据，而进行内存对齐可以一次性把数据全部读取出来，提高效率。

在 c++11 之前如果想创建内存对齐需要：

voidalign_cpp11_before()
{
staticchar data[sizeof(void *) + sizeof(A)];
constuintptr_t kAlign = sizeof(void *) - 1;
char *align_ptr =
reinterpret_cast<char *>(reinterpret_cast<uintptr_t>(data + kAlign) & ~kAlign);
A *attr = new (align_ptr) A;
}

c++11 关于内存对齐新增了一些函数：

voidalign_cpp11_after()
{
staticstd::aligned_storage<sizeof(A),
alignof(A)>::type data;
A \*attr = new (&data) A;
}

还有：alignof()、std::alignment_of()、alignas()，关于内存对齐详情可以看这篇文章：内存对齐之格式修订版

thread_local

c++11 引入 thread_local，用 thread_local 修饰的变量具有 thread 周期，每一个线程都拥有并只拥有一个该变量的独立实例，一般用于需要保证线程安全的函数中。

#include<iostream>
#include<thread>

classA {
public:
A() {}
~A() {}

voidtest(conststd::string &name){
thread_localint count = 0;
++count;
std::cout << name << ": " << count << std::endl;
}
};

voidfunc(conststd::string &name){
A a1;
a1.test(name);
a1.test(name);
A a2;
a2.test(name);
a2.test(name);
}

intmain(){
std::thread(func, "thread1").join();
std::thread(func, "thread2").join();
return0;
}

输出：

thread1: 1
thread1: 2
thread1: 3
thread1: 4
thread2: 1
thread2: 2
thread2: 3
thread2: 4

验证上述说法，对于一个线程私有变量，一个线程拥有且只拥有一个该实例，类似于 static。

基础数值类型

c++11 新增了几种数据类型：long long、char16_t、char32_t 等

随机数功能

c++11 关于随机数功能则较之前丰富了很多，典型的可以选择概率分布类型，先看如下代码：

#include<time.h>

#include<iostream>
#include<random>

usingnamespacestd;

intmain(){
std::default_random_engine random(time(nullptr));

std::uniform_int_distribution<int> int_dis(0, 100); // 整数均匀分布
std::uniform_real_distribution<float> real_dis(0.0, 1.0); // 浮点数均匀分布

for (int i = 0; i < 10; ++i) {
cout << int_dis(random) << ' ';
}
cout << endl;

for (int i = 0; i < 10; ++i) {
cout << real_dis(random) << ' ';
}
cout << endl;

return0;
}

输出：

38 100 93 7 66 0 68 99 41 7
0.232202 0.617716 0.959241 0.970859 0.230406 0.430682 0.477359 0.971858 0.0171148 0.64863

代码中举例的是整数均匀分布和浮点数均匀分布，c++11 提供的概率分布类型还有好多，例如伯努利分布、正态分布等，具体可以见最后的参考资料。

正则表达式

c++11 引入了 regex 库更好的支持正则表达式，见代码：

#include<iostream>
#include<iterator>
#include<regex>
#include<string>

intmain(){
std::string s = "I know, I'll use2 regular expressions.";
// 忽略大小写
std::regex self_regex("REGULAR EXPRESSIONS", std::regex_constants::icase);
if (std::regex_search(s, self_regex)) {
std::cout << "Text contains the phrase 'regular expressions'\n";
}

std::regex word_regex("(\\w+)"); // 匹配字母数字等字符
auto words_begin = std::sregex_iterator(s.begin(), s.end(), word_regex);
auto words_end = std::sregex_iterator();

std::cout << "Found " << std::distance(words_begin, words_end) << " words\n";

constint N = 6;
std::cout << "Words longer than " << N << " characters:\n";
for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
std::smatch match = \*i;
std::string match_str = match.str();
if (match_str.size() > N) {
std::cout << " " << match_str << '\n';
}
}

std::regex long_word_regex("(\\w{7,})");
// 超过 7 个字符的单词用[]包围
std::string new_s = std::regex_replace(s, long_word_regex, "[$&]");
std::cout << new_s << '\n';
}
chrono

c++11 关于时间引入了 chrono 库，源于 boost，功能强大，chrono 主要有三个点：

duration
time_point
clocks
duration

std::chrono::duration 表示一段时间，常见的单位有 s、ms 等，示例代码：

// 拿休眠一段时间举例，这里表示休眠 100ms
std::this_thread::sleep_for(std::chrono::milliseconds(100));

sleep_for 里面其实就是 std::chrono::duration，表示一段时间，实际是这样：

typedef duration<int64_t, milli> milliseconds;
typedef duration<int64_t> seconds;

duration 具体模板如下：

1 template <classRep, classPeriod= ratio<1> > classduration;

Rep 表示一种数值类型，用来表示 Period 的数量，比如 int、float、double，Period 是 ratio 类型，用来表示【用秒表示的时间单位】比如 second，常用的 duration<Rep, Period>已经定义好了，在 std::chrono::duration 下：

ratio<3600, 1>：hours
ratio<60, 1>：minutes
ratio<1, 1>：seconds
ratio<1, 1000>：microseconds
ratio<1, 1000000>：microseconds
ratio<1, 1000000000>：nanosecons

ratio 的具体模板如下：

template <intmax_t N, intmax_t D = 1> class ratio;

N 代表分子，D 代表分母，所以 ratio 表示一个分数，我们可以自定义 Period，比如 ratio<2, 1>表示单位时间是 2 秒。

time_point

表示一个具体时间点，如 2020 年 5 月 10 日 10 点 10 分 10 秒，拿获取当前时间举例：

std::chrono::time_point<std::chrono::high_resolution_clock> Now() {
returnstd::chrono::high_resolution_clock::now();
}
// std::chrono::high_resolution_clock 为高精度时钟，下面会提到

clocks

时钟，chrono 里面提供了三种时钟：

steady_clock
system_clock
high_resolution_clock
steady_clock

稳定的时间间隔，表示相对时间，相对于系统开机启动的时间，无论系统时间如何被更改，后一次调用 now()肯定比前一次调用 now()的数值大，可用于计时。

system_clock

表示当前的系统时钟，可以用于获取当前时间：

intmain(){
usingstd::chrono::system_clock;
system_clock::time_point today = system_clock::now();

std::time_t tt = system_clock::to_time_t(today);
std::cout << "today is: " << ctime(&tt);

return0;
}
// today is: Sun May 10 09:48:36 2020

high_resolution_clock

high_resolution_clock 表示系统可用的最高精度的时钟，实际上就是 system_clock 或者 steady_clock 其中一种的定义，官方没有说明具体是哪个，不同系统可能不一样，我之前看 gcc chrono 源码中 high_resolution_clock 是 steady_clock 的 typedef。

更多关于 chrono 的介绍可以看下我之前的文章：RAII 妙用之计算函数耗时

新增数据结构
std::forward_list：单向链表，只可以前进，在特定场景下使用，相比于 std::list 节省了内存，提高了性能
std::forward_list<int> fl = {1, 2, 3, 4, 5};
for (constauto &elem : fl) {
cout << elem;
}
std::unordered_set：基于 hash 表实现的 set，内部不会排序，使用方法和 set 类似
std::unordered_map：基于 hash 表实现的 map，内部不会排序，使用方法和 set 类似
std::array：数组，在越界访问时抛出异常，建议使用 std::array 替代普通的数组
std::tuple：元组类型，类似 pair，但比 pair 扩展性好
typedefstd::tuple<int, double, int, double> Mytuple;
Mytuple t(0, 1, 2, 3);
std::cout << "0 " << std::get<0>(t);
std::cout << "1 " << std::get<1>(t);
std::cout << "2 " << std::get<2>(t);
std::cout << "3 " << std::get<3>(t);

新增算法

all_of：检测表达式是否对范围[first, last)中所有元素都返回 true，如果都满足，则返回 true
std::vector<int> v(10, 2);
if (std::all_of(v.cbegin(), v.cend(), [](int i) { return i % 2 == 0; })) {
std::cout << "All numbers are even\n";
}
any_of：检测表达式是否对范围[first, last)中至少一个元素返回 true，如果满足，则返回 true，否则返回 false，用法和上面一样
none_of：检测表达式是否对范围[first, last)中所有元素都不返回 true，如果都不满足，则返回 true，否则返回 false，用法和上面一样
find_if_not：找到第一个不符合要求的元素迭代器，和 find_if 相反
copy_if：复制满足条件的元素
itoa：对容器内的元素按序递增
std::vector<int> l(10);
std::iota(l.begin(), l.end(), 19); // 19 为初始值
for (auto n : l) std::cout << n << ' ';
// 19 20 21 22 23 24 25 26 27 28
minmax_element：返回容器内最大元素和最小元素位置
intmain(){
std::vector<int> v = {3, 9, 1, 4, 2, 5, 9};

auto result = std::minmax*element(v.begin(), v.end());
std::cout << "min element at: " << *(result.first) << '\n';
std::cout << "max element at: " << \_(result.second) << '\n';
return0;
}
// min element at: 1
// max element at: 9
is_sorted、is_sorted_until：返回容器内元素是否已经排好序。

关于 c++11 的新特性基本上就是这些，相信各位看完一定会有所收获。

参考资料
https://zh.cppreference.com/w/cpp/language/range-for
https://juejin.im/post/5dcaa857e51d457f7675360b
https://zhuanlan.zhihu.com/p/21930436
https://zh.wikipedia.org/wiki/Nullptr
https://zh.wikipedia.org/wiki/Constexpr
https://zh.cppreference.com/w/cpp/language/enum
https://kheresy.wordpress.com/2019/03/27/using-enum-class/
https://zh.cppreference.com/w/cpp/language/union
http://c.biancheng.net/view/7165.html
https://zhuanlan.zhihu.com/p/77585472
http://www.cplusplus.com/reference/random/
https://zh.cppreference.com/w/cpp/regex
https://www.cnblogs.com/jwk000/p/3560086.html
https://zh.cppreference.com/w/cpp/algorithm/all_any_none_of

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