---
title: Android对抗手法收集
date: 2020-08-24 09:48:04
tags: 逆向
category: Android
---

# 检测ROOT
```
/sbin/su
/system/bin/su
/system/xbin/su
/data/local/xbin/su
/data/local/bin/su
/system/sd/xbin/su
/system/bin/failsafe/su
/data/local/su
```
https://github.com/lamster2018/EasyProtector
https://www.coolapk.com/apk/io.github.vvb2060.mahoshojo

# 代理


## VPN
```
java.net.NetworkInterface.getName()
```

# Xposed
https://github.com/w568w/XposedDetectLib

https://github.com/KagurazakaHanabi/XposedHider

https://github.com/vvb2060/XposedDetector

# Frida
https://github.com/darvincisec/DetectFrida


# Magisk
https://github.com/vvb2060/MagiskDetector

https://github.com/darvincisec/DetectMagiskHide


# HideRoot
https://github.com/rmnscnce/hsu


# Anti VM
https://github.com/samohyes/Anti-vm-in-android
