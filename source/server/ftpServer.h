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

typedef struct {
    int sock_ctl; // control socket
    int sock_data; // data socket
    char UserName[MAXSIZE];
    char FTP_PATH[MAXSIZE];
} UserSession;

void ftpserver_process(UserSession *session);

int ftpserver_recv_cmd(UserSession *session,char* cmd,char* arg);

int ftpserver_login(UserSession *session);

int ftpserver_check_user(const char* user,const char* pass);

int ftpserver_start_data_conn(int sock_ctl);

int ftpserver_start_pasv_data_conn(int sock_ctl);

void ftpserver_pasv(UserSession *session);

int ftpserver_list(UserSession *session);

void ftpserver_retr(UserSession *session,char *filename);

void ftpserver_push(UserSession *session,char *filename);

void ftpserver_delet(UserSession *session,char *filename);

void ftpserver_rename_directory(UserSession *session,char *path);

int ftpserver_remove_directory(UserSession *session,char *path);

void ftpserver_make_directory(UserSession *session,char *path);

#endif  //_FTP_SERVER_H__
