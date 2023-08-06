# 编译64位，需要指定 host=aarch64，例如: make host=aarch64
host=arm
ifeq ($(OS), Windows_NT)
	PLATFORM=windows
else
	PLATFORM=linux
endif

# export ANDROID_NDK="$ANDROID_HOME/ndk/21.0.6113669"
CC=$(ANDROID_NDK)/toolchains/llvm/prebuilt/$(PLATFORM)-x86_64/bin/clang
all:
	$(CC) -target $(host)-linux-android21 -E hello.c -o $(host)_hello.i
	$(CC) -target $(host)-linux-android21 -S $(host)_hello.i -o $(host)_hello.s
	$(CC) -target $(host)-linux-android21 -c $(host)_hello.s -o $(host)_hello.o
	$(CC) -target $(host)-linux-android21 $(host)_hello.o -o $(host)_hello

install:
	adb push $(host)_hello /data/local/tmp/$(host)_hello
	adb shell chmod +x /data/local/tmp/$(host)_hello

run:
	adb shell /data/local/tmp/$(host)_hello

.PHONY : clean
clean:
	-rm -f *.o *.i *.s $(host)_hello