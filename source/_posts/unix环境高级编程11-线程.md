---
title: unix环境高级编程11-线程
date: 2023-04-10 17:22:18
tags: linux 编程
category: unix环境高级编程
---

## 线程概念

每个线程都包含有表示执行环境所必需的信息，其中包括进程中标识线程的线程 ID、一组寄存器值、栈、调度优先级和策略、信号屏蔽字、`errno` 变量以及线程私有数据。**一个进程的所有信息对该进程的所有线程都是共享的，包括可执行程序的代码、程序的全局内存和堆内存、栈以及文件描述符。**

## 线程标识

就像每个进程有一个进程 ID 一样，每个线程也有一个线程 ID。线程 ID 是用 `pthread_t` 数据类型来表示的，实现的时候可以用一个结构来代表 `pthread_t` 数据类型，所以可移植的操作系统实现不能把它作为整数处理。因此必须使用一个函数 `pthread_equal` 来对两个线程 ID 进行比较。

```c
#include <pthread.h>
int pthread_equal(pthread_t tid1, pthread_t tid2);
```

返回值：

- 若相等，返回非 0 数值；
- 否则，返回 0。

Linux 3.2.0 使用无符号长整型表示 `pthread_t` 数据类型。

线程可以通过调用 `pthread_self` 函数获得自身的线程 ID。

```c
#include <pthread.h>
pthread_t pthread_self(void);
```

返回值：

- 调用线程的线程 ID。

## 线程创建

在传统 UNIX 进程模型中，每个进程只有一个控制线程。从概念上讲，这与基于线程的模型中每个进程只包含一个线程是相同的。在 `POSIX` 线程（`pthread`）的情况下，程序开始运行时，它也是以单进程中的单个控制线程启动的。在创建多个控制线程以前，程序的行为与传统的进程并没有什么区别。新增的线程可以通过调用 `pthread_create` 函数创建。

```c
#include <pthread.h>
int pthread_create(pthread_t *restrict tidp, const pthread_attr_t *restrict attr,
                    void *(*start_rtn)(void *), void *restrict arg);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

参数：

- `tidp`：指向 `pthread_t` 类型的指针，用于存储新线程的标识符。
- `attr`：指向 `pthread_attr_t` 类型的指针，用于指定新线程的属性，通常可以设置为 `NULL` 以使用默认属性。
- `start_rtn`：是一个指向线程主函数的指针，该函数在新线程中运行。它接受一个 `void*` 类型的参数，并返回一个 `void*` 类型的值。
- `arg`：是传递给 `start_rtn` 函数的参数。

当 `pthread_create` 成功返回时，新创建线程的线程 `ID` 会被设置成 `tidp` 指向的内存单元。`attr` 参数用于定制各种不同的线程属性，置为 `NULL`，创建一个具有默认属性的线程。新创建的线程从 `start_rtn` 函数的地址开始运行，该函数只有一个无类型指针参数 `arg`。

新创建的线程可以访问进程的地址空间，并且继承调用线程的浮点环境和信号屏蔽字，但是该线程的挂起信号集会被清除。

例子，打印线程 ID。

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_t ntid;

void printids(const char *s){
	pid_t pid;
	pthread_t tid;
	pid = getpid();
	tid = pthread_self();
	printf("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long)pid,
			(unsigned long)tid, (unsigned long)tid);
}

void *thr_fn(void* arg){
	printids("new thread: ");
	return (void*)0;
}

int main(){
	int err;
	err = pthread_create(&ntid, NULL, thr_fn, NULL);
	if(err != 0){
		perror("pthread_create");
		return 1;
	}
	printids("main thread: ");
	sleep(1);
	return 0;
}
```

编译运行：

```bash
$ gcc 01pthread_create.c -lpthread
$ ./a.out
main thread:  pid 6391 tid 140047663408960 (0x7f5f633aa740)
new thread:  pid 6391 tid 140047663404800 (0x7f5f633a9700)
```

## 线程终止

如果进程中的任意线程调用了 `exit`、`_Exit` 或者 `_exit`，那么整个进程就会终止。与此相类似，如果默认的动作是终止进程，那么，发送到线程的信号就会终止整个进程

单个线程可以通过 3 种方式退出，因此可以在不终止整个进程的情况下，停止它的控制流。

1. 线程可以简单地从启动例程中返回，返回值是线程的退出码。
1. 线程可以被同一进程中的其他线程取消。
1. 线程调用 `pthread_exit`。

`pthread_exit` 是 `POSIX` 线程库中的一个函数，它用于终止当前线程的执行，并可以传递一个退出状态给等待它的其他线程或进程。

```c
#include <pthread.h>
void pthread_exit(void *rval_ptr);
```

参数：
`rval_ptr` 参数是一个无类型指针，与传给启动例程的单个参数类似。进程中的其他线程也可以通过调用 `pthread_join` 函数访问到这个指针。

`pthread_join` 是 `POSIX` 线程库中的一个函数，它用于等待指定线程的终止，并且可以获取该线程的返回值。

```c
#include <pthread.h>
int pthread_join(pthread_t thread, void **rval_ptr);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号

参数：

- `thread`：要等待的线程的标识符，通常由 `pthread_create` 返回。
- `rval_ptr`：一个指向指针的指针，用于存储目标线程的返回值。

调用线程在调用 `pthread_join` 函数后将一直阻塞，直到指定的线程调用 `pthread_exit`、从启动例程中返回或者被取消。如果线程简单地从它的启动例程返回，`rval_ptr` 就包含返回码。如果线程被取消，由 `rval_ptr` 指定的内存单元就设置为 `PTHREAD_CANCELED`。

默认情况下，通过 `pthread_create` 创建的线程是非分离状态的。这意味着在线程终止后，它的资源（如线程描述符和内存）不会被自动释放，直到另一个线程通过调用 `pthread_join` 来等待并获取该线程的终止状态和返回值。这种情况下，线程的资源会一直被保留，直到其他线程明确地要求回收它们。

通过调用 `pthread_join` 并将 `rval_ptr` 参数设置为 `NULL`，或者使用 `pthread_detach` 函数，可以将线程置于分离状态。在分离状态下，线程的资源会在它终止时自动释放，而无需其他线程显式等待和获取线程的状态和返回值。如果对线程的返回值并不感兴趣，那么可以把 `rval_ptr` 设置为 `NULL`。

例子，获得线程退出状态。

```c
#include <stdio.h>
#include <pthread.h>

void *thr_fn1(void *arg){
	printf("thread 1 returning.\n");
	return (void*)1;
}

void *thr_fn2(void *arg){
	printf("thread 2 returning.\n");
	return (void*)2;
}

int main(){
	int err;
	pthread_t tid1, tid2;
	void *tret;

	err = pthread_create(&tid1, NULL, thr_fn1, NULL);
	if (err != 0){
		perror("can't create thread 1");
		return 1;
	}
	err = pthread_create(&tid2, NULL, thr_fn2, NULL);
	if (err != 0){
		perror("can't create thread 2");
		return 1;
	}
	// 这里将阻塞，等待线程退出
	err = pthread_join(tid1, &tret);
	if(err != 0){
		perror("can't join thread 1");
		return 1;
	}
	printf("thread 1 exit code %ld\n", (long)tret);

	err = pthread_join(tid2, &tret);
	if(err != 0){
		perror("can't join thread 2");
		return 1;
	}
	printf("thread 2 exit code %ld\n", (long)tret);
	return 0;
}
```

编译运行：

```bash
$ gcc 02pthread_join.c -lpthread
$ ./a.out
thread 1 returning.
thread 1 exit code 1
thread 2 returning.
thread 2 exit code 2
```

当一个线程通过调用 `pthread_exit` 退出或者简单地从启动例程中返回时，进程中的其他线程可以通过调用 `pthread_join` 函数获得该线程的退出状态。

`pthread_create` 和 `pthread_exit` 函数的无类型指针参数可以传递的值不止一个，这个指针可以传递包含复杂信息的结构的地址，但是注意，这个结构所使用的内存在调用者完成调用以后必须仍然是有效的。

例子，`pthread_exit` 参数的不正确使用。

```c
#include <stdio.h>
#include <pthread.h>

struct foo{
	int a, b, c, d;
};

void printfoo(const char *s, const struct foo *fp){
	printf("%s", s);
	printf("  structure at 0x%lx\n", (unsigned long)fp);
	printf("  foo.a = %d\n", fp->a);
	printf("  foo.b = %d\n", fp->b);
	printf("  foo.c = %d\n", fp->c);
	printf("  foo.d = %d\n", fp->d);
}

void *thr_fn(void *arg){
	struct foo fo = {1, 2, 3, 4};
	printfoo("in thread 1:\n", &fo);
	pthread_exit((void*)&fo);
}

int main(){
	int err;
	pthread_t tid;
	struct foo *fp;
	err = pthread_create(&tid, NULL, thr_fn, NULL);
	if(err != 0){
		perror("can't create thread");
		return 1;
	}
	err = pthread_join(tid, (void *)&fp);
	if(err != 0){
		perror(" can't join thread");
		return 1;
	}
	printfoo("in parent:\n", fp);
	return 0;
}
```

编译运行：

```bash
$ gcc 03pthread_exit.c -lpthread
$ ./a.out
in thread 1:
  structure at 0x7f18f8c79ed0
  foo.a = 1
  foo.b = 2
  foo.c = 3
  foo.d = 4
in parent:
  structure at 0x7f18f8c79ed0
  foo.a = 0
  foo.b = 0
  foo.c = -1743576962
  foo.d = 32767
```

运行结果根据内存体系结构、编译器以及线程库的实现会有所不同。可以看到，当主线程访问这个结构时，结构的内容已经改变了。

线程可以通过调用 `pthread_cancel` 函数来请求取消同一进程中的其他线程。

```c
#include <pthread.h>
int pthread_cancel(pthread_t tid);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

在默认情况下，`pthread_cancel` 函数会使得由 `tid` 标识的线程的行为表现为如同调用了参数为 `PTHREAD_CANCELED` 的 `pthread_exit` 函数，但是，线程可以选择忽略取消或者控制如何被取消。

**注意 `pthread_cancel` 并不等待线程终止，它仅仅提出请求。**

线程可以安排它退出时需要调用的函数，这与进程在退出时可以用 `atexit` 函数安排退出是类似的。这样的函数称为线程清理处理程序（thread cleanup handler）。一个线程可以建立多个清理处理程序。处理程序记录在栈中，也就是说，它们的执行顺序与它们注册时相反。

```c
#include <pthread.h>
void pthread_cleanup_push(void (*rtn)(void *), void *arg);
void pthread_cleanup_pop(int execute);
```

当线程执行以下动作时，清理函数 `rtn` 是由 `pthread_cleanup_push` 函数调度的，调用时只有一个参数 `arg`：

- 调用 `pthread_exit` 时；
- 响应取消请求时；
- 用非零 `execute` 参数调用 `pthread_cleanup_pop` 时。

如果 `execute` 参数设置为 0，清理函数将不被调用。不管发生上述哪种情况，`pthread_cleanup_pop` 都将删除上次 `pthread_cleanup_push` 调用建立的清理处理程序。
这些函数有一个限制，由于它们可以实现为宏，所以必须在与线程相同的作用域中以匹配对的形式使用。

例子，线程清理处理程序。

```c
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void cleanup(void *arg){
	printf("cleanup: %s\n", (char *)arg);
}

void *thr_fn1(void *arg){
	printf("thread 1 start.\n");
	pthread_cleanup_push(cleanup, "thread 1 first handler.");
	pthread_cleanup_push(cleanup, "thread 1 second handler.");
	printf("thread 1 push complete.\n");
	if (arg)
		return (void *)1;
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
	return ((void *)1);
}

void *thr_fn2(void *arg){
	printf("thread 2 start.\n");
	pthread_cleanup_push(cleanup, "thread 2 first handler.");
	pthread_cleanup_push(cleanup, "thread 2 second handler.");
	printf("thread 2 push complete.\n");
	if (arg)
		pthread_exit((void *)2);
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
	pthread_exit((void *)2);
}

int main(void){
	int err;
	pthread_t tid1, tid2;
	void *tret;

	err = pthread_create(&tid1, NULL, thr_fn1, (void *)1);
	if (err != 0){
		perror("can't create thread 1");
		return 1;
	}
	err = pthread_create(&tid2, NULL, thr_fn2, (void *)1);
	if (err != 0){
		perror("can't create thread 2");
		return 1;
	}
	err = pthread_join(tid1, &tret);
	if (err != 0){
		perror("can't join with thread 1");
		return 1;
	}
	printf("thread 1 exit code %ld.\n", (long)tret);
	err = pthread_join(tid2, &tret);
	if (err != 0){
		perror("can't join with thread 2");
		return 1;
	}
	printf("thread 2 exit code %ld.\n", (long)tret);
	exit(0);
}
```

编译运行：

```bash
$ gcc 04pthread_cleanup.c -lpthread
$ ./a.out
thread 1 start.
thread 1 push complete.
thread 2 start.
thread 2 push complete.
thread 1 exit code 1.
cleanup: thread 2 second handler.
cleanup: thread 2 first handler.
thread 2 exit code 2.
```

从输出结果可以看出，两个线程都正确地启动和退出了，但是只有第二个线程的清理处理程序被调用了。因此，如果线程是通过从它的启动例程中返回而终止的话，它的清理处理程序就不会被调用。

进程和线程原语的比较。

| 进程原语  | 线程原语              | 描述                         |
| --------- | --------------------- | ---------------------------- |
| `fork`    | `pthread_create`      | 创建新的控制流               |
| `exit`    | `pthread_exit`        | 从现有的控制流中退出         |
| `waitpid` | `pthread_join`        | 从控制流中得到退出状态       |
| `atexit`  | `pthread_cancel_push` | 注册在退出控制流时调用的函数 |
| `getpid`  | `pthread_self`        | 获取控制流的 ID              |
| `abort`   | `pthread_cancel`      | 请求控制流的非正常退出       |

在默认情况下，线程的终止状态会保存直到对该线程调用 `pthread_join`。如果线程已经被分离，线程的底层存储资源可以在线程终止时立即被收回。

可以调用 `pthread_detach` 分离线程。

```c
#include <pthread.h>
int pthread_detach(pthread_t tid);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

## 线程同步

当多个控制线程共享相同的内存时，需要确保每个线程看到一致的数据视图。如果每个线程使用的变量都是其他线程不会读取和修改的，那么就不存在一致性问题。同样，如果变量是只读的，多个线程同时读取该变量也不会有一致性问题。但是，当一个线程可以修改的变量，其他线程也可以读取或者修改的时候，我们就需要对这些线程进行同步，确保它们在访问变量的存储内容时不会访问到无效的值。

两个或多个线程试图在同一时间修改同一变量时，也需要进行同步。

### 互斥量

可以使用 `pthread` 的互斥接口来保护数据，确保同一时间只有一个线程访问数据。互斥量（mutex）从本质上说是一把锁，在访问共享资源前对互斥量进行设置（加锁），在访问完成后释放（解锁）互斥量。对互斥量进行加锁以后，任何其他试图再次对互斥量加锁的线程都会被阻塞直到当前线程释放该互斥锁。如果释放互斥量时有一个以上的线程阻塞，那么所有该锁上的阻塞线程都会变成可运行状态，第一个变为运行的线程就可以对互斥量加锁，其他线程就会看到互斥量依然是锁着的，只能回去再次等待它重新变为可用。在这种方式下，每次只有一个线程可以向前执行。

互斥变量是用 `pthread_mutex_t` 数据类型表示的。在使用互斥变量以前，必须首先对它进行初始化，可以把它设置为常量 `PTHREAD_MUTEX_INITIALIZER`（只适用于静态分配的互斥量），也可以通过调用 `pthread_mutex_init` 函数进行初始化。如果动态分配互斥量，在释放内存前需要调用 `pthread_mutex_destroy`。

```c
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t *restrict mutex,
                        const pthread_mutexattr_t *restrict attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

要用默认的属性初始化互斥量，只需把 `attr` 设为 `NULL`。

对互斥量进行加锁，需要调用 `pthread_mutex_lock`。如果互斥量已经上锁，调用线程将阻塞直到互斥量被解锁。对互斥量解锁，需要调用 `pthread_mutex_unlock`。

```c
#include <pthread.h>
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

所有函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

如果线程不希望被阻塞，它可以使用 `pthread_mutex_trylock` 尝试对互斥量进行加锁。如果调用 `pthread_mutex_trylock` 时互斥量处于未锁住状态，那么 `pthread_mutex_trylock` 将锁住互斥量，不会出现阻塞直接返回 0，否则 `pthread_mutex_trylock` 就会失败，不能锁住互斥量，返回 `EBUSY`。

例子，使用互斥量保护数据结构。

```c
#include <stdlib.h>
#include <pthread.h>

struct foo{
    int f_count; // 使用引用计数
    pthread_mutex_t f_lock;
    int f_id;
    /* ... more stuff here ... */
};

struct foo *foo_alloc(int id) {
    struct foo *fp;

    if ((fp = malloc(sizeof(struct foo))) != NULL){
        fp->f_count = 1; // 引用计数初始化为 1
        fp->f_id = id;
        if (pthread_mutex_init(&fp->f_lock, NULL) != 0)
        {
            free(fp);
            return (NULL);
        }
        /* ... continue initialization ... */
    }
    return (fp);
}

// 使用该对象时，线程需要调用 foo_hold 对这个对象的引用计数加 1。
void foo_hold(struct foo *fp) {
    pthread_mutex_lock(&fp->f_lock);
    fp->f_count++;
    pthread_mutex_unlock(&fp->f_lock);
}
// 当对象使用完毕时，必须调用 foo_rele 释放引用。最后一个引用被释放时，
// 对象所占的内存空间就被释放。
void foo_rele(struct foo *fp) {
    pthread_mutex_lock(&fp->f_lock);
    if (--fp->f_count == 0)
    { /* last reference */
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    }
    else
    {
        pthread_mutex_unlock(&fp->f_lock);
    }
}
```

当一个以上的线程需要访问动态分配的对象时，我们可以在对象中嵌入引用计数，确保在所有使用该对象的线程完成数据访问之前，该对象内存空间不会被释放。

在 `foo_alloc` 函数中将引用计数初始化为 1 时没必要加锁，因为在这个操作之前分配线程是唯一引用该对象的线程。

在使用该对象前，线程需要调用 `foo_hold` 对这个对象的引用计数加 1。当对象使用完毕时，必须调用 `foo_rele` 释放引用。最后一个引用被释放时，对象所占的内存空间就被释放。

### 避免死锁

如果线程试图对同一个互斥量加锁两次，那么它自身就会陷入死锁状态，但是使用互斥量时，还有其他不太明显的方式也能产生死锁。例如，程序中使用一个以上的互斥量时，如果允许一个线程一直占有第一个互斥量，并且在试图锁住第二个互斥量时处于阻塞状态，但是拥有第二个互斥量的线程也在试图锁住第一个互斥量。因为两个线程都在相互请求另一个线程拥有的资源，所以这两个线程都无法向前运行，于是就产生死锁。

可以通过仔细控制互斥量加锁的顺序来避免死锁的发生。例如，假设需要对两个互斥量 A 和 B 同时加锁。如果所有线程总是在对互斥量 B 加锁之前锁住互斥量 A，那么使用这两个互斥量就不会产生死锁（当然在其他的资源上仍可能出现死锁）。类似地，如果所有的线程总是在锁住互斥量 A 之前锁住互斥量 B，那么也不会发生死锁。可能出现的死锁只会发生在一个线程试图锁住另一个线程以相反的顺序锁住的互斥量。

例子，两个互斥量的使用方法。

```c
#include <stdlib.h>
#include <pthread.h>

#define NHASH 29
#define HASH(id) (((unsigned long)id) % NHASH)

struct foo *fh[NHASH];

pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo{
    int f_count;
    pthread_mutex_t f_lock;
    int f_id;
    struct foo *f_next; /* protected by hashlock */
                        /* ... more stuff here ... */
};

struct foo *foo_alloc(int id){ /* allocate the object */
    struct foo *fp;
    int idx;

    if ((fp = malloc(sizeof(struct foo))) != NULL){
        fp->f_count = 1;
        fp->f_id = id;
        if (pthread_mutex_init(&fp->f_lock, NULL) != 0)
        {
            free(fp);
            return (NULL);
        }
        idx = HASH(id);
        // 分配函数现在锁住了散列列表锁，把新的结构添加到了散列桶中，
        // 而且在对散列列表的锁解锁之前，先锁定了新结构中的互斥量。
        pthread_mutex_lock(&hashlock);
        fp->f_next = fh[idx];
        fh[idx] = fp;
        pthread_mutex_lock(&fp->f_lock);
        pthread_mutex_unlock(&hashlock);
        /* ... continue initialization ... */
        pthread_mutex_unlock(&fp->f_lock);
    }
    return (fp);
}

void foo_hold(struct foo *fp) /* add a reference to the object */
{
    pthread_mutex_lock(&fp->f_lock);
    fp->f_count++;
    pthread_mutex_unlock(&fp->f_lock);
}

struct foo *foo_find(int id) /* find an existing object */
{
    struct foo *fp;
    // 锁住散列列表锁，然后搜索被请求的结构
    pthread_mutex_lock(&hashlock);
    for (fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next){
        if (fp->f_id == id){
            // 如果找到了，就增加其引用计数并返回指向该结构的指针。
            foo_hold(fp);
            break;
        }
    }
    pthread_mutex_unlock(&hashlock);
    return (fp);
}

void foo_rele(struct foo *fp){ /* release a reference to the object */
    struct foo *tfp;
    int idx;

    pthread_mutex_lock(&fp->f_lock);
    if (fp->f_count == 1){
        // 如果这是最后一个引用，就需要对这个结构互斥量进行解锁，
        // 因为我们需要从散列列表中删除这个结构，这样才可以获取散列列表锁，然后重新获取结构互斥量。
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_lock(&hashlock);
        pthread_mutex_lock(&fp->f_lock);
        // 需要重新检查条件，判断是否还需要释放这个结构。
        if (fp->f_count != 1){
            // 另一个线程在我们为满足锁顺序而阻塞时发现了这个结构并对其引用计数加 1 的情况。
            fp->f_count--;
            pthread_mutex_unlock(&fp->f_lock);
            pthread_mutex_unlock(&hashlock);
            return;
        }
        /* remove from list */
        idx = HASH(fp->f_id);
        tfp = fh[idx];
        if (tfp == fp)
        {
            fh[idx] = fp->f_next;
        }
        else
        {
            while (tfp->f_next != fp)
                tfp = tfp->f_next;
            tfp->f_next = fp->f_next;
        }
        pthread_mutex_unlock(&hashlock);
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    }
    else
    {
        fp->f_count--;
        pthread_mutex_unlock(&fp->f_lock);
    }
}
```

我们也可以使用散列列表锁来保护结构引用计数，使事情大大简化。结构互斥量可以用于保护 `foo` 结构中的其他任何东西。

例子，简化的锁。

```c
#include <stdlib.h>
#include <pthread.h>

#define NHASH 29
#define HASH(id) (((unsigned long)id) % NHASH)

struct foo *fh[NHASH];
pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo{
	int f_count; /* protected by hashlock */
	pthread_mutex_t f_lock;
	int f_id;
	struct foo *f_next; /* protected by hashlock */
						/* ... more stuff here ... */
};

struct foo *foo_alloc(int id){ /* allocate the object */
	struct foo *fp;
	int idx;

	if ((fp = malloc(sizeof(struct foo))) != NULL)
	{
		fp->f_count = 1;
		fp->f_id = id;
		if (pthread_mutex_init(&fp->f_lock, NULL) != 0)
		{
			free(fp);
			return (NULL);
		}
		idx = HASH(id);
		pthread_mutex_lock(&hashlock);
		fp->f_next = fh[idx];
		fh[idx] = fp;
		pthread_mutex_lock(&fp->f_lock);
		pthread_mutex_unlock(&hashlock);
		/* ... continue initialization ... */
		pthread_mutex_unlock(&fp->f_lock);
	}
	return (fp);
}

void foo_hold(struct foo *fp) /* add a reference to the object */
{
	pthread_mutex_lock(&hashlock);
	fp->f_count++;
	pthread_mutex_unlock(&hashlock);
}

struct foo *foo_find(int id) /* find an existing object */
{
	struct foo *fp;

	pthread_mutex_lock(&hashlock);
	for (fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next)
	{
		if (fp->f_id == id)
		{
			fp->f_count++;
			break;
		}
	}
	pthread_mutex_unlock(&hashlock);
	return (fp);
}

void foo_rele(struct foo *fp) /* release a reference to the object */
{
	struct foo *tfp;
	int idx;

	pthread_mutex_lock(&hashlock);
	if (--fp->f_count == 0)
	{ /* last reference, remove from list */
		idx = HASH(fp->f_id);
		tfp = fh[idx];
		if (tfp == fp)
		{
			fh[idx] = fp->f_next;
		}
		else
		{
			while (tfp->f_next != fp)
				tfp = tfp->f_next;
			tfp->f_next = fp->f_next;
		}
		pthread_mutex_unlock(&hashlock);
		pthread_mutex_destroy(&fp->f_lock);
		free(fp);
	}
	else
	{
		pthread_mutex_unlock(&hashlock);
	}
}
```

两种用途使用相同的锁时，围绕散列列表和引用计数的锁的排序问题就不存在了。多线程的软件设计涉及这两者之间的折中。如果锁的粒度太粗，就会出现很多线程阻塞等待相同的锁，这可能并不能改善并发性。如果锁的粒度太细，那么过多的锁开销会使系统性能受到影响，而且代码变得复杂。作为一个程序员，需要在满足锁需求的情况下，在代码复杂性和性能之间找到正确的平衡。

### 函数 pthread_mutex_timedlock

当线程试图获取一个已加锁的互斥量时，`pthread_mutex_timedlock` 互斥量原语允许绑定线程阻塞时间。`pthread_mutex_timedlock` 函数与 `pthread_mutex_lock` 是基本等价的，但是在达到超时时间值时，`pthread_mutex_timedlock` 不会对互斥量进行加锁，而是返回错误码 `ETIMEDOUT`。

```c
#include <pthread.h>
#include <time.h>
int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex,
                            const struct timespec *restrict tsptr);
```

返回值：

- 若成功，返回 0；
- 否则，返回错误编号

超时指定愿意等待的绝对时间。这个超时时间是用 `timespec` 结构来表示的，它用秒和纳秒来描述时间。

例子，使用 `pthread_mutex_timedlock` 。

```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

int main(){
	int err;
	struct timespec tout;
	struct tm *tmp;
	char buf[64];
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&lock);

	printf("mutex is locked.\n");
	clock_gettime(CLOCK_REALTIME, &tout);
	tmp = localtime(&tout.tv_sec);
	strftime(buf, sizeof(buf), "%r", tmp);
	printf("current time is %s.\n", buf);
	tout.tv_sec += 10;

	err = pthread_mutex_timedlock(&lock, &tout);

	clock_gettime(CLOCK_REALTIME, &tout);
	tmp = localtime(&tout.tv_sec);
	strftime(buf, sizeof(buf), "%r", tmp);
	printf("the time is now %s.\n", buf);
	if(err == 0)
		printf("mutex locked again!\n");
	else
		printf("can't lock mutex again: %s.\n", strerror(err));
	return 0;
}
```

编译运行：

```bash
$ gcc 05pthread_mutex_timedlock.c -lpthread
$ ./a.out
mutex is locked
current time is 09:44:09 AM
the time is now 09:44:19 AM
can't lock mutex again: Connection timed out
```

这个程序故意对它已有的互斥量进行加锁，目的是演示 `pthread_mutex_timedlock` 是如何工作的。

### 读写锁

读写锁（reader-writer lock）与互斥量类似，不过读写锁允许更高的并行性。互斥量要么是锁住状态，要么就是不加锁状态，而且一次只有一个线程可以对其加锁。读写锁可以有 3 种状态：读模式下加锁状态，写模式下加锁状态，不加锁状态。一次只有一个线程可以占有写模式的读写锁，但是多个线程可以同时占有读模式的读写锁。

当读写锁是写加锁状态时，在这个锁被解锁之前，所有试图对这个锁加锁的线程都会被阻塞。当读写锁在读加锁状态时，所有试图以读模式对它进行加锁的线程都可以得到访问权，但是任何希望以写模式对此锁进行加锁的线程都会阻塞，直到所有的线程释放它们的读锁为止。虽然各操作系统对读写锁的实现各不相同，但当读写锁处于读模式锁住的状态，而这时有一个线程试图以写模式获取锁时，读写锁通常会阻塞随后的读模式锁请求。这样可以避免读模式锁长期占用，而等待的写模式锁请求一直得不到满足。

读写锁非常适合于对数据结构读的次数远大于写的情况。

读写锁也叫做共享互斥锁（shared-exclusivelock）。当读写锁是读模式锁住时，就可以说成是以共享模式锁住的。当它是写模式锁住的时候，就可以说成是以互斥模式锁住的。

与互斥量相比，读写锁在使用之前必须初始化，在释放它们底层的内存之前必须销毁。

```c
#include <pthread.h>
int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,
                        const pthread_rwlockattr_t *restrict attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

读写锁通过调用 `pthread_rwlock_init` 进行初始化。如果希望读写锁有默认的属性，可以传一个 `NULL` 指针给 `attr` 。

Single UNIX Specification 在 XSI 扩展中定义了 `PTHREAD_RWLOCK_INITIALIZER` 常量。如果默认属性就足够的话，可以用它对静态分配的读写锁进行初始化。

在释放读写锁占用的内存之前，需要调用 `pthread_rwlock_destroy` 做清理工作。如果 `pthread_rwlock_init` 为读写锁分配了资源，`pthread_rwlock_destroy` 将释放这些资源。如果在调用 `pthread_rwlock_destroy` 之前就释放了读写锁占用的内存空间，那么分配给这个锁的资源就会丢失。

要在读模式下锁定读写锁，需要调用 `pthread_rwlock_rdlock`。要在写模式下锁定读写锁，需要调用 `pthread_rwlock_wrlock`。不管以何种方式锁住读写锁，都可以调用 `pthread_rwlock_unlock` 进行解锁。

```c
#include <pthread.h>
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
```

所有函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号

各种实现可能会对共享模式下可获取的读写锁的次数进行限制，所以需要检查 `pthread_rwlock_rdlock` 的返回值。错误返回值的定义只是针对不正确使用读写锁的情况（如未经初始化的锁），或者试图获取已拥有的锁从而可能产生死锁的情况。但是需要注意，有些特定的实现可能会定义另外的错误返回。

Single UNIX Specification 还定义了读写锁原语的条件版本。

```c
#include <pthread.h>
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

可以获取锁时，这两个函数返回 0。否则，它们返回错误 `EBUSY`。

例子，使用读写锁。

```c
#include <stdlib.h>
#include <pthread.h>

struct job
{
    struct job *j_next;
    struct job *j_prev;
    pthread_t j_id; /* tells which thread handles this job */
                    /* ... more stuff here ... */
};

struct queue
{
    struct job *q_head;
    struct job *q_tail;
    pthread_rwlock_t q_lock;
};

/*
 * Initialize a queue.
 */
int queue_init(struct queue *qp)
{
    int err;

    qp->q_head = NULL;
    qp->q_tail = NULL;
    err = pthread_rwlock_init(&qp->q_lock, NULL);
    if (err != 0)
        return (err);
    /* ... continue initialization ... */
    return (0);
}

/*
 * Insert a job at the head of the queue.
 */
void job_insert(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    jp->j_next = qp->q_head;
    jp->j_prev = NULL;
    if (qp->q_head != NULL)
        qp->q_head->j_prev = jp;
    else
        qp->q_tail = jp; /* list was empty */
    qp->q_head = jp;
    pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Append a job on the tail of the queue.
 */
void job_append(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    jp->j_next = NULL;
    jp->j_prev = qp->q_tail;
    if (qp->q_tail != NULL)
        qp->q_tail->j_next = jp;
    else
        qp->q_head = jp; /* list was empty */
    qp->q_tail = jp;
    pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Remove the given job from a queue.
 */
void job_remove(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    if (jp == qp->q_head)
    {
        qp->q_head = jp->j_next;
        if (qp->q_tail == jp)
            qp->q_tail = NULL;
        else
            jp->j_next->j_prev = jp->j_prev;
    }
    else if (jp == qp->q_tail)
    {
        qp->q_tail = jp->j_prev;
        jp->j_prev->j_next = jp->j_next;
    }
    else
    {
        jp->j_prev->j_next = jp->j_next;
        jp->j_next->j_prev = jp->j_prev;
    }
    pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Find a job for the given thread ID.
 */
struct job *job_find(struct queue *qp, pthread_t id)
{
    struct job *jp;

    if (pthread_rwlock_rdlock(&qp->q_lock) != 0)
        return (NULL);

    for (jp = qp->q_head; jp != NULL; jp = jp->j_next)
        if (pthread_equal(jp->j_id, id))
            break;

    pthread_rwlock_unlock(&qp->q_lock);
    return (jp);
}
```

在这个例子中，凡是需要向队列中增加作业或者从队列中删除作业的时候，都采用了写模式来锁住队列的读写锁。不管何时搜索队列，都需要获取读模式下的锁，允许所有的工作线程并发地搜索队列。在这种情况下，只有在线程搜索作业的频率远远高于增加或删除作业时，使用读写锁才可能改善性能。

### 带有超时的读写锁

与互斥量一样，Single UNIX Specification 提供了带有超时的读写锁加锁函数，使应用程序在获取读写锁时避免陷入永久阻塞状态。这两个函数是 `pthread_rwlock_timedrdlock` 和 `pthread_rwlock_timedwrlock`。

```c
#include <pthread.h>
#include <time.h>
int pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rwlock,
                            const struct timespec *restrict tsptr);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rwlock,
                            const struct timespec *restrict tsptr);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

这两个函数的行为与它们“不计时的”版本类似。`tsptr` 参数指向 `timespec` 结构，指定线程应该停止阻塞的时间。如果它们不能获取锁，那么超时到期时，这两个函数将返回 `ETIMEDOUT` 错误。与 `pthread_mutex_timedlock` 函数类似，超时指定的是绝对时间，而不是相对时间。

### 条件变量

条件变量是线程可用的另一种同步机制。条件变量给多个线程提供了一个会合的场所。条件变量与互斥量一起使用时，允许线程以无竞争的方式等待特定的条件发生。

条件本身是由互斥量保护的。线程在改变条件状态之前必须首先锁住互斥量。其他线程在获得互斥量之前不会察觉到这种改变，因为互斥量必须在锁定以后才能计算条件。

在使用条件变量之前，必须先对它进行初始化。由 `pthread_cond_t` 数据类型表示的条件变量可以用两种方式进行初始化，可以把常量 `PTHREAD_COND_INITIALIZER` 赋给静态分配的条件变量，但是如果条件变量是动态分配的，则需要使用 `pthread_cond_init` 函数对它进行初始化。

在释放条件变量底层的内存空间之前，可以使用 `pthread_cond_destroy` 函数对条件变量进行反初始化（deinitialize）。

```c
#include <pthread.h>
int pthread_cond_init(pthread_cond_t *restrict cond,
                    const pthread_condattr_t *restrict attr);
int pthread_cond_destroy(pthread_cond_t *cond);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

创建一个具有默认属性的条件变量，`pthread_cond_init` 函数的 `attr` 参数可以设置为 `NULL`。

我们使用 `pthread_cond_wait` 等待条件变量变为真。如果在给定的时间内条件不能满足，那么会生成一个返回错误码的变量。

```c
#include <pthread.h>
int pthread_cond_wait(pthread_cond_t *restrict cond,
                    pthread_mutex_t *restrict mutex);
int pthread_cond_timedwait(pthread_cond_t *restrict cond,
                        pthread_mutex_t *restrict mutex,
                        const struct timespec *restrict tsptr);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

传递给 `pthread_cond_wait` 的互斥量对条件进行保护。调用者把锁住的互斥量传给函数，函数然后自动把调用线程放到等待条件的线程列表上，对互斥量解锁。这就关闭了条件检查和线程进入休眠状态等待条件改变这两个操作之间的时间通道，这样线程就不会错过条件的任何变化。`pthread_cond_wait` 返回时，互斥量再次被锁住。

`pthread_cond_timedwait` 函数的功能与 `pthread_cond_wait` 函数相似，只是多了一个超时，这个时间值是一个绝对数而不是相对数。

如果超时到期时条件还是没有出现，`pthread_cond_timewait` 将重新获取互斥量，然后返回错误 `ETIMEDOUT`。从 `pthread_cond_wait` 或者 `pthread_cond_timedwait` 调用成功返回时，线程需要重新计算条件，因为另一个线程可能已经在运行并改变了条件。

有两个函数可以用于通知线程条件已经满足。`pthread_cond_signal` 函数至少能唤醒一个等待该条件的线程，而 `pthread_cond_broadcast` 函数则能唤醒等待该条件的所有线程。

```c
#include <pthread.h>
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

在调用 `pthread_cond_signal` 或者 `pthread_cond_broadcast` 时，我们说这是在给线程或者条件发信号。必须注意，一定要在改变条件状态以后再给线程发信号。

例子，使用条件变量。

```c
#include <pthread.h>

struct msg
{
    struct msg *m_next;
    /* ... more stuff here ... */
};

struct msg *workq;
// 条件是工作队列的状态。
pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
// 互斥量保护条件
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

void process_msg(void){
    struct msg *mp;

    for (;;)
    {
        pthread_mutex_lock(&qlock);
        // 在 while 循环中判断条件。
        while (workq == NULL)
            pthread_cond_wait(&qready, &qlock);
        mp = workq;
        workq = mp->m_next;
        pthread_mutex_unlock(&qlock);
        /* now process the message mp */
    }
}

void enqueue_msg(struct msg *mp){
    pthread_mutex_lock(&qlock);
    mp->m_next = workq;
    workq = mp;
    pthread_mutex_unlock(&qlock);
    pthread_cond_signal(&qready);
}
```

### 自旋锁

自旋锁与互斥量类似，但它不是通过休眠使进程阻塞，而是在获取锁之前一直处于忙等（自旋）阻塞状态。自旋锁可用于以下情况：锁被持有的时间短，而且线程并不希望在重新调度上花费太多的成本。

自旋锁通常作为底层原语用于实现其他类型的锁。根据它们所基于的系统体系结构，可以通过使用测试并设置指令有效地实现。当然这里说的有效也还是会导致 CPU 资源的浪费：当线程自旋等待锁变为可用时，CPU 不能做其他的事情。这也是自旋锁只能够被持有一小段时间的原因。

当自旋锁用在非抢占式内核中时是非常有用的：除了提供互斥机制以外，它们会阻塞中断，这样中断处理程序就不会让系统陷入死锁状态，因为它需要获取已被加锁的自旋锁（把中断想成是另一种抢占）。在这种类型的内核中，中断处理程序不能休眠，因此它们能用的同步原语只能是自旋锁。

自旋锁的接口与互斥量的接口类似，这使得它可以比较容易地从一个替换为另一个。可以用 `pthread_spin_init` 函数对自旋锁进行初始化。用 `pthread_spin_destroy` 函数进行自旋锁的反初始化。

```c
#include <pthread.h>
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_destroy(pthread_spinlock_t *lock);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

只有一个属性是自旋锁特有的，这个属性只在支持线程进程共享同步（Thread Process-Shared Synchronization）选项的平台上才用得到。`pshared` 参数表示进程共享属性，表明自旋锁是如何获取的。如果它设为 `PTHREAD_PROCESS_SHARED`，则自旋锁能被可以访问锁底层内存的线程所获取，即便那些线程属于不同的进程，情况也是如此。否则 `pshared` 参数设为 `PTHREAD_PROCESS_PRIVATE`，自旋锁就只能被初始化该锁的进程内部的线程所访问。

可以用 `pthread_spin_lock` 或 `pthread_spin_trylock` 对自旋锁进行加锁，前者在获取锁之前一直自旋，后者如果不能获取锁，就立即返回 `EBUSY` 错误。注意，`pthread_spin_trylock` 不能自旋。不管以何种方式加锁，自旋锁都可以调用 `pthread_spin_unlock` 函数解锁。

```c
#include <pthread.h>
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);
```

所有函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

注意，如果自旋锁当前在解锁状态的话，`pthread_spin_lock` 函数不要自旋就可以对它加锁。如果线程已经对它加锁了，结果就是未定义的。调用 `pthread_spin_lock` 会返回 `EDEADLK` 错误（或其他错误），或者调用可能会永久自旋。具体行为依赖于实际的实现。试图对没有加锁的自旋锁进行解锁，结果也是未定义的。

不管是 `pthread_spin_lock` 还是 `pthread_spin_trylock`，返回值为 0 的话就表示自旋锁被加锁。需要注意，不要调用在持有自旋锁情况下可能会进入休眠状态的函数。如果调用了这些函数，会浪费 CPU 资源，因为其他线程需要获取自旋锁需要等待的时间就延长了

### 屏障

屏障（barrier）是用户协调多个线程并行工作的同步机制。屏障允许每个线程等待，直到所有的合作线程都到达某一点，然后从该点继续执行。我们已经看到一种屏障，`pthread_join` 函数就是一种屏障，允许一个线程等待，直到另一个线程退出。

但是屏障对象的概念更广，它们允许任意数量的线程等待，直到所有的线程完成处理工作，而线程不需要退出。所有线程达到屏障后可以接着工作。

可以使用 `pthread_barrier_init` 函数对屏障进行初始化，用 `thread_barrier_destroy` 函数反初始化。

```c
#include <pthread.h>
int pthread_barrier_init(pthread_barrier_t *restrict barrier,
                        const pthread_barrierattr_t *restrict attr, unsigned int count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
```

两个函数的返回值：

- 若成功，返回 0；
- 否则，返回错误编号。

初始化屏障时，可以使用 `count` 参数指定，在允许所有线程继续运行之前，必须到达屏障的线程数目。使用 `attr` 参数指定屏障对象的属性，设置 `attr` 为 `NULL`，用默认属性初始化屏障。如果使用 `pthread_barrier_init` 函数为屏障分配资源，那么在反初始化屏障时可以调用 `pthread_barrier_destroy` 函数释放相应的资源。

可以使用 `pthread_barrier_wait` 函数来表明，线程已完成工作，准备等所有其他线程赶上来。

```c
#include <pthread.h>
int pthread_barrier_wait(pthread_barrier_t *barrier);
```

返回值：

- 若成功，返回 0 或者 `PTHREAD_BARRIER_SERIAL_THREAD`；
- 否则，返回错误编号。

调用 `pthread_barrier_wait` 的线程在屏障计数未满足条件时，会进入休眠状态。如果该线程是最后一个调用 `pthread_barrier_wait` 的线程，就满足了屏障计数，所有的线程都被唤醒。

对于一个任意线程，`pthread_barrier_wait` 函数返回了 `PTHREAD_BARRIER_SERIAL_THREAD`。剩下的线程看到的返回值是 0。这使得一个线程可以作为主线程，它可以工作在其他所有线程已完成的工作结果上。

一旦达到屏障计数值，而且线程处于非阻塞状态，屏障就可以被重用。但是除非在调用了 `pthread_barrier_destroy` 函数之后，又调用了 `pthread_barrier_init` 函数对计数用另外的数进行初始化，否则屏障计数不会改变。

例子，使用屏障。

```c
#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <sys/time.h>
#include <stdlib.h>

#define NTHR 8			// 线程数量
#define NUMNUM 8000000L	// 需要排序的数据数量
#define TNUM (NUMNUM / NTHR)

long random_nums[NUMNUM];
long sorted_nums[NUMNUM];

pthread_barrier_t b;

int complong(const void *arg1, const void *arg2){
	long l1 = *(long *)arg1;
	long l2 = *(long *)arg2;
	if(l1 == l2)
		return 1;
	else if(l1 < l2)
		return -1;
	else
		return 1;
}

void *thr_fn(void *arg){
	long idx = (long)arg;
	qsort(&random_nums[idx], TNUM, sizeof(long), complong);
	pthread_barrier_wait(&b);
	return (void*)0;
}

void merge(){
	long idx[NTHR];
	long i, minidx, sidx, num;
	for(i = 0; i < NTHR; i++)
		idx[i] = i * TNUM;
	for(sidx = 0; sidx < NUMNUM; sidx++){
		num = LONG_MAX;
		for(i = 0; i < NTHR; i++){
			if((idx[i] < ((i+1) * TNUM)) && (random_nums[idx[i]] < num)){
				num = random_nums[idx[i]];
				minidx = i;
			}
		}
		sorted_nums[sidx] = random_nums[idx[minidx]];
		idx[minidx]++;
	}
}

int main(){
	unsigned long i;
	struct timeval start, end;
	long long startusec, endusec;
	double elapsed;
	int err;
	pthread_t tid;

	srandom(1);
	for (i = 0; i < NUMNUM; i++)
		random_nums[i] = random();
	gettimeofday(&start, NULL);
	pthread_barrier_init(&b, NULL, NTHR + 1);
	for (i = 0; i < NTHR; i++){
		// 使用8个线程分解了800万个数的排序工作。每个线程用堆排序算法对100万个数进行排序
		err = pthread_create(&tid, NULL, thr_fn, (void *)(i * TNUM));
		if(err != 0){
			perror(" can't create thread");
			return 1;
		}
	}
	pthread_barrier_wait(&b);
	// 主线程调用一个函数对这些结果进行合并。
	merge();
	gettimeofday(&end, NULL);

	startusec = start.tv_sec * 1000000 + start.tv_usec;
	endusec = end.tv_sec * 1000000 + end.tv_usec;
	elapsed = (double)(endusec - startusec) / 1000000.0;
	printf("%d thread sort took %.4f seconds\n", NTHR, elapsed);
	// for (i = 0; i < NUMNUM; i++)
	// 	printf("%ld\n", sorted_nums[i]);

	gettimeofday(&start, NULL);

	// 主线程调用 qsort 进行排序
	qsort(&random_nums, NUMNUM, sizeof(long), complong);

	gettimeofday(&end, NULL);
	startusec = start.tv_sec * 1000000 + start.tv_usec;
	endusec = end.tv_sec * 1000000 + end.tv_usec;
	elapsed = (double)(endusec - startusec) / 1000000.0;
	printf("1 thread sort took %.4f seconds\n", elapsed);
	return 0;
}
```

编译运行：

```bash
 gcc 06pthread_barrier.c -lpthread
 ./a.out
8 thread sort took 0.6863 seconds
1 thread sort took 0.5769 seconds
```

在这个实例中，使用 8 个线程分解了 800 万个数的排序工作。每个线程用堆排序算法对 100 万个数进行排序。然后主线程调用一个函数对这些结果进行合并。根据运行结果，可以发现使用一个线程进行排序的速度反而更快，多线程并不一定会加快运行速度，还是要根据实际情况使用。

并不需要使用 `pthread_barrier_wait` 函数中的返回值 `PTHREAD_BARRIER_SERIAL_THREAD` 来决定哪个线程执行结果合并操作，因为我们使用了主线程来完成这个任务。这也是把屏障计数值设为工作线程数加 1 的原因，主线程也作为其中的一个候选线程。
