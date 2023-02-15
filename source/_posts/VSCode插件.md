---
title: VSCode插件
date: 2019-11-13 21:39:00
tags: VSCode 插件
category: 其他
---

# 记录以下自己常用的 VSCode 插件

- [Arm Assembly](https://marketplace.visualstudio.com/items?itemName=dan-c-underwood.arm) - ARM 汇编语法高亮。
- [File Template](https://marketplace.visualstudio.com/items?itemName=RalfZhang.filetemplate) - 一个自动生成文件模板的 ode 扩展。
- [Markdown Extended](https://marketplace.visualstudio.com/items?itemName=jebbs.markdown-extended) - markdown 扩展。
- [Markdown TOC](https://marketplace.visualstudio.com/items?itemName=huntertran.auto-markdown-toc) - 自动生成 toc 。
- [Paste Image](https://marketplace.visualstudio.com/items?itemName=mushan.vscode-paste-image) - 将图像直接从剪贴板粘贴到 kdown/asciidoc 。
- [Smalise](https://marketplace.visualstudio.com/items?itemName=LoyieKing.smalise) : smali 语法高亮插件。
- [TabNine](https://marketplace.visualstudio.com/items?itemName=TabNine.tabnine-vscode) : AI 代码神器。
- [vscode-icons](https://marketplace.visualstudio.com/items?itemName=vscode-icons-team.vscode-icons) : - Icons for ual Studio Code。
- [YARA](https://marketplace.visualstudio.com/items?itemName=infosec-intern.yara) - yara 语法高亮。
- [kite](https://marketplace.visualstudio.com/items?itemName=kiteco.kite) - AI 代码神奇。
- [Doxygen Documentation Generator](https://marketplace.visualstudio.com/items?itemName=cschlosser.doxdocgen) - ygen 文档注释生成。
- [highlight-words](https://marketplace.visualstudio.com/items?itemName=rsbondi.highlight-words) - 选中高亮

# 修改 terminal 默认为 cmder

打开终端设置 `terminal.integrated.env.windows` 配置文件，添加下列配置。

```bash
"terminal.integrated.defaultProfile.windows": "Cmder",
"terminal.integrated.profiles.windows": {
    "Cmder": {
        "path": [
            "${env:windir}\\Sysnative\\cmd.exe",
            "${env:windir}\\System32\\cmd.exe"
        ],
        "args": [
            "/k D:\\Downloads\\cmder\\vendor\\init.bat"
        ],
        "icon": "terminal-cmd"
    },
}
```
