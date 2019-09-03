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


class LineSensor
{
public:
	LineSensor() = delete;
	LineSensor(std::string device);
	~LineSensor () = default;
	int readSensor();
	int getVal(int index) {return read_val[index];};
	char getIntensity(int index);
	unsigned int readLine();
private:
	int fd;
	uint16_t read_val[6];
	std::array<char, 10> intensity_chars_;
};
