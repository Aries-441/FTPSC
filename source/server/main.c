/*
 * @FileName: 
 * @Description: 
 * @Autor: Liujunjie/Aries-441
 * @StudentNumber: 521021911059
 * @Date: 2023-11-30 21:37:02
 * @E-mail: sjtu.liu.jj@gmail.com/sjtu.1518228705@sjtu.edu.cn
 * @LastEditTime: 2023-12-18 16:47:22
 */
#include "ftpServer.h"
#include <sha.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

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

    // 添加处理添加用户的逻辑
    if (argc >= 3 && strcmp(argv[1], "-adduser") == 0) {
        char* username_password = argv[2];
        char* username = strtok(username_password, ":");
        char* password = strtok(NULL, ":");
        
        // 使用 OpenSSL 库的 SHA256 函数生成密码的哈希值
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)password, strlen(password), hash);
        char* output = base64_encode(hash, SHA256_DIGEST_LENGTH);
        // 将用户名和密码哈希值写入到 MySQL 数据库中
        MYSQL* mysql_conn = mysql_init(NULL);
        if (!mysql_real_connect(mysql_conn, "localhost", "root", "******", "ftpusers", 0, NULL, 0)) {
            fprintf(stderr, "Failed to connect to MySQL database: %s\n", mysql_error(mysql_conn));
            return 1;
        }

        char query[256];
        snprintf(query, sizeof(query), "INSERT INTO users (username, password) VALUES ('%s', '%s')", username, output);
        if (mysql_real_query(mysql_conn, query, strlen(query)) != 0) {
            fprintf(stderr, "Failed to insert user into database: %s\n", mysql_error(mysql_conn));
            mysql_close(mysql_conn);
            return 1;
        }

        mysql_close(mysql_conn);
        printf("User added successfully.\n");
        return 0;
    }

    int sock = socket_create("0.0.0.0", 21);

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