/*
 * @FileName: 
 * @Description: 
 * @Autor: Liujunjie/Aries-441
 * @StudentNumber: 521021911059
 * @Date: 2023-11-30 21:37:02
 * @E-mail: sjtu.liu.jj@gmail.com/sjtu.1518228705@sjtu.edu.cn
 * @LastEditTime: 2023-12-26 19:56:33
 */
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
#include <mysql.h>
#include <limits.h>
#include <libgen.h>

typedef struct {
    int sock_ctl; // control socket
    int sock_data; // data socket
    char UserName[MAXSIZE];
    char FTP_PATH[MAXSIZE];
    int pasv_flag;
} UserSession;

void ftpserver_process(UserSession *session);

int ftpserver_recv_cmd(UserSession *session,char* cmd,char* arg);

int ftpserver_login(UserSession *session);

int ftpserver_check_user(const char* user,const char* pass);

int ftpserver_start_data_conn(int sock_ctl);

int ftpserver_start_pasv_data_conn(UserSession *session);

void ftpserver_pasv(UserSession *session);

int ftpserver_list(UserSession *session);

void ftpserver_retr(UserSession *session,char *filename);

void ftpserver_push(UserSession *session,char *filename);

void ftpserver_delet(UserSession *session,char *filename);

void ftpserver_rename_directory(UserSession *session,char *path);

int ftpserver_remove_directory(UserSession *session,char *path);

void ftpserver_make_directory(UserSession *session,char *path);

// for a new user create an ACL file
void create_acl_file(char *UserName, char *path);

// Check permissions for a user in an ACL file
int check_permissions(char *UserName, char *PermissionType, char *aclFilePath);

// Check if a user has a specific permission
int user_has_permission(char *UserName, char *PermissionList);

char* find_ctl_acl(const char* path);

#endif  //_FTP_SERVER_H__
