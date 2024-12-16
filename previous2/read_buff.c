// #include <stdio.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <string.h>

// int main() {
//     int fd = open("/dev/Sansu", O_RDONLY);
//     if (fd < 0) {
//         perror("Failed to open device");
//         return -1;
//     }

//     char buffer[50];
//     int bytes_read = read(fd, buffer, sizeof(buffer));
//     if (bytes_read > 0) {
//         buffer[bytes_read] = '\0';
//         printf("Data read from device: %s\n", buffer);
//     }

//     close(fd);
//     return 0;
// }
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE "/dev/my_char"

int main() {
    int fd;
    char output_buffer[50];
    ssize_t bytes_read;

    // Open the device for reading
    fd = open(DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    // Read data from the device
    bytes_read = read(fd, output_buffer, sizeof(output_buffer) - 1);
    if (bytes_read < 0) {
        perror("Failed to read from device");
        close(fd);
        return EXIT_FAILURE;
    }

    // Null-terminate the output buffer
    output_buffer[bytes_read] = '\0';

    printf("Data read from device: %s\n", output_buffer);

    // Close the device
    close(fd);
    return EXIT_SUCCESS;
}
