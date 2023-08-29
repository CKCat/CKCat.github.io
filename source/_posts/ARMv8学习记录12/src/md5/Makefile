# 编译64位，需要指定 host=aarch64，例如: make host=aarch64
host=armv7

FILE_NAME=main-$(host)

run:
	adb push $(FILE_NAME) /data/local/tmp/$(FILE_NAME)
	adb shell chmod +x /data/local/tmp/$(FILE_NAME)
	adb shell /data/local/tmp/$(FILE_NAME)
