#include"ftpServer.h"
#include<pthread.h>

char UserName[MAXSIZE];
char FTP_PATH[MAXSIZE] = "./source/server/ftp/";

/*
void Usage(const char *filename)
{
	printf("Usage: %s servet_ip server_port\n",filename);
}
*/

void* handler_msg(void *arg)
{
	pthread_detach(pthread_self());
	int sock=(int)arg;
	ftpserver_process(sock);
	close(sock);
	return NULL;
}

int main(int argc,char* argv[])
{
	/*
	if(argc!=3)
	{
		Usage(argv[0]);
		return -1;
	}
	*/

	int sock=socket_create("127.0.0.1",21);
	
	while(1)
	{
		int connf=socket_accept(sock);
		if(connf<0)
		{
			continue;
		}
		pthread_t tid;
		pthread_create(&tid,NULL,handler_msg,(void*)connf);
	}
	close(sock);
	return 0;
}





