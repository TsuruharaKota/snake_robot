#include<iostream>
#include<cmath>
#include<array>
#include<chrono>
#include"joystick.h"
#include<unistd.h>
#include<string>
//#include"matplotlibcpp.h"
#include"vector"
#include<fstream>
#include<iomanip>
#include"serial_ubuntu.h"

using namespace std::chrono;
//namespace plt = matplotlibcpp;


constexpr int joint_num = 4;
constexpr double amplitude = 10.0; //振幅(位相)
constexpr double max_speed = 4.0;  //[m/s]

double SerpenoidCurve(double angular_frequency_, double timer_, double offset_theta_, double turn_theta_){
    return amplitude * sin(angular_frequency_ * timer_ + offset_theta_) + turn_theta_; 
}
inline double get_time_sec(void){
    return static_cast<double>(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count())/1000000000;
}
int main(){
    constexpr std::array<double, joint_num> offset_theta{0, 60, 120, 180}; //前回角度
    std::array<double, joint_num> servo_theta;
    double turn_theta;
    double angular_frequency;
    Joystick joystick("/dev/input/js0");
    if(!joystick.isFound()){
        std::cerr << "open failed" << std::endl;
    }
    bool stop_loop = false;
    std::array<std::vector<double>, joint_num> data_box;
    double timer_calb = get_time_sec();
    double timer{};
    SerialTermios serial;
    float write_data[joint_num]{};
    while(!stop_loop){
        usleep(1000);
        timer = get_time_sec() - timer_calb;
        JoystickEvent event;
        if(joystick.sample(&event)){
            if(event.isButton() && event.number == 8 && event.value == 1){
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
                        //This program doesn't deal backwards.
                        angular_frequency = map(key_val, 0, -32767, 0, max_speed);
                        break;
                    case 2:
                        break;
                    case 3:
                        turn_theta = map(key_val, -32767, 32767, M_PI / 2, -M_PI / 2);
                        break;
                    case 4:
                        break;
                    case 5:
                        break;

                }
            }
        }
        for(int i = 0; i < joint_num; ++i){
            servo_theta[i] = SerpenoidCurve(angular_frequency, timer, offset_theta[i], turn_theta);
            data_box[i].push_back(servo_theta[i]);
            //std::cout << i << " " << servo_theta[i] << std::endl;
            write_data[i] = servo_theta[i];
        }
        //plt::plot(time_box, data_box, ".-r");
        //plt::pause(0.01);
        std::cout << servo_theta[0] << std::endl;

        //serial write
        serial.serialWrite(write_data);
    }
    std::ofstream outputFile("trajectory.txt");
    for(int i = 0; i < data_box[0].size(); ++i){
        outputFile << data_box[0].at(i) << " " << data_box[1].at(i) << " " << data_box[2].at(i) << " " << data_box[3].at(i) << "\n";
    }
    outputFile.close();
}