#include <stdio.h>
#include <unistd.h> // close
#include <fcntl.h>  // open
#include <stdlib.h>
#include <termio.h>
#include <memory.h>

#define BUF_SIZE  1024
static const char dev[] = "/dev/ttyS3";

int serial_init(int *fd, const char *device) {
    struct termios Opt;
    *fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (*fd < 0) {
        perror("init");
        return 0;
    }
    tcgetattr(*fd, &Opt);
    cfsetispeed(&Opt, B115200);
    cfsetospeed(&Opt, B115200);
    Opt.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    Opt.c_oflag &= ~OPOST;
    Opt.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    Opt.c_cflag &= ~(CSIZE | PARENB);
    Opt.c_cflag |= CS8;
    tcsetattr(*fd, TCSANOW, &Opt);

    return 1;
}

int serial_write(const int *fd, const char *data, size_t size) {
    int ret = write(*fd, data, size);
    if (ret < 0) {
        perror("write");
        tcflush(*fd, TCOFLUSH);
    }
    return ret;
}

int serial_read(const int *fd, char *data, size_t size) {
    ssize_t read_size = 0;
    size_t read_left = size;

    while (read_left > 0) {
        if ((read_size = read(*fd, data, read_left)) < 0) {
            if (read_size == -1)continue;
            else perror("read");
        } else if (read_size == 0) {
            break; /* EOF */
        }
        read_left -= read_size;
        data += read_size;
        printf("%s\n", data);
    }
    return (size - read_left); /* return >= 0 */
}


int main() {
    int fd = 0;
    int times = 8;
    char buf_data = rand() % 26 + 65;
    char read_buf[BUF_SIZE + 1], write_buf[BUF_SIZE];
    int ret = serial_init(&fd, dev);
    if (!ret) {
        printf("Serial Port Init Failed!!!\n");
        return -1;
    }
    printf("Start Test!!!\n");


    while (times--) {
        buf_data = rand() % 26 + 65;
        memset(write_buf, buf_data, BUF_SIZE);
        memset(read_buf, 0, BUF_SIZE + 1);

        ret = serial_write(&fd, write_buf, BUF_SIZE);
        printf("send size: %d\n", ret);

        usleep(115200 * 2);     // 保证写完

        ret = serial_read(&fd, read_buf, BUF_SIZE);
        printf("recv size: %d\n", ret);

        printf("RECV:\n%s\n", read_buf);

        ret = memcmp(read_buf, write_buf, BUF_SIZE);
        if (ret != 0) {
            printf("Result: Inconsistent data\n\n");
            break;
        } else {
            printf("Result: Consistent data\n\n");
        }

        usleep(100 * 1000);
    }
    close(fd);
    return 0;
}