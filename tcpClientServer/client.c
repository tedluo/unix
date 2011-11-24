//-----------------------------------------------------------
//A simple TCP Client,Send a request to remote server and get
//the response message.
//Usage:./tcp <remote address> <port>
//-----------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>

#include <unistd.h>
#include <sys/socket.h>

#include <arpa/inet.h>


#define BUFFER_SIZE 	1024

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		fprintf(stderr,"Usage:%s <remote addr> <port> \n",argv[0]);
		exit(-1);
	}

	int fd;
	unsigned int port;
	//long int strtol(const char* nptr,char **endptr,int base);
	port = (unsigned short)strtol(argv[2],NULL,10);
	//Create socket
	//int socket(int domain,int type,int protocol);
	if((fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		fprintf(stderr,"Failed to create socket:%s\n",strerror(errno));
		exit(-1);
	}
	fprintf(stdout,"Create a new TCP socket fd = %d\n",fd);
	//for client bind is optional
	//connect
	struct sockaddr_in remote_ipv4_address;
#if 0
	struct sockaddr_in
	{
		sa_family sin_family;
		in_port_t sin_port;
		struct in_addr sin_addr;
		unsigned char sin_zero[8];
	}
#endif
	memset(&remote_ipv4_address,0,sizeof(remote_ipv4_address));
	remote_ipv4_address.sin_family = AF_INET;
	remote_ipv4_address.sin_port = htons(port);
	inet_pton(AF_INET,argv[1],&remote_ipv4_address.sin_addr);

	if((connect(fd,(struct sockaddr *)&(remote_ipv4_address),sizeof(remote_ipv4_address))) < 0)
	{
		fprintf(stderr,"Connect to remote server %s:%d failed:%s\n",argv[1],port,strerror(errno));
		close(fd);
		exit(-1);
	}
	fprintf(stdout,"Connect to server sucess\n");

	//send request  to server
	char http_request[] = "GET /\n\n";
	ssize_t n;
	int required = strlen(http_request);

	int total = 0;

	while(total < required)
	{
		if((n = write(fd,http_request+total,required-total)) < 0)
		{
			if(errno == EINTR)
				continue;
			else
			{
				fprintf(stderr,"Send request to server failed:%s\n",strerror(errno));
				break;
			}
		}
		else
		{
			total += n; 
		}
	}

	//wait for message from server
	char buffer[BUFFER_SIZE];

	for(;;)
	{
		if((n = read(fd,buffer,sizeof(buffer) -1)) < 0)
		{
			if(errno == EINTR)
				continue;
			else
			{
				fprintf(stderr,"Read from peer server %s:%d failed:%s\n",argv[1],port,strerror(errno));
				close(fd);
				break;
			}
		}
		else if(n == 0)
		{
			fprintf(stdout,"Server closed by remote server\n");
			close(fd);
			break;
		}
		else
		{
			buffer[n] = '\0';
			fprintf(stdout,"Received %d bytes from remote server:%s\n",n,buffer);

		}
	}
	close(fd);
	return 0;
}
