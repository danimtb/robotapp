#include "color_sensor.hpp"


ColorSensor::ColorSensor(std::string device)
{
	if ((file = open(device.c_str(), O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, TCS34725 I2C address is 0x29(41)
	ioctl(file, I2C_SLAVE, 0x29);

	// Select enable register(0x80)
	// Power ON, RGBC enable, wait time disable(0x03)
	char config[2] = {0};
	config[0] = 0x80;
	config[1] = 0x03;
	write(file, config, 2);
	// Select ALS time register(0x81)
	// Atime = 700 ms(0x00)
	config[0] = 0x81;
	config[1] = 0x00;
	write(file, config, 2);
	// Select Wait Time register(0x83)
	// WTIME : 2.4ms(0xFF)
	config[0] = 0x83;
	config[1] = 0xFF;
	write(file, config, 2);
	// Select control register(0x8F)
	// AGAIN = 1x(0x00)
	config[0] = 0x8F;
	config[1] = 0x00;
	write(file, config, 2);
	usleep(1000);
}

void ColorSensor::readSensor() {
	// Read 8 bytes of data from register(0x94)
	// cData lsb, cData msb, red lsb, red msb, green lsb, green msb, blue lsb, blue msb
	char reg[1] = {0x94};
	write(file, reg, 1);
	char data[8] = {0};
	if(read(file, data, 8) != 8)
	{
		printf("Erorr : Input/output Erorr \n");
	}
	else
	{
		// Convert the data
		int cData = (data[1] * 256 + data[0]);
		int red = (data[3] * 256 + data[2]);
		int green = (data[5] * 256 + data[4]);
		int blue = (data[7] * 256 + data[6]);

		// Calculate luminance
		float luminance = (-0.32466) * (red) + (1.57837) * (green) + (-0.73191) * (blue);
		if(luminance < 0)
		{
			luminance = 0;
		}

		// Output data to screen
		printf("Red color luminance : %d lux \n", red);
		printf("Green color luminance : %d lux \n", green);
		printf("Blue color luminance : %d lux \n", blue);
		printf("IR  luminance : %d lux \n", cData);
		printf("Ambient Light Luminance : %.2f lux \n", luminance);
	} 
}