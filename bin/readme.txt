本文件夹内存放经过测试，最终提交的最终版可执行程序。

开发平台为wsl ubuntu 22.04

测试方法：
1. ServerFTP：

1.1 FTP路径设置 `sudo /path/to/ServerFTP/ -ftppath /path/to/FTPfiles/`
1.2 账号密码设置 `sudo /path/to/ServerFTP/ -adduser <username>:<passsword>`
1.3 访问权限设置 参考报告，手动修改

2. ClientFTP

2.1可用指令：
2.1.1 `LIST & PUSH & RETR & DELE /path/to/file`
2.1.2 `MKND & RMD /path/to/dir`
2.1.3 `RNAM /path/to/old_name_file /path/to/new_name_file
2.1.4 `PASV`

2.2 登录：
`sudo /path/to/ClientFTP/ <server ip address>`
请注意，先在服务器端数据库里注册才可用
