#include"common.h"

int socket_create(const char* ip,const int port)  //创建一个监听套接字
{
	if((NULL==ip) || (port<0))
	{
		//print_log()
		return -1;
	}

	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		//print_log();
		return -1;
	}

	int op=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&op,sizeof(op));
	
	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(port);
	local.sin_addr.s_addr=inet_addr(ip);

	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		//print_log();
		return -1;
	}

	if(listen(sock,5)<0)
	{
		//print_log();
		return -1;
	}
	return sock;
}


int socket_accept(int sock)     //接受连接
{
	struct sockaddr_in peer;
	socklen_t len=sizeof(peer);

	int connfd=accept(sock,(struct sockaddr*)&peer,&len);
	if(connfd<0)
	{
		//print_log()
		return -1;
	}
	return connfd;
}

int socket_connect(const char* ip,const int port)   //连接远端主机
{
	if((NULL==ip) || (port<0))
	{
		//print_log()
		return -1;
	}
	
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		//print_log()
		return -1;
	}

	struct sockaddr_in peer;
	peer.sin_family=AF_INET;
	peer.sin_port=htons(port);
	peer.sin_addr.s_addr=inet_addr(ip);
	
	if(connect(sock,(struct sockaddr*)&peer,sizeof(peer))<0)
	{
		//print_log()
		return -1;
	}
	return sock;
}

int recv_data(int sock,char* buf,int bufsize)  //从sock读取数据
{
	memset(buf,0,bufsize);
	ssize_t s=recv(sock,buf,bufsize,0);
	if(s<=0)
		return -1;
	return s;
}

int send_response(int sock,int code)   //发送响应码到sock
{
	int stat_code=htonl(code);
	if(send(sock,&stat_code,sizeof(stat_code),0)<0)
	{
		//print_log()
		return -1;
	}
	return 0;
}

void read_input(char *buf,int buffsize)  //从标准输入读取一行
{
	memset(buf,0,buffsize);
	if(NULL!=fgets(buf,buffsize,stdin))
	{
		char *n=strchr(buf,'\n');
		if(n)
			*n='\0';
	}
}

void trimstr(char* str,int n)   //去除字符串中的空白和换行
{
	int i=0;
	for(i=0;i<n;i++)
	{
		if(isspace(str[i])||('\0'==str[i]))
			str[i]=0;
	}
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

int check_permissions(char *UserName, char *PermissionType, char *aclFilePath) {
    FILE *file = fopen(aclFilePath, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", aclFilePath);
        return 0; // Could not open file
    }

    char line[MAXSIZE];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character
        line[strcspn(line, "\n")] = 0;

        // Check if line contains permission type
        char *currentPermissionType = strtok(line, ":");
        char *permissionList = strtok(NULL, ":");

        if (strcmp(PermissionType, currentPermissionType) == 0 &&
            user_has_permission(UserName, permissionList)) {
            fclose(file);
            return 1; // User has permission
        }
    }

    fclose(file);
    return 0; // User does not have permission
}









