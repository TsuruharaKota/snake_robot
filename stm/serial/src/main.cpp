#include "mbed.h"
#include "serial.h"
#include <string>

Serial pc(PB_6, PB_7, 115200); // tx, rx

/*
int main(){
    //std::string str = "abcdefghijklmnopqrstuvwxyz"; 
    std::string str = "1.000000,2.000000,3.000000,4.000000,5.000000,6.000000,7.000000,8.000000,9.000000";
    while(1){
        pc.printf("%c", HEAD_BYTE);
        pc.printf("%c", STX);
        uint8_t size = str.size();
        pc.printf("%c", size);
        pc.printf("%s\n", str.c_str());
    }
}*/

int main() {
    float receive_data[9];
    float send_data[9] = {1.0 , 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    while (true) { 
        /*serialReceive(receive_data, pc); 
        for(int i = 0; i < 9; ++i){
            send_data[i] = receive_data[i];
        }*/
        serialSend(send_data, pc);
    }
}