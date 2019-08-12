#include <iostream>

#ifdef WIRINGPI
    #include <wiringPi.h>
    #include <softServo.h>
#endif


int main (void)
{
    #ifdef WIRINGPI
        wiringPiSetup();
        softServoSetup(0);
    #endif

    while(1)
    {
        std::cout << "HIGH" << std::endl;
        #ifdef WIRINGPI
            softServoWrite(0, 500);
            delay(5000);
            softServoWrite(0, 0);
            delay(5000);
        #endif
    }

    return 0;
}
