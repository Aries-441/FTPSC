/*
 * @FileName: 
 * @Description: 
 * @Autor: Liujunjie/Aries-441
 * @StudentNumber: 521021911059
 * @Date: 2023-11-30 21:37:02
 * @E-mail: sjtu.liu.jj@gmail.com/sjtu.1518228705@sjtu.edu.cn
 * @LastEditTime: 2023-12-05 14:52:37
 */
#include "ftpServer.h"
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>

#define MAXSIZE 1024

void* handler_msg(void* arg)
{
    pthread_detach(pthread_self());
    UserSession* session = (UserSession*)arg;
    ftpserver_process(session);
    close(session->sock_ctl);
    free(session); // 记得释放动态分配的内存
    return NULL;
}

int main(int argc, char* argv[])
{
    char ftpPath[MAXSIZE];
    strcpy(ftpPath, "./source/server/ftp/"); // 默认路径

    // 检查命令行参数
    if (argc >= 3 && strcmp(argv[1], "-ftppath") == 0) {
        strcpy(ftpPath, argv[2]);
    }

    // 检查路径是否存在，不存在则创建
    struct stat st;
    if (stat(ftpPath, &st) == -1) {
        if (mkdir(ftpPath, 0700) == -1) {
            fprintf(stderr, "Failed to create FTP path.\n");
            return 1;
        }
    }

    int sock = socket_create("127.0.0.1", 21);

    while (1) {
        int connf = socket_accept(sock);
        if (connf < 0) {
            continue;
        }

        // 创建一个新的用户会话，并初始化套接字和FTP路径
        UserSession* session = (UserSession*)malloc(sizeof(UserSession));
        if (!session) {
            // 如果内存分配失败，关闭连接并继续
            close(connf);
            continue;
        }
        session->sock_ctl = connf;
        session->pasv_flag = 0;
        strcpy(session->FTP_PATH, ftpPath);

        pthread_t tid;
        pthread_create(&tid, NULL, handler_msg, session);
    }
    close(sock);
    return 0;
}