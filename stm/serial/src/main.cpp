#include "mbed.h"
#include "serial.h"

Serial pc(USBTX, USBRX, 9600); // tx, rx

int main() {
    //float send_data[9] = {};
    float send_data[9] = {0.98, 1.45, 2.85, 3.43, 4.08, 5.76, 6.98, 7.64, 8.96};
    float receive_data[9]{};
    int counter{};
    DigitalOut led(LED1);
    while(1) {
        //++counter;
        //float send_data[9];
        serialReceive(receive_data, pc);
        //memcpy(send_data, receive_data, sizeof(receive_data));
        for(int i = 0; i < 9; ++i){
            send_data[i] = receive_data[i];
        }
        serialSend(send_data, pc);
    }
}

/*
int main() {
    float send_data[9] = {0.98, 1.45, 2.85, 3.43, 4.08, 5.76, 6.98, 7.64, 8.96};
    int counter{};
    while(1) {
        /*++counter;
        for(int i = 0; i < 9; ++i)
        send_data[i] += 0.01;
        if(counter > 1000){
            for(int i = 0; i < 9; ++i)
            send_data[i] = 0;
        }
        serialSend(send_data, pc);
    }
}
*/