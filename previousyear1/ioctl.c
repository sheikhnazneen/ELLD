#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define DEVICE_PATH "/dev/chardev"
#define GETSTATS _IOR(240, 1, struct stats)

struct stats {
    int size;
    char buff[256];
    int r_w;
};

int main() {
    int fd;
    struct stats device_stats;

    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    if (ioctl(fd, GETSTATS, &device_stats) < 0) {
        perror("Failed to get stats from device");
    } else {
        printf("Device Stats:\n");
        printf("Buffer Size: %d\n", device_stats.size);
        printf("Buffer Content: %s\n", device_stats.buff);
        printf("Recent Activity: %s\n", device_stats.r_w == 1 ? "write" : "read");
    }

    close(fd);
    return 0;
}
