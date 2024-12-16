#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/chardev+"

int main() {
    int fd;
    char buffer[256];

    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    printf("Enter string to write to device: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline

    if (write(fd, buffer, strlen(buffer)) < 0) {
        perror("Failed to write to device");
    } else {
        printf("Data written to device\n");
    }

    close(fd);
    return 0;
}
