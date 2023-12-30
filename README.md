# FTPSC

学号：521021911059
姓名：刘俊杰

# 项目概述

本项目开发环境是WSL2，Ubuntu22.04系统，在Ubuntu22.04虚拟机或者是主机内都可以运行。

编译工具是 `gcc version 11.4.0 (Ubuntu 11.4.0-1ubuntu1~22.04)`

本项目主要文件以及文件夹：

- bin/: 该文件夹里保存的是项目的可执行文件以及使用说明文档。
- build/: 该文件夹里保存的是编译结果，经过测试确认无问题之后就可以转移到bin文件夹里正式发布。
- logs/: 该文件夹里保存着项目的编译记录，可以查看报错信息等。
- source/: 该文件夹保存着项目的源代码。以下是该文件夹里的内容：
	- client/: 该文件夹包含客户端的c语言实现代码文件以及头文件等。
	- server/: 该文件夹包含服务器端的c语言实现代码文件以及头文件等。
	- common/: 该文件夹包含客户端以及服务器端都需要使用的功能函数，如socket通信相关的功能函数、加密算法以及权限控制函数等的实现文件以及头文件等。
- ctl.acl文件: 存储访问控制列表，用于定义文件或资源的访问权限的数据结构，可以指定哪些用户或组具有读取、写入或执行等操作的权利，以确保只有授权的用户或组能够进行相应的操作。

# 测试方法：

## ServerFTP：

- FTP路径设置 `sudo /path/to/ServerFTP/ -ftppath /path/to/FTPfiles/`
- 账号密码设置 `sudo /path/to/ServerFTP/ -adduser <username>:<passsword>`
- 访问权限设置 参考报告，手动修改
## ClientFTP
### 登录：

`sudo /path/to/ClientFTP/ <server ip address>`
请注意，先在服务器端数据库里注册才可用

### 可用指令：

-  `LIST & PUSH & RETR & DELE /path/to/file`
-  `MKND & RMD /path/to/dir`
-  `RNAM /path/to/old_name_file /path/to/new_name_file
-  `PASV`

