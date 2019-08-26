#pragma once

#include <string>
#include <array>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdexcept>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>

#define WRITE_BUF_SIZE       5
#define READ_BUF_SIZE        32
#define line_read_cmd        3
#define i2c_address              0x06

class LineSensor
{
public:
	LineSensor() = delete;
	LineSensor(std::string device);
	~LineSensor () = default;
	int readSensor();
	int getVal(int index) {return read_val[index];};
	char getIntensity(int index);
private:
	int writeBlock(char cmd,char v1,char v2,char v3);
	char readByte();
	int fd;
	unsigned char w_buf[WRITE_BUF_SIZE];
	unsigned char r_buf[READ_BUF_SIZE];
	int read_val[5];
	int i=0;
	unsigned long reg_addr=0;
	std::array<char, 10> intensity_chars_;
};
