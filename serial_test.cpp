#include<iostream>
#include<string.h>
#include"serial_ubuntu.h"

int main(){
    float write_data[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    float read_data[9]{};
    SerialTermios serial;
    while(1){
        /*memcpy(serial._write_data, write_data, sizeof(write_data));
        serial.serialWrite();
        serial.serialRead();
        for(int i = 0; i < 9; ++i){
            std::cout << i << " : " <<  serial._read_data[i] << std::endl;
        }*/
        /*std::string data = serial.serialReadOne();
        if(data != "NO DATA"){
            if(data != "\n"){
                std::cout << data;
            }else{
                std::cout << std::endl;
            }
        }*/
        //serial.serialWrite(write_data);
        serial.serialRead(read_data);
        for(auto data : read_data){
            std::cout << data;
        }
        std::cout << std::endl;
    }
}