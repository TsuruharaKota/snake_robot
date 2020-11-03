#ifndef SERIAL_H_
#define SERIAL_H_

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

#define HEAD_BYTE 0xFF
#define STX 0x02
#define float32_t float

class SerialTermios{
    public:
        SerialTermios(){
            _fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);
            /*fcntl(_fd, F_SETFL, 0);
            tcgetattr(_fd, &_tio);
            //set baudrate
            speed_t BAUDRATE = B4800;
            cfsetispeed(&_tio, BAUDRATE);
            cfsetospeed(&_tio, BAUDRATE);
            //non canonical, non echo back
            _tio.c_lflag &= ~(ECHO | ICANON);
            //non blocking
            _tio.c_cc[VMIN]=0;
            _tio.c_cc[VTIME]=0;
            //store configuration
            tcsetattr(_fd, TCSANOW, &_tio);
            */
            _tio.c_cflag += CREAD;               // 受信有効
            _tio.c_cflag += CLOCAL;              // ローカルライン（モデム制御なし）
            _tio.c_cflag += CS8;                 // データビット:8bit
            _tio.c_cflag += 0;                   // ストップビット:1bit
            _tio.c_cflag += 0;                   // パリティ:None

            cfsetispeed(&_tio, B9600);
            cfsetospeed(&_tio, B9600);

            cfmakeraw(&_tio);                    // RAWモード

            tcsetattr(_fd, TCSANOW, &_tio);     // デバイスに設定を行う
            ioctl(_fd, TCSETS, &_tio);            // ポートの設定を有効にする

            if(_fd < 0){
                std::cerr << "Serial Fail" << std::endl;
            }
        }
        /*void serialWriteOne(char write_c_){
            std::string write_str = std::to_string(write_c_);
            //char c = 0xFF;
            //std::cout << static_cast<int>(c) << std::endl;
            //std::string write_str = std::to_string(c);
            //int data  = stoi(write_str);
            //std::cout << "0 = " << write_str[0] << " 1 = " << write_str[1] << std::endl;
            //const char * sum = write_str.c_str();
            //std::cout << static_cast<int>(sum[0]) << std::endl;
            std::cout << "size = " << write_str.size() << std::endl;
            int rec = write(_fd, write_str.c_str(), write_str.size());
        }*/
        void serialWrite(float *send_data){
            uint8_t checksum_send = 0;
            unsigned char write_buf[13]{};
            unsigned char data_h_h[2]{};
            unsigned char data_h_l[2]{};
            unsigned char data_l_h[2]{};
            unsigned char data_l_l[2]{};
            uint32_t byte_divide[2]{};
            for(int i = 0; i < 2; ++i){
                memcpy(&byte_divide[i], &send_data[i], 4);
                data_h_h[i] = (byte_divide[i] >> 24) & 0xFF;
                data_h_l[i] = (byte_divide[i] >> 16) & 0xFF;
                data_l_h[i] = (byte_divide[i] >> 8) & 0xFF;
                data_l_l[i] = (byte_divide[i] >> 0) & 0xFF;
            }
            unsigned char sendFormat[2][5] = {
                {0, data_h_h[0], data_h_l[0], data_l_h[0], data_l_l[0]},
                {1, data_h_h[1], data_h_l[1], data_l_h[1], data_l_l[1]},
            };
            write_buf[0] = HEAD_BYTE;
            //serialWriteOne(HEAD_BYTE);
            checksum_send += HEAD_BYTE;
            write_buf[1] = STX;
            //serialWriteOne(STX);
            checksum_send += STX;
            int count = 1;
            for(int i = 0; i < 2; ++i){
                for(int k = 0; k < 5; ++k){
                    ++count;
                    write_buf[count] = sendFormat[i][k];
                    //serialWriteOne(sendFormat[i][k]);
                    checksum_send += sendFormat[i][k];
                }
            }
            count++;
            write_buf[count] = checksum_send;
            //serialWriteOne(checksum_send);
            //std::cout << "sum = " << static_cast<int>(checksum_send) << "size = " << write_str.size() << std::endl;
            write(_fd, write_buf, 13);
        }
        /*bool timeOut(){
            return _timeout_counter > _timeout ? true : false;
        }*/
        char serialReadOne(){
            constexpr int COUNT_MAX = 10000; //timeout
            int size = 0;
            int count = 0;
            while(size != 1){
                //++_timeout_counter;
                ++count;
                if(count > COUNT_MAX){
                    //std::cout << "COUNT ERROR" << std::endl;
                    return 'a'; //timeoutが発生しているのでゴミデータの送信を行う
                }else{
                    size = read(_fd, _read_buf, sizeof(_read_buf));
                    if(size == 1){
                        return  _read_buf[0];
                    }
                }
            }
        }        
        void serialRead(){
            //serialLoopInit();   
            uint8_t got_data{};
            uint8_t checksum_receive{};
            uint8_t receive_data[5]{};
            unsigned char receiveFormat[2][5] = {
                {0, 0, 0, 0, 0},
                {1, 0, 0, 0, 0},
            };
            got_data = static_cast<uint8_t>(serialReadOne());
            if(got_data == HEAD_BYTE){
                got_data = static_cast<uint8_t>(serialReadOne());
                if(got_data == STX){
                    checksum_receive += HEAD_BYTE;
                    checksum_receive += STX;
                    for(int k = 0; k < 2; ++k){
                        for(int i = 0; i < 5; ++i){
                            receive_data[i] = static_cast<uint8_t>(serialReadOne());
                            //if(timeOut())break;
                            checksum_receive += receive_data[i];
                            int temp_data  = static_cast<int>(receive_data[0]);
                            if((0 <= temp_data) && (temp_data <= 1)){
                                receiveFormat[receive_data[0]][i] = receive_data[i];
                            }else{
                                break;
                            }
                        }
                    }
                    got_data = static_cast<uint8_t>(serialReadOne());
                    if(got_data == checksum_receive){
                        int32_t result[2]{};
                        for(int i = 0; i < 2; ++i){
                            result[i] = static_cast<int32_t>((receiveFormat[i][1] << 24 & 0xFF000000)
                                                           | (receiveFormat[i][2] << 16 & 0x00FF0000)
                                                           | (receiveFormat[i][3] <<  8 & 0x0000FF00)
                                                           | (receiveFormat[i][4] <<  0 & 0x000000FF)
                            );
                            memcpy(&_read_data[i], &result[i], 4);
                        }
                    }else{
                        std::cout << "OK" << std::endl;
                    }
                }
            }
        }
        float _write_data[2];
        float _read_data[2];
    private:
        /*void serialLoopInit(){
            _timeout_counter = 0;
        }
        const int _timeout;*/
        int _timeout_counter;
        termios _tio;
        int _fd;
        char _read_buf[255];
};

#endif