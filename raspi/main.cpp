#include<iostream>
#include<cstdint>
#include<cstring>
#include<pigpiod_if2.h>

using std::memcpy;

#define HEAD_BYTE 0xFF
#define STX 0x02

const char *port = "/dev/ttyAMA0";
int baudrate = 115200;
int pi = pigpio_start(0, 0);
int serial_handle{};
unsigned char dummy_flag{};
//send
float send_data[2]{};
//receive
uint8_t got_data{};
uint8_t byte_now{};
uint8_t byte[2]{};
float receive_result[5]{};

typedef struct{
  uint16_t gyro_data;
  uint16_t tof_data[4];
}ReceiveFormat;

void sendSerial(){
  send_data[0] = 12.3;
  send_data[1] = 45.6789;
  uint8_t checksum_send = 0;
  unsigned char data_h_h[9], data_h_l[9], data_l_h[9], data_l_l[9];
  //divide byte
  int32_t byte_divide[9];
  for(int i = 0; i < 9; ++i){
    //float32_t to int32_t
    memcpy(&byte_divide[i], &send_data[i], 4);
    data_h_h[i] = (byte_divide[i] >> 24) & 0xFF;
    data_h_l[i] = (byte_divide[i] >> 16) & 0xFF;
    data_l_h[i] = (byte_divide[i] >> 8) & 0xFF;
    data_l_l[i] = (byte_divide[i] >> 0) & 0xFF;
  };
  unsigned char sendFormat[9][5] = {
    {0, data_h_h[0], data_h_l[0], data_l_h[0], data_l_l[0]},
    {1, data_h_h[1], data_h_l[1], data_l_h[1], data_l_l[1]},
    {2, data_h_h[2], data_h_l[2], data_l_h[2], data_l_l[2]},
    {3, data_h_h[3], data_h_l[3], data_l_h[3], data_l_l[3]},
    {4, data_h_h[4], data_h_l[4], data_l_h[4], data_l_l[4]},
    {5, data_h_h[5], data_h_l[5], data_l_h[5], data_l_l[5]},
    {6, data_h_h[6], data_h_l[6], data_l_h[6], data_l_l[6]},
    {7, data_h_h[7], data_h_l[7], data_l_h[7], data_l_l[7]},
    {8, data_h_h[8], data_h_l[8], data_l_h[8], data_l_l[8]},
  };
  //send head byte
  serial_write_byte(pi, serial_handle, HEAD_BYTE);
  checksum_send += HEAD_BYTE;
  serial_write_byte(pi, serial_handle, STX);
  checksum_send += STX;
  for(int i = 0; i < 9; ++i){
    for(int k = 0; k < 5; ++k){
      serial_write_byte(pi, serial_handle, sendFormat[i][k]);
      checksum_send += sendFormat[i][k];
    }
  }
  //send checksum
  serial_write_byte(pi, serial_handle, checksum_send);
}
void receiveSerial(){
  uint8_t checksum_receive{};
  uint8_t receive_data[9];
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
    printf("HEAD %d\n", (int)got_data);
    got_data = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
    if(got_data == STX){
      printf("STX\n");
      checksum_receive += HEAD_BYTE;
      checksum_receive += STX;
      for(int k = 0; k < 9; ++k){
        for(int i = 0; i < 5; ++i){
          receive_data[i] = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
          checksum_receive += receive_data[i];
          receiveFormat[receive_data[0]][i] = receive_data[i];
        }
      }
      got_data = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
      if(got_data == checksum_receive){
        int32_t result[9];
        for(int i = 0; i < 9; ++i){
          //receiveFormat[i][0]はidである
          result[i] = static_cast<int32_t>((receiveFormat[i][1] << 24 & 0xFF000000)
                                           | (receiveFormat[i][2] << 16 & 0x00FF0000)
                                           | (receiveFormat[i][3] <<  8 & 0x0000FF00)
                                           | (receiveFormat[i][4] <<  0 & 0x000000FF)
          );
          memcpy(&receive_result[i], &result[i], 4);
        }
      }
    }
  }
}
int main(int argc, char **argv) {
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

  while (1) {
    receiveSerial();
    //printf("%f %f %f %f %f\n", receive_result[0], receive_result[1], receive_result[2], receive_result[3], receive_result[4]);
  }
  serial_close(pi, serial_handle);
}
