---
title: ARMv8学习记录08
date: 2023-07-04 13:19:32
tags: 汇编
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

