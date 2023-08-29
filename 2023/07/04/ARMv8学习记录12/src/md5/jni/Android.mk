LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := main.c md5.c
LOCAL_MODULE := main
include $(BUILD_EXECUTABLE)