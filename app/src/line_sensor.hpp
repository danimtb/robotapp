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


class LineSensor
{
public:
	LineSensor();
	~LineSensor () = default;
	int readSensor();
	int getVal(int index) {return read_val[index];};
	char getIntensity(int index);
	unsigned int readLine();
	void maskLeft();
	void maskRight();
	void resetMask();

private:
	int fd;
	uint16_t read_val[6];
	std::array<char, 10> intensity_chars_;
	float weights[6];
};
