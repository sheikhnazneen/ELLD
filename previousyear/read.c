#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#define DEVICE_PATH "/dev/multi2"
int main()
{
int fd;
char buffer[50];
fd = open(DEVICE_PATH, O_RDWR);
if (fd < 0) {
perror("Error opening device");
return -1;
}
printf("Reading data from device...\n");
ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
if (bytes_read < 0) {
perror("Error reading data");
} else {
buffer[bytes_read] = '\0';
printf("Read data: %s\n", buffer);
}
close(fd);
return 0;
}