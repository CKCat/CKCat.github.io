---
title: unix环境高级编程12-线程控制
date: 2023-04-10 17:22:37
tags: unix
category: unix环境高级编程
---

## 线程限制

线程限制和 `sysconf` 的 name 参数

| 限制名称                        | 描述                                                 | name 参数                          |
| ------------------------------- | ---------------------------------------------------- | ---------------------------------- |
| `PTHREAD_DESTRUCTOR_ITERATIONS` | 线程退出时操作系统实现试图销毁线程特定数据的最大次数 | `_SC_THREAD_DESTRUCTOR_ITERATIONS` |
| `PTHREAD_KEYS_MAX`              | 进程可以创建的键的最大数目                           | `_SC_THREAD_KEYS_MAX`              |
| `PTHREAD_STACK_MIN`             | 一个线程的栈可用的最小字节数                         | `_SC_THREAD_STACK_MIN`             |
| `PTHREAD_THREADS_MAX`           | 进程可以创建的最大线程数                             | `_SC_THREAD_THREADS_MAX`           |

线程配置限制的实例

| 限制名称                        | FreeBSD 8.0    | Linux 3.2.0    | Mac OS X 10.6.8 | Solaris 10     |
| ------------------------------- | -------------- | -------------- | --------------- | -------------- |
| `PTHREAD_DESTRUCTOR_ITERATIONS` | 4              | 4              | 4               | 没有确定的限制 |
| `PTHREAD_KEYS_MAX`              | 256            | 1024           | 512             | 没有确定的限制 |
| `PTHREAD_STACK_MIN`             | 2048           | 16384          | 8192            | 8192           |
| `PTHREAD_THREADS_MAX`           | 没有确定的限制 | 没有确定的限制 | 没有确定的限制  | 没有确定的限制 |

注意，虽然某个操作系统实现可能没有提供访问这些限制的方法，但这并不意味着这些限制不存在，这只是意味着操作系统实现没有为使用 `sysconf` 访问这些值提供可用的方法。

## 线程属性

`pthread` 接口允许我们通过设置每个对象关联的不同属性来细调线程和同步对象的行为。通常，管理这些属性的函数都遵循相同的模式。

1. 每个对象与它自己类型的属性对象进行关联。
1. 有一个初始化函数，把属性设置为默认值。
1. 还有一个销毁属性对象的函数。
1. 每个属性都有一个从属性对象中获取属性值的函数。
1. 每个属性都有一个设置属性值的函数。

可以使用 `pthread_attr_t` 结构修改线程默认属性，并把这些属性与创建的线程联系起来。可以使用 `pthread_attr_init` 函数初始化 `pthread_attr_t` 结构。在调用 `pthread_attr_init` `以后，pthread_attr_t` 结构所包含的就是操作系统实现支持的所有线程属性的默认值。

```c
#include <pthread.h>
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号

如果 `pthread_attr_init` 的实现对属性对象的内存空间是动态分配的，`pthread_attr_destroy` 就会释放该内存空间。

POSIX.1 线程属性

| 名称        | 描述                               |
| ----------- | ---------------------------------- |
| detachstate | 线程的分离状态属性                 |
| guardsize   | 线程栈末尾的警戒缓冲区大小(字节数) |
| stackaddr   | 线程栈的最低地址                   |
| stacksize   | 线程栈的最小长度(字节数)           |

如果在创建线程时就知道不需要了解线程的终止状态，就可以修改 `pthread_attr_t` 结构中的 `detachstate` 线程属性，让线程一开始就处于分离状态。可以使用 `pthread_attr_setdetachstate` 函数把线程属性 `detachstate` 设置成以下两个合法值之一：`PTHREAD_CREATE_DETACHED`，以分离状态启动线程；或者 `PTHREAD_CREATE_JOINABLE`，正常启动线程，应用程序可以获取线程的终止状态。

```c
#include <pthread.h>
int pthread_attr_getdetachstate(const pthread_attr_t *restrict attr, int *detachstate);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int *detachstate);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号

可以调用 `pthread_attr_getdetachstate` 函数获取当前的 `detachstate` 线程属性。第二个参数所指向的整数要么设置成 `PTHREAD_CREATE_DETACHED`，要么设置成 `PTHREAD_CREATE_JOINABLE`，具体要取决于给定 `pthread_attr_t` 结构中的属性值。

例子，以分离状态创建线程。

```c
#include <pthread.h>

int makethread(void *(*fn)(void *), void *arg){
    int err;
    pthread_t tid;
    pthread_attr_t attr;

    err = pthread_attr_init(&attr);
    if (err != 0)
        return (err);
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err == 0)
        err = pthread_create(&tid, &attr, fn, arg);
    pthread_attr_destroy(&attr);
    return (err);
}
```

可以使用函数 `pthread_attr_getstack` 和 `pthread_attr_setstack` 对线程栈属性进行管理。

```c
#include <pthread.h>
int pthread_attr_getstack(const pthread_attr_t *restrict attr,
                        void **restrict stackaddr, size_t *restrict stacksize);
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号

如果线程栈的虚地址空间都用完了，那可以使用 `malloc` 或者 `mmap` 来为可替代的栈分配空间，并用 `pthread_attr_setstack` 函数来改变新建线程的栈位置。由 `stackaddr` 参数指定的地址可以用作线程栈的内存范围中的最低可寻址地址，该地址与处理器结构相应的边界应对齐。

`stackaddr` 线程属性被定义为栈的最低内存地址，但这并不一定是栈的开始位置。对于一个给定的处理器结构来说，如果栈是从高地址向低地址方向增长的，那么 `stackaddr` 线程属性将是栈的结尾位置，而不是开始位置。

应用程序也可以通过 `pthread_attr_getstacksize` 和 `pthread_attr_setstacksize` 函数读取或设置线程属性 `stacksize`。

```c
#include <pthread.h>
int pthread_attr_getstacksize(const pthread_attr_t *restrict attr, size_t *restrict stacksize);
int pthread_attr_setstacksize (pthread_attr_t *attr, size_t stacksize);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号

如果希望改变默认的栈大小，但又不想自己处理线程栈的分配问题，这时使用 `pthread_attr_setstacksize` 函数就非常有用。设置 `stacksize` 属性时，选择的 `stacksize` 不能小于 `PTHREAD_STACK_MIN`。

线程属性 `guardsize` 控制着线程栈末尾之后用以避免栈溢出的扩展内存的大小。这个属性默认值是由具体实现来定义的，但常用值是系统页大小。可以把 `guardsize` 线程属性设置为 0，不允许属性的这种特征行为发生：在这种情况下，不会提供警戒缓冲区。同样，如果修改了线程属性 `stackaddr`，系统就认为我们将自己管理栈，进而使栈警戒缓冲区机制无效，这等同于把 `guardsize` 线程属性设置为 0。

```c
#include <pthread.h>
int pthread_attr_getguardsize(const pthread_attr_t *restrict attr, size_t *restrict guardsize);
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号

如果 `guardsize` 线程属性被修改了，操作系统可能会把它取为页大小的整数倍。如果线程的栈指针溢出到警戒区域，应用程序就可能通过信号接收到出错信息。

## 同步属性

就像线程具有属性一样，线程的同步对象也有属性。

### 互斥量属性

互斥量属性是用 pthread_mutexattr_t 结构表示的。对于非默认属性，可以用 pthread_mutexattr_init 初始化 pthread_mutexattr_t 结构，用 pthread_mutexattr_destroy 来反初始化。

```c
#include <pthread.h>
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
```

两个函数的返回值：若成功，返回 0；否则，返回错误编号

`pthread_mutexattr_init` 函数将用默认的互斥量属性初始化 `pthread_mutexattr_t` 结构。值得注意的 3 个属性是：进程共享属性、健壮属性以及类型属性。

在进程中，多个线程可以访问同一个同步对象。如果进程共享互斥量属性设置为 `PTHREAD_PROCESS_SHARED`，从多个进程彼此之间共享的内存数据块中分配的互斥量就可以用于这些进程的同步。

可以使用 pthread_mutexattr_getpshared 函数查询 pthread_mutexattr_t 结构，得到它的进程共享属性，使用 pthread_mutexattr_setpshared 函数修改进程共享属性。

```c
#include <pthread.h>
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *restrict attr, int *restrict pshared);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
```

两个函数的返回值：若成功，返回 0；否则，返回错误编号

互斥量健壮属性与在多个进程间共享的互斥量有关。这意味着，当持有互斥量的进程终止时，需要解决互斥量状态恢复的问题。这种情况发生时，互斥量处于锁定状态，恢复起来很困难。其他阻塞在这个锁的进程将会一直阻塞下去。

可以使用 `pthread_mutexattr_getrobust` 函数获取健壮的互斥量属性的值。可以调用 `pthread_mutexattr_setrobust` 函数设置健壮的互斥量属性的值。

```c
#include <pthread.h>
int pthread_mutexattr_getrobust(const pthread_mutexattr_t *restrict attr, int *restrict robust);
int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr, int robust);
```

两个函数的返回值：若成功，返回 0；否则，返回错误编号

健壮属性取值有两种可能的情况。默认值是 `PTHREAD_MUTEX_STALLED`，这意味着持有互斥量的进程终止时不需要采取特别的动作。另一个取值是 `PTHREAD_MUTEX_ROBUST`。这个值将导致线程调用 `pthread_mutex_lock` 获取锁，而该锁被另一个进程持有，但它终止时并没有对该锁进行解锁，此时线程会阻塞，从 `pthread_mutex_lock` 返回的值为 EOWNERDEAD 而不是 0。

如果应用状态无法恢复，在线程对互斥量解锁以后，该互斥量将处于永久不可用状态。为了避免这样的问题，线程可以调用 `pthread_mutex_consistent` 函数，指明与该互斥量相关的状态在互斥量解锁之前是一致的。

```c
#include <pthread.h>
int pthread_mutex_consistent(pthread_mutex_t *mutex);
```

返回值：若成功，返回 0；否则，返回错误编号

类型互斥量属性控制着互斥量的锁定特性。

- `PTHREAD_MUTEX_NORMAL` 一种标准互斥量类型，不做任何特殊的错误检查或死锁检测。
- `PTHREAD_MUTEX_ERRORCHECK` 此互斥量类型提供错误检查。
- `PTHREAD_MUTEX_RECURSIVE` 此互斥量类型允许同一线程在互斥量解锁之前对该互斥量进行多次加锁。
- `PTHREAD_MUTEX_DEFAULT` 此互斥量类型可以提供默认特性和行为。

互斥量类型行为

| 互斥量类型               | 没有解锁时重新加锁? | 不占用时解锁? | 在己解锁时解锁? |
| ------------------------ | ------------------- | ------------- | --------------- |
| `PTHREAD_MUTEX_NORMAL`     | 死锁                | 未定义        | 未定义          |
| `PTHREAD_MUTEX_ERRORCHECK` | 返回错误            | 返回错误      | 返回错误        |
| `PTHREAD_MUTEX_RECURSIVE`  | 允许                | 返回错误      | 返回错误        |
| `PTHREAD_MUTEX_DEFAULT`    | 未定义              | 未定义        | 未定义          |

可以用 `pthread_mutexattr_gettype` 函数得到互斥量类型属性，用 `pthread_mutexattr_settype` 函数修改互斥量类型属性。
```c
#include <pthread.h>
int pthread_mutexattr_gettype(const pthread_mutexattr_t*restrict attr,int*restrict type);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
```
两个函数的返回值: 
- 若成功，返回0；
- 否则，返回错误编号

互斥量用于保护与条件变量关联的条件。



