#include "gopigo.h"
#include "gopigo-wrapper.h"


//Initialize
int gopigo::init(void)
{
    return init();
}

//Write a register
int gopigo::write_block(char cmd, char v1, char v2, char v3)
{
    return write_block(cmd, v1, v2, v3);
}

//Read 1 byte of data
char gopigo::read_byte(void)
{
    return read_byte();
}

//Get voltage
float gopigo::volt(void)
{
    return volt();
}

//Sleep in ms
void gopigo::pi_sleep(int t)
{
    pi_sleep(t);
}

//Control Motor 1
int gopigo::motor1(int direction, int speed)
{
    return motor1(direction, speed);
}

//Control Motor 2
int gopigo::motor2(int direction, int speed)
{
    return motor2(direction, speed);
}

//Move the GoPiGo forward
int gopigo::fwd(void)
{
    return fwd();
}

//Move the GoPiGo forward without PID
int gopigo::motor_fwd(void)
{
    return motor_fwd();
}

//Move GoPiGo back
int gopigo::bwd(void)
{
    return gopigo::bwd();
}

//Move GoPiGo back without PID control
int gopigo::motor_bwd(void)
{
    return motor_bwd();
}

//Turn GoPiGo Left slow (one motor off, better control)
int gopigo::left(void)
{
    return left();
}

//Rotate GoPiGo left in same position (both motors moving in the opposite direction)
int gopigo::left_rot(void)
{
    return left_rot();
}

//Turn GoPiGo right slow (one motor off, better control)
int gopigo::right(void)
{
    return right();
}

//Rotate GoPiGo right in same position both motors moving in the opposite direction)
int gopigo::right_rot(void)
{
    return right_rot();
}

//Stop the GoPiGo
int gopigo::stop(void)
{
    return stop();
}

//Increase the speed
int gopigo::increase_speed(void)
{
    return increase_speed();
}

//Decrease the speed
int gopigo::decrease_speed(void)
{
    return decrease_speed();
}

//Trim test with the value specified
int gopigo::trim_test(int value)
{
    return trim_test(value);
}

//Read the trim value in    EEPROM if present else return -3
int gopigo::trim_read(void)
{
    return trim_read();
}

//Write the trim value to EEPROM, where -100=0 and 100=200
int gopigo::trim_write(int value)
{
    return trim_write(value);
}

// Arduino Digital Read
int gopigo::digitalRead(int pin)
{
    return digitalRead(pin);
}

// Arduino Digital Write
int gopigo::digitalWrite(int pin, int value)
{
    return digitalWrite(pin, value);
}

// Setting Up Pin mode on Arduino
int gopigo::pinMode(int pin, char * mode)
{
    return pinMode(pin, mode);
}

// Read analog value from Pin
int gopigo::analogRead(int pin)
{
    return analogRead(pin);
}

// Write PWM
int gopigo::analogWrite(int pin, int value)
{
    return analogWrite(pin, value);
}

//Read board revision
//    return:    voltage in V
int gopigo::brd_rev(void)
{
    return brd_rev();
}

//Read ultrasonic sensor
//    arg:
//        pin ->     Pin number on which the US sensor is connected
//    return:        distance in cm
int gopigo::us_dist(int pin)
{
    return us_dist(pin);
}

//Read motor speed (0-255)
//    arg:
//        speed -> pointer to array of 2 bytes [motor1, motor2], allocated before
void gopigo::read_motor_speed(unsigned char* speed)
{
    read_motor_speed(speed);
}

//Turn led on or off
//    arg:
//        l_id: 1 for left LED and 0 for right LED
//        onoff: 0 off, 1 on
int gopigo::led_toggle(int l_id, bool onoff)
{
    return led_toggle(l_id, onoff);
}

//Turn led on
//    arg:
//        l_id: 1 for left LED and 0 for right LED
int gopigo::led_on(int l_id)
{
    return led_on(l_id);
}

//Turn led off
//    arg:
//        l_id: 1 for left LED and 0 for right LED
int gopigo::led_off(int l_id)
{
    return led_off(l_id);
}

//Set servo position
//    arg:
//        position: angle in degrees to set the servo at
int gopigo::servo(int position)
{
    return servo(position);
}

//Returns the firmware version
int gopigo::fw_ver(void)
{
    return fw_ver();
}

//Set speed of the left motor
//    arg:
//        speed-> 0-255
int gopigo::set_left_speed(int speed)
{
    return set_left_speed(speed);
}

//Set speed of the right motor
//    arg:
//        speed-> 0-255
int gopigo::set_right_speed(int speed)
{
    return set_right_speed(speed);
}

//Set speed of the both motors
//    arg:
//        speed-> 0-255
int gopigo::set_speed(int speed)
{
    return set_speed(speed);
}

//Read encoder value
//    arg:
//        motor ->     0 for motor1 and 1 for motor2
//    return:        distance in cm
int gopigo::enc_read(int motor)
{
    return enc_read(motor);
}

// Enable the encoders (enabled by default)
int gopigo::enable_encoders(void)
{
    return enable_encoders();
}

// Disable the encoders (use this if you don't want to use the encoders)
int gopigo::disable_encoders(void)
{
    return disable_encoders();
}

//Set encoder targeting on
//arg:
//    m1: 0 to disable targeting for m1, 1 to enable it
//    m2:    1 to disable targeting for m2, 1 to enable it
//    target: number of encoder pulses to target (18 per revolution)
int gopigo::enc_tgt(int m1, int m2, int target)
{
    return enc_tgt(m1, m2, target);
}

//Read encoder status
//    return:    0 if encoder target is reached
int gopigo::read_enc_status(void)
{
    return read_enc_status();
}

//Read timeout status
//    return:    0 if timeout is reached
int gopigo::read_timeout_status(void)
{
    return read_timeout_status();
}
