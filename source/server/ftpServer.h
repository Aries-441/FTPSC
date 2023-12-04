#ifndef _FTP_SERVER_H__
#define _FTP_SERVER_H__

#include"../common/common.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>


void ftpserver_process(int sock_ctl);

int ftpserver_recv_cmd(int sock_ctl,char* cmd,char* arg);

int ftpserver_login(int sock_ctl);

int ftpserver_check_user(const char* user,const char* pass);

int ftpserver_start_data_conn(int sock_ctl);

void ftpserver_pasv(int sock_data,int sock_ctl);

int ftpserver_list(int sock_data,int sock_ctl);

void ftpserver_retr(int sock_data,int sock_ctl,char *filename);

void ftpserver_push(int sock_data,int sock_ctl,char *filename);

void ftpserver_delet(int sock_ctl,char *filename);

void ftpserver_rename_directory(int sock_ctl,char *path);

int ftpserver_remove_directory(int sock_ctl,char *path);

void ftpserver_make_directory(int sock_ctl,char *path);

#endif  //_FTP_SERVER_H__
