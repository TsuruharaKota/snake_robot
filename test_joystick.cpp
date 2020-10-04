#include"joystick.h"
#include<unistd.h>
#include<iostream>
#include<string>

int main(){
    Joystick joystick("/dev/input/js0");
    if(!joystick.isFound()){
        std::cout << "open failed" << std::endl;
    }
    while(1){
        usleep(1000);
        JoystickEvent event;
        if(joystick.sample(&event)){
            if(event.isButton()){
                std::string key_val;
                event.value == 0 ? key_val = "up" : key_val = "down"; 
                std::cout << "Button " << event.number << " "<< key_val << std::endl;
            }else if(event.isAxis()){
                std::cout << "Axis " << event.number << "is at position " << event.value << std::endl;
            }
        }
    }
}