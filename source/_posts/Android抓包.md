---
title: Android抓包
date: 2020-12-30 14:53:08
tags: 逆向
category: Android
---

# 使用 Charles 抓包

1. PC端共享无线网络，使用 ipconfig 命令查看 ip 地址：

![](Android抓包/2020-12-30-16-09-26.png)

2. 手机端设置代理

![](Android抓包/2020-12-30-16-10-42.png)

3. 抓包工具设置代理

![](Android抓包/2020-12-30-16-12-38.png)

4. 安装证书

手机端访问 chls.pro/ssl 下载证书并安装。其中可以通过 Magisk 插件 [movecerts](https://github.com/Androidacy/movecerts) 将证书从用户证书移动到系统证书。后续即可进行抓包了。 **如果无法进行抓包，尝试一下关闭 Windows 防火墙。**

> brup 配置好代理后，下载证书地址 http://burp 。

手动将用户证书移动到系统证书。
```bash
# 挂载根目录，正常情况下，使其可读写
mount -o rw,remount /
# 将用户证书移入系统证书目录
mv -f /data/misc/user/0/cacerts-added/123abc456.0 /system/etc/security/cacerts
# 重新挂载根目录，使其为只读
mount -o ro,remount /
```

# VPN 配合 Charles 抓包

## Charles 设置
首先要把 Charles 当做一个 SOCKS5 的代理服务器，所以要先设置 SOCKS5 代理服务器。打开 Proxy 设置选项，开启 SOCKS 服务器，我这里开启的端口为 8889 ，也可以随意填写。

![](Android抓包/2020-12-30-16-52-51.png)

下面介绍 3 款手机端的 VPN 应用。



[SagerNet](https://github.com/SagerNet/SagerNet)

[SocksDroid](https://github.com/bndeff/socksdroid)

## SocksDroid 设置
这里还有另一款 VPN 工具 [SocksDroid](Android抓包/SocksDroid.apk) 可以使用，这里也顺便说一下其配置方法，除了设置服务器 IP 和端口外，还需要设置一下 DNS 服务器，以适应国内网络环境。

![](Android抓包/2020-12-30-17-01-17.png)
![](Android抓包/2020-12-30-17-01-48.png)

## HTTP/HTTPS转发到Burp Suite

在 Charles 中，打开 External Proxy Settings 选项卡，选择把数据转到 Burp Suite 的代理服务器中。

![](Android抓包/2020-12-30-17-21-25.png)






参考连接：
```bash
https://mp.weixin.qq.com/s/ahPbBSfkkBsv4oy265rI2Q
https://www.cnblogs.com/lulianqi/p/11380794.html
```