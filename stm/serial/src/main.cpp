#include "mbed.h"
#include "serial.h"

Serial pc(USBTX, USBRX, 9600); // tx, rx

int main() {
    float send_data[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    while(1) {
        //float send_data[9];
        //serialReceive(receive_data, pc);
        //memcpy(send_data, receive_data, sizeof(receive_data));
        serialSend(send_data, pc);
    }
}