#include<iostream>
#include<cmath>
#include<array>
#include<chrono>
#include"joystick.h"
#include<unistd.h>
#include<string>

using namespace std::chrono;

constexpr int joint_num = 7;
constexpr double amplitude = 10.0; //振幅
constexpr double max_speed = 1.0;  //[m/s]

double SerpenoidCurve(double angular_frequency_, double timer_, double offset_theta_, double turn_theta_){
    return amplitude * sin(angular_frequency_ * timer_ + offset_theta_) + turn_theta_;  
}
inline double get_time_sec(void){
    return static_cast<double>(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count())/1000000000;
}
int main(){
    std::array<double, joint_num> offset_theta; //前回角度
    std::array<double, joint_num> servo_theta;
    double turn_theta;
    double angular_frequency;
    double timer{};
    Joystick joystick("/dev/input/js0");
    if(!joystick.isFound()){
        std::cerr << "open failed" << std::endl;
    }
    bool stop_loop = false;
    
    while(!stop_loop){
        usleep(1000);
        JoystickEvent event;
        if(joystick.sample(&event)){
            if(event.isButton() && event.number == 10 && event.value == 1){
                stop_loop = true;
                std::cout << "exit" << std::endl;
            }
            if(event.isAxis()){
                auto map = [](long x, long in_min, long in_max, long out_min, long out_max){
                    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
                };
                int key_num = event.number;
                int key_val = event.value;
                switch(key_num){
                    case 0:
                        break;
                    case 1:
                        angular_frequency = map(key_val, -32767, 32767, 0, max_speed);
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4:
                        turn_theta = map(key_val, -32767, 32767, 0, M_PI);
                        break;
                    case 5:
                        break;

                }
            }
        }
        for(int i = 0; i < joint_num; ++i){
            servo_theta[i] = SerpenoidCurve(angular_frequency, timer, offset_theta[i], turn_theta);
            offset_theta[i] = servo_theta[i];
            std::cout << i << " " << servo_theta[i] << std::endl;
        }
    }
}