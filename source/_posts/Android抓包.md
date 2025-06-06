---
title: Android抓包
date: 2020-12-30 14:53:08
tags: 逆向
category: Android
---

# Android 抓包

## 使用 Charles 抓包

1. PC 端共享无线网络，使用 `ipconfig` 命令查看 ip 地址：

![](Android抓包/2020-12-30-16-09-26.png)

2. 手机端设置代理

![](Android抓包/2020-12-30-16-10-42.png)

3. 抓包工具设置代理

![](Android抓包/2020-12-30-16-12-38.png)

![](Android抓包/2024-02-27-11-34-07.png)

4. 安装证书

手机端访问 `chls.pro/ssl` 下载证书并安装。其中可以通过 Magisk 插件 [MoveCertificate](https://github.com/ys1231/MoveCertificate) 将证书从用户证书移动到系统证书。后续即可进行抓包了。 **如果无法进行抓包，尝试一下关闭 Windows 防火墙。**

brup 配置好代理后，下载证书地址 http://burp 。

brup 通过 `Proxy Setttings -> Import /export CA certificate` 导出 `Certificate in DER format` 的证书，然后再通过手机安装该证书后却无法抓 https 数据包，提示你的连接不是私密连接 `NET::ERR_CERT_AUTHORITY_INVALID` 。

如果系统为 Android 7 以下的版本，可以手动将用户证书移动到系统证书。

```bash
# 挂载根目录，正常情况下，使其可读写
mount -o rw,remount /
# 将用户证书移入系统证书目录
mv -f /data/misc/user/0/cacerts-added/123abc456.0 /system/etc/security/cacerts
# 重新挂载根目录，使其为只读
mount -o ro,remount /
```

Android 7 以上的版本，可以使用下列脚本临时移动证书到系统证书。

```bash
set -e # Fail on error

# Create a separate temp directory, to hold the current certificates
# Without this, when we add the mount we can't read the current certs anymore.
mkdir -p -m 700 /data/local/tmp/htk-ca-copy

# Copy out the existing certificates
if [ -d "/apex/com.android.conscrypt/cacerts" ]; then
      cp /apex/com.android.conscrypt/cacerts/* /data/local/tmp/htk-ca-copy/
else
      cp /system/etc/security/cacerts/* /data/local/tmp/htk-ca-copy/
fi

# Create the in-memory mount on top of the system certs folder
mount -t tmpfs tmpfs /system/etc/security/cacerts

# Copy the existing certs back into the tmpfs mount, so we keep trusting them
mv /data/local/tmp/htk-ca-copy/* /system/etc/security/cacerts/

# Copy our new cert in, so we trust that too
mv ${certificatePath} /system/etc/security/cacerts/

# Update the perms & selinux context labels, so everything is as readable as before
chown root:root /system/etc/security/cacerts/*
chmod 644 /system/etc/security/cacerts/*
chcon u:object_r:system_file:s0 /system/etc/security/cacerts/*

echo 'System cacerts setup completed'

# Deal with the APEX overrides in Android 14+, which need injecting into each namespace:
if [ -d "/apex/com.android.conscrypt/cacerts" ]; then
      echo 'Injecting certificates into APEX cacerts'

      # When the APEX manages cacerts, we need to mount them at that path too. We can't do
      # this globally as APEX mounts are namespaced per process, so we need to inject a
      # bind mount for this directory into every mount namespace.

      # First we get the Zygote process(es), which launch each app
      ZYGOTE_PID=$(pidof zygote || true)
      ZYGOTE64_PID=$(pidof zygote64 || true)
      Z_PIDS="$ZYGOTE_PID $ZYGOTE64_PID"
      # N.b. some devices appear to have both, some have >1 of each (!)

      # Apps inherit the Zygote's mounts at startup, so we inject here to ensure all newly
      # started apps will see these certs straight away:
      for Z_PID in $Z_PIDS; do
         if [ -n "$Z_PID" ]; then
            nsenter --mount=/proc/$Z_PID/ns/mnt -- \
                  /bin/mount --bind /system/etc/security/cacerts /apex/com.android.conscrypt/cacerts
         fi
      done

      echo 'Zygote APEX certificates remounted'

      # Then we inject the mount into all already running apps, so they see these certs immediately.

      # Get the PID of every process whose parent is one of the Zygotes:
      APP_PIDS=$(
         echo $Z_PIDS | \
         xargs -n1 ps -o 'PID' -P | \
         grep -v PID
      )

      # Inject into the mount namespace of each of those apps:
      for PID in $APP_PIDS; do
         nsenter --mount=/proc/$PID/ns/mnt -- \
            /bin/mount --bind /system/etc/security/cacerts /apex/com.android.conscrypt/cacerts &
      done
      wait # Launched in parallel - wait for completion here

      echo "APEX certificates remounted for $(echo $APP_PIDS | wc -w) apps"
fi

# Delete the temp cert directory & this script itself
rm -r /data/local/tmp/htk-ca-copy
rm ${injectionScriptPath}

echo "System cert successfully injected"
```

上面的脚本来自 [httptoolkit](https://github.com/httptoolkit/httptoolkit-server/blob/main/src/interceptors/android/adb-commands.ts#L256) 。

## VPN 配合 Charles 抓包

## Charles 设置

首先要把 Charles 当做一个 SOCKS5 的代理服务器，所以要先设置 SOCKS5 代理服务器。打开 Proxy 设置选项，开启 SOCKS 服务器，我这里开启的端口为 8889 ，也可以随意填写。

![](Android抓包/2020-12-30-16-52-51.png)

下面介绍 3 款手机端的 VPN 应用。

[SagerNet](https://github.com/SagerNet/SagerNet)

[SocksDroid](https://github.com/bndeff/socksdroid)

### SocksDroid 设置

这里还有另一款 VPN 工具 [SocksDroid](Android抓包/SocksDroid.apk) 可以使用，这里也顺便说一下其配置方法，除了设置服务器 IP 和端口外，还需要设置一下 DNS 服务器，以适应国内网络环境。

![](Android抓包/2020-12-30-17-01-17.png)
![](Android抓包/2020-12-30-17-01-48.png)

### HTTP/HTTPS 转发到 Burp Suite

在 Charles 中，打开 External Proxy Settings 选项卡，选择把数据转到 Burp Suite 的代理服务器中。

![](Android抓包/2020-12-30-17-21-25.png)

# Windows 虚拟机抓包

Charles 安装在物理机上。

1. 首先保存证书
   在 Charles 菜单栏选择 `Help -> SSL proxying -> Save Charles Root Certificate` 保存证书，其中证书格式为 `.cer`。

然后将证书拖入虚拟中，直接双击安装，将证书存储改为：受信任的根证书颁发机构。

2. 设置代理
   将虚拟机的代理设置为物理机的 IP，并且将代理端口设置为 Charles 的代理端口。其中设置代理的位置在：所有设置 -> 网络和 Internet -> 代理 -> 手动设置代理。

3. 设置 Charles
   在 Charles 菜单栏选择 `Proxy -> SSL Proxying Settings -> SSL Proxying` ，选中 Enable SSL Proxying，并且在 Include 选项添加 Location，Host 和 Port 都设置为 `*`。

##  Chrome访问提示不安全的网站

Chrome浏览器对证书的信任策略一直在变化，比如最新版本的Chrome浏览器会忽略安装到Android系统证书目录的自签CA证书，如果需要对Chrome浏览器进行抓包，请按照下面的方式进行处理：

- 如果是高版本Chrome浏览器，需要将CA证书安装到用户证书目录；
- 如果是低版本的Chrome浏览器，需要将CA证书安装到系统证书目录。

如果不确定选用那种方式，可以分别尝试下。

## Firefox访问提示不安全的网站

Firefox浏览器使用内置的CA Store，系统安装的CA证书无法生效，需要在Firefox调试菜单中启用信任。

- Firefox设置 -> 关于Firefox -> 点击顶部Logo 5下启用调试菜单。
- Firefox设置 -> Secret Settings -> 启用 Use third party CA certificates。

# 微信抓包

1. 首先用数据线把手机连接到电脑。
2. 手机打开 USB 调试。
3. 手机进入微信随便打开一个聊天窗口输入并发送： `http://debugxweb.qq.com/?inspector=true` 点击打开这个链接，弹出“执行成功”，即可。

4. 手机微信打开想要抓包调试的网页。
5. 电脑上打开 chrome 内核的浏览器或 edge 浏览器。
   chrome 内核的浏览器输入 `chrome://inspect/#devices`
   edge 浏览器输入：`edge://inspect/#devices`

打开后稍等片刻，然后在打开的界面中点击 inspect fallback 。




# 参考连接：

https://mp.weixin.qq.com/s/ahPbBSfkkBsv4oy265rI2Q

https://www.cnblogs.com/lulianqi/p/11380794.html

https://www.52pojie.cn/thread-1826013-1-1.html

http://91fans.com.cn/post/certificate/#gsc.tab=0
