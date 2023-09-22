---
title: unix环境高级编程12-线程控制
date: 2023-04-10 17:22:37
tags: linux 编程
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
- 否则，返回错误编号。

如果 `pthread_attr_init` 的实现对属性对象的内存空间是动态分配的，`pthread_attr_destroy` 就会释放该内存空间。

POSIX.1 线程属性

| 名称          | 描述                               |
| ------------- | ---------------------------------- |
| `detachstate` | 线程的分离状态属性                 |
| `guardsize`   | 线程栈末尾的警戒缓冲区大小(字节数) |
| `stackaddr`   | 线程栈的最低地址                   |
| `stacksize`   | 线程栈的最小长度(字节数)           |

如果在创建线程时就知道不需要了解线程的终止状态，就可以修改 `pthread_attr_t` 结构中的 `detachstate` 线程属性，让线程一开始就处于分离状态。可以使用 `pthread_attr_setdetachstate` 函数把线程属性 `detachstate` 设置成以下两个合法值之一：`PTHREAD_CREATE_DETACHED`，以分离状态启动线程；或者 `PTHREAD_CREATE_JOINABLE`，正常启动线程，应用程序可以获取线程的终止状态。

```c
#include <pthread.h>
int pthread_attr_getdetachstate(const pthread_attr_t *restrict attr, int *detachstate);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int *detachstate);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

`pthread_attr_getdetachstate` 参数：

- `attr`：指向要查询的线程属性对象的指针。
- `detachstate`：一个整数指针，用于存储线程的分离状态。如果线程属性对象 `attr` 包含有效的分离状态信息，该整数将被设置为 `PTHREAD_CREATE_DETACHED` 或 `PTHREAD_CREATE_JOINABLE`。

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
- 否则，返回错误编号。

`pthread_attr_setstack` 参数：

- `attr`：指向要设置栈属性的线程属性对象的指针。
- `stackaddr`：一个指向线程栈起始地址的指针。通常可以设置为 `NULL`，让系统自动分配栈空间。
- `stacksize`：线程栈的大小（以字节为单位）。

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
- 否则，返回错误编号。

`pthread_attr_setstacksize` 参数：

- `attr`：指向要设置栈大小属性的线程属性对象的指针。
- `stacksize`：线程栈的大小，以字节为单位。

如果希望改变默认的栈大小，但又不想自己处理线程栈的分配问题，这时使用 `pthread_attr_setstacksize` 函数就非常有用。设置 `stacksize` 属性时，选择的 `stacksize` 不能小于 `PTHREAD_STACK_MIN`。

线程属性 `guardsize` 控制着线程栈末尾之后用以避免栈溢出的扩展内存的大小。这个属性默认值是由具体实现来定义的，但常用值是系统页大小。可以把 `guardsize` 线程属性设置为 0，不允许属性的这种特征行为发生：在这种情况下，不会提供警戒缓冲区。同样，如果修改了线程属性 `stackaddr`，系统就认为我们将自己管理栈，进而使栈警戒缓冲区机制无效，这等同于把 `guardsize` 线程属性设置为 0。

```c
#include <pthread.h>
int pthread_attr_getguardsize(const pthread_attr_t *restrict attr, size_t *restrict guardsize);
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

`pthread_attr_setguardsize` 参数：

- `attr`：指向要设置守护区大小属性的线程属性对象的指针。
- `guardsize`：线程栈的守护区大小，以字节为单位。

如果 `guardsize` 线程属性被修改了，操作系统可能会把它取为页大小的整数倍。如果线程的栈指针溢出到警戒区域，应用程序就可能通过信号接收到出错信息。

## 同步属性

就像线程具有属性一样，线程的同步对象也有属性。

### 互斥量属性

互斥量属性是用 `pthread_mutexattr_t` 结构表示的。对于非默认属性，可以用 `pthread_mutexattr_init` 初始化 `pthread_mutexattr_t` 结构，用 `pthread_mutexattr_destroy` 来反初始化。

```c
#include <pthread.h>
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

`pthread_mutexattr_init` 函数将用默认的互斥量属性初始化 `pthread_mutexattr_t` 结构。值得注意的 3 个属性是：进程共享属性、健壮属性以及类型属性。

在进程中，多个线程可以访问同一个同步对象。如果进程共享互斥量属性设置为 `PTHREAD_PROCESS_SHARED`，从多个进程彼此之间共享的内存数据块中分配的互斥量就可以用于这些进程的同步。

可以使用 `pthread_mutexattr_getpshared` 函数查询 `pthread_mutexattr_t` 结构，得到它的进程共享属性，使用 `pthread_mutexattr_setpshared` 函数修改进程共享属性。

```c
#include <pthread.h>
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *restrict attr, int *restrict pshared);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

进程共享互斥量属性设置为 `PTHREAD_PROCESS_PRIVATE` 时，允许 `pthread` 线程库提供更有效的互斥量实现，这在多线程应用程序中是默认的情况。

互斥量健壮属性与在多个进程间共享的互斥量有关。这意味着，当持有互斥量的进程终止时，需要解决互斥量状态恢复的问题。这种情况发生时，互斥量处于锁定状态，恢复起来很困难。其他阻塞在这个锁的进程将会一直阻塞下去。

可以使用 `pthread_mutexattr_getrobust` 函数获取健壮的互斥量属性的值。可以调用 `pthread_mutexattr_setrobust` 函数设置健壮的互斥量属性的值。

```c
#include <pthread.h>
int pthread_mutexattr_getrobust(const pthread_mutexattr_t *restrict attr, int *restrict robust);
int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr, int robust);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

`pthread_mutexattr_setrobust` 参数：

- `attr`：指向要设置互斥量属性的属性对象的指针。
- `robust`：一个整数值，用于指定互斥量的健壮性属性。可以是以下值之一：
  - `PTHREAD_MUTEX_STALLED`：默认值。互斥量的健壮性未启用。在互斥量的持有者异常终止时，等待互斥量的线程可能会无限期地阻塞。
  - `PTHREAD_MUTEX_ROBUST`：启用互斥量的健壮性。当互斥量的持有者异常终止时，等待互斥量的线程将被唤醒，并且可以继续执行。
  - `PTHREAD_MUTEX_ROBUST_NP`：类似于 `PTHREAD_MUTEX_ROBUST`，但不是 POSIX 标准的一部分，因此可移植性较差。

互斥量的健壮性属性决定了在线程异常终止时等待互斥量的线程的行为。健壮性属性的使用可以确保在互斥量持有者异常终止时，等待该互斥量的线程能够正常继续执行。

健壮属性取值有两种可能的情况。默认值是 `PTHREAD_MUTEX_STALLED`，这意味着持有互斥量的进程终止时不需要采取特别的动作。另一个取值是 `PTHREAD_MUTEX_ROBUST`，这个值将导致线程调用 `pthread_mutex_lock` 获取锁，而该锁被另一个进程持有，但它终止时并没有对该锁进行解锁，此时线程会阻塞，从 `pthread_mutex_lock` 返回的值为 `EOWNERDEAD` 而不是 0。

如果应用状态无法恢复，在线程对互斥量解锁以后，该互斥量将处于永久不可用状态。为了避免这样的问题，线程可以调用 `pthread_mutex_consistent` 函数，指明与该互斥量相关的状态在互斥量解锁之前是一致的。

```c
#include <pthread.h>
int pthread_mutex_consistent(pthread_mutex_t *mutex);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

如果线程没有先调用 `pthread_mutex_consistent` 就对互斥量进行了解锁，那么其他试图获取该互斥量的阻塞线程就会得到错误码 `ENOTRECOVERABLE`。如果发生这种情况，互斥量将不再可用。线程通过提前调用 `pthread_mutex_consistent`，能让互斥量正常工作，这样它就可以持续被使用。

类型互斥量属性控制着互斥量的锁定特性。

- `PTHREAD_MUTEX_NORMAL` 一种标准互斥量类型，不做任何特殊的错误检查或死锁检测。
- `PTHREAD_MUTEX_ERRORCHECK` 此互斥量类型提供错误检查。
- `PTHREAD_MUTEX_RECURSIVE` 此互斥量类型允许同一线程在互斥量解锁之前对该互斥量进行多次加锁。递归互斥量维护锁的计数，在解锁次数和加锁次数不相同的情况下，不会释放锁。所以，如果对一个递归互斥量加锁两次，然后解锁一次，那么这个互斥量将依然处于加锁状态，对它再次解锁以前不能释放该锁。
- `PTHREAD_MUTEX_DEFAULT` 此互斥量类型可以提供默认特性和行为。操作系统在实现它的时候可以把这种类型自由地映射到其他互斥量类型中的一种。

互斥量类型行为

| 互斥量类型                 | 没有解锁时重新加锁? | 不占用时解锁? | 在己解锁时解锁? |
| -------------------------- | ------------------- | ------------- | --------------- |
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

- 若成功，返回 0；
- 否则，返回错误编号。

`pthread_mutexattr_settype` 参数：

- `attr`：指向要设置互斥量属性的属性对象的指针。
- `type`：一个整数值，用于指定互斥量的类型。可以是以下值之一：
  - `PTHREAD_MUTEX_NORMAL`：默认值。普通互斥量，不支持递归锁定。如果同一线程多次锁定这种互斥量，会导致死锁。
  - `PTHREAD_MUTEX_ERRORCHECK`：错误检查互斥量。如果同一线程多次锁定这种互斥量，会返回错误，而不是导致死锁。
  - `PTHREAD_MUTEX_RECURSIVE`：递归互斥量。允许同一线程多次锁定互斥量，每次都必须有相应数量的解锁操作，才能释放互斥量。
  - `PTHREAD_MUTEX_DEFAULT`：实现定义的默认类型。通常等同于 `PTHREAD_MUTEX_NORMAL`。

互斥量用于保护与条件变量关联的条件，在阻塞线程之前，`pthread_cond_wait` 和 `pthread_cond_timedwait` 函数释放与条件相关的互斥量。这就允许其他线程获取互斥量、改变条件、释放互斥量以及给条件变量发信号。既然改变条件时必须占有互斥量，使用递归互斥量就不是一个好主意。如果递归互斥量被多次加锁，然后用在调用 `pthread_cond_wait` 函数中，那么条件永远都不会得到满足，因为 `pthread_cond_wait` 所做的解锁操作并不能释放互斥量。

如果需要把现有的单线程接口放到多线程环境中，递归互斥量是非常有用的，但由于现有程序兼容性的限制，不能对函数接口进行修改。然而，使用递归锁可能很难处理，因此应该只在没有其他可行方案的时候才使用它们。

例子，使用递归互斥量。

```cpp
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <malloc.h>

struct to_info {
	void	      (*to_fn)(void *);	/* function */
	void           *to_arg;			/* argument */
	struct timespec to_wait;		/* time to wait */
};

#define SECTONSEC  1000000000	/* seconds to nanoseconds */

int makethread(void *(*fn)(void *), void *arg){
	int err;
	pthread_t tid;
	pthread_attr_t attr;

	err = pthread_attr_init(&attr);
	if(err != 0)
		return err;
	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(err == 0)
		err = pthread_create(&tid, &attr, fn, arg);
	pthread_attr_destroy(&attr);
	return err;
}

void *timeout_helper(void *arg){
	struct to_info *tip;
	tip = (struct to_info *)arg;
	clock_nanosleep(CLOCK_REALTIME, 0, &tip->to_wait, NULL);
	(*tip->to_fn)(tip->to_arg);
	free(arg);
	return 0;
}

void timeout(const struct timespec *when, void (*func)(void*), void *arg){
	struct timespec now;
	struct to_info *tip;
	int err;

	clock_gettime(CLOCK_REALTIME, &now);
	if((when->tv_sec > now.tv_sec) ||
		(when->tv_sec == now.tv_sec && when->tv_nsec > now.tv_nsec)){
		tip = malloc(sizeof(struct to_info));
		if(tip != NULL){
			tip->to_fn = func;
			tip->to_arg = arg;
			tip->to_wait.tv_nsec = when->tv_sec - now.tv_sec;
			if(when->tv_nsec >= now.tv_nsec){
				tip->to_wait.tv_nsec = when->tv_nsec - now.tv_nsec;
			}else{
				tip->to_wait.tv_sec--;
				tip->to_wait.tv_nsec = SECTONSEC - now.tv_nsec + when->tv_nsec;
			}
			err = makethread(timeout_helper, (void *)tip);
			if(err == 0)
				return;
			else
				free(tip);
		}
	}
	/**
	 * 发生下面几种情况才会执行到这里
	 * 1. when <= now；
	 * 2. malloc 失败；
	 * 3. 无法创建一个线程。
	 */
	(*func)(arg);
}

pthread_mutexattr_t attr;
pthread_mutex_t mutex;

void retry(void *arg){
	pthread_mutex_lock(&mutex);

	// 执行重试步骤......

	pthread_mutex_unlock(&mutex);
}

int main(){
	int err, condition, arg;
	struct timespec when;
	if((err = pthread_mutexattr_init(&attr)) != 0){
		perror("pthread_mutexattr_init failed");
		return 1;
	}
	if((err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) != 0){
		perror("can't set recursive type");
		return 1;
	}
	if((err = pthread_mutex_init(&mutex, &attr)) != 0){
		perror("can't create recursive mutex");
		return 1;
	}
	// 继续处理......

	pthread_mutex_lock(&mutex);
	// 将条件检查和超时调用放在锁的保护下，以使检查和超时调用是原子操作。
	if(condition){
		// 计算我们希望重试的绝对时间。
		clock_gettime(CLOCK_REALTIME, &when);
		when.tv_sec += 10;
		timeout(&when, retry, (void *)((unsigned long)arg));
	}
	pthread_mutex_unlock(&mutex);
	// 继续处理......

	return 0;
}
```

### 读写锁属性

读写锁与互斥量类似，也是有属性的。可以用 `pthread_rwlockattr_init` 初始化 `pthread_rwlockattr_t` 结构，用 `pthread_rwlockattr_destroy` 反初始化该结构。

```cpp
#include <pthread.h>
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

读写锁支持的唯一属性是进程共享属性。它与互斥量的进程共享属性是相同的。

就像互斥量的进程共享属性一样，有一对函数用于读取和设置读写锁的进程共享属性。

```cpp
#include <pthread.h>
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *restrict attr, int *restrict pshared);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

`pthread_rwlockattr_setpshared` 参数：

- `attr`：指向读写锁属性对象（`pthread_rwlockattr_t`）的指针，可以在此对象中设置属性。
- `pshared`：一个整数值，用于指定共享属性。通常有两个可能的取值：
  - `PTHREAD_PROCESS_PRIVATE`：锁是进程内私有的，只能在同一进程的不同线程之间共享。
  - `PTHREAD_PROCESS_SHARED`：锁可以在不同进程之间共享。

### 条件变量属性

Single UNIX Specification 目前定义了条件变量的两个属性：进程共享属性和时钟属性。与其他的属性对象一样，有一对函数用于初始化和反初始化条件变量属性。

```cpp
#include <pthread.h>
int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

与其他的同步属性一样，条件变量支持进程共享属性。它控制着条件变量是可以被单进程的多个线程使用，还是可以被多进程的线程使用。

要获取进程共享属性的当前值，可以用 `pthread_condattr_getpshared` 函数。设置该值可以用 `pthread_condattr_setpshared` 函数。

```cpp
#include <pthread.h>
int pthread_condattr_getpshared(const pthread_condattr_t *restrict attr, int *restrict pshared);
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

时钟属性控制计算 `pthread_cond_timedwait` 函数的超时参数（`tsptr`）时采用的是哪个时钟，其时钟 ID 可以使用 `pthread_condattr_getclock` 函数获取；`pthread_condattr_setclock` 函数用于对时钟 ID 进行修改。

```cpp
#include <pthread.h>
int pthread_condattr_getclock(const pthread_condattr_t *restrict attr, clockid_t *restrict clock_id);
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);
```

两个函数的返回值:

- 若成功，返回 0；
- 否则，返回错误编号

在使用 `pthread_cond_timedwait` 函数前需要用 `pthread_condattr_t` 对象对条件变量进行初始化。

合法的时钟 ID 值：

| 标识符                     | 选项                    | 说明                     |
| -------------------------- | ----------------------- | ------------------------ |
| `CLOCK_REALTIME`           |                         | 实时系统时间             |
| `CLOCK_MONOTONIC`          | `POSIX_MONOTONIC_CLOCK` | 不带负跳数的实时系统时间 |
| `CLOCK_PROCESS_CPUTIME_ID` | `POSIX_CPUTIME`         | 调用进程的 CPU 时间      |
| `CLOCK_THREAD_CPUTIME_ID`  | `POSIX_THREAD_CPUTIME`  | 调用线程的 CPU 时间      |

### 屏障属性

屏障也有属性。可以使用 `pthread_barrierattr_init` 函数对屏障属性对象进行初始化，用 `pthread_barrierattr_destroy` 函数对屏障属性对象进行反初始化。

```cpp
#include <pthread.h>
int pthread_barrierattr_init(pthread_barrierattr_t *attr);
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

目前定义的屏障属性只有进程共享属性，它控制着屏障是可以被多进程的线程使用，还是只能被初始化屏障的进程内的多线程使用。与其他属性对象一样，有一个获取属性值的函数 `pthread_barrierattr_getpshared` 和一个设置属性值的函数 `pthread_barrierattr_setpshared`。

```cpp
#include <pthread.h>
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *restrict attr, int *restrict pshared);
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

进程共享属性的值可以是 `PTHREAD_PROCESS_SHARED`（多进程中的多个线程可用），也可以是 `PTHREAD_PROCESS_PRIVATE`（只有初始化屏障的那个进程内的多个线程可用）。

## 重入

线程在遇到重入问题时与信号处理程序是类似的。在这两种情况下，多个控制线程在相同的时间有可能调用相同的函数。

如果一个函数在相同的时间点可以被多个线程安全地调用，就称该函数是线程安全的。

`POSIX.1` 中不能保证线程安全的函数：

| `basename`      | `getchar_unlocked` | `getservent`  | `putc_unlocked`    |
| --------------- | ------------------ | ------------- | ------------------ |
| `catgets`       | `getdate`          | `getutxent`   | `putchar_unlocked` |
| `crypt`         | `getenv`           | `getutxid`    | `putenv`           |
| `dbm_clearerr`  | `getgrent`         | `getutxline`  | `pututxline`       |
| `dbm_close`     | `getgrgid`         | `gmtime`      | `rand`             |
| `dbm_delete`    | `getgrnam`         | `hcreate`     | `readdir`          |
| `dbm_error`     | `gethostent`       | `hdestroy`    | `setenv`           |
| `dbm_fetch`     | `getlogin`         | `hsearch`     | `setgrent`         |
| `dbm_firstkey`  | `getnetbyaddr`     | `inet_ntoa`   | `setkey`           |
| `dbm_nextkey`   | `getnetbyname`     | `l64a`        | `setpwent`         |
| `dbm_open`      | `getnetent`        | `lgamma`      | `setutxent`        |
| `dbm_store`     | `getopt`           | `lgammaf`     | `strerror`         |
| `dirname`       | `getprotobyname`   | `lgammal`     | `strsignal`        |
| `dlerror`       | `getprotobynumber` | `localeconv`  | `strtok`           |
| `drand48`       | `getprotoent`      | `localtime`   | `system`           |
| `encrypt`       | `getpwent`         | `lrand48`     | `ttyname`          |
| `endgrent`      | `getpwnam`         | `mrand48`     | `unsetenv`         |
| `endpwent`      | `getpwuid`         | `nftw`        | `wcstombs`         |
| `endutxent`     | `getservbyname`    | `nl_langinfo` | `wctomb`           |
| `getc_unlocked` | `getservbyport`    | `ptsname`     |                    |

**除了上面的列出的函数，其他函数都保证是线程安全的。** 支持线程安全函数的操作系统实现会在 `<unistd.h>` 中定义符号 `_POSIX_THREAD_SAFE_FUNCTIONS`。应用程序也可以在 `sysconf` 函数中传入 `_SC_THREAD_SAFE_FUNCTIONS` 参数在运行时检查是否支持线程安全函数。

操作系统实现支持线程安全函数这个特性时，对 `POSIX.1` 中的一些非线程安全函数，它会提供可替代的线程安全版本。下面列出了这些函数的线程安全版本。

| `getgrgid_r` | `localtime_r` |
| ------------ | ------------- |
| `getgrnam_r` | `readdir_r`   |
| `getlogin_r` | `strerror_r`  |
| `getpwnam_r` | `strtok_r`    |
| `getpwuid_r` | `ttyname_r`   |
| `gmtime_r`   |               |

如果一个函数对多个线程来说是可重入的，就说这个函数就是线程安全的。但这并不能说明对信号处理程序来说该函数也是可重入的。如果函数对异步信号处理程序的重入是安全的，那么就可以说函数是异步信号安全的。

`POSIX.1` 还提供了以线程安全的方式管理 `FILE` 对象的方法。可以使用 `flockfile` 和 `ftrylockfile` 获取给定 `FILE` 对象关联的锁。这个锁是递归的：当你占有这把锁的时候，还是可以再次获取该锁，而且不会导致死锁。虽然这种锁的具体实现并无规定，但要求所有操作 `FILE` 对象的标准 `I/O` 例程的动作行为必须看起来就像它们内部调用了 `flockfile` 和 `funlockfile`。

```cpp
#include <stdio.h>
int ftrylockfile(FILE *fp);
void flockfile(FILE *fp);
void funlockfile(FILE *fp);
```

`ftrylockfile` 返回值：

- 若成功，返回 0；
- 若不能获取锁，返回非 0 数值。

为了避免对每一个字符的读写操作进行获取锁和释放锁的开销，出现了不加锁版本的基于字符的标准 `I/O` 例程。

```cpp
#include <stdio.h>
int getchar_unlocked(void);
int getc_unlocked(FILE *fp);
```

两个函数的返回值:

- 若成功，返回下一个字符；
- 若遇到文件尾或者出错，返回 `EOF`。

```cpp
int putchar_unlocked(int c);
int putc_unlocked(int c, FILE *fp);
```

两个函数的返回值：

- 若成功，返回 c；
- 若出错，返回 EOF。

除非被 `flockfile`（或 `ftrylockfile`）和 `funlockfile` 的调用包围，否则尽量不要调用这 4 个函数，因为它们会导致不可预期的结果。

一旦对 `FILE` 对象进行加锁，就可以在释放锁之前对这些函数进行多次调用。这样就可以在多次的数据读写上分摊总的加解锁的开销。

例子，`getenv` 的一个可能实现，这个版本不是可重入的。

```cpp
#include <limits.h>
#include <string.h>

#define MAXSTRINGSZ 4096

static char envbuf[MAXSTRINGSZ];

extern char **environ;

char * getenv(const char *name){
	int i, len;
	len = strlen(name);
	for(i = 0; environ[i] != NULL; i++){
		if((strncmp(name, environ[i], len) == 0) &&
			(environ[i][len] == '=')){
			strncpy(envbuf, &environ[i][len+1], MAXSTRINGSZ-1);
			return envbuf;
		}
	}
	return NULL;
}
```

这个版本不是可重入的。如果两个线程同时调用这个函数，就会看到不一致的结果，因为所有调用 `getenv` 的线程返回的字符串都存储在同一个静态缓冲区中。

例子，`getenv` 的可重入（线程安全）版本。

```cpp
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

extern char **environ;

pthread_mutex_t env_mutex;

static pthread_once_t init_done = PTHREAD_ONCE_INIT;

static void thread_init(void){
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&env_mutex, &attr);
	pthread_mutexattr_destroy(&attr);
}

int getenv_r(const char *name, char *buf, int buflen){
	int i, len, olen;

	// 不管多少线程同时竞争调用g etenv_r，每个进程只调用thread_init函数一次
	pthread_once(&init_done, thread_init);
	len = strlen(name);
	pthread_mutex_lock(&env_mutex);
	for(i = 0; environ[i] != NULL; i++){
		if((strncmp(name, environ[i], len) == 0) &&
			(environ[i][len] == '=')){
			olen = strlen(&environ[i][len+1]);
			if(olen >= buflen){
				pthread_mutex_unlock(&env_mutex);
				return ENOSPC;
			}
			strcpy(buf, &environ[i][len+1]);
			pthread_mutex_unlock(&env_mutex);
			return 0;
		}
	}
	pthread_mutex_unlock(&env_mutex);
	return ENOENT;
}
```

要使 `getenv_r` 可重入，需要改变接口，调用者必须提供它自己的缓冲区，这样每个线程可以使用各自不同的缓冲区避免其他线程的干扰。还需要在搜索请求的字符时保护环境不被修改。

## 线程特定数据

线程特定数据（thread-specific data），也称为线程私有数据（thread-private data），是存储和查询某个特定线程相关数据的一种机制。我们把这种数据称为线程特定数据或线程私有数据的原因是，我们希望每个线程可以访问它自己单独的数据副本，而不需要担心与其他线程的同步访问问题。

采用线程私有数据的原因：

1. 有时候需要维护基于每线程（per-thread）的数据。
2. 它提供了让基于进程的接口适应多线程环境的机制。

`pthread_key_create` 用于创建线程特定数据键（Thread-Specific Data Key，简称 TSD 键）。TSD 键允许您为每个线程关联一个特定的数据，以便在线程之间共享数据而不会出现竞争条件。

```cpp
#include <pthread.h>
int pthread_key_create(pthread_key_t *keyp, void (*destructor)(void *));
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

参数：

- `keyp` 是一个指向 `pthread_key_t` 类型的指针，创建的键存储在 `keyp` 指向的内存单元中，这个键可以被进程中的所有线程使用，但每个线程把这个键与不同的线程特定数据地址进行关联。创建新键时，每个线程的数据地址设为空值。
- `destructor` 是一个函数指针，用于指定在线程退出时如何销毁 TSD 数据。这个函数会在线程退出时自动被调用，它的原型应该是 `void destructor(void *)`。您可以传递一个自定义的析构函数，或者使用 `NULL` 表示不需要析构函数。

当线程调用 `pthread_exit` 或者线程执行返回，正常退出时，析构函数就会被调用。同样，线程取消时，只有在最后的清理处理程序返回之后，析构函数才会被调用。如果线程调用了 `exit` 、 `_exit` 、 `_Exit` 或 `abort` ，或者出现其他非正常的退出时，就不会调用析构函数。

线程可以为线程特定数据分配多个键，每个键都可以有一个析构函数与它关联。每个键的析构函数可以互不相同，当然所有键也可以使用相同的析构函数。

线程退出时，线程特定数据的析构函数将按照操作系统实现中定义的顺序被调用。析构函数可能会调用另一个函数，该函数可能会创建新的线程特定数据，并且把这个数据与当前的键关联起来。当所有的析构函数都调用完成以后，系统会检查是否还有非空的线程特定数据值与键关联，如果有的话，再次调用析构函数。

对所有的线程，我们都可以通过调用 `pthread_key_delete` 来取消键与线程特定数据值之间的关联关系。

```cpp
#include <pthread.h>
int pthread_key_delete(pthread_key_t key);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

参数：

- `key` 是要删除的 TSD 键的标识符，它是在调用 `pthread_key_create` 函数时分配的。

**调用 `pthread_key_delete` 并不会激活与键关联的析构函数。**

有些线程可能看到一个键值，而其他的线程看到的可能是另一个不同的键值，这取决于系统是如何调度线程的，解决这种竞争的办法是使用 `pthread_once`。

`pthread_once` 用于确保在多线程环境下某个初始化函数只被执行一次。它通常用于在程序启动时执行全局初始化操作，以确保只有一个线程执行初始化，并且其他线程等待初始化完成。

```cpp
#include <pthread.h>
pthread_once_t initflag = PTHREAD_ONCE_INIT;
int pthread_once(pthread_once_t *initflag, void (*initfn)(void));
```

回值：

- 若成功，返回 0；
- 否则，返回错误编号。

`initflag` 必须是一个非本地变量（如全局变量或静态变量），而且必须初始化为 `PTHREAD_ONCE_INIT`。

键一旦创建以后，就可以通过调用 `pthread_setspecific` 函数把键和线程特定数据关联起来。可以通过 `pthread_getspecific` 函数获得线程特定数据的地址。

```cpp
#include <pthread.h>
void *pthread_getspecific(pthread_key_t key);
```

返回值：

- 线程特定数据值；
- 若没有值与该键关联，返回 `NULL`。

```cpp
int pthread_setspecific(pthread_key_t key, const void *value);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

如果没有线程特定数据值与键关联，`pthread_getspecific` 将返回一个空指针，我们可以用这个返回值来确定是否需要调用 `pthread_setspecific`。

例子，线程安全的 `getenv` 的兼容版本

```cpp
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define MAXSTRINGSZ	4096

static pthread_key_t key;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER;

extern char **environ;

static void thread_init(){
	pthread_key_create(&key, free);
}

char * getenv_r(const char *name){
	int i, len;
	char *envbuf;

	pthread_once(&init_done, thread_init);
	pthread_mutex_lock(&env_mutex);
	envbuf = (char *)pthread_getspecific(key);
	if(envbuf == NULL){
		envbuf = malloc(MAXSTRINGSZ);
		if(envbuf == NULL){
			pthread_mutex_unlock(&env_mutex);
			return NULL;
		}
		pthread_setspecific(key, envbuf);
	}
	len = strlen(name);
	for(i = 0; environ[i] != NULL; i++){
		if((strncmp(name, environ[i], len) == 0) &&
			(environ[i][len] == '=')){
			strncpy(envbuf, &environ[i][len+1], MAXSTRINGSZ-1);
			pthread_mutex_unlock(&env_mutex);
			return envbuf;
		}
	}
	pthread_mutex_unlock(&env_mutex);
	return NULL;
}
```

我们使用 `pthread_once` 来确保只为我们将使用的线程特定数据创建一个键。如果 `pthread_getspecific` 返回的是空指针，就需要先分配内存缓冲区，然后再把键与该内存缓冲区关联。否则就使用 `pthread_getspecific` 返回的内存缓冲区。对析构函数，使用 `free` 来释放之前由 `malloc` 分配的内存。只有当线程特定数据值为非空时，析构函数才会被调用。

## 取消选项

有两个线程属性并没有包含在 `pthread_attr_t` 结构中，它们是可取消状态和可取消类型。这两个属性影响着线程在响应 `pthread_cancel` 函数调用时所呈现的行为。

可取消状态属性可以是 `PTHREAD_CANCEL_ENABLE`，也可以是 `PTHREAD_CANCEL_DISABLE`。线程可以通过调用 `pthread_setcancelstate` 修改它的可取消状态。

```cpp
#include <pthread.h>
int pthread_setcancelstate(int state, int *oldstate);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

`pthread_setcancelstate` 把当前的可取消状态设置为 `state`，把原来的可取消状态存储在由 `oldstate` 指向的内存单元，这两步是一个原子操作。

`pthread_cancel` 调用并不等待线程终止。在默认情况下，线程在取消请求发出以后还是继续运行，直到线程到达某个取消点。取消点是线程检查它是否被取消的一个位置，如果取消了，则按照请求行事。

`POSIX.1` 保证在线程调用下列的任何函数时，取消点都会出现。

| `accept`          | `mg_timedsend`           | `pthread_join`       | `sendto`       |
| ----------------- | ------------------------ | -------------------- | -------------- |
| `aio_suspend`     | `msgrcv`                 | `pthread_testcancel` | `sigsuspend`   |
| `clock_nanosleep` | `msgsnd`                 | `pwrite`             | `sigtimedwait` |
| `close`           | `msync`                  | `read`               | `sigwait`      |
| `connect`         | `nanosleep`              | `readv`              | `sigwaitinfo`  |
| `creat`           | `open`                   | `recv`               | `sleep`        |
| `fcntl`           | `openat`                 | `recvfrom`           | `system`       |
| `fdatasync`       | `pause`                  | `recvmsg`            | `tcdrain`      |
| `fsync`           | `poll`                   | `select`             | `wait`         |
| `lockf`           | `pread`                  | `sem_timedwait`      | `waitid`       |
| `mq_receive`      | `pselect`                | `sem_wait`           | `waitpid`      |
| `mg_send`         | `pthread_cond_timedwait` | `send`               | `write`        |
| `mg_timedreceive` | `pthread_cond_wait`      | `sendmsg`            | `writev`       |

线程启动时默认的可取消状态是 `PTHREAD_CANCEL_ENABLE`。当状态设为 `PTHREAD_CANCEL_DISABLE` 时，对 `pthread_cancel` 的调用并不会杀死线程。相反，取消请求对这个线程来说还处于挂起状态，当取消状态再次变为 `PTHREAD_CANCEL_ENABLE` 时，线程将在下一个取消点上对所有挂起的取消请求进行处理。

`POSIX.1` 还指定了下列的函数作为可选的取消点。

| `access`      | `fseeko`           | `getwchar`                   | `putwc`       |
| ------------- | ------------------ | ---------------------------- | ------------- |
| `catclose`    | `fsetpos`          | `glob`                       | `putwchar`    |
| `catgets`     | `fstat`            | `iconv_close`                | `readdir`     |
| `catopen`     | `fstatat`          | `iconv_open`                 | `readdir_r`   |
| `chmod`       | `ftell`            | `ioctl`                      | `readlink`    |
| `chown`       | `ftello`           | `link`                       | `readlinkat`  |
| `closedir`    | `futimens`         | `linkat`                     | `remove`      |
| `closelog`    | `fwprintf`         | `lio_listio`                 | `rename`      |
| `ctermid`     | `fwrite`           | `localtime`                  | `renameat`    |
| `dbm_close`   | `fwscanf`          | `localtime_r`                | `rewind`      |
| `dbm_delete`  | `getaddrinfo`      | `lockf`                      | `rewinddir`   |
| `dbm_fetch`   | `getc`             | `lseek`                      | `scandir`     |
| `dbm_nextkey` | `getc_unlocked`    | `lstat`                      | `scanf`       |
| `dbm_open`    | `getchar`          | `mkdir`                      | `seekdir`     |
| `dbm_store`   | `getchar_unlocked` | `mkdirat`                    | `semop`       |
| `dl_close`    | `getcwd`           | `mkdtemp`                    | `setgrent`    |
| `dl_open`     | `getdate`          | `mkfifo`                     | `sethostent`  |
| `dprintf`     | `getdelim`         | `mkfifoat`                   | `setnetent`   |
| `endgrent`    | `getgrent`         | `mknod`                      | `setprotoent` |
| `endhostent`  | `getgrgid`         | `mknodat`                    | `setpwent`    |
| `endnetent`   | `getgrgid_r`       | `mkstemp`                    | `setservent`  |
| `endprotoent` | `getgrnam`         | `mktime`                     | `setutxent`   |
| `endpwent`    | `getgrnam_r`       | `nftw`                       | `stat`        |
| `endservent`  | `gethostent`       | `opendir`                    | `strerror`    |
| `endutxent`   | `gethostid`        | `openlog`                    | `strerror_r`  |
| `faccessat`   | `gethostname`      | `pathconf`                   | `strftime`    |
| `fchmod`      | `getline`          | `pclose`                     | `symlink`     |
| `fchmodat`    | `getlogin`         | `perror`                     | `symlinkat`   |
| `fchown`      | `getlogin_r`       | `popen`                      | `sync`        |
| `fchownat`    | `getnameinfo`      | `posix_fadvise`              | `syslog`      |
| `fclose`      | `getnetbyaddr`     | `posix_fallocate`            | `tmpfile`     |
| `fcntl`       | `getnetbyname`     | `posix_madvise`              | `ttyname`     |
| `fflush`      | `getnetent`        | `posix_openpt`               | `ttyname_r`   |
| `fgetc`       | `getopt`           | `posix_spawn`                | `tzset`       |
| `fgetpos`     | `getprotobyname`   | `posix_spawnp`               | `ungetc`      |
| `fgets`       | `getprotobynumber` | `posix_typed_mem_open`       | `ungetwc`     |
| `fgetwc`      | `getprotoent`      | `printf`                     | `unlink`      |
| `fgetws`      | `getpwent`         | `psiginfo`                   | `unlinkat`    |
| `fmtmsg`      | `getpwnam`         | `psignal`                    | `utimensat`   |
| `fopen`       | `getpwnam_r`       | `pthread_rwlock_rdlock`      | `utimes`      |
| `fpathconf`   | `getpwuid`         | `pthread_rwlock_timedrdlock` | `vdprintf`    |
| `fprintf`     | `getpwuid_r`       | `pthread_rwlock_timedwrlock` | `vfprintf`    |
| `fputc`       | `getservbyname`    | `pthread_rwlock_wrlock`      | `vfwprintf`   |
| `fputs`       | `getservbyport`    | `putc`                       | `vprintf`     |
| `fputwc`      | `getservent`       | `putc_unlocked`              | `wwprintf`    |
| `fputws`      | `getutxent`        | `putchar`                    | `wcsftime`    |
| `fread`       | `getutxid`         | `putchar_unlocked`           | `wordexp`     |
| `freopen`     | `getutxline`       | `puts`                       | `wprintf`     |
| `fscanf`      | `getwc`            | `pututxline`                 | `wscanf`      |
| `fseek`       |                    |                              |               |

如果应用程序在很长的一段时间内都不会调用上面两个表中的函数，那么你可以调用 `pthread_testcancel` 函数在程序中添加自己的取消点。

```cpp
#include <pthread.h>
void pthread_testcancel(void);
```

调用 `pthread_testcancel` `时，如果有某个取消请求正处于挂起状态，而且取消并没有置为无效，那么线程就会被取消。但是，如果取消被置为无效，pthread_testcancel` 调用就没有任何效果了。

我们所描述的默认的取消类型也称为推迟取消。调用 `pthread_cancel` 以后，在线程到达取消点之前，并不会出现真正的取消。可以通过调用 `pthread_setcanceltype` 来修改取消类型。

```cpp
#include <pthread.h>
int pthread_setcanceltype(int type, int *oldtype);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

`pthread_setcanceltype` 函数把取消类型设置为 `type`（类型参数可以是 `PTHREADCANCEL_DEFERRED`，也可以是 `PTHREAD_CANCEL_ASYNCHRONOUS`），把原来的取消类型返回到 `oldtype` 指向的整型单元。

异步取消与推迟取消不同，因为使用异步取消时，线程可以在任意时间撤消，不是非得遇到取消点才能被取消。

## 线程和信号

每个线程都有自己的信号屏蔽字，但是信号的处理是进程中所有线程共享的。这意味着单个线程可以阻止某些信号，但当某个线程修改了与某个给定信号相关的处理行为以后，所有的线程都必须共享这个处理行为的改变。这样，如果一个线程选择忽略某个给定信号，那么另一个线程就可以通过以下两种方式撤消上述线程的信号选择：恢复信号的默认处理行为，或者为信号设置一个新的信号处理程序。

进程中的信号是递送到单个线程的。如果一个信号与硬件故障相关，那么该信号一般会被发送到引起该事件的线程中去，而其他的信号则被发送到任意一个线程。

`sigprocmask` 的行为在多线程的进程中并没有定义，线程必须使用 `pthread_sigmask`。

```cpp
#include <signal.h>
int pthread_sigmask(int how, const sigset_t *restrict set, sigset_t *restrict oset);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

参数：

- `set` 参数包含线程用于修改信号屏蔽字的信号集。
- `how` 参数可以取下列 3 个值之一：
  - `SIG_BLOCK`，把信号集添加到线程信号屏蔽字中，
  - `SIG_SETMASK`，用信号集替换线程的信号屏蔽字；
  - `SIG_UNBLOCK`，从线程信号屏蔽字中移除信号集。
- `oset` 参数为线程之前的信号屏蔽字就存储在它指向的 `sigset_t` 结构中。线程可以通过把 `set` 参数设置为 `NULL`，并把 `oset` 参数设置为 `sigset_t` 结构的地址，来获取当前的信号屏蔽字。这种情况中的 `how` 参数会被忽略。

`pthread_sigmask` 函数与 `sigprocmask` 函数基本相同，不过 `pthread_sigmask` 工作在线程中，而且失败时返回错误码，不再像 `sigprocmask` 中那样设置 `errno` 并返回 −1。

线程可以通过调用 `sigwait` 等待一个或多个信号的出现。

```cpp
#include <signal.h>
int sigwait(const sigset_t *restrict set, int *restrict signop);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

`set` 参数指定了线程等待的信号集。返回时，`signop` 指向的整数将包含发送信号的数量。

如果信号集中的某个信号在 `sigwait` 调用的时候处于挂起状态，那么 `sigwait` 将无阻塞地返回。在返回之前，`sigwait` 将从进程中移除那些处于挂起等待状态的信号。如果具体实现支持排队信号，并且信号的多个实例被挂起，那么 `sigwait` 将会移除该信号的一个实例，其他的实例还要继续排队。

要把信号发送给进程，可以调用 `kill`。要把信号发送给线程，可以调用 `pthread_kill`。

```cpp
#include <signal.h>
int pthread_kill(pthread_t thread, int signo);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

可以传一个 0 值的 `signo` 来检查线程是否存在。如果信号的默认处理动作是终止该进程，那么把信号传递给某个线程仍然会杀死整个进程。

例子，同步信号处理。

```cpp
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

int quitflag;
sigset_t mask;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waitloc = PTHREAD_COND_INITIALIZER;

void *thr_fn(void *arg){
	int err, signo;
	for(;;){
		err = sigwait(&mask, &signo);
		if(err != 0){
			perror("sigwait failed");
			exit(1);
		}
		switch(signo){
		case SIGINT:
			printf("\ninterrupt\n");
			break;
		case SIGQUIT:
			pthread_mutex_lock(&lock);
			quitflag = 1;
			pthread_mutex_unlock(&lock);
			pthread_cond_signal(&waitloc);
			return 0;
		default:
			printf("unexpected signal %d\n", signo);
			exit(1);
		}
	}
}

int main(){
	int err;
	sigset_t oldmask;
	pthread_t tid;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	if((err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask)) != 0){
		perror("SIG_BLOCK error");
		return 1;
	}

	err = pthread_create(&tid, NULL, thr_fn, 0);
	if(err != 0){
		perror("can't create thread");
		return 1;
	}

	pthread_mutex_lock(&lock);
	while(quitflag == 0)
		pthread_cond_wait(&waitloc, &lock);
	pthread_mutex_unlock(&lock);

	quitflag = 0;

	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0){
		perror("SIG_SETMASK error");
		return 1;
	}
	return 0;
}
```

编译运行：

```bash
$ gcc 04pthread_sig.c -lpthread
$ ./a.out
^C            # Ctrl + C
interrupt
^\%           # Ctrl + \
```

我们不用依赖信号处理程序中断主控线程，有专门的独立控制线程进行信号处理。在互斥量的保护下改动 `quitflag` 的值，这样主控线程不会在调用 `pthread_cond_signal` 时错失唤醒调用。在主控线程中使用相同的互斥量来检查标志的值，并且原子地释放互斥量，等待条件的发生。

## 线程和 fork

当线程调用 `fork` 时，就为子进程创建了整个进程地址空间的副本。子进程通过继承整个地址空间的副本，还从父进程那儿继承了每个互斥量、读写锁和条件变量的状态。如果父进程包含一个以上的线程，子进程在 `fork` 返回以后，如果紧接着不是马上调用 `exec` 的话，就需要清理锁状态。

在子进程内部，只存在一个线程，它是由父进程中调用 `fork` 的线程的副本构成的。如果父进程中的线程占有锁，子进程将同样占有这些锁。

如果子进程从 `fork` 返回以后马上调用其中一个 `exec` 函数，就可以避免这样的问题。这种情况下，旧的地址空间就被丢弃，所以锁的状态无关紧要。

在多线程的进程中，为了避免不一致状态的问题，`POSIX.1` 声明，在 `fork` 返回和子进程调用其中一个 `exec` 函数之间，子进程只能调用异步信号安全的函数。

要清除锁状态，可以通过调用 `pthread_atfork` 函数建立 `fork` 处理程序。

```cpp
#include <pthread.h>
int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

参数：

- `prepare` 函数是一个回调函数，它在 `fork` 操作之前（即在父进程中）执行。这个函数通常用于准备线程和资源，以确保它们在子进程中正确复制。
- `parent` 函数是一个回调函数，它在 `fork` 操作成功后的父进程中执行。这个函数通常用于父进程在 `fork` 后继续执行前恢复任何线程或资源。
- `child` 函数是一个回调函数，它在 `fork` 操作成功后的子进程中执行。这个函数通常用于在子进程中清理任何不需要的线程或资源。

可以多次调用 `pthread_atfork` 函数从而设置多套处理程序。如果不需要使用其中某个处理程序，可以给特定的处理程序参数传入空指针，它就不会起任何作用了。使用多个处理程序时，处理程序的调用顺序并不相同。`parent` 和 `child` 处理程序是以它们注册时的顺序进行调用的，而 `prepare` 处理程序的调用顺序与它们注册时的顺序相反。这样可以允许多个模块注册它们自己的处理程序，而且可以保持锁的层次。

例子，`pthread_atfork` 实例。

```cpp
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void prepare(){
	int err;
	printf("preparing locks...\n");
	if((err = pthread_mutex_lock(&lock1)) != 0)
		perror("can't lock lock1 in prepare handler");
	if((err = pthread_mutex_lock(&lock2)) != 0)
		perror("can't lock lock2 in prepare handler");
}

void parent(){
	int err;
	printf("parent unlocking locks...\n");
	if((err = pthread_mutex_unlock(&lock1)) != 0)
		perror("can't unlock lock1 in parent handler");
	if((err = pthread_mutex_unlock(&lock2)) != 0)
		perror("can't unlock lock2 in parent handler");
}

void child(){
	int err;
	printf("child unlocking locks...\n");
	if((err = pthread_mutex_unlock(&lock1)) != 0)
		perror("can't unlock lock1 in child handler");
	if((err = pthread_mutex_unlock(&lock2)) != 0)
		perror("can't unlock lock2 in child handler");
}

void *thr_fn(void *arg){
	printf("thread started...\n");
	pause();
	return 0;
}

int main(){
	int err;
	pid_t pid;
	pthread_t tid;

	if((err = pthread_atfork(prepare, parent,child)) != 0){
		perror("can't install fork handlers");
		exit(1);
	}
	if((err = pthread_create(&tid, NULL, thr_fn, 0)) != 0){
		perror("can't create thread");
		exit(1);
	}

	sleep(2);
	printf("parent about to fork...\n");

	if((pid = fork()) < 0){
		perror("fork failed");
		exit(1);
	}else if(pid == 0)
		printf("child returned from fork.\n");
	else
		printf("parent returned from fork.\n");
	return 0;
}
```

编译运行：

```bash
$ gcc 05pthread_atfork.c -lpthread
$ ./a.out
thread started...
parent about to fork...
preparing locks...
parent unlocking locks...
parent returned from fork.
child unlocking locks...
child returned from fork.
```

定义了两个互斥量，`lock1` 和 `lock2`，`prepare` 处理程序获取这两把锁，`child` 处理程序在子进程上下文中释放它们，`parent` 处理程序在父进程上下文中释放它们。

虽然 `pthread_atfork` 机制的意图是使 `fork` 之后的锁状态保持一致，但它还是存在一些不足之处，只能在有限情况下可用。

- 没有很好的办法对较复杂的同步对象（如条件变量或者屏障）进行状态的重新初始化。
- 某些错误检查的互斥量实现在 `child` 处理程序试图对被父进程加锁的互斥量进行解锁时会产生错误。
- 递归互斥量不能在 `child` 处理程序中清理，因为没有办法确定该互斥量被加锁的次数。
- 如果子进程只允许调用异步信号安全的函数，`child` 处理程序就不可能清理同步对象，因为用于操作清理的所有函数都不是异步信号安全的。实际的问题是同步对象在某个线程调用 `fork` 时可能处于中间状态，除非同步对象处于一致状态，否则无法被清理。
- 如果应用程序在信号处理程序中调用了 `fork`（这是合法的，因为 `fork` 本身是异步信号安全的），`pthread_atfork` 注册的处理程序只能调用异步信号安全的函数，否则结果将是未定义的。

## 线程和 I/O

进程中的所有线程共享相同的文件描述符。`pread` 和 `pwrite` 函数的读写是原子操作，在多线程环境下是非常有用的，可以使用它们来解决并发线程对同一文件进行写操作的问题。
