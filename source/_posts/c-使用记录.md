---
title: c++使用记录
date: 2019-12-20 16:19:10
tags: C++
category: C++
---

# `C++` 名称粉碎使用 `C++filt` 工具恢复
例如：
```
$ c++filt _Z3addii
add(int, int)

```
# 判断编译平台宏定义
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

# error C2027: use of undefined type

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


