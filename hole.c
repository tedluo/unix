/*************************************************/
/*This funtion test If make a hole in file "test"*/
/*and write nothing,Will this accur some changes */
/*at the size of the file?*/

/*ATHOR:	ted*/
/*EMAIL:	luoming8@gmail.com*/
/*************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//for lseek
#include <sys/types.h>
#include <unistd.h>

//for open 
#include <sys/stat.h>
#include <fcntl.h>

//for strerror()
#include <errno.h>

int main(int argc, char **argv)
{
	int fd;
	if(argc < 2)
	{
		fprintf(stdout,"Usage %s <file name>\n",argv[0]);
		fprintf(stdout,"You have to specify <file name> to <test>\n");
		exit(1);
	}
	if(strcmp(argv[1],"test"))
	{
		fprintf(stdout,"You have to specify <file name> to <test>\n");
		exit(1);
	}

	//create a file to reposition
	system("rm test");
	system("touch test");
	system("echo this is just a test > ./test");
	
	//test the file size when i have done nothing
	fprintf(stdout,"Now i have done nothing at the file\n");
	system("ls -alh ./test");

	//open fila argv[1] for lseek
	if((fd = open(argv[1],O_RDWR)) == -1)
	{
		fprintf(stderr,"Open file %s failed:%s\n",argv[1],strerror(errno));
		exit(1);
	}

	if(lseek(fd,1024-1 ,SEEK_END) == (off_t)-1)
	{
		fprintf(stderr,"Reposition  file %s offset failed:%s\n",argv[1],strerror(errno));
		close(fd);
		exit(1);
	}
	fprintf(stdout,"Now i have reposition the file offset at 1024\n");
	system("ls -alh ./test");

	if(write(fd,"x",1) != 1)
	{
		fprintf(stderr,"write file %s failed:%s\n",argv[1],strerror(errno));
		exit(1);
	}
	fprintf(stdout,"Now i have write a char at 1024\n");
	system("ls -alh ./test");
	close(fd);
	return 0;
}
