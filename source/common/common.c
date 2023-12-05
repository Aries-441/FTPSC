/*
 * @FileName: 
 * @Description: 
 * @Autor: Liujunjie/Aries-441
 * @StudentNumber: 521021911059
 * @Date: 2023-11-30 21:37:02
 * @E-mail: sjtu.liu.jj@gmail.com/sjtu.1518228705@sjtu.edu.cn
 * @LastEditTime: 2023-12-05 14:26:38
 */
#include"common.h"

struct timeval timeout = {5, 1000};  // 3秒和10微秒


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

// 打印套接字连接的本地地址和远程地址
void socket_Info(int sockfd) 
{
    struct sockaddr_in local_addr, peer_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    // 获取本地地址
    if (getsockname(sockfd, (struct sockaddr*)&local_addr, &addr_len) == 0) 
	{
        char local_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(local_addr.sin_addr), local_ip, INET_ADDRSTRLEN);
        int local_port = ntohs(local_addr.sin_port);
        printf("Local address: %s:%d\n", local_ip, local_port);
    }

    // 获取远程地址
    if (getpeername(sockfd, (struct sockaddr*)&peer_addr, &addr_len) == 0) 
	{
        char peer_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(peer_addr.sin_addr), peer_ip, INET_ADDRSTRLEN);
        int peer_port = ntohs(peer_addr.sin_port);
        printf("Remote address: %s:%d\n", peer_ip, peer_port);
    }
}


char* base64_encode(const unsigned char* input, int length) {
    BIO *bmem, *b64;
    char *buff;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // Ignore newlines - write everything in one line
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);

    // Get the length of the data in the BIO
    int mem_data_len = BIO_get_mem_data(b64, &buff);

    // Allocate memory for the encoded data
    char* encoded_data = (char*)malloc(mem_data_len + 1);
    if (encoded_data) {
        memcpy(encoded_data, buff, mem_data_len);
        encoded_data[mem_data_len] = '\0'; // Null-terminate the string
    }

    BIO_free_all(b64);

    return encoded_data;
}