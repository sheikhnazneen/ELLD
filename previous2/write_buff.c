// #include<stdio.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <string.h>

// int main() {
//     int fd = open("/dev/Sansu", O_WRONLY);
//     if (fd < 0) {
//         perror("Failed to open device");
//         return -1;
//     }

//     char data[] = "Test Data";
//     write(fd, data, strlen(data));
//     printf("Data written to device\n");

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
    char input_buffer[50];

    // Open the device for writing
    fd = open(DEVICE, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    // Prompt the user for input
    printf("Enter a string to write to the device: ");
    if (!fgets(input_buffer, sizeof(input_buffer), stdin)) {
        perror("Failed to read input");
        close(fd);
        return EXIT_FAILURE;
    }

    // Remove trailing newline
    size_t len = strlen(input_buffer);
    if (len > 0 && input_buffer[len - 1] == '\n') {
        input_buffer[len - 1] = '\0';
    }

    // Write data to the device
    if (write(fd, input_buffer, strlen(input_buffer)) < 0) {
        perror("Failed to write to device");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Data written to device: %s\n", input_buffer);

    // Close the device
    close(fd);
    return EXIT_SUCCESS;
}
