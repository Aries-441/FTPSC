/*
 * @FileName: 
 * @Description: 
 * @Autor: Liujunjie/Aries-441
 * @StudentNumber: 521021911059
 * @Date: 2023-11-30 21:37:10
 * @E-mail: sjtu.liu.jj@gmail.com/sjtu.1518228705@sjtu.edu.cn
 * @LastEditTime: 2023-12-27 22:39:56
 */
#include"ftpClient.h"
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>

int read_reply(int sock_ctl)       //读取服务器的回复
{

	int status=0;
	if(recv(sock_ctl,&status,sizeof(status),0)<0)
	{
		printf("client:error reading message from server.\n");
		return -1;
	}
	return ntohl(status);
}

void print_reply(int status)     //打印回复信息
{
	switch(status)
	{
	case 220:
		printf("220 welcome,server ready.\n");
		break;
	case 221:
		printf("221 bye!\n");
		break;
	case 226:
		printf("226 close data connection. requested fileaction successful.\n");
		break;
	case 502:
		printf("502 command execute failed.\n");
		break;
	case 550:
		printf("550 request action not taken.file unavailable.\n");
		break;
	case 553:
		printf("553 Could not file.\n");
		break;
	default:
		printf("unknown error.\n");
		break;
	}
}

int ftpclient_read_cmd(char* buf,size_t size,struct command *cmd) //读取客户端输入的命令
{
	memset(cmd->code,0,sizeof(cmd->code));
	memset(cmd->arg,0,sizeof(cmd->arg));
	printf("\n\n");
	printf("ftpclient> ");  //输入提示符
	fflush(stdout);
	read_input(buf,size);
	char *arg=NULL;
	arg=strtok(buf," ");     //将读取的内容以空格分隔

	if(arg != NULL) {
		arg = strtok(NULL, "");  //获取所有参数
		if(arg != NULL) {
			strncpy(cmd->arg, arg, sizeof(cmd->arg)-1); //将所有参数复制到cmd->arg中
		}
	}

	if((strncmp(buf,"ls",2)==0) || strncmp(buf,"list",4)==0 || strncmp(buf,"LIST",4)==0)
		strcpy(cmd->code,"LIST");
	else if((strncmp(buf,"get",3)==0) || strncmp(buf,"retr",4)==0 || strncmp(buf,"RETR",4)==0)
		strcpy(cmd->code,"RETR");
	else if((strncmp(buf,"bye",3)==0) || (strncmp(buf,"exit",4)==0) || (strncmp(buf,"quit",4)==0))
		strcpy(cmd->code,"QUIT");
	else if((strncmp(buf,"put",3)==0) || (strncmp(buf,"push",4)==0) || (strncmp(buf,"PUSH",4)==0))
		strcpy(cmd->code,"PUSH");
	else if((strncmp(buf,"pasv",4)==0) || (strncmp(buf,"PASV",4)==0))
		strcpy(cmd->code,"PASV");
	else if((strncmp(buf,"DELE",4)==0) || (strncmp(buf,"dele",4)==0))
		strcpy(cmd->code,"DELE");
	else if((strncmp(buf,"rmd",3)==0) || (strncmp(buf,"RMD",3)==0))
		strcpy(cmd->code,"RMVD");
	else if((strncmp(buf,"rnam",4)==0) || (strncmp(buf,"RNAM",4)==0))
		strcpy(cmd->code,"RNAM");
	else if((strncmp(buf,"MKND",4)==0) || (strncmp(buf,"mknd",4)==0))
		strcpy(cmd->code,"MKND");
	else
		return -1;

	//将这个命令再存到buf中
	memset(buf,0,size);
	strcpy(buf,cmd->code);
	if(strlen(cmd->arg) > 0)        //如果命令带参数的话，追加到命令后面
	{	
		strcat(buf," ");
		strncat(buf,cmd->arg,strlen(cmd->arg));
	}	
	return 0;
}


int ftpclient_put(int sock_data,char *filename)
{
	int fd=open(filename,O_RDONLY);  
	if(fd<0)
		return -1;

	struct stat st;
	if(stat(filename,&st)<0)
		return -1;

	sendfile(sock_data,fd,NULL,st.st_size);
	close(fd);
	return 0;
}


int ftpclient_get(int sock_data, char* filename) {
    char newFilename[4096];
    snprintf(newFilename, sizeof(newFilename), "./ftpdownloads/%s", filename);
    int fd = open(newFilename, O_CREAT | O_WRONLY, 0664);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    while (1) {
        char data[MAXSIZE];
        memset(data, 0, sizeof(data));
        ssize_t s = recv(sock_data, data, sizeof(data), 0);
        if (s <= 0) {
            if (s < 0)
                perror("error");
            break;
        }
        write(fd, data, s);
    }

    close(fd);
    return 0;
}

int ftpclient_open_conn(int sock_ctl)   //打开数据连接
{
	int sock_listen=socket_create("0.0.0.0",CLIENT_PORT);   //创建一个数据连接
	int ack=1;
	//给服务器发送一个确认，告诉服务器客户端创建好了一条数据链路
	//printf("ftpclient_open_conn:sock_ctl:");
	//socket_Info(sock_ctl);
	if(send(sock_ctl,(char*)&ack,sizeof(ack),0)<0) 
	{
		printf("client:ack write error:%d\n",errno);
		return -1;
	}
	int sock_data=socket_accept(sock_listen);
	//socket_Info(sock_data);
	close(sock_listen);
	return sock_data;     
}

int ftpclient_list(int sock_ctl,int sock_data) //处理list命令
{
	if (setsockopt(sock_ctl, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) 
	{
		perror("Error setting socket options");
		return -1;
	}
	int tmp=0;
	
	if(recv(sock_ctl,&tmp,sizeof(tmp),0)<0)    //等待服务器连接成功发送过来一个信号
	{
		perror("client: action list error reading message from server.\n");
		return -1;
	}

	while(1)      //接收服务器发送过来的数据
	{
		char buf[MAXSIZE];
		memset(buf,0,sizeof(buf));
		ssize_t s=recv(sock_data,buf,MAXSIZE,0);
		if(s<=0)
		{
			if(s<0)
				perror("error");
			break;
		}
		printf("%s",buf);
	}

	return 0;
}

int ftpclient_pasv(int sock_ctl,int sock_data,char *response) //处理pasv命令
{
	if (setsockopt(sock_ctl, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) 
	{
		perror("Error setting socket options");
		return -1;
	}
	int tmp=0;
	if(recv(sock_ctl,&tmp,sizeof(tmp),0)<0)    //等待服务器连接成功发送过来一个信号
	{
		perror("client: action list error reading message from server.\n");
		return -1;
	}
	memset(response,0,sizeof(response));
	ssize_t s=recv(sock_data,response,MAXSIZE,0);
	return 0;
}

int ftpclient_send_cmd(int sock_ctl,struct command* cmd)     //发送命令
{
	char buf[MAXSIZE];
	sprintf(buf,"%s %s",cmd->code,cmd->arg);
	if(send(sock_ctl,buf,strlen(buf),0)<0)
	{
		perror("error sending command to server.\n");
		return -1;
	}
	return 0;
}


int ftpclient_send_pasv(int sock_ctl, char* response, int response_size)
{   
	printf("send pasv");
	
	struct command cmd_instance;  // 创建一个command结构体实例
	struct command *cmd = &cmd_instance;  

	memset(cmd->code,0,sizeof(cmd->code));
	memset(cmd->arg,0,sizeof(cmd->arg));

	strcpy(cmd->code, "PASV");


	char buf[MAXSIZE];

	sprintf(buf,"%s",cmd->code);


	if(send(sock_ctl,buf,strlen(buf),0)<0)
    {
        printf("pasv failed");
        return -1;
    }

    if (recv_data(sock_ctl, response, response_size) < 0)
    {
        printf("pasv failed");
        return -1;
    }

    return 0;
}


int ftpclient_start_pasv_data_conn(char* response)
{
    int h1, h2, h3, h4, p1, p2;
	int matched = sscanf(response, "(%d,%d,%d,%d,%d,%d)", &h1, &h2, &h3, &h4, &p1, &p2);
	if (matched != 6)
	{
		printf("parse response fail, sscanf matched %d items\n", matched);
		return -1;
	}
    char ip[16];
    sprintf(ip, "%d.%d.%d.%d", h1, h2, h3, h4);
    int port = p1 * 256 + p2;

    int sock_data = socket_connect(ip, port);  // Assume socket_connect is a function that connects to the server and returns a socket.
    if (sock_data < 0)
    {
        printf("connect fail");
        return -1;
    }
    return sock_data;
}


int ftpclient_login(int sock_ctl)
{
	struct command cmd;
	char user[256];
	memset(user,0,sizeof(user));

	char response[MAXSIZE];
	memset(response,0,sizeof(response));

	printf("Name: ");
	fflush(stdout);
	read_input(user,256);   //获取用户名

	strcpy(cmd.code,"USER");
	strcpy(cmd.arg,user);
	ftpclient_send_cmd(sock_ctl,&cmd);     //发送用户名到服务器
	
	int wait=0;
	recv(sock_ctl,&wait,sizeof(wait),0);      //接受应答码 331

	fflush(stdout);
	char* pass=getpass("Password: ");      //使用getpass函数获取密码

	// 使用 OpenSSL 库的 SHA256 函数生成密码的哈希值
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256((unsigned char*)pass, strlen(pass), hash);
	char* output = base64_encode(hash, SHA256_DIGEST_LENGTH);
	strcpy(cmd.code,"PASS");
	strcpy(cmd.arg, output);
	ftpclient_send_cmd(sock_ctl,&cmd);   //发送密码到服务器
	int retcode = read_reply(sock_ctl);   //读取服务器的回应

	switch(retcode)
	{
	case 430:
		printf("Invalid username/password.\n");
		return -1;

	case 230:
		printf("Successful login.\n");      //登录成功
		break;
		
	default:
		printf("error reading message from server.\n");
		return -1;
	}
	return 0;
}
