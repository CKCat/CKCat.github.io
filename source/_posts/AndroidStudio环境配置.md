---
title: AndroidStudio环境配置
date: 2022-05-30 18:37:31
tag: AndroidStudio
category: Android
---

# Android Studio 修改 `.android` `.gradle` `.AndroidStudio` 文件夹位置

## `.android` 文件夹的修改

首先，需要添加一个系统的环境变量 `ANDROID_SDK_HOME` 模拟器环境变量

默认情况下，模拟器会将配置文件存储在 `$HOME/.android/` 下，将 AVD 数据存储在 `$HOME/.android/avd/` 下。您可以通过设置以下环境变量来替换默认设置。

| ANDROID_EMULATOR_HOME | 设置特定于用户的模拟器配置目录的路径。默认位置是 `$ANDROID_SDK_HOME/.android/`。                                                                                        |
| --------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ANDROID_AVD_HOME      | 设置包含所有 AVD 特定文件的目录的路径，这些文件大多由非常大的磁盘映像组成。默认位置是 `$ANDROID_EMULATOR_HOME/avd/`。如果默认位置的磁盘空间不足，您可能需要指定新位置。 |

## `.Gradle` 文件夹的修改

首先，需要添加一个系统的环境变量 `GRADLE_USER_HOME` ，然后进入 `Android Studio -> File -> Settings -> Gradle` 设置好路径即可。

## `.AndroidStudio`文件夹的修改

进入 Android Studio 的安装目录，进入 bin 文件夹，用文本编辑软件打开 `idea.properties`，去掉以下两项的注释符号 #，修改对应的路径为新路径即可。

```bash

# idea.config.path=${user.home}/.AndroidStudio/config

# idea.system.path=${user.home}/.AndroidStudio/system
```

参考：
https://juejin.cn/post/6967215049446260772

https://developer.android.com/studio/command-line/variables?hl=zh-cn

https://docs.gradle.org/current/userguide/build_environment.html

# Android Studio Build Output 控制台输出乱码解决

点击 Android Studio 菜单栏 Help 下拉菜单找到 `Edit Custom VM Options...`选项。

打开 `studio64.exe.vmoptions `文件，输入

```bash
-Dfile.encoding=UTF-8
```
关闭 studio，重新打开即可。


# 设置 Terminal 为cmder
打开 android studio 的设置，找到 Tools 下的 terminal 修改 Shell path 为 `"cmd" /k ""%CMDER_ROOT%\vendor\init.bat""` 。


