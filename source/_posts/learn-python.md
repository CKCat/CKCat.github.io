---
title: python使用记录
date: 2019-12-11 17:22:25
tags: Python使用
category: python
---

本文章记录使用 python 过程中易忘的知识，方便自己查阅。

## 设置 pypi 镜像

### 临时使用

```bash
pip install -i https://pypi.tuna.tsinghua.edu.cn/simple some-package
```

注意，simple 不能少, 是 https 而不是 http

### 设为默认

升级 pip 到最新的版本 (>=10.0.0) 后进行配置：

```bash
pip install pip -U

# 清华源
pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple

# 或：
# 阿里源
pip config set global.index-url https://mirrors.aliyun.com/pypi/simple/
# 腾讯源
pip config set global.index-url http://mirrors.cloud.tencent.com/pypi/simple
# 豆瓣源
pip config set global.index-url http://pypi.douban.com/simple/
```

## pycharm

### pycharm 设置忽略大小写进行自动补齐

进入下列设置

> settings -> Editor -> General -> Code Completion

将 `Match case` 不勾选就可以忽略大小写进行自动补齐了。

### pycharm 设置 python 代码模板

设置 File > Settings > File and Code Template > Python Script

```python
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# File Name : ${NAME}
# Created by ${USER} on $DATE

__author__ = '${USER}'

if __name__ == '__main__':
    pass
```

一些模板变量含义

```python
${PROJECT_NAME} - 当前Project名称;
 ${NAME} - 在创建文件的对话框中指定的文件名;
 ${USER} - 当前用户名;
 ${DATE} - 当前系统日期;
 ${TIME} - 当前系统时间;
 ${YEAR} - 年;
 ${MONTH} - 月;
 ${DAY} - 日;
 ${HOUR} - 小时;
 ${MINUTE} - 分钟；
 ${PRODUCT_NAME} - 创建文件的IDE名称;
 ${MONTH_NAME_SHORT} - 英文月份缩写, 如: Jan, Feb, etc;
 ${MONTH_NAME_FULL} - 英文月份全称, 如: January, February, etc；
```

## python知识点

### Python 中 join 函数和 os.path.join 用法

> join ： 连接字符串数组。将字符串、元组、列表中的元素以指定的字符(分隔符)连接生成一个新的字符串

> os.path.join() ： 将多个路径组合后返回

```
语法：'sep'.join（seq）

参数说明：
    sep：分隔符。可以为空
    seq：要连接的元素序列、字符串、元组、字典等
上面的语法即：以sep作为分隔符，将seq所有的元素合并成一个新的字符串.

返回值：返回一个以分隔符sep连接各个元素后生成的字符串
```

例子：

```
import os

seq = ['hello', 'world', '世界','你好']
print(' '.join(seq))
print(os.path.join('home', "me", "mypath"))
```

输出

```
hello world 世界 你好
home/me/mypath
```



### PYTHONPATH 作用

`PYTHONPATH` 是 Python 搜索路径，默认我们 `import` 的模块都会从 `PYTHONPATH` 里面寻找。

默认情况下，`PYTHONPATH` 是个空值，可以通过命令设置临时环境变量 `PYTHONPATH` 。

```bash
# windows
set PYTHONPATH=path1;path2

# linux
export  PYTHONPATH=path1:path2
```

pycharm 默认会设置 `PYTHONPATH` 为源码根目录，vscode 则不会设置。

python 中获取 `PYTHONPATH` 值。

```bash
import os

os.environ['PYTHONPATH']
```

使用 `python -m` 执行python命令时， Python 将会在 `PYTHONPATH` 中添加当前目录。

### locals 和 globals 以及反射

## python 第三方库

### Django

#### Django 测试脚本

当你只是想测试 django 中的某一个 py 文件内容 那么你可以不用书写前后端交互的形式，而是直接写一个测试脚本即可

测试环境的准备：

去 `manage.py` 中拷贝前四行代码 然后自己写两行

```python
import os
import sys
​
if __name__ == "__main__":
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "day64.settings")
    import django
    django.setup()

```

在这个代码块的下面就可以测试 django 里面的单个 py 文件了。

> https://www.strongforu.top/archives/153



### celery



### mypy

#### json 类型定义

当 json 结果固定时，使用 [TypedDicts](https://mypy.readthedocs.io/en/latest/more_types.html#typeddict)，当结构不固定时，使用下面方法：

```python
from typing import Dict, List, Union, TypeAlias

Json: TypeAlias = Union[Dict[str, "Json"], List["Json"], str, int, float, bool, None]
```

参考：

https://stackoverflow.com/questions/51291722/define-a-jsonable-type-using-mypy-pep-526

https://github.com/python/typing/issues/182#issuecomment-1320974824
