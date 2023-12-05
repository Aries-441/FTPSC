/*
 * @FileName: 
 * @Description: 
 * @Autor: Liujunjie/Aries-441
 * @StudentNumber: 521021911059
 * @Date: 2023-11-30 21:37:02
 * @E-mail: sjtu.liu.jj@gmail.com/sjtu.1518228705@sjtu.edu.cn
 * @LastEditTime: 2023-12-05 22:10:48
 */

#include"ftpServer.h"
#include <mysql.h>
#define AUTH "./source/server/.auth"

int ftpserver_start_pasv_data_conn(UserSession *session)
{
    int sock_pasv = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_pasv < 0)
    {
        //print_log()
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &(server_addr.sin_addr));  // Set IP address to 127.0.0.1
    server_addr.sin_port = htons(0);  // Set port number to any available port

    if (bind(sock_pasv, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        close(sock_pasv);
        //print_log()
        return -1;
    }

    if (listen(sock_pasv, 1) < 0)
    {
        close(sock_pasv);
        //print_log()
        return -1;
    }

    socklen_t len = sizeof(server_addr);
    if (getsockname(sock_pasv, (struct sockaddr*)&server_addr, &len) < 0)
    {
        close(sock_pasv);
        //print_log()
        return -1;
    }

	char response[MAXSIZE];
	int h1, h2, h3, h4;
	sscanf(inet_ntoa(server_addr.sin_addr), "%d.%d.%d.%d", &h1, &h2, &h3, &h4);
	sprintf(response, "(127,0,0,1,%d,%d)",
			ntohs(server_addr.sin_port) / 256,
			ntohs(server_addr.sin_port) % 256);

	char command[256];
	snprintf(command, sizeof(command), "echo \"%s\" > temp.txt", response);
	int ret = system(command);

	if(ret<0)
	{
		//print_log
		return -1;
	}

	int fd=open("temp.txt",O_RDONLY);
	send_response(session->sock_ctl,1);        //准备发送数据
	struct stat st;
	stat("temp.txt",&st);
	size_t size=st.st_size;

	sendfile(session->sock_data,fd,NULL,size);
	close(fd);

	send_response(session->sock_ctl,226);        //发送应答码

	printf("Server response: %s\n", response);

    return sock_pasv;
}

void ftpserver_process(UserSession *session)
{
    send_response(session->sock_ctl,220);   //发送接受处理响应码

    if(1==ftpserver_login(session) || 0)    //登录成功
    {
        send_response(session->sock_ctl,230);  
    }
    else                         //登录失败，结束本次会话
    {
        send_response(session->sock_ctl,430);  
        return;
    }

    //创建与用户名同名的子文件夹
    ftpserver_make_directory(session,session->UserName);
    strcat(session->FTP_PATH, session->UserName);
    strcat(session->FTP_PATH, "/");
	create_acl_file(session->UserName, session->FTP_PATH);

    char cmd[5];
    char arg[MAXSIZE];
    while(1)
    {
        //接收命令，解析，获取命令和参数
        int status=ftpserver_recv_cmd(session, cmd, arg);
        if((status<0)||(221==status))   //出错或者时QUIT
            break;
            
        if(200==status)     //处理
        {
			if(session->pasv_flag == 0) {
				session->sock_data=ftpserver_start_data_conn(session->sock_ctl);
			}
			else {
				session->sock_data = dup(session->sock_data_pasv);
			}
            	
				
			socket_Info(session->sock_data);

            if(session->sock_data<0)
            {
                //close(session->sock);
                printf("data链接异常中断\n");
                return;
            }

            if(strcmp(cmd,"PASV")==0)
            {
                ftpserver_pasv(session);
            }

            else if(strcmp(cmd,"LIST")==0)
            {
                ftpserver_list(session);
            }
            else if(strcmp(cmd,"RETR")==0)
            {
                ftpserver_retr(session, arg);
            }
            else if(strcmp(cmd,"PUSH")==0)
            {
                ftpserver_push(session, arg);
            }
            else if(strcmp(cmd,"DELE")==0)
            {
                ftpserver_delet(session, arg);
            }
            else if(strcmp(cmd, "RMVD") == 0) 
            {
                ftpserver_remove_directory(session, arg);
            }
            else if(strcmp(cmd, "RNAM") == 0) 
            {
                ftpserver_rename_directory(session, arg);
            }
            else if(strcmp(cmd, "MKND") == 0) 
            {
                ftpserver_make_directory(session, arg);
            }

			
			if(session->sock_data >= 0) {
				close(session->sock_data);
				session->sock_data = -1;
			}

        }
    }
}


int ftpserver_recv_cmd(UserSession *session,char* cmd,char* arg)
{
	int status=200;
	char buf[MAXSIZE];

	memset(buf,0,MAXSIZE);
	memset(cmd,0,5);
	memset(arg,0,MAXSIZE);

	if(-1==recv_data(session->sock_ctl,buf,MAXSIZE))
	{
		//print_log()
		return -1;
	}

	strncpy(cmd,buf,4);
	strcpy(arg,buf+5);

	if(strcmp(cmd,"QUIT")==0)
		status=221;
	else if((strcmp(cmd,"USER")==0)||(strcmp(cmd,"PASS")==0)||\
			(strcmp(cmd,"LIST")==0)||(strcmp(cmd,"RETR")==0)||\
			(strcmp(cmd,"PUSH")==0)||(strcmp(cmd,"PASV")==0)||\
			(strcmp(cmd,"DELE")==0)||(strcmp(cmd,"RMVD")==0)||\
			(strcmp(cmd,"MKND")==0)||(strcmp(cmd,"RNAM")==0))
		status=200;
	else{
		status=502;
		//弹出提示窗口，指导用法
	}


	send_response(session->sock_ctl,status);
	return status;
}

int ftpserver_login(UserSession *session)
{

	char buf[MAXSIZE];
	char user[MAXSIZE];
	char pass[MAXSIZE];
	memset(buf,0,MAXSIZE);
	memset(user,0,MAXSIZE);
	memset(pass,0,MAXSIZE);

	if(recv_data(session->sock_ctl,buf,sizeof(buf))<0)
	{
		//print_log()
		return -1;
	}

	int i=5;    //buf[0-4] "Name "
	int j=0;
	while(buf[i]!=0)       //将用户名保存起来
		user[j++]=buf[i++];

	strcpy(session->UserName, user);//将用户名存入全局变量用作访问控制

	send_response(session->sock_ctl,331);    //通知用户输入密码

	memset(buf,0,MAXSIZE);
	if(recv_data(session->sock_ctl,buf,sizeof(buf))<0)  //获取密码
	{
		//print_log()
		return -1;
	}

	i=5;    //buf[0-4] "pass "
	j=0;
	while(buf[i]!=0)       //将密码保存起来
		pass[j++]=buf[i++];
	printf(">>cmd>>%s,%s\n",user,pass);

	int ret=ftpserver_check_user(user,pass);    //验证用户名和密码

	return ret;
}

int ftpserver_check_user(const char* user,const char* pass )
{
    MYSQL *con = mysql_init(NULL);
    if (con == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        return -1;
    }

    if (mysql_real_connect(con, "localhost", "root", "ljj0403!", "ftpusers", 0, NULL, 0) == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT password FROM users WHERE username = '%s'", user);

    if (mysql_query(con, query)) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(con);
    if (result == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int auth = -1;
    if (row != NULL) {
        if (strcmp(pass, row[0]) == 0) {
            auth = 1;
        }
    }

    mysql_free_result(result);
    mysql_close(con);

    return auth;
}

int ftpserver_start_data_conn(int sock_ctl)
{	
	int wait;
	if(recv(sock_ctl,&wait,sizeof(wait),0)<0)
	{
		//print_log()
		return -1;
	}

	char buf[1024];
	struct sockaddr_in client_addr;
	socklen_t len=sizeof(client_addr);
	getpeername(sock_ctl,(struct sockaddr*)&client_addr,&len);
	inet_ntop(AF_INET,&client_addr.sin_addr,buf,sizeof(buf));

	int sock_data=socket_connect(buf,CLIENT_PORT);
	if(sock_data<0)
	{
		//print_log()
		return -1;
	}
	return sock_data;
}

void ftpserver_pasv(UserSession *session)
{
	session->pasv_flag = 1;
	int sock_pasv = ftpserver_start_pasv_data_conn(session);
	if (sock_pasv < 0)
	{	
		//print_log()
		return;
	}

	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	session->sock_data_pasv = accept(sock_pasv, (struct sockaddr*)&client_addr, &len);
	if (session->sock_data < 0)
	{
		
		close(sock_pasv);
		printf("sock_pasv失败!\n");
		return;
	}

	close(sock_pasv);
}

int ftpserver_list(UserSession *session)
{
	char aclfile[260];
	memset(aclfile, 0, sizeof(aclfile));
	strcpy(aclfile, session->FTP_PATH);
	strcat(aclfile, "ctl.acl");

	//检查是否具有list权限
	if(check_permissions(session->UserName, "LIST", aclfile) == 0)
	{
		send_response(session->sock_ctl, 550);//无权限
		printf("无权限\n");
		return -1;
	}

	char command[256];
	snprintf(command, sizeof(command), "ls -l %s > temp.txt", session->FTP_PATH);
	int ret = system(command);

	if(ret<0)
	{
		//print_log
		return -1;
	}

	int fd=open("temp.txt",O_RDONLY);
	send_response(session->sock_ctl,1);        //准备发送数据
	struct stat st;
	stat("temp.txt",&st);
	size_t size=st.st_size;

	sendfile(session->sock_data,fd,NULL,size);
	close(fd);

	send_response(session->sock_ctl,226);        //发送应答码
	return 0;
}

void ftpserver_retr(UserSession *session,char *filename)
{
	char name[260];
	memset(name,0,sizeof(name));
	strcpy(name,session->FTP_PATH);
	strcat(name,filename);

	//检查是否具有retr权限
	if(check_permissions(session->UserName, "RETR",	find_ctl_acl(name)) == 0)
	{
		send_response(session->sock_ctl, 550);//无权限
		printf("无权限\n");
		return -1;
	}

	int fd=open(name,O_RDONLY);
	if(fd<0)
	{
		send_response(session->sock_ctl,550);        //发送错误码
	}
	else
	{
		send_response(session->sock_ctl,150);        //发送 ok
		struct stat st;
		stat(name,&st);
		size_t size=st.st_size;
		sendfile(session->sock_data,fd,NULL,size);
		send_response(session->sock_ctl,226); //文件传输完成
		close(fd);
	}
}

void ftpserver_push(UserSession *session,char* filename)
{
	int ack;
	if(recv(session->sock_ctl,&ack,sizeof(ack),0)<0)
	{
		send_response(session->sock_ctl,502); //命令执行失败
		return;
	}
	
	int status=ntohl(ack);

	//检查是否具有push权限
	if(check_permissions(session->UserName, "PUSH",	session->FTP_PATH) == 0)
	{
		status = 553;//无权限
		printf("无权限\n");
		return -1;
	}

	if(553==status)     //客户端上传文件失败
	{
		send_response(session->sock_ctl,553); 
		return;
	}

	char name[260];
	memset(name, 0, sizeof(name));
	strcpy(name, session->FTP_PATH);
	strcat(name, filename);

	int fd=open(name,O_CREAT|O_WRONLY,0664);
	if(fd<0)
	{
		send_response(session->sock_ctl,502); //命令执行失败
		return;
	}
	
	while(1)
	{
		char data[MAXSIZE];
		memset(data,0,sizeof(data));
		ssize_t s=recv(session->sock_data,data,sizeof(data),0);
		if(s<=0)
		{
			if(s<0)
				send_response(session->sock_ctl,502); //命令执行失败
			else
				send_response(session->sock_ctl,226); //命令执行成功
			break;
		}
		write(fd,data,s);
	}
	close(fd);
}


void ftpserver_delet(UserSession *session, char *filename)
{
	char name[260];
	memset(name, 0, sizeof(name));
	strcpy(name, session->FTP_PATH);
	strcat(name, filename);

	//检查是否具有dele权限
	if(check_permissions(session->UserName, "DELE",	find_ctl_acl(name)) == 0)
	{
		send_response(session->sock_ctl, 551);//无权限
		printf("无权限\n");
		return -1;
	}

	if (remove(name) == 0) {
		send_response(session->sock_ctl,250); //文件删除成功
	} else {
		switch (errno) {
			case ENOENT:
				send_response(session->sock_ctl,550); //文件不存在
				break;
			case EACCES:
				send_response(session->sock_ctl,551); //权限不足
				break;
			case EBUSY:
				send_response(session->sock_ctl,552); //文件正在被其他进程使用
				break;
			default:
				send_response(session->sock_ctl,553); //其他错误
				break;
		}
	}
}


int ftpserver_remove_directory(UserSession *session, char *path)
{
    if((path == NULL) || (path[0] == '\0')) {
        send_response(session->sock_ctl, 551); // 551表示请求的操作权限不足，不可以删除根目录
        return -1;
    }
    
    char full_path[1024];
    memset(full_path,0,sizeof(full_path));
    strcpy(full_path,session->FTP_PATH);
    strcat(full_path,path);

	//检查是否具有remove权限
	if(check_permissions(session->UserName, "RMVD",	find_ctl_acl(full_path)) == 0)
	{
		send_response(session->sock_ctl, 551);//无权限
		printf("无权限\n");
		return -1;
	}

    DIR *d = opendir(full_path);
    size_t path_len = strlen(full_path);
    int r = -1;

    if (d)
    {
        struct dirent *p;
        r = 0;

        while (!r && (p=readdir(d)))
        {
            int r2 = -1;
            char *buf;
            size_t len;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
            {
                continue;
            }

            len = path_len + strlen(p->d_name) + 2; 
            buf = malloc(len);

			if (buf)
			{
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", full_path, p->d_name);

				if (!stat(buf, &statbuf))
				{
					if (S_ISDIR(statbuf.st_mode))
					{
						char relative_path[1024];
						snprintf(relative_path, sizeof(relative_path), "%s/%s", path, p->d_name);
						r2 = ftpserver_remove_directory(session, relative_path); // 将 ftpfilespath 传入
					}
					else
					{
						r2 = unlink(buf);
					}
				}

				free(buf);
			}

            r = r2;
        }

        closedir(d);
    }

    if (!r)
    {
        r = rmdir(full_path);
        if (r != 0) {
            send_response(session->sock_ctl, 550); // 550表示请求的操作未执行，文件不可用（例如，文件未找到，未知路径）
        } else {
            send_response(session->sock_ctl, 250); // 250表示请求的文件操作正确，已完成
        }
    }
    else {
        send_response(session->sock_ctl, 550); // 550表示请求的操作未执行，文件不可用（例如，文件未找到，未知路径）
    }
    return r;
}


void ftpserver_rename_directory(UserSession *session, char *arg) 
{
    char *oldname = strtok(arg, " ");
    char *newname = strtok(NULL, " ");

    if(oldname == NULL || oldname[0] == '\0' || newname == NULL || newname[0] == '\0') 
	{
        send_response(session->sock_ctl, 500); // 500 Syntax error, command unrecognized
        return;
    }

    char full_path_old[1024];
    memset(full_path_old,0,sizeof(full_path_old));
    strcpy(full_path_old,session->FTP_PATH);
    strcat(full_path_old,oldname);

    char full_path_new[1024];
    memset(full_path_new,0,sizeof(full_path_new));
    strcpy(full_path_new,session->FTP_PATH);
    strcat(full_path_new,newname);

	//检查是否具有rename权限
	if(check_permissions(session->UserName, "RNAM",	find_ctl_acl(full_path_new)) == 0 &&\
		check_permissions(session->UserName, "RNAM",	find_ctl_acl(full_path_old)) == 0)
	{
		send_response(session->sock_ctl, 551);//无权限
		printf("无权限\n");
		return -1;
	}

    int rename_result = rename(full_path_old, full_path_new);
    if (rename_result == 0) 
	{
        send_response(session->sock_ctl, 250); // 250 Requested file action okay, completed
    } 
	else 
	{
        send_response(session->sock_ctl, 550); // 550 Requested action not taken. File unavailable
    }
}

void ftpserver_make_directory(UserSession *session,char *path)
{
    char dir_path[512];
    snprintf(dir_path, sizeof(dir_path), "%s%s", session->FTP_PATH, path);

	//检查是否具有MKND权限
	if(check_permissions(session->UserName, "MKND",	find_ctl_acl(dir_path)) == 0)
	{
		send_response(session->sock_ctl, 551);//无权限
		printf("无权限\n");
		return -1;
	}

    // Try to create the directory
    if(mkdir(dir_path, 0755) == -1) 
	{
        // Send error code to client
        // 550 Requested action not taken. File unavailable (e.g., file not found, no access).
        send_response(session->sock_ctl, 550);
    } 
	else 
	{
        // Send success code to client
        // 257 "PATHNAME" created.
        send_response(session->sock_ctl, 257);
    }
}

void create_acl_file(char *UserName, char *path) {
    // 创建完整文件路径
    char filepath[MAXSIZE];
    snprintf(filepath, sizeof(filepath), "%s/ctl.acl", path);

    // 打开文件
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // 写入内容
    const char *commands[] = {"LIST", "RETR", "PUSH", "DELE", "RMVD", "MKND", "RNAM"};
    size_t num_commands = sizeof(commands) / sizeof(commands[0]);
    for (size_t i = 0; i < num_commands; i++) {
        fprintf(file, "%s:\n%s,\n", commands[i], UserName);
    }

    // 关闭文件
    fclose(file);
}

int check_permissions(char *UserName, char *PermissionType, char *aclFilePath) {
    FILE *file = fopen(aclFilePath, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", aclFilePath);
        return 0; // Could not open file
    }

    char line[MAXSIZE];
	printf("username:%s\n",UserName);

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        // Check if line contains permission type
        char *currentPermissionType = strtok(line, ":");

        if (strcmp(PermissionType, currentPermissionType) == 0)
        {	
			while (fgets(line, sizeof(line), file)) {
				// Remove newline character
				line[strcspn(line, "\n")] = 0;
				// Check if the last character is ':'
				if (line[strlen(line) - 2] == ':') 
				{
					break;
				}
				char *permissionList = strtok(line, "\n,");
				printf("permissionList:%s\n",permissionList);
				if(user_has_permission(UserName, permissionList))
				{
					fclose(file);
					return 1;
				}
				
			}
			return 0;
        }
    }

    fclose(file);
    return 0; // User does not have permission
}

int user_has_permission(char *UserName, char *PermissionList) {
    char *token = strtok(PermissionList, ",");
    while (token != NULL) {
        if (strcmp(UserName, token) == 0) {
            return 1; // User has permission
        }
        token = strtok(NULL, ",");
    }
    return 0; // User does not have permission
}

char* find_ctl_acl(const char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        return NULL; // 打开目录失败
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; // 跳过当前目录和父目录项
        }

        // 构造当前项的绝对路径
        char absolutePath[PATH_MAX];
        snprintf(absolutePath, sizeof(absolutePath), "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            // 递归搜索子目录
            char* result = find_ctl_acl(absolutePath);
            if (result != NULL) {
                closedir(dir);
                return result; // 在子目录中找到了 "ctl.acl" 文件
            }
        } else if (entry->d_type == DT_REG && strcmp(entry->d_name, "ctl.acl") == 0) {
            // 在当前目录中找到了 "ctl.acl" 文件
            closedir(dir);
            return strdup(absolutePath);
        }
    }

    closedir(dir);
    return NULL; // 在当前目录及其子目录中未找到 "ctl.acl" 文件
}