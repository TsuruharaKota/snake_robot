#include<iostream>
#include<cstdint>
#include<cstring>
#include<pigpiod_if2.h>
#include<cmath>
#include"joystick.h"
#include<unistd.h>
#include<string>
#include<string.h>
#include<fstream>
#include<iomanip>

using std::memcpy;
constexpr double max_speed = 6.0;
#define HEAD_BYTE 0xFF
#define STX 0x02

const char *port = "/dev/ttyACM5";
int baudrate = 115200;
int pi = pigpio_start(0, 0);
int serial_handle{};
unsigned char dummy_flag{};

void sendSerial(float *_send_data){
	uint8_t checksum_send = 0;
	unsigned char data_h_h[2], data_h_l[2], data_l_h[2], data_l_l[2];
	//divide byte
	int32_t byte_divide[5];
	for(int i = 0; i < 2; ++i){
		//float32_t to int32_t
		memcpy(&byte_divide[i], &_send_data[i], 4);
		data_h_h[i] = (byte_divide[i] >> 24) & 0xFF;
		data_h_l[i] = (byte_divide[i] >> 16) & 0xFF;
		data_l_h[i] = (byte_divide[i] >> 8) & 0xFF;
		data_l_l[i] = (byte_divide[i] >> 0) & 0xFF;
	};
	unsigned char sendFormat[2][5] = {
		{0, data_h_h[0], data_h_l[0], data_l_h[0], data_l_l[0]},
		{1, data_h_h[1], data_h_l[1], data_l_h[1], data_l_l[1]}
	};
	//send head byte
	serial_write_byte(pi, serial_handle, HEAD_BYTE);
	checksum_send += HEAD_BYTE;
	serial_write_byte(pi, serial_handle, STX);
	checksum_send += STX;
	for(int i = 0; i < 2; ++i){
		for(int k = 0; k < 5; ++k){
			serial_write_byte(pi, serial_handle, sendFormat[i][k]);
			checksum_send += sendFormat[i][k];
		}
	}
	//send checksum
	serial_write_byte(pi, serial_handle, checksum_send);
}
void receiveSerial(float *_receive_result){
	uint8_t checksum_receive{};
	uint8_t receive_data[9];
	uint8_t got_data{};
	unsigned char receiveFormat[9][5] = {
		{0, 0, 0, 0, 0},
		{1, 0, 0, 0, 0},
		{2, 0, 0, 0, 0},
		{3, 0, 0, 0, 0},
		{4, 0, 0, 0, 0},
		{5, 0, 0, 0, 0},
		{6, 0, 0, 0, 0},
		{7, 0, 0, 0, 0},
		{8, 0, 0, 0, 0},
	};
	got_data = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
	if(got_data == HEAD_BYTE){
                std::cout << "HEAD_BYTE" << std::endl;
		got_data = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
		if(got_data == STX){
			std::cout << "STX" << std::endl;
			checksum_receive += HEAD_BYTE;
			checksum_receive += STX;
			for(int k = 0; k < 9; ++k){
				for(int i = 0; i < 5; ++i){
					receive_data[i] = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
					checksum_receive += receive_data[i];
					int temp_data  = static_cast<int>(receive_data[0]);
                            		if((0 <= temp_data) && (temp_data <= 9)){
                                		receiveFormat[receive_data[0]][i] = receive_data[i];
                            		}else{
                                		break;
                            		}
				}
			}
			got_data = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
			if(got_data == checksum_receive){
				std::cout << "checksum" << std::endl;
				int32_t result[9];
				for(int i = 0; i < 9; ++i){
					//receiveFormat[i][0]はidである
					result[i] = static_cast<int32_t>((receiveFormat[i][1] << 24 & 0xFF000000)
							| (receiveFormat[i][2] << 16 & 0x00FF0000)
							| (receiveFormat[i][3] <<  8 & 0x0000FF00)
							| (receiveFormat[i][4] <<  0 & 0x000000FF)
							);
					memcpy(&_receive_result[i], &result[i], 4);
				}
			}
		}
	}
}
int main() {
	try {
		serial_handle =
			serial_open(pi, const_cast<char *>(port), baudrate, dummy_flag);
		if (serial_handle < 0) {
			throw serial_handle;
		} else {
			printf("Serial Initialize complete\n");
		}
	}
	catch (int _serial_handle) {
		printf("Serial Initialize Failed\n");
		return 1;
	}
	float read_data[9]{};
	float write_data[2]{}; //[0] = 推進速度, [1] = 旋回角度[deg]
	Joystick joystick("/dev/input/js0");
	if(!joystick.isFound()){
		std::cerr << "open failed" << std::endl;
	}
	bool stop_loop = false;

	while (!stop_loop) {
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
		sendSerial(write_data);
		receiveSerial(read_data);
		printf("%f %f %f %f %f %f %f %f %f\n", read_data[0], read_data[1], read_data[2], read_data[3], read_data[4], read_data[5], read_data[6], read_data[7], read_data[8]);
	}
	serial_close(pi, serial_handle);
}
