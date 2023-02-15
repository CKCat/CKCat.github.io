---
title: Flask简单学习
date: 2020-01-29 13:55:27
tags: Flask
category: python
---

# 安装
由于是学习，所以直接在django的虚拟环境中安装Flask。
```
指定Flask版本安装
$ pip install flask==0.10.1
```

# 从 Hello World 开始
## Flask程序运行过程：

所有Flask程序必须有一个程序实例。

Flask调用视图函数后，会将视图函数的返回值作为响应的内容，返回给客户端。一般情况下，响应内容主要是字符串和状态码。

当客户端想要获取资源时，一般会通过浏览器发起HTTP请求。此时，Web服务器使用WSGI（Web Server Gateway Interface）协议，把来自客户端的所有请求都交给Flask程序实例。WSGI是为 Python 语言定义的Web服务器和Web应用程序之间的一种简单而通用的接口，它封装了接受HTTP请求、解析HTTP请求、发送HTTP，响应等等的这些底层的代码和操作，使开发者可以高效的编写Web应用。

程序实例使用Werkzeug来做路由分发（URL请求和视图函数之间的对应关系）。根据每个URL请求，找到具体的视图函数。 在Flask程序中，路由的实现一般是通过程序实例的route装饰器实现。route装饰器内部会调用add_url_route()方法实现路由注册。

调用视图函数，获取响应数据后，把数据传入HTML模板文件中，模板引擎负责渲染响应数据，然后由Flask返回响应数据给浏览器，最后浏览器处理返回的结果显示给客户端。










