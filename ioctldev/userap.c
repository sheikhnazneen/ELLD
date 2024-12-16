#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include"ioctl.h"

int main()
{
	int fd;

	fd=open("/dev/iodv",O_RDWR);
	if(!fd)
	{
		perror("Error In Opening File\n");
		return -1;
	}

	printf("fd= %d\n",fd);
	ioctl(fd, CMD_1);
	ioctl(fd, CMD_2,8);

	close(fd);

	return 0;
}