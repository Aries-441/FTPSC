/*
 * @FileName: 
 * @Description: 
 * @Autor: Liujunjie/Aries-441
 * @StudentNumber: 521021911059
 * @Date: 2023-11-30 21:37:02
 * @E-mail: sjtu.liu.jj@gmail.com/sjtu.1518228705@sjtu.edu.cn
 * @LastEditTime: 2023-12-01 22:28:34
 */
#ifndef _FTP_CLIENT_H__
#define _FTP_CLIENT_H__
#include"../common/common.h"

struct command
{
	char arg[256];
	char code[5];
};

int read_reply(int sock_ctl);   //读取服务器的回复

void print_reply(int status);     //打印回复信息

int ftpclient_read_cmd(char* buf,size_t size,struct command *cmd); //读取客户端输入的命令

int ftpclient_get(int sock_data,char *filanem);  //下载文件

int ftpclient_open_conn(int sock_ctl);   //打开数据连接

int ftpclient_list(int sock_ctl,int sock_data); //处理list命令

int ftpclient_send_cmd(int sock_ctl,struct command* cmd);     //发送命令

int ftpclient_login(int sock_ctl);

int ftpclient_put(int sock_data,char *filename);

//解析服务器返回的227响应来获取服务器的IP地址和端口号，然后使用这些信息来建立数据连接
int ftpclient_start_pasv_data_conn(int sock_ctl, char* response); 

int ftpclient_send_pasv(int sock_ctl, char* response, int response_size);

#endif //_FTP_CLIENT_H__
