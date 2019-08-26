#include "line_sensor.hpp"

LineSensor::LineSensor(std::string device)
{
    intensity_chars_ = {' ','.',':','-','=','+','*','#','%','@'};
    if ((fd = open(device.c_str(), O_RDWR)) < 0)
    {
        printf("Failed to open i2c port\n");
    }
    if (ioctl(fd, I2C_SLAVE, address) < 0)
    {
        printf("Unable to get bus access to talk to slave\n");
    }
}

int LineSensor::readSensor()
{
    int flag = 0;
    writeBlock(line_read_cmd, 0, 0, 0); // This Write statement takes 0.895ms
    int reg_size = 10;
    //Used to read the data from line sensor using i2c - read(int fildes, void *buf, size_t nbyte);
    ssize_t ret = read(fd, r_buf, reg_size); //This read statement takes 1.635ms to read from 5 IR sensors
    if (ret != reg_size)
    {
        if (ret == -1)
        {
            printf("Unable to read from Line Sensor (errno %i): %s\n", errno, strerror(errno));
        }
        else
        {
            printf("Unable to read from Line_Sensor\n");
        }
        return -1;
    }
    for (i = 0; i < 10; i = i + 2)
    {                                                    // To convert the 10 bit analog reading of each sensor to decimal and store it in read_val[]
        read_val[i / 2] = r_buf[i] * 256 + r_buf[i + 1]; // Values less than 100 - White, Values greater than 800- Black
        if (read_val[i / 2] > 65000)                     // Checking for junk values in the input
            flag = 1;
    }
    if (flag == 1)
    {
        for (i = 0; i < 5; i++)
            read_val[i] = -1; // Making junk input values to -1
    }
    return 0;
}

int LineSensor::writeBlock(char cmd, char v1, char v2, char v3)
{
    w_buf[0] = 1;
    w_buf[1] = cmd;
    w_buf[2] = v1;
    w_buf[3] = v2;
    w_buf[4] = v3;

    ssize_t ret = write(fd, w_buf, WRITE_BUF_SIZE);
    // sleep for 1 ms to prevent too fast writing
    usleep(1);

    if (ret != WRITE_BUF_SIZE)
    {
        if (ret == -1)
        {
            printf("Error writing to GoPiGo (errno %i): %s\n", errno, strerror(errno));
        }
        else
        {
            printf("Error writing to GoPiGo\n");
        }
        return ret;
    }
    return 1;
}

char LineSensor::getIntensity(int index) {
    int sensor_value = read_val[index];
    return intensity_chars_[9-9*sensor_value/1024.0];
}

char LineSensor::readByte()
{
    int reg_size = 1;
    ssize_t ret = read(fd, r_buf, reg_size);

    if (ret != reg_size)
    {
        if (ret == -1)
        {
            printf("Unable to read from GoPiGo (errno %i): %s\n", errno, strerror(errno));
        }
        else
        {
            printf("Unable to read from GoPiGo\n");
        }
        return -1;
    }

    return r_buf[0];
}
