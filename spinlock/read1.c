#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>


int main()
{
	int fd,op_result;

	printf("Welcome To Read App-1.\n");

	fd =open("/dev/Rd1",O_RDWR,0777);
	if(fd <0)
	{
		printf("Error In Opening File\n");
		return -1;
	}
	printf("Success In Opening File.\n");

	int result =read(fd, &op_result,sizeof(op_result));
	if(result <0)
	{
		printf("Error In Reading Data From Kernel To User.\n");
		return -1;
	}
	printf("Success In Reading Data From Kernel To User Data-->%d.\n",result);
	close(fd);

	return 0;
}