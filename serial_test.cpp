#include<iostream>
#include<string.h>
#include"serial_ubuntu.h"

int main(){
    float write_data[2] = {0, -1.3};
    float read_data[9]{};
    SerialTermios serial;
    while(1){
        //serial.serialWrite(write_data);
        serial.serialRead();
        for(auto data : serial._read_data){
            std::cout << data << " ";
        }
        std::cout << std::endl;
    }
}
