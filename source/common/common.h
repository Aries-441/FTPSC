/*
 * @FileName: 
 * @Description: 
 * @Autor: Liujunjie/Aries-441
 * @StudentNumber: 521021911059
 * @Date: 2023-11-30 21:37:02
 * @E-mail: sjtu.liu.jj@gmail.com/sjtu.1518228705@sjtu.edu.cn
 * @LastEditTime: 2023-12-05 16:50:39
 */

#ifndef _COMMON_H__
#define _COMMON_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<ctype.h>
#include<fcntl.h>
#include<errno.h>
#include <openssl/sha.h>

#define MAXSIZE 1024
#define CLIENT_PORT 12345

extern struct timeval timeout;

int socket_create(const char* ip,const int port);

int socket_accept(int sock);

int socket_connect(const char* ip,const int port);

int recv_data(int sock,char* buf,int bufsize);

int send_response(int sock,int code);

void trimstr(char* str,int n);

void socket_Info(int sockfd);

#endif //_COMMON_H__
