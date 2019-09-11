#include "color_sensor.hpp"
#include <math.h>
#include <iostream>

#define min_f(a, b, c)  (fminf(a, fminf(b, c)))
#define max_f(a, b, c)  (fmaxf(a, fmaxf(b, c)))

/*!
 *  @brief  Constructor
 *  @param  it
 *          Integration Time
 *  @param  gain
 *          Gain
 */
ColorSensor::ColorSensor(std::string device, tcs34725IntegrationTime_t it,
						 tcs34725Gain_t gain)
{
	_device = device;
	_tcs34725Initialised = false;
	_tcs34725IntegrationTime = it;
	_tcs34725Gain = gain;
}

/*!
 *  @brief  Writes a register and an 8 bit value over I2C
 *  @param  reg
 *  @param  value
 */
void ColorSensor::write8(uint8_t reg, uint32_t value)
{
	char config[2] = {0};
	config[0] = (TCS34725_COMMAND_BIT | reg);
	config[1] = value & 0xFF;
	write(_file, config, 2);
}

/*!
 *  @brief  Reads an 8 bit value over I2C
 *  @param  reg
 *  @return value
 */
uint8_t ColorSensor::read8(uint8_t reg)
{
	char _reg[1]; _reg[0]=(TCS34725_COMMAND_BIT | reg);
	write(_file, _reg, 1);
	char data[1] = {0};
	read(_file, data, 1);
	return data[0];
}

/*!
 *  @brief  Reads a 16 bit values over I2C
 *  @param  reg
 *  @return value
 */
uint16_t ColorSensor::read16(uint8_t reg)
{
	uint16_t x;
	uint16_t t;
	char _reg[1]; _reg[0]=TCS34725_COMMAND_BIT | reg;
	write(_file, _reg, 1);
	char data[2] = {0};
	read(_file, data, 2);
	t = data[1];
	x = data[0];
	x <<= 8;
	x |= t;
	return x;
}

void ColorSensor::sleep() {
	switch (_tcs34725IntegrationTime)
	{
	case TCS34725_INTEGRATIONTIME_2_4MS:
		usleep(3000);
		break;
	case TCS34725_INTEGRATIONTIME_24MS:
		usleep(24000);
		break;
	case TCS34725_INTEGRATIONTIME_50MS:
		usleep(50000);
		break;
	case TCS34725_INTEGRATIONTIME_101MS:
		usleep(101000);
		break;
	case TCS34725_INTEGRATIONTIME_154MS:
		usleep(154000);
		break;
	case TCS34725_INTEGRATIONTIME_700MS:
		usleep(700000);
		break;
	}
}

/*!
 *  @brief  Enables the device
 */
void ColorSensor::enable()
{
	write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
	usleep(3000);
	write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
	//usleep(3000);
	//write8(TCS34725_WTIME, TCS34725_WTIME_2_4MS);
	/* Set a usleep for the integration time.
    This is only necessary in the case where enabling and then
    immediately trying to read values back. This is because setting
    AEN triggers an automatic integration, so if a read RGBC is
    performed too quickly, the data is not yet valid and all 0's are
    returned */
	sleep();
}

/*!
 *  @brief  Disables the device (putting it in lower power sleep mode)
 */
void ColorSensor::disable()
{
	/* Turn the device off to save power */
	uint8_t reg = 0;
	reg = read8(TCS34725_ENABLE);
	write8(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}

/*!
 *  @brief  Initializes I2C and configures the sensor
 *  @param  addr
 *          i2c address
 *  @return True if initialization was successful, otherwise false.
 */
bool ColorSensor::begin(uint8_t addr)
{
	_i2caddr = addr;
	return init();
}

/*!
 *  @brief  Initializes I2C and configures the sensor
 *  @return True if initialization was successful, otherwise false.
 */
bool ColorSensor::begin()
{
	_i2caddr = TCS34725_ADDRESS;
	return init();
}

/*!
 *  @brief  Part of begin
 *  @return True if initialization was successful, otherwise false.
 */
bool ColorSensor::init()
{
	if ((_file = open(_device.c_str(), O_RDWR)) < 0)
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, TCS34725 I2C address is 0x29(41)
	ioctl(_file, I2C_SLAVE, _i2caddr);
	/* Make sure we're actually connected */
	uint8_t x = read8(TCS34725_ID);
	std::cout << "--->>" << x << std::endl;
	//if ((x != 0x44) && (x != 0x10))
	//{
	//	return false;
	//}
	_tcs34725Initialised = true;

	/* Set default integration time and gain */
	setIntegrationTime(_tcs34725IntegrationTime);
	setGain(_tcs34725Gain);

	/* Note: by default, the device is in power down mode on bootup */
	enable();

	return true;
}

/*!
 *  @brief  Sets the integration time for the TC34725
 *  @param  it
 *          Integration Time
 */
void ColorSensor::setIntegrationTime(tcs34725IntegrationTime_t it)
{
	if (!_tcs34725Initialised)
		begin();

	/* Update the timing register */
	write8(TCS34725_ATIME, it);

	/* Update value placeholders */
	_tcs34725IntegrationTime = it;
}

/*!
 *  @brief  Adjusts the gain on the TCS34725
 *  @param  gain
 *          Gain (sensitivity to light)
 */
void ColorSensor::setGain(tcs34725Gain_t gain)
{
	if (!_tcs34725Initialised)
		begin();

	/* Update the timing register */
	write8(TCS34725_CONTROL, gain);

	/* Update value placeholders */
	_tcs34725Gain = gain;
}

/*!
 *  @brief  Reads the raw red, green, blue and clear channel values
 *  @param  *r
 *          Red value
 *  @param  *g
 *          Green value
 *  @param  *b
 *          Blue value
 *  @param  *c
 *          Clear channel value
 */
void ColorSensor::getRawData(uint16_t *r, uint16_t *g, uint16_t *b,
							 uint16_t *c)
{
	if (!_tcs34725Initialised)
		begin();

    setInterrupt(true);
	char reg[1] = {0x94};
	write(_file, reg, 1);
	char data[8] = {0};
	if(read(_file, data, 8) != 8)
	{
		printf("Erorr : Input/output Erorr \n");
	}
	else
	{
		*c = (data[1] * 256 + data[0]);
		*r = (data[3] * 256 + data[2]);
		*g = (data[5] * 256 + data[4]);
		*b = (data[7] * 256 + data[6]);
	}
	sleep();
}

/*!
 *  @brief  Reads the raw red, green, blue and clear channel values in
 *          one-shot mode (e.g., wakes from sleep, takes measurement, enters
 *          sleep)
 *  @param  *r
 *          Red value
 *  @param  *g
 *          Green value
 *  @param  *b
 *          Blue value
 *  @param  *c
 *          Clear channel value
 */
void ColorSensor::getRawDataOneShot(uint16_t *r, uint16_t *g, uint16_t *b,
									uint16_t *c)
{
	if (!_tcs34725Initialised)
		begin();

	enable();
	getRawData(r, g, b, c);
	disable();
}

/*!
 *  @brief  Read the RGB color detected by the sensor.
 *  @param  *r
 *          Red value normalized to 0-255
 *  @param  *g
 *          Green value normalized to 0-255
 *  @param  *b
 *          Blue value normalized to 0-255
 */
void ColorSensor::getRGB(float *r, float *g, float *b)
{
	uint16_t red, green, blue, clear;
	getRawData(&red, &green, &blue, &clear);
	uint32_t sum = clear;

	// Avoid divide by zero errors ... if clear = 0 return black
	if (clear == 0)
	{
		*r = *g = *b = 0;
		return;
	}

	*r = (float)red / sum * 255.0;
	*g = (float)green / sum * 255.0;
	*b = (float)blue / sum * 255.0;
}

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)
void ColorSensor::getHSV(float *h, float *s, float *v )
{
	float r,g,b;
	getRGB(&r,&g,&b);
	r=r/255.0; g=g/255.0; b=b/255.0;
	float min, max, delta;
	min = min_f( r, g, b );
	max = max_f( r, g, b );
	*v = max;				// v
	delta = max - min;
	if( max != 0 )
		*s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}
	if( r == max )
		*h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan
	*h *= 60;				// degrees
	if( *h < 0 )
		*h += 360;
}

/*!
 *  @brief  Converts the raw R/G/B values to color temperature in degrees Kelvin
 *  @param  r
 *          Red value
 *  @param  g
 *          Green value
 *  @param  b
 *          Blue value
 *  @return Color temperature in degrees Kelvin
 */
uint16_t ColorSensor::calculateColorTemperature(uint16_t r, uint16_t g,
												uint16_t b)
{
	float X, Y, Z; /* RGB to XYZ correlation      */
	float xc, yc;  /* Chromaticity co-ordinates   */
	float n;	   /* McCamy's formula            */
	float cct;

	if (r == 0 && g == 0 && b == 0)
	{
		return 0;
	}

	/* 1. Map RGB values to their XYZ counterparts.    */
	/* Based on 6500K fluorescent, 3000K fluorescent   */
	/* and 60W incandescent values for a wide range.   */
	/* Note: Y = Illuminance or lux                    */
	X = (-0.14282F * r) + (1.54924F * g) + (-0.95641F * b);
	Y = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
	Z = (-0.68202F * r) + (0.77073F * g) + (0.56332F * b);

	/* 2. Calculate the chromaticity co-ordinates      */
	xc = (X) / (X + Y + Z);
	yc = (Y) / (X + Y + Z);

	/* 3. Use McCamy's formula to determine the CCT    */
	n = (xc - 0.3320F) / (0.1858F - yc);

	/* Calculate the final CCT */
	cct =
		(449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;

	/* Return the results in degrees Kelvin */
	return (uint16_t)cct;
}

/*!
 *  @brief  Converts the raw R/G/B values to color temperature in degrees
 *          Kelvin using the algorithm described in DN40 from Taos (now AMS).
 *  @param  r
 *          Red value
 *  @param  g
 *          Green value
 *  @param  b
 *          Blue value
 *  @param  c
 *          Clear channel value
 *  @return Color temperature in degrees Kelvin
 */
uint16_t ColorSensor::calculateColorTemperature_dn40(uint16_t r,
													 uint16_t g,
													 uint16_t b,
													 uint16_t c)
{
	uint16_t r2, b2; /* RGB values minus IR component */
	uint16_t sat;	/* Digital saturation level */
	uint16_t ir;	 /* Inferred IR content */

	if (c == 0)
	{
		return 0;
	}

	/* Analog/Digital saturation:
   *
   * (a) As light becomes brighter, the clear channel will tend to
   *     saturate first since R+G+B is approximately equal to C.
   * (b) The TCS34725 accumulates 1024 counts per 2.4ms of integration
   *     time, up to a maximum values of 65535. This means analog
   *     saturation can occur up to an integration time of 153.6ms
   *     (64*2.4ms=153.6ms).
   * (c) If the integration time is > 153.6ms, digital saturation will
   *     occur before analog saturation. Digital saturation occurs when
   *     the count reaches 65535.
   */
	if ((256 - _tcs34725IntegrationTime) > 63)
	{
		/* Track digital saturation */
		sat = 65535;
	}
	else
	{
		/* Track analog saturation */
		sat = 1024 * (256 - _tcs34725IntegrationTime);
	}

	/* Ripple rejection:
   *
   * (a) An integration time of 50ms or multiples of 50ms are required to
   *     reject both 50Hz and 60Hz ripple.
   * (b) If an integration time faster than 50ms is required, you may need
   *     to average a number of samples over a 50ms period to reject ripple
   *     from fluorescent and incandescent light sources.
   *
   * Ripple saturation notes:
   *
   * (a) If there is ripple in the received signal, the value read from C
   *     will be less than the max, but still have some effects of being
   *     saturated. This means that you can be below the 'sat' value, but
   *     still be saturating. At integration times >150ms this can be
   *     ignored, but <= 150ms you should calculate the 75% saturation
   *     level to avoid this problem.
   */
	if ((256 - _tcs34725IntegrationTime) <= 63)
	{
		/* Adjust sat to 75% to avoid analog saturation if atime < 153.6ms */
		sat -= sat / 4;
	}

	/* Check for saturation and mark the sample as invalid if true */
	if (c >= sat)
	{
		return 0;
	}

	/* AMS RGB sensors have no IR channel, so the IR content must be */
	/* calculated indirectly. */
	ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;

	/* Remove the IR component from the raw RGB values */
	r2 = r - ir;
	b2 = b - ir;

	if (r2 == 0)
	{
		return 0;
	}

	/* A simple method of measuring color temp is to use the ratio of blue */
	/* to red light, taking IR cancellation into account. */
	uint16_t cct = (3810 * (uint32_t)b2) / /** Color temp coefficient. */
					   (uint32_t)r2 +
				   1391; /** Color temp offset. */

	return cct;
}

/*!
 *  @brief  Converts the raw R/G/B values to lux
 *  @param  r
 *          Red value
 *  @param  g
 *          Green value
 *  @param  b
 *          Blue value
 *  @return Lux value
 */
uint16_t ColorSensor::calculateLux(uint16_t r, uint16_t g, uint16_t b)
{
	float illuminance;

	/* This only uses RGB ... how can we integrate clear or calculate lux */
	/* based exclusively on clear since this might be more reliable?      */
	illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);

	return (uint16_t)illuminance;
}

/*!
 *  @brief  Sets inerrupt for TCS34725
 *  @param  i
 *          Interrupt (True/False)
 */
void ColorSensor::setInterrupt(bool i)
{
	write8(TCS34725_PERS,TCS34725_PERS_NONE);
	uint8_t r = read8(TCS34725_ENABLE);
	if (i) {
		r |= TCS34725_ENABLE_AIEN;
	} else {
		r &= ~TCS34725_ENABLE_AIEN;
	}
	write8(TCS34725_ENABLE, r);
	/*
	write8(TCS34725_PERS,TCS34725_PERS_NONE);
	uint8_t _enable = read8(TCS34725_ENABLE);
	if (i) {
		_enable |= TCS34725_ENABLE_AIEN;		
	}
	else {
		_enable &= ~TCS34725_ENABLE_AIEN;		
	}
	write8(TCS34725_ENABLE,_enable);
	
	
	*/
}

/*!
 *  @brief  Clears inerrupt for TCS34725
 */
void ColorSensor::clearInterrupt()
{
	char config[1] = {0};
	config[0] = TCS34725_COMMAND_BIT | 0x66;
	write(_file, config, 1);
}

/*!
 *  @brief  Sets inerrupt limits
 *  @param  low
 *          Low limit
 *  @param  high
 *          High limit
 */
void ColorSensor::setIntLimits(uint16_t low, uint16_t high)
{
	write8(0x04, low & 0xFF);
	write8(0x05, low >> 8);
	write8(0x06, high & 0xFF);
	write8(0x07, high >> 8);
}

std::string ColorSensor::getColor()
{
	uint16_t red=0, yellow=60, green=113, cyan=170, blue=238, pink=300;
	float h, s, v;
	getHSV(&h, &s, &v);
	//std::cout << h << " " << s << " " << v << std::endl;
	uint16_t margin = 20;
    std::string color;

	if (((360-red-margin) <= h  && h < 360) || (0 <= h && h < (red+margin)) && v>0.55)
	{
	    color = "red";
	}
	else if ((yellow-margin) <= h && h < (yellow+margin) && s>0.5 && v>0.4)
	{
	    color = "yellow";
	}
	else if ((green-margin) <= h  && h < (green+margin) && v>0.45)
	{
		color = "green";
	}
	  else if ((cyan-margin) <= h  && h < (cyan+margin) && v>0.35)
	{
		color = "cyan";
	}
	else if ((blue-margin) <= h  && h < (blue+margin))
	{
	    color = "blue";
	}
	else if ((pink-margin) <= h  && h < (pink+margin))
	{
	    color = "pink";
	}
	else
	{
		color = "unknown";
	}
	return color;
}
