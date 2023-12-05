#include"ftpClient.h"
#include"../common/common.h"

void Usage(const char *filename)
{
	printf("Usage: %s servet_ip\n",filename);
}

int main(int argc,char* argv[])
{
	if(argc!=2)
	{
		Usage(argv[0]);
		return -1;
	}
	
	int pasv_flag = 0; //是否使用过pasv指令
	int sock_data_pasv = 0;
	int sock_data;  // 声明数据连接套接字变量

	int sock_ctl=socket_connect(argv[1],21);//默认使用21端口
	if(sock_ctl<0)
	{
		printf("connected failed,\n");
		return -1;
	}

	printf("Connected  to %s.\n",argv[1]);
	print_reply(read_reply(sock_ctl));
	
	if(ftpclient_login(sock_ctl)<0)
	{
		return -1;
	}

	while(1)
	{
		char buf[MAXSIZE];
		memset(buf,0,sizeof(buf));
		struct command cmd;
		memset(&cmd,0,sizeof(cmd));
		if(ftpclient_read_cmd(buf,sizeof(buf),&cmd)<0)
		{
			printf("Invalid command\n");
			continue;
		}
		
		if(send(sock_ctl,buf,strlen(buf),0)<0)  //发送命令到服务器
		{
			printf("send cmd failed!");
			break;
		}
	
		int status=read_reply(sock_ctl);    //读取服务器的返回码

		if(status==221)     //退出命令
		{
			print_reply(221);      //提示用户退出
			break;
		}

		if(status==502)      //不合法的命令
		{
			printf("%d Invalid commond.\n",status);
		}

		else        //status is 200
		{
			printf("status 200\n");



			if (pasv_flag == 0) {
				sock_data = ftpclient_open_conn(sock_ctl);  // 创建一个数据连接
			} else {
				printf("sock_data_pasv:%d\n",sock_data_pasv);
				socket_Info(sock_data_pasv);
				printf("\n");
				sock_data = dup(sock_data_pasv);
			}

			if(sock_data<0)
			{
				perror("Error open data connect failed\n");
				break;
			}
			
			if(strcmp(cmd.code,"LIST")==0)      //list命令
			{
				printf("sock_data:%d\n",sock_data);
				socket_Info(sock_data);
				ftpclient_list(sock_ctl,sock_data);

				int reply = read_reply(sock_ctl);

				if(reply == 200)  // Command is valid
				{
					if(read_reply(sock_ctl)==550)  //判断服务器端文件正常
					{
						print_reply(550);    //打印550回复
						close(sock_data);
						continue;
					}
				}

				if(reply == 550)  //判断服务器端文件正常
				{
					print_reply(550);    //打印550回复
					close(sock_data);
					continue;
				}
				close(sock_data);      
			}

			else if(strcmp(cmd.code,"RETR")==0)  //retr命令
			{
			
				if(read_reply(sock_ctl)==550)  //判断服务器端文件正常
				{
					print_reply(550);    //打印550回复
					close(sock_data);
					continue;
				}
				ftpclient_get(sock_data,cmd.arg);         //下载文件
				print_reply(read_reply(sock_ctl));    
				close(sock_data);      
			}
			else if(strcmp(cmd.code,"PUSH")==0)
			{
				if(ftpclient_put(sock_data,cmd.arg)<0)   //告诉服务器文件上传失败
					send_response(sock_ctl,553);
				else									  //告诉服务器文件已经上传
					send_response(sock_ctl,200);

				close(sock_data);      
				print_reply(read_reply(sock_ctl));    //打印服务器端的响应
			}

			else if(strcmp(cmd.code,"PASV")==0)
			{
				pasv_flag = 1;
				char response[MAXSIZE];
				memset(response,0,1024);

				ftpclient_pasv(sock_ctl,sock_data,response);

				int data_sock = ftpclient_start_pasv_data_conn(response);
				printf("data_sock:%d\n",data_sock);
				socket_Info(data_sock);
				printf("sock_data:%d\n",sock_data);
				socket_Info(sock_data);

				if(data_sock > 0){
					sock_data_pasv = dup(data_sock);
					close(data_sock);
				}
					
			}
			
			else if(strcmp(cmd.code,"DELE")==0) //RMD删除文件夹命令
			{
				int reply = read_reply(sock_ctl);
				if(reply == 200)  // Command is valid
				{
					reply = read_reply(sock_ctl);  // Wait for the second reply
					printf("reply:%d\n",reply);
				}
				if(reply == 250) 
				{
					printf("Successfully deleted file %s\n", cmd.arg);
				} 
				else if(reply == 550) 
				{
					printf("File %s does not exist \n", cmd.arg);
				}
				else if(reply == 551) 
				{
					printf("551 Requested action aborted: Insufficient Permission\n");
					printf("You don't have permission to delete the file %s\n", cmd.arg);
				}
				else if(reply == 552)
				{
					printf("The file %s is being occupied by another process\n", cmd.arg);
				}
				else 
				{
					printf("Other errors!\n");
				}
			}

			else if(strcmp(cmd.code,"RMVD")==0) //RMD删除文件夹命令
			{
				int reply = read_reply(sock_ctl);
				if(reply == 200)  // Command is valid
				{
					reply = read_reply(sock_ctl);  // Wait for the second reply
					printf("reply:%d\n",reply);
				}
				if(reply == 250) 
				{
					printf("Successfully removed directory %s\n", cmd.arg);
				} 
				else if(reply == 550) 
				{
					printf("Error removing directory %s\n", cmd.arg);
				}
				else if(reply == 551) 
				{
					printf("551 Requested action aborted: Insufficient Permission\n");
				}
			}

			else if(strcmp(cmd.code,"RNAM")==0) //RNAM重命名文件 or 目录
			{
				int reply = read_reply(sock_ctl);
				if(reply == 200)  // Command is valid
				{
					reply = read_reply(sock_ctl);  // Wait for the second reply
					printf("reply:%d\n",reply);
				}
				if(reply == 250) 
				{
					printf("250 Requested file action rename okay, completed\n");
				} 
				else if(reply == 550) 
				{
					printf("550 Requested action not taken. File unavailable\n");
				}
				else if(reply == 500) 
				{
					printf("500 Syntax error, command unrecognized\n");
				}
				else if(reply == 551) 
				{
					printf("551 Requested action aborted: Insufficient Permission\n");
				}
			}


			else if(strcmp(cmd.code,"MKND")==0) //创建新的目录或者是文件
			{
				int reply = read_reply(sock_ctl);
				if(reply == 200)  // Command is valid
				{
					reply = read_reply(sock_ctl);  // Wait for the second reply
					printf("reply:%d\n",reply);
				}
				if(reply == 257) 
				{
					printf("257 PATHNAME created.\n");
				} 
				else if(reply == 551) 
				{
					printf("551 Requested action aborted: Insufficient Permission\n");
				}
				else 
				{
					printf("550 Requested action not taken. File unavailable\n");
				}
			}
		}

		if(sock_data > 0)
			close(sock_data);
	}
	close(sock_ctl);
	return 0;
}