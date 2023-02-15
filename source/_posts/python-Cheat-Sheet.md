---
title: python Cheat Sheet
date: 2020-02-12 21:53:16
tags: Python使用
category: python
---

# 格式

## 类
**Bad**
```
class fooClass: ...
class foo_class: ...
```

**Good**
```
class FooClass: ...
```

## 函数
**Bad**
```
def CapCamelCase(*a): ...
def mixCamelCase(*a): ...
```
**Good**
```
def func_separated_by_underscores(*a): ...
```

## 变量
**Bad**
```
FooVar = "CapWords"
fooVar = "mixedCase"
Foo_Var = "CapWords_With_Underscore"
```

**Good**
```
# 局部变量
var = "lowercase"

# 内部使用
_var = "_single_leading_underscore"

# 避免与Python关键字冲突
var_ = "single_trailing_underscore_"

# 私有类属性 (private use in class)
__var = " __double_leading_underscore"

# "magic" 对象 或属性, ex: __init__
__name__

# 一次性变量, ex: _, v = (1, 2)
_ = "throwaway"
```

# python 语法
## python 版本
```
In [1]: import sys                                                              

In [2]: sys.version                                                             
Out[2]: '3.6.9 (default, Nov  7 2019, 10:44:02) \n[GCC 8.3.0]'

In [3]: sys.version_info                                                        
Out[3]: sys.version_info(major=3, minor=6, micro=9, releaselevel='final', serial=0)

In [4]: import platform                                                         

In [5]: platform.python_version()                                               
Out[5]: '3.6.9'

In [6]:  sys.version_info >= (3, 6)                                             
Out[6]: True

In [7]: sys.version_info >= (3, 7)                                              
Out[7]: False
```

## 使用range
在 pthon3 中， 内置函数 `range` 返回一个迭代 range object 替代列表，等价 python2 中的 `xrange` 。
```
In [2]: type(range(10))                                                         
Out[2]: range

In [3]: import sys                                                              

In [4]: sys.getsizeof(range(10000000000))                                       
Out[4]: 48

```

## while... else ...和for... else ...
当循环中没有``break` 出现时，才会执行 `else` 中的语句。
```
In [7]: for _ in range(5): 
   ...:     pass 
   ...: else: 
   ...:     print("no break") 
   ...:                                                                         
no break

In [10]: for i in range(5): 
    ...:     if i == 2: 
    ...:         print("break", i) 
    ...:         break 
    ...: else: 
    ...:     print("no break") 
    ...:                                                                        
break 2
```

## try ... except ... else ...
没有异常发生时将会执行 `else` 中的语句.
```
In [11]: try: 
    ...:     print("no exception") 
    ...: except: 
    ...:     pass 
    ...: else: 
    ...:     print("Success") 
    ...:                                                                        
no exception
Success

```
















