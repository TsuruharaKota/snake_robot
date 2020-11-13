#include<iostream>
#include<cmath>
#include<array>
#include<chrono>
#include"joystick.h"
#include<unistd.h>
#include<string>
#include<string.h>
//#include"matplotlibcpp.h"
#include"vector"
#include<fstream>
#include<iomanip>
#include"serial_ubuntu.h"

using namespace std::chrono;
//namespace plt = matplotlibcpp;


constexpr double max_speed = 6.0;  //[m/s]

int main(){
    Joystick joystick("/dev/input/js0");
    if(!joystick.isFound()){
        std::cerr << "open failed" << std::endl;
    }
    bool stop_loop = false;
    SerialTermios serial;
    float write_data[2]{}; //[0] = 推進速度, [1] = 旋回角度[deg]
    while(!stop_loop){
        usleep(1000);
        JoystickEvent event;
        if(joystick.sample(&event)){
            if(event.isButton() && event.number == 8 && event.value == 1){
                stop_loop = true;
                std::cout << "exit" << std::endl;
            }
            if(event.isAxis()){
                auto map = [](float x, float in_min, float in_max, float out_min, float out_max){
                    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
                };
                int key_num = event.number;
                int key_val = event.value;
                switch(key_num){
                    case 0:
                        break;
                    case 1:
                        //This program doesn't deal backwards.
                        write_data[0] = map(key_val, 0, -32767, 0, max_speed);
                        if(fabs(write_data[0]) < 1)write_data[0] = 0;
                        break;
                    case 2:
                        break;
                    case 3:
                        write_data[1] = map(key_val, -32767, 32767, 15, -15);
                        if(fabs(write_data[1]) < 7)write_data[1] = 0;
                        break;
                    case 4:
                        break;
                    case 5:
                        break;

                }
            }
        }
        //std::cout << "write_data[0] = " << write_data[0] << " write_data[1] = " << write_data[1] << std::endl;
        //serial write
        std::cout << "write_data[0] = " << write_data[0] << " write_data[1] = " << write_data[1] << std::endl;
        serial.serialWrite(write_data);
        serial.serialRead();
        for(auto data : serial._read_data){
            std::cout << data << " ";
        }
        std::cout << std::endl;
    }
}