#include "mbed.h"
#include "serial_print.h"
#include <string>
#define HEAD_BYTE 0xFF
#define STX 0x02

Serial pc(USBTX, USBRX, 115200); // tx, rx

int main(){
    std::string str = "abcdefghijklmnopqrstuvwxyz"; 
    while(1){
        pc.printf("%c", HEAD_BYTE);
        pc.printf("%c", STX);
        uint8_t size = str.size();
        pc.printf("%c", size);
        pc.printf("%s\n", str.c_str());
    }
}

/* 
int main() {
    float receive_data[9];
    float send_data[9] = {1.0 , 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    while (true) { 
        serialReceive(receive_data, pc); 
        for(int i = 0; i < 9; ++i){
            send_data[i] = receive_data[i];
        }
        serialSend(send_data, pc);
    }
}*/