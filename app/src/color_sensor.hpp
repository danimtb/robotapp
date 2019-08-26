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

class ColorSensor {
    public:
        ColorSensor() = delete;
        ColorSensor(std::string device);
        ~ColorSensor () = default;
        void readSensor();
    	int file;
};