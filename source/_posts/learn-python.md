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

## conda cheat sheet

### 使用 TUNA 镜像源

可以通过修改用户目录下的 `.condarc` 文件来使用 TUNA 镜像源。Windows 用户无法直接创建名为 `.condarc` 的文件，可先执行 `conda config --set show_channel_urls yes` 生成该文件之后再修改。

```yaml
channels:
  - defaults
show_channel_urls: true
default_channels:
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/r
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/msys2
custom_channels:
  conda-forge: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  msys2: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  bioconda: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  menpo: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  pytorch: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  pytorch-lts: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  simpleitk: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  deepmodeling: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/
```

运行 `conda clean -i` 清除索引缓存，保证用的是镜像站提供的索引。

### 管理 Conda

```bash
conda info                                        # 验证 conda 是否已安装，检查版本
conda update conda                                # 更新 conda 包和环境管理器
conda update --all                                # 更新所有的包
conda config --show channels                      # 列出通道
conda config --prepend channels conda-forge       # 添加一个通道（最高优先级）
conda config --append channels bioconda           # 添加一个通道（最低优先级）
conda config --get                                # 从 .condarc 文件中获取所有键和值
```

### 管理环境

```bash
conda info --envs                                     # 获取所有环境的列表
conda info -e
conda env list

conda create --name py35 python=3.5                   # 创建一个名为 py35 的虚拟环境, 安装 python3.5
conda create -n py35 python=3.5
conda create --yes -n py35 python=3.5
conda create --clone py35 --name py35-2               # clone 一个已存在的环境

conda list                                            # 显示当前环境的所有包信息
conda list ^z                                         # 显示以 z 开始的包信息
conda list --revisions                                # 列出对当前环境的每个更改的历史

conda install numpy pandas                            # 使用conda install命令安装包，格式为conda install {pkg1} {pkg2}
conda install --revision 2                            # 将环境还原到先前的版本。
conda uninstall pycaret                               # 使用conda uninstall命令卸载包，格式为conda uninstall {pkg}

conda list --explicit > bio-env.txt                   # 将环境保存到文本文件
conda env create --file bio-env.txt                   # 从文本文件创建环境
conda env remove --name bio-env                       # 删除一个环境以及其中的所有内容
conda search PACKAGENAME                              # 使用conda搜索包

conda env export > environment.yml                    # 导出每个包及其依赖项（最大可再现性）。
conda env export --from-history > environment.yml     # 只导出显式请求的包（增加可移植性）。
conda env create --name my_env2 --file environment.yml # 从YAML文件导入环境。

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

```bash
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

## python cheat sheet

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

使用 `python -m` 执行 python 命令时， Python 将会在 `PYTHONPATH` 中添加当前目录。

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

### mypy cheat sheet

禁用特定行或代码库中特定文件内的类型检查。为此，您可以使用 `# type: ignore` 注释。

#### 基本类型

```python
# 对于大多数类型，mypy通常可以从变量的值中推断出其类型
x: int = 1
x: float = 1.0
x: bool = True
x: str = "test"
x: bytes = b"test"

# For collections on Python 3.9+, the type of the collection item is in brackets
x: list[int] = [1]
x: set[int] = {6, 7}

# For mappings, we need the types of both keys and values
x: dict[str, float] = {"field": 2.0}  # Python 3.9+

# For tuples of fixed size, we specify the types of all the elements
x: tuple[int, str, float] = (3, "yes", 7.5)  # Python 3.9+

# For tuples of variable size, we use one type and ellipsis
x: tuple[int, ...] = (1, 2, 3)  # Python 3.9+

# On Python 3.8 and earlier, the name of the collection type is
# capitalized, and the type is imported from the 'typing' module
from typing import List, Set, Dict, Tuple
x: List[int] = [1]
x: Set[int] = {6, 7}
x: Dict[str, float] = {"field": 2.0}
x: Tuple[int, str, float] = (3, "yes", 7.5)
x: Tuple[int, ...] = (1, 2, 3)

from typing import Union, Optional

# On Python 3.10+, use the | operator when something could be one of a few types
x: list[int | str] = [3, 5, "test", "fun"]  # Python 3.10+
# On earlier versions, use Union
x: list[Union[int, str]] = [3, 5, "test", "fun"]

# Use Optional[X] for a value that could be None
# Optional[X] is the same as X | None or Union[X, None]
x: Optional[str] = "something" if some_condition() else None
if x is not None:
    # Mypy 可以理解 if 语句，x在这里不会是None。
    print(x.upper())
# 如果你知道一个值永远不会是None，但是 mypy 不理解的逻辑，可以使用assert。
assert x is not None
print(x.upper())
```

#### 函数

```python
from typing import Callable, Iterator, Union, Optional

# 这是如何对函数定义进行注释的
def stringify(num: int) -> str:
    return str(num)

# 这是如何指定多个参数的方式
def plus(num1: int, num2: int) -> int:
    return num1 + num2

# 如果一个函数没有返回值，使用None作为返回类型。参数的默认值在类型注释之后。
def show(value: str, excitement: int = 10) -> None:
    print(value + "!" * excitement)

# 请注意，没有类型的参数是动态类型（被视为Any），没有任何注释的函数不会进行检查。
def untyped(x):
    x.anything() + 1 + "string"  # no errors

# 这是如何对可调用（函数）值进行注释的。
x: Callable[[int, float], float] = f
def register(callback: Callable[[str], int]) -> None: ...

# 一个产生整数的生成器函数实际上就是一个返回整数迭代器的函数，因此我们就是这样对其进行注释的。
def gen(n: int) -> Iterator[int]:
    i = 0
    while i < n:
        yield i
        i += 1

# 当然，你可以将一个函数注释拆分成多行。
def send_email(address: Union[str, list[str]],
               sender: str,
               cc: Optional[list[str]],
               bcc: Optional[list[str]],
               subject: str = '',
               body: Optional[list[str]] = None
               ) -> bool:
    ...

# Mypy理解位置参数和关键字参数。位置参数也可以通过使用以两个下划线开头的名称进行标记。
def quux(x: int, /, *, y: int) -> None:
    pass

quux(3, y=5)  # Ok
quux(3, 5)  # error: Too many positional arguments for "quux"
quux(x=3, y=5)  # error: Unexpected keyword argument "x" for "quux"

# 这表示每个位置参数和每个关键字参数都是 "str" 类型。
def call(self, *args: str, **kwargs: str) -> str:
    reveal_type(args)  # Revealed type is "tuple[str, ...]"
    reveal_type(kwargs)  # Revealed type is "dict[str, str]"
    request = make_request(*args, **kwargs)
    return self.do_api_query(request)
```

#### 类

```python
class BankAccount:
    # The "__init__" method doesn't return anything, so it gets return
    # type "None" just like any other method that doesn't return anything
    def __init__(self, account_name: str, initial_balance: int = 0) -> None:
        # mypy will infer the correct types for these instance variables
        # based on the types of the parameters.
        self.account_name = account_name
        self.balance = initial_balance

    # For instance methods, omit type for "self"
    def deposit(self, amount: int) -> None:
        self.balance += amount

    def withdraw(self, amount: int) -> None:
        self.balance -= amount

# User-defined classes are valid as types in annotations
account: BankAccount = BankAccount("Alice", 400)
def transfer(src: BankAccount, dst: BankAccount, amount: int) -> None:
    src.withdraw(amount)
    dst.deposit(amount)

# Functions that accept BankAccount also accept any subclass of BankAccount!
class AuditedBankAccount(BankAccount):
    # You can optionally declare instance variables in the class body
    audit_log: list[str]

    def __init__(self, account_name: str, initial_balance: int = 0) -> None:
        super().__init__(account_name, initial_balance)
        self.audit_log: list[str] = []

    def deposit(self, amount: int) -> None:
        self.audit_log.append(f"Deposited {amount}")
        self.balance += amount

    def withdraw(self, amount: int) -> None:
        self.audit_log.append(f"Withdrew {amount}")
        self.balance -= amount

audited = AuditedBankAccount("Bob", 300)
transfer(audited, account, 100)  # type checks!

# You can use the ClassVar annotation to declare a class variable
class Car:
    seats: ClassVar[int] = 4
    passengers: ClassVar[list[str]]

# If you want dynamic attributes on your class, have it
# override "__setattr__" or "__getattr__"
class A:
    # This will allow assignment to any A.x, if x is the same type as "value"
    # (use "value: Any" to allow arbitrary types)
    def __setattr__(self, name: str, value: int) -> None: ...

    # This will allow access to any A.x, if x is compatible with the return type
    def __getattr__(self, name: str) -> int: ...

a.foo = 42  # Works
a.bar = 'Ex-parrot'  # Fails type checking
```

https://github.com/python/mypy

#### json 类型定义

当 json 结果固定时，使用 [TypedDicts](https://mypy.readthedocs.io/en/latest/more_types.html#typeddict)，当结构不固定时，使用下面方法：

```python
from typing import Dict, List, Union, TypeAlias

Json: TypeAlias = Union[Dict[str, "Json"], List["Json"], str, int, float, bool, None]
```

参考：

https://stackoverflow.com/questions/51291722/define-a-jsonable-type-using-mypy-pep-526

https://github.com/python/typing/issues/182#issuecomment-1320974824

### pydantic

### poetry-cheatsheet

https://python-poetry.org/

#### 创建项目

```bash
poetry new <project-name>  # Create a new project
poetry init                # Create pyproject.toml
```

#### 包管理

```bash
poetry add <package_name>       # Add a new lib
poetry remove <package_name>    # Remove a lib
poetry update <package_name>    # Update a lib
poetry show <package_name>      # show a lib
poetry build                    # Create a package
poetry version prerelease       # Update the alpha version of the next release number.
poetry version patch            # Update the patch version of the next release number.
poetry install                  # install dependencies
```

例子：

```bash
poetry add Flask                   # 添加 Flask 包
poetry add "Flask==1.1.2"          # 添加 Flask 包，并指定版本为 1.1.2
poetry add "Flask==1.1.2" --dev    # 添加包到开发依赖项中
poetry add "/path/to/locallib"     # 通过指定库路径添加本地依赖
poetry remove Flask                # 移除 Flask 包
poetry update                      # 更新所有的依赖包
poetry update Flask                # 更新 Flask 包
poetry show                        # 显示所有已安装软件包的列表以及说明
poetry show Flask                  # 显示 Flask 包的信息
poetry install --no-dev            # 安装所有依赖项（开发依赖项除外)
poetry install -n                  # 禁用任何可能使安装暂停的交互式问题
```

#### 虚拟环境

```bash
poetry env use $(which python3)            # 在当前项目中创建一个新的虚拟环境
poetry env list                            # 查找虚拟环境列表
poetry env list                            # 查找虚拟环境列表，包括其完整路径
poetry env remove <env_name>               # 通过指定虚拟环境的名称删除虚拟环境
poetry run which python                    # 获取当前活动的 Python 解释器的路径
poetry shell                               # 激活Python虚拟环境
poetry run python                          # 在虚拟环境中执行python命令
poetry run python -m unittest discover     # Run tests
```

#### 配置

```bash
poetry config virtualenvs.create false    # Disable virtual environment creation
poetry config --list                      # List configuratiom
```

# python 打包工具

⭐⭐⭐⭐⭐[PyInstaller](https://github.com/pyinstaller/pyinstaller): 支持 Windows、Mac 和 Linux 上的 Python 3.8-3.12，比较容易使用。

⭐⭐⭐[cx_Freeze](https://github.com/marcelotduarte/cx_Freeze): 支持 Windows、Mac 和 Linux 上的 Python 3.7-3.11，易用程度一般。

⭐⭐[py2exe](https://github.com/py2exe/py2exe): 仅支持 Windows 上的 Python 3.7 - 3.10。

⭐⭐[py2app](https://github.com/ronaldoussoren/py2app): 仅在 Mac 上支持 Python 3.6 - 3.10。

⭐[Nuitka](https://github.com/kayhayen/Nuitka): Nuitka 会逐字编译您的 Python 代码并生成一个应用来尝试加速您的代码。支持 Windows、Mac 和 Linux 上的 Python 2.6 - 2.7 和 Python 3.4 - 3.11。
