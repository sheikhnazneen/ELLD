#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/chardev"

int main() {
    int fd;
    char buffer[256];
    ssize_t bytes_read;

    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Failed to read from device");
    } else {
        buffer[bytes_read] = '\0'; // Null-terminate string
        printf("Data read from device: %s\n", buffer);
    }

    close(fd);
    return 0;
}
