#ifndef SERIAL_H_
#define SERIAL_H_

#include"mbed.h"
#include"string.h"

#define HEAD_BYTE 0xFF
#define STX 0x02
void serialSend(float *send_data, Serial &obj){ 
  uint8_t checksum_send = 0;
  unsigned char data_h_h[9]{};
  unsigned char data_h_l[9]{};
  unsigned char data_l_h[9]{};
  unsigned char data_l_l[9]{};
  uint32_t byte_divide[9]{};
  for(int i = 0; i < 9; ++i){
    memcpy(&byte_divide[i], &send_data[i], 4);
    data_h_h[i] = (byte_divide[i] >> 24) & 0xFF;
    data_h_l[i] = (byte_divide[i] >> 16) & 0xFF;
    data_l_h[i] = (byte_divide[i] >> 8) & 0xFF;
    data_l_l[i] = (byte_divide[i] >> 0) & 0xFF;
  }
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
  obj.putc(HEAD_BYTE);
  checksum_send += HEAD_BYTE;
  obj.putc(STX);
  checksum_send += STX;
  for(int i = 0; i < 9; ++i){
    for(int k = 0; k < 5; ++k){
      obj.putc(sendFormat[i][k]);
      checksum_send += sendFormat[i][k];
    }
  }
  //send checksum
  obj.putc(checksum_send);
}
//DigitalOut led(LED1);
void serialReceive(float *receive_result, Serial &obj){
  uint8_t got_data{};
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
    {8, 0, 0, 0, 0}
  };
  got_data = static_cast<uint8_t>(obj.getc());
  if(got_data == HEAD_BYTE){
    //led = 1;
    got_data = static_cast<uint8_t>(obj.getc());
    if(got_data == STX){
      //led = 1;
      checksum_receive += HEAD_BYTE;
      checksum_receive += STX;
      for(int k = 0; k < 9; ++k){
        for(int i = 0; i < 5; ++i){
          receive_data[i] = static_cast<uint8_t>(obj.getc());
          checksum_receive += receive_data[i];
          int temp_data  = static_cast<int>(receive_data[0]);
          if((0 <= temp_data) && (temp_data <= 9)){
            receiveFormat[receive_data[0]][i] = receive_data[i];
          }
        }
      }
      got_data = static_cast<uint8_t>(obj.getc());
      if(got_data == checksum_receive){
        //led = 0;
        int32_t result[2];
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
#endif
