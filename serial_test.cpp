#include<iostream>
#include<string.h>
#include"serial_ubuntu.h"
#include<thread>

int main(){
    float write_data[9] = {0, -1.3, 2.98, 3.98, 4.532, 5.6, 6.31, 7.43, 8};
    float read_data[9]{};
    SerialTermios serial;
    while(1){
        serial.serialWrite(write_data);
        serial.serialRead();
        for(auto data : serial._read_data){
            std::cout << data << " ";
        }
        std::cout << std::endl;
    }
}