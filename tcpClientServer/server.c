//-----------------------------------------------------------------
//This is a simple test for TCP server,while get some message from
//the client,the server will send a string to respone the client
//Usage:server <local address> <port> <listen_blacklog>
//-----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>

#include <sys/types.h>
#include <arpa/inet.h>

#include <errno.h>

#define	BUFFER_SIZE 	1024

int main(int argc,char **argv)
{
	if(argc <  3)
	{
		fprintf(stdout,"Usage:./server <local address> <port> <listen_backlog>\n");
		exit(-1);
	}

	int listening_socket;
	unsigned short port;
	int listen_backlog;
	
	//---------------------------------------------------------------
	//Parse command line arguments
	//FIXME:use getopt()/getopt_long() to parse command line arguments
	//---------------------------------------------------------------
	
	port = (unsigned short)strtol(argv[2],NULL,10);
	listen_backlog = (int)strtol(argv[3],NULL,10);

	//Create socket
	if((listening_socket = socket(PF_INET,SOCK_STREAM,0)) < 0)
	{
		fprintf(stderr,"Failed to create new TCP socket:%s\n",strerror(errno));
		exit(-1);
	}

	//---------------------------------------------------------------
	//FIXME: set sSO_REUSERADDR & SO_REUSEPORT options
	//---------------------------------------------------------------
	int optval;

	optval = 1;
	setsockopt(listening_socket,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));

#ifdef SO_REUSEPORT
	optval = 1;
	setsockopt(listening_socket,SOL_SOCKET,SO_REUSERPORT,&optval,sizeof(optval));
#endif

	//Initialize struct  sockaddr_in
	struct sockaddr_in local_ipv4_address;

	memset(&local_ipv4_address,0,sizeof(local_ipv4_address));
	local_ipv4_address.sin_family = AF_INET;
	local_ipv4_address.sin_port = htons(port);
	inet_pton(AF_INET,argv[1],&local_ipv4_address.sin_addr);
	
	//bind the socket to local addr
	if((bind(listening_socket,(struct sockaddr *)&local_ipv4_address,sizeof(local_ipv4_address))) < 0)
	{
		fprintf(stderr,"Bind to %s:%d failed :%s\n",argv[1],port,strerror(errno));
		close(listening_socket);
		exit(-1);
	}

	fprintf(stdout,"Bind to %s:%d successfully\n",argv[1],port);

	//liseten
	if(listen(listening_socket,listen_backlog) < 0)
	{
		fprintf(stderr,"Listen on %s:%d failed:%s\n",argv[1],port,strerror(errno));
		close(listening_socket);
		exit(-1);
	}

	//Do accept
	int new_connect_socket;
	struct sockaddr_in peer_ipv4_address;

	socklen_t peer_ipv4_address_length;
	char peer_ipv4_address_string[] = "ddd.ddd.ddd.ddd\0";

	for(;;)
	{
		peer_ipv4_address_length = sizeof(peer_ipv4_address);

		if((new_connect_socket = accept(listening_socket,(struct sockaddr *)&peer_ipv4_address,&peer_ipv4_address_length)) < 0)
		{
			fprintf(stderr,"Accept failed:%s\n",strerror(errno));
			break;
		}
		else
		{
			//if accept success
			memset(&peer_ipv4_address,0,sizeof(peer_ipv4_address));
			inet_ntop(AF_INET,&peer_ipv4_address.sin_addr,peer_ipv4_address_string,sizeof(peer_ipv4_address_string));

			fprintf(stdout,"Accepted a new connection %d,form  %s:%d\n",new_connect_socket,peer_ipv4_address_string,ntohs(peer_ipv4_address.sin_port));

			//read/write on new_connect_socket.
			char receive[BUFFER_SIZE];
			char send[] = "hello,i'm got the message.!";
			
			ssize_t write_length = strlen(send);
			ssize_t read_bytes;
			ssize_t write_bytes;
			//FIXME:This may cause some problems in read and write,more code according
			//to the realization.
again:	
			if((read_bytes = read(new_connect_socket,receive,sizeof(receive))) < 0)
			{
				if(errno == EINTR)
					goto again;
				else
				{
					fprintf(stderr,"Get messege from client failed:%s\n",strerror(errno));
					close(new_connect_socket);
					break;
				}
			}
			int total = 0;
			while(total < read_bytes)
			{
				if((write_bytes = write(new_connect_socket,send+total,write_length-total)) < 0)
				{
					if(errno == EINTR)
						continue;
					else
					{
						fprintf(stderr,"write new_connect_socket failed:%s\n",strerror(errno));
						break;
					}
				}
				else
				{
					fprintf(stdout,"write %d bytes to new connect socket\n",write_bytes);
					if(total == read_bytes)
						break;
				}
			}
			close(new_connect_socket);
		}
			
	}
	close(listening_socket);
	return 0;
}
