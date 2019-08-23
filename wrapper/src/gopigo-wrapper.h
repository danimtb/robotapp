namespace gopigo
{
    //Initialize
    int init(void);
    //Write a register
    int write_block(char cmd,char v1,char v2,char v3);
    //Read 1 byte of data
    char read_byte(void);
    //Get voltage
    float volt(void);

    //Sleep in ms
    void pi_sleep(int t);

    //Control Motor 1
    int motor1(int direction,int speed);

    //Control Motor 2
    int motor2(int direction,int speed);

    //Move the GoPiGo forward
    int fwd(void);

    //Move the GoPiGo forward without PID
    int motor_fwd(void);

    //Move GoPiGo back
    int bwd(void);

    //Move GoPiGo back without PID control
    int motor_bwd(void);

    //Turn GoPiGo Left slow (one motor off, better control)
    int left(void);

    //Rotate GoPiGo left in same position (both motors moving in the opposite direction)
    int left_rot(void);

    //Turn GoPiGo right slow (one motor off, better control)
    int right(void);

    //Rotate GoPiGo right in same position both motors moving in the opposite direction)
    int right_rot(void);

    //Stop the GoPiGo
    int stop(void);

    //Increase the speed
    int increase_speed(void);

    //Decrease the speed
    int decrease_speed(void);

    //Trim test with the value specified
    int trim_test(int value);

    //Read the trim value in    EEPROM if present else return -3
    int trim_read(void);

    //Write the trim value to EEPROM, where -100=0 and 100=200
    int trim_write(int value);

    // Arduino Digital Read
    int digitalRead(int pin);

    // Arduino Digital Write
    int digitalWrite(int pin, int value);

    // Setting Up Pin mode on Arduino
    int pinMode(int pin, char * mode);

    // Read analog value from Pin
    int analogRead(int pin);

    // Write PWM
    int analogWrite(int pin, int value);

    //Read board revision
    //    return:    voltage in V
    int brd_rev(void);

    //Read ultrasonic sensor
    //    arg:
    //        pin ->     Pin number on which the US sensor is connected
    //    return:        distance in cm
    int us_dist(int pin);

    //Read motor speed (0-255)
    //    arg:
    //        speed -> pointer to array of 2 bytes [motor1, motor2], allocated before
    void read_motor_speed(unsigned char* speed);

    //Turn led on or off
    //    arg:
    //        l_id: 1 for left LED and 0 for right LED
    //        onoff: 0 off, 1 on
    int led_toggle(int l_id, bool onoff);

    //Turn led on
    //    arg:
    //        l_id: 1 for left LED and 0 for right LED
    int led_on(int l_id);

    //Turn led off
    //    arg:
    //        l_id: 1 for left LED and 0 for right LED
    int led_off(int l_id);

    //Set servo position
    //    arg:
    //        position: angle in degrees to set the servo at
    int servo(int position);

    //Returns the firmware version
    int fw_ver(void);

    //Set speed of the left motor
    //    arg:
    //        speed-> 0-255
    int set_left_speed(int speed);

    //Set speed of the right motor
    //    arg:
    //        speed-> 0-255
    int set_right_speed(int speed);

    //Set speed of the both motors
    //    arg:
    //        speed-> 0-255
    int set_speed(int speed);

    //Read encoder value
    //    arg:
    //        motor ->     0 for motor1 and 1 for motor2
    //    return:        distance in cm
    int enc_read(int motor);

    // Enable the encoders (enabled by default)
    int enable_encoders(void);

    // Disable the encoders (use this if you don't want to use the encoders)
    int disable_encoders(void);

    //Set encoder targeting on
    //arg:
    //    m1: 0 to disable targeting for m1, 1 to enable it
    //    m2:    1 to disable targeting for m2, 1 to enable it
    //    target: number of encoder pulses to target (18 per revolution)
    int enc_tgt(int m1, int m2, int target);

    //Read encoder status
    //    return:    0 if encoder target is reached
    int read_enc_status(void);

    //Read timeout status
    //    return:    0 if timeout is reached
    int read_timeout_status(void);
}