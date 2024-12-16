#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_PATH "/dev/led_driver"
#define SET_PERIOD _IOW(240, 1, int)

int main() {
    int fd;
    int period;

    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    printf("Enter blink period in milliseconds (>= 100): ");
    scanf("%d", &period);

    if (ioctl(fd, SET_PERIOD, &period) < 0) {
        perror("Failed to set blink period");
    } else {
        printf("Blink period set to %d ms\n", period);
    }

    close(fd);
    return 0;
}
