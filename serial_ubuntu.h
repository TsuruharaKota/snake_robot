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
#include <string.h>
#include <iostream>

#define SERIAL_PORT "/dev/ttyACM0"

#define HEAD_BYTE 0xFF
#define STX 0x02
#define float32_t float

class SerialTermios{
    public:
        SerialTermios(){
            _fd = open(SERIAL_PORT, O_RDWR);
            if(_fd == -1){
                std::cerr << "open error" << std::endl;
            }

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

        }
        void serialWriteByte(char write_c_){
            char write_data[] = {write_c_, '\0'};
            write(_fd, write_data, sizeof(write_data));
        }
        void serialWrite(float *write_data){
            uint8_t checksum_send = 0;
            unsigned char data_h_h[9]{};
            unsigned char data_h_l[9]{};
            unsigned char data_l_h[9]{};
            unsigned char data_l_l[9]{};
            uint32_t byte_divide[9]{};
            for(int i = 0; i < 9; ++i){
                memcpy(&byte_divide[i], &write_data[i], 4);
                data_h_h[i] = (byte_divide[i] >> 24) & 0xFF;
                data_h_l[i] = (byte_divide[i] >> 16) & 0xFF;
                data_l_h[i] = (byte_divide[i] >> 8) & 0xFF;
                data_l_l[i] = (byte_divide[i] >> 0) & 0xFF;
            }
            unsigned char sendFormat[9][5] = {
                {0, data_h_h[0], data_h_l[0], data_l_h[0], data_l_l[0]},
                {1, data_h_h[1], data_h_l[1], data_l_h[1], data_l_l[1]},
                {2, data_h_h[2], data_h_l[2], data_l_h[2], data_l_l[2]},
                {3, data_h_h[3], data_h_l[3], data_l_h[3], data_l_l[3]},
                {4, data_h_h[4], data_h_l[4], data_l_h[4], data_l_l[4]},
                {5, data_h_h[5], data_h_l[5], data_l_h[5], data_l_l[5]},
                {6, data_h_h[6], data_h_l[6], data_l_h[6], data_l_l[6]},
                {7, data_h_h[7], data_h_l[7], data_l_h[7], data_l_l[7]},
                {8, data_h_h[8], data_h_l[8], data_l_h[8], data_l_l[8]},
            };
            //send head byte
            serialWriteByte(HEAD_BYTE);
            checksum_send += HEAD_BYTE;
            serialWriteByte(STX);
            checksum_send += STX;
            for(int i = 0; i < 9; ++i){
                for(int k = 0; k < 5; ++k){
                    serialWriteByte(sendFormat[i][k]);
                    checksum_send += sendFormat[i][k];
                }
            }
            //send checksum
            serialWriteByte(checksum_send);
        }
        
        float receive_result[9]{};
        
        char serialReadByte(){
            int size = read(_fd, _read_buf, sizeof(_read_buf));
            if(size == 1){
                std::cerr << "ok" << std::endl;
                return _read_buf[0];
            }else{
                if (0 < size) {
                    for(int i = 0; i < sizeof(_read_buf); i++) {
                        printf("%c", _read_buf[i]);
                    }
                    printf("\n");
                }
            }
        }
        void serialRead(){
            uint8_t got_data{};
            uint8_t checksum_receive{};
            uint8_t receive_data[9];
            unsigned char receiveFormat[9][5] = {
                {1, 0, 0, 0, 0},
                {2, 0, 0, 0, 0},
                {3, 0, 0, 0, 0},
                {4, 0, 0, 0, 0},
                {5, 0, 0, 0, 0},
                {6, 0, 0, 0, 0},
                {7, 0, 0, 0, 0},
                {8, 0, 0, 0, 0},
                {9, 0, 0, 0, 0},
            };
            got_data = static_cast<uint8_t>(serialReadByte());
            if(got_data == HEAD_BYTE){
                std::cout << "fuck" << std::endl;
                got_data = static_cast<uint8_t>(serialReadByte());
                if(got_data == STX){
                    checksum_receive += HEAD_BYTE;
                    checksum_receive += STX;
                    for(int k = 0; k < 9; ++k){
                        for(int i = 0; i < 5; ++i){
                            receive_data[i] = static_cast<uint8_t>(serialReadByte());
                            checksum_receive += receive_data[i];
                            receiveFormat[receive_data[0]][i] = receive_data[i];
                        }   
                    }
                    got_data = static_cast<uint8_t>(serialReadByte());
                    if(got_data == checksum_receive){
                        int32_t result[9];
                        for(int i = 0; i < 9; ++i){
                            //receiveFormat[i][0]はidである
                            result[i] = static_cast<int32_t>((receiveFormat[i][1] << 24 & 0xFF000000)
                                                           | (receiveFormat[i][2] << 16 & 0x00FF0000)
                                                           | (receiveFormat[i][3] <<  8 & 0x0000FF00)
                                                           | (receiveFormat[i][4] <<  0 & 0x000000FF)
                            );
                            memcpy(&receive_result[i], &result[i], 4);
                        }
                    }
                }
            }
        }
    private:
        termios _tio;
        int _fd;
        char _read_buf[255];
};

#endif
