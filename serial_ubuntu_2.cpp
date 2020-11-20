#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>

#define SERIAL_PORT "/dev/ttyACM0"

int main(int argc, char *argv[])
{
    unsigned char msg[] = "serial port open...\n";
    unsigned char buf[255];             // バッファ
    int fd;                             // ファイルディスクリプタ
    struct termios tio;                 // シリアル通信設定
    int baudRate = B9600;
    int i;
    int len;
    int ret;
    int size;

    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);
            fcntl(fd, F_SETFL, 0);
            tcgetattr(fd, &tio);
            //set baudrate
            speed_t BAUDRATE = B9600;
            cfsetispeed(&tio, BAUDRATE);
            cfsetospeed(&tio, BAUDRATE);
            //non canonical, non echo back
            tio.c_lflag &= ~(ECHO | ICANON);
            //non blocking
            tio.c_cc[VMIN]=0;
            tio.c_cc[VTIME]=0;
            //store configuration
            tcsetattr(fd, TCSANOW, &tio);
    // 送受信処理ループ
    while(1) {
        len = read(fd, buf, sizeof(buf));
        if (0 < len) {
            std::cout << "len = " << len << std::endl;
        }
    }

    close(fd);                              // デバイスのクローズ
    return 0;
}