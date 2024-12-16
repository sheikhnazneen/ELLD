#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int main()
{
	int fd,num1,num2;

	printf("Welcome To Write App-1.\n");
	printf("Enter Two Number For Addition.\n");
	scanf("%d %d",&num1,&num2);
	int num[2] ={num1, num2};

	fd =open("/dev/Wr1",O_RDWR,0777);

	if(fd <0)
	{
		printf("Error In Openig File\n");
		return -1;
	}

	printf("Success In Opening File\n");

	int result =write(fd,&num,sizeof(num));
	if(result <0)
	{
		printf("Error In Writing Data From User To Kernel.\n");
		return -1;
	}
	printf("Success In Writing Data From User To Kernel.\n");
	close(fd);
	return 0;
}