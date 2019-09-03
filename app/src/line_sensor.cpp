#include "line_sensor.hpp"
#include <iostream>

LineSensor::LineSensor(std::string device)
{
    intensity_chars_ = {' ', '.', ':', '-', '=', '+', '*', '#', '%', '@'};
    if ((fd = open(device.c_str(), O_RDWR)) < 0)
    {
        printf("Failed to open i2c port\n");
    }
    if (ioctl(fd, I2C_SLAVE, 0x06) < 0)
    {
        printf("Unable to get bus access to talk to slave\n");
    }
}

int LineSensor::readSensor()
{
    char _reg[1];
    _reg[0] = {0x01};
    write(fd, _reg, 1);
    usleep(10000);
    char values[8] = {0};
    read(fd, values, 8);
    for (int s = 0; s < 6; s++)
    {
        read_val[s] = (values[s] << 2) | ((values[6 + int(s / 4)] >> (2 * (s % 4))) & 0x03);
    }
    return 0;
}

// from https://github.com/pololu/libpololu-avr
// Operates the same as read calibrated, but also returns an
// estimated position of the robot with respect to a line. The
// estimate is made using a weighted average of the sensor indices
// multiplied by 1000, so that a return value of 0 indicates that
// the line is directly below sensor 0, a return value of 1000
// indicates that the line is directly below sensor 1, 2000
// indicates that it's below sensor 2000, etc.  Intermediate
// values indicate that the line is between two sensors.  The
// formula is:
//
//    0*value0 + 1000*value1 + 2000*value2 + ...
//   --------------------------------------------
//         value0  +  value1  +  value2 + ...
//
// By default, this function assumes a dark line (high values)
// surrounded by white (low values).  If your line is light on
// black, set the optional second argument white_line to true.  In
// this case, each sensor value will be replaced by (1000-value)
// before the averaging.

unsigned int LineSensor::readLine()
{
    unsigned char i, on_line = 0;
    unsigned long avg;         // this is for the weighted total, which is long
                               // before division
    unsigned int sum;          // this is for the denominator which is <= 64000
    static int last_value = 0; // assume initially that the line is left.

    unsigned int _numSensors = 6;
    avg = 0;
    sum = 0;

    for (i = 0; i < _numSensors; i++)
    {
        int value = read_val[i];

        // keep track of whether we see the line at all
        if (value > 500)
        {
            on_line = 1;
        }

        // only average in values that are above a noise threshold
        if (value > 200)
        {
            avg += (long)(value) * (i * 1000);
            sum += value;
        }
    }

    if (!on_line)
    {
        // If it last read to the left of center, return 0.
        if (last_value < (_numSensors - 1) * 1000 / 2)
            return 0;

        // If it last read to the right of center, return the max.
        else
            return (_numSensors - 1) * 1000;
    }

    last_value = avg / sum;

    return last_value;
}

char LineSensor::getIntensity(int index)
{
    int sensor_value = read_val[index];
    return intensity_chars_[9 - 9 * sensor_value / 1024.0];
}
