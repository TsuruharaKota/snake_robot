
#ifndef SERIAL_H_
#define SERIAL_H_

#include"mbed.h"
#include"string.h"

#define HEAD_BYTE 0xFF
#define STX 0x02

void serialSend(float *send_data, Serial &pc){
    std::string write_str;
    for(int i = 0; i < 9; ++i){
        write_str += std::to_string(send_data[i]);
        if(i != 8){
            write_str.push_back(',');
        }else{
            write_str.push_back('\n');
        }
    }
    pc.printf("%c", HEAD_BYTE);
    pc.printf("%c", STX);
    uint8_t write_str_size = write_str.size();
    pc.printf("%c", write_str_size);
    pc.printf("%s", write_str.c_str());
}
void serialReceive(float *receive_result, Serial &pc){
    const int kMaxBufferSize = 100;
    char      buffer[kMaxBufferSize];
    int       len = 0;
    while (pc.readable()) {
        std::string data_str = "0, 0, 0, 0, 0, 0, 0, 0, 0";
        buffer[0] = '\0';
        char *ptr;
        char new_char = pc.getc(); 
            
        buffer[len++] = new_char;    
        buffer[len] = '\0';          
        
        // new message received, handle
        if (new_char == '\n') {
            printf("%s",buffer);
            if(0 < len){
                for(int i = 0; i < len; ++i){
                    data_str.push_back(buffer[i]);
                }
            }
            char *str = const_cast<char *>(data_str.c_str());
            ptr = strtok(str, ",");
            //printf("%s\n", ptr);
            receive_result[0] = std::stof(ptr);
            int i = 0;
            while(ptr != NULL){
                ++i;
                ptr = strtok(NULL, ",");
                if(ptr != NULL){
                    //printf("%s\n", ptr);
                    receive_result[i] = std::stof(ptr);
                }
            }
            len = 0;
        }
    }
}
#endif