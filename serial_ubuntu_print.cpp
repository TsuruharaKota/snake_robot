#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
#include <tuple>

#define SERIAL_PORT "/dev/ttyACM0"

class SerialTermios{
    public:
        SerialTermios(){
            _fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);
            fcntl(_fd, F_SETFL, 0);
            tcgetattr(_fd, &_tio);
            //set baudrate
            speed_t BAUDRATE = B115200;
            cfsetispeed(&_tio, BAUDRATE);
            cfsetospeed(&_tio, BAUDRATE);
            //non canonical, non echo back
            _tio.c_lflag &= ~(ECHO | ICANON);
            //non blocking
            _tio.c_cc[VMIN]=0;
            _tio.c_cc[VTIME]=0;
            //store configuration
            tcsetattr(_fd, TCSANOW, &_tio);
            if(_fd < 0){
                std::cerr << "Serial Fail" << std::endl;
            }
        }
        void serialWrite(float *send_data){
            std::string write_str;
            for(int i = 0; i < 9; ++i){
                write_str += std::to_string(send_data[i]);
                if(i != 8){
                    write_str.push_back(',');
                }
            }
            //std::cout << write_str << std::endl;
            write(_fd, write_str.c_str(), write_str.size());
            /*float cast_data[9];
            char *ptr;
            char *str = const_cast<char *>(write_str.c_str());
            ptr = strtok(str, ",");
            //printf("%s\n", ptr);
            cast_data[0] = std::stof(ptr);
            int i = 0;
            while(ptr != NULL){
                ++i;
                ptr = strtok(NULL, ",");
                if(ptr != NULL){
                    //printf("%s\n", ptr);
                    cast_data[i] = std::stof(ptr);
                }
            }*/
        }
        void serialRead(){
            std::string data_str = "0, 0, 0, 0, 0, 0, 0, 0, 0";
            float cast_data[9];
            char buf[255];
            int len;
            char *ptr;
            len = read(_fd, buf, sizeof(buf));
            if(0 < len){
                for(int i = 0; i < len; ++i){
                    data_str.push_back(buf[i]);
                }
            }
            char *str = const_cast<char *>(data_str.c_str());
            ptr = strtok(str, ",");
            //printf("%s\n", ptr);
            _read_data[0] = std::stof(ptr);
            int i = 0;
            while(ptr != NULL){
                ++i;
                ptr = strtok(NULL, ",");
                if(ptr != NULL){
                    //printf("%s\n", ptr);
                    _read_data[i] = std::stof(ptr);
                }
            }
        }
        float _write_data[2];
        float _read_data[9];
    private:
        int _timeout_counter;
        termios _tio;
        int _fd;
        char _read_buf[255];
};
int main(){
    SerialTermios serial;
    float write_data[9] = {0.5f, 1.1f, 1.4f, 1.9f, 1.0f, 1.6f, 4.7f, 2.9f, 1.7f};
    serial.serialWrite(write_data);
    serial.serialRead();
    for(auto data : serial._read_data){
        std::cout << data << " ";
    }
    std::cout << std::endl;
}