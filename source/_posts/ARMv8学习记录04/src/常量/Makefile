# 编译64位，需要指定 host=aarch64，例如: make host=aarch64
host=arm
ifeq ($(OS), Windows_NT)
	PLATFORM=windows
else
	PLATFORM=linux
endif
FILE_NAME=const
# export ANDROID_NDK="$ANDROID_HOME/ndk/21.0.6113669"
CC=$(ANDROID_NDK)/toolchains/llvm/prebuilt/$(PLATFORM)-x86_64/bin/clang
all:
	$(CC) -target $(host)-linux-android21 $(FILE_NAME).c -o $(host)_$(FILE_NAME)

install:
	adb push $(host)_$(FILE_NAME) /data/local/tmp/$(host)_$(FILE_NAME)
	adb shell chmod +x /data/local/tmp/$(host)_$(FILE_NAME)

run:
	adb shell /data/local/tmp/$(host)_$(FILE_NAME)

.PHONY : clean
clean:
	-rm -f *.o *.i *.s $(host)_$(FILE_NAME)