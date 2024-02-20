---
title: 编译x64dbg
date: 2021-11-09 22:01:34
tags: Debug
category: Windows
---

编译环境：VS2019 + QT5.15.2

参考： https://github.com/x64dbg/x64dbg/wiki/Compiling-the-whole-project

# 准备环境
1. 下载源码
```bash
git clone --recurse-submodules -b development https://github.com/x64dbg/x64dbg.git
```

2. 配置好 VS2019 + QT5.15.2 的环境。

# 编译
1. 在 x64dbg 源码目录运行 `install.bat` 。
```bash
λ install.bat
Run this to install the auto-format hook.
已复制         1 个文件。
```

2. 运行 `setupdeps.bat` 复制依赖库。
```bash
λ setupdeps.bat
deps\x32\asmjit.dll
deps\x32\dbghelp.dll
deps\x32\DeviceNameResolver.dll
deps\x32\jansson.dll
deps\x32\ldconvert.dll
deps\x32\libeay32.dll
deps\x32\lz4.dll
deps\x32\msdia140.dll
deps\x32\msvcp120.dll
deps\x32\msvcr120.dll
deps\x32\Qt5Core.dll
deps\x32\Qt5Gui.dll
deps\x32\Qt5Network.dll
deps\x32\Qt5Widgets.dll
deps\x32\Qt5WinExtras.dll
deps\x32\Scylla.dll
deps\x32\ssleay32.dll
deps\x32\symsrv.dll
deps\x32\TitanEngine.dll
deps\x32\x32_bridge.dll
deps\x32\x32_dbg.dll
deps\x32\XEDParse.dll
deps\x32\yara.dll
deps\x32\GleeBug\TitanEngine.dll
deps\x32\platforms\qwindows.dll
deps\x32\StaticEngine\TitanEngine.dll
复制了 26 个文件
......
```

3. 使用 VS2019 打开 `x64dbg.sln` ，依次选择需要编译的 Project，进入 Project-->Property 选项，将 Platform Toolset 设置为 Visual Studio 2019 (v142)，然后直接编译。

4. 使用 qt creator 打开 `src/gui/x64dbg.pro`，配置好编译选项，直接编译。

5. 编译完成后，运行 `windeployqt.exe --force x64gui.dll` 命令，获取相应的依赖库。
```bash
λ windeployqt.exe --force E:\Code\WinProjects\x64dbg\bin\x32d\x32gui.dll
E:\Code\WinProjects\x64dbg\bin\x32d\x32gui.dll 32 bit, debug executable
Adding Qt5Svg for qsvgicond.dll
Skipping plugin qtvirtualkeyboardplugind.dll due to disabled dependencies (Qt5Qml Qt5Quick).
Skipping plugin qtvirtualkeyboard_hanguld.dll due to disabled dependencies (Qt5Qml Qt5Quick).
Skipping plugin qtvirtualkeyboard_openwnnd.dll due to disabled dependencies (Qt5Qml Qt5Quick).
Skipping plugin qtvirtualkeyboard_pinyind.dll due to disabled dependencies (Qt5Qml Qt5Quick).
Skipping plugin qtvirtualkeyboard_tcimed.dll due to disabled dependencies (Qt5Qml Qt5Quick).
Skipping plugin qtvirtualkeyboard_thaid.dll due to disabled dependencies (Qt5Qml Qt5Quick).
Direct dependencies: Qt5Core Qt5Gui Qt5Network Qt5Widgets Qt5WinExtras
All dependencies   : Qt5Core Qt5Gui Qt5Network Qt5Widgets Qt5WinExtras
To be deployed     : Qt5Core Qt5Gui Qt5Network Qt5Svg Qt5Widgets Qt5WinExtras
Warning: Cannot find Visual Studio installation directory, VCINSTALLDIR is not set.
Updating Qt5Cored.dll.
Updating Qt5Guid.dll.
Updating Qt5Networkd.dll.
......
Updating qwindowsvistastyled.dll.
Creating qt_da.qm...
Creating qt_de.qm...
Creating qt_en.qm...
Creating qt_es.qm...
Creating qt_fi.qm...
......
```
这里需要注意的 windeployqt.exe 需要与编译选项对应，这里我选择的是`msvc2019/bin/windeployqt.exe`。