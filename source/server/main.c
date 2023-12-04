#include"ftpServer.h"
#include<pthread.h>

#define MAXSIZE 1024

void* handler_msg(void *arg)
{
    pthread_detach(pthread_self());
    UserSession* session = (UserSession*)arg;
    ftpserver_process(session);
    close(session->sock_ctl);
    free(session); // 记得释放动态分配的内存
    return NULL;
}

int main(int argc,char* argv[])
{
    int sock=socket_create("127.0.0.1",21);
    
    while(1)
    {
        int connf=socket_accept(sock);
        if(connf<0)
        {
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
        strcpy(session->FTP_PATH, "./source/server/ftp/");

        pthread_t tid;
        pthread_create(&tid,NULL,handler_msg,session);
    }
    close(sock);
    return 0;
}
