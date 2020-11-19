#include<iostream>
#include<cstdint>
#include<cstring>
#include<pigpiod_if2.h>
#include<iostream>

using std::memcpy;

#define HEAD_BYTE 0xFF
#define STX 0x02

const char *port = "/dev/ttyAMA0";
int baudrate = 115200;
int pi = pigpio_start(0, 0);
int serial_handle{};
unsigned char dummy_flag{};

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

  uint8_t got_data{};
  while (1) {
    got_data = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
    std::cout << static_cast<int>(got_data) << std::endl;
  }
  serial_close(pi, serial_handle);
}
