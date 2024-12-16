#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#define DEVICE_PATH "/dev/multi2"
int main()
{
int fd;
const char *data = "Hello, Device!";
fd = open(DEVICE_PATH, O_RDWR);
if (fd < 0) {
perror("Error opening device");
return -1;
}
printf("Writing data to device: %s\n", data);
ssize_t bytes_written = write(fd, data, strlen(data));
if (bytes_written < 0) {
perror("Error writing data");
}
close(fd);
return 0;
}