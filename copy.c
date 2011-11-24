/*This function copy a file to another file*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//for open()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//for strerror()
#include <errno.h>

//for close read write
#include <unistd.h>

#define	BUFFER_SIZE	1024

int main(int argc,char **argv)
{
	int source_fd;
	int target_fd;

	if(argc < 3)
	{
		fprintf(stdout,"Usage: %s <source filename> <target filename>\n",argv[0]);
		exit(1);
	}

	if((source_fd = open(argv[1],O_RDONLY)) == -1)
	{
		fprintf(stderr,"Open source file %s faied:%s\n",argv[1],strerror(errno));
		exit(1);
	}

	mode_t mode = 0644;

	//open argv[2] for writing,if file name is already exited or some error accured
	//exit.
	if((target_fd = open(argv[2],O_RDWR|O_CREAT|O_EXCL, mode)) == -1)
	{
		if(errno == EEXIST)
		{
			fprintf(stderr,"Targets file %s existed.overwrite forbidden\n",argv[2]);
			close(target_fd);
			exit(1);
		}
		fprintf(stderr,"Cannot open target file %s for writing %s\n",argv[2],strerror(errno));
		close(target_fd);
		exit(1);
	}

	//initialize some buffer
	char buffer[BUFFER_SIZE];
	ssize_t read_bytes;
	ssize_t written_bytes;

	while(1)
	{
		if((read_bytes = read(source_fd,buffer,sizeof(buffer))) < 0)
		{
			if(errno == EINTR)
				continue;
			fprintf(stderr,"Read form source file %s failed:%s\n",argv[1],strerror(errno));
			break;
		}
		else if(read_bytes == 0)
		{
			//end of file argv[1]
			break;
		}
		else
		{
			int total = 0;
			while(total < read_bytes)
			{
				if((written_bytes = write(target_fd,buffer+total,read_bytes - total)) < 0)
				{
					if(errno == EINTR)
						continue;
					fprintf(stderr,"write to target file %s failed:%s\n",argv[2],strerror(errno));
					break;
				}
				else
				{
					fprintf(stdout,"Write %d bytes to target file.\n",written_bytes);
					total += written_bytes;
				}

			}
		}

	}
	return 0;
}

