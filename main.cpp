#include <iostream>

#ifdef WIRINGPI
    #include <wiringPi.h>
    #include <softServo.h>
#endif

#define M_RIGHT 3
#define M_RIGHT_FORWARD 1250
#define M_RIGHT_BACKWARD -250

#define M_LEFT 0
#define M_LEFT_FORWARD -250
#define M_LEFT_BACKWARD 1250

#define STOP 500

#define IR_RIGHT 5
#define IR_LEFT 4

#define BLACK 0
#define WHITE 1


int main (void)
{
    std::cout << "INIT" << std::endl;
    #ifdef WIRINGPI
        wiringPiSetup();
        softServoSetup(M_LEFT, M_RIGHT, -1, -1, -1, -1, -1, -1);
        pinMode(IR_RIGHT, INPUT);
        pinMode(IR_LEFT, INPUT);
    #endif

    while(1)
    {
        std::cout << "BEAT" << std::endl;
        #ifdef WIRINGPI

            if (digitalRead(IR_RIGHT) == BLACK && digitalRead(IR_LEFT) == BLACK)
            {
                std::cout << "FORWARD" << std::endl;
                softServoWrite(M_RIGHT, M_RIGHT_FORWARD);
                softServoWrite(M_LEFT, M_LEFT_FORWARD);
            }
            else if (digitalRead(IR_RIGHT) == BLACK && digitalRead(IR_LEFT) == WHITE)
            {
                std::cout << "RIGHT" << std::endl;
                softServoWrite(M_RIGHT, M_RIGHT_BACKWARD);
                softServoWrite(M_LEFT, M_LEFT_FORWARD);
            }
            else if (digitalRead(IR_RIGHT) == WHITE && digitalRead(IR_LEFT) == BLACK)
            {
                std::cout << "LEFT" << std::endl;
                softServoWrite(M_RIGHT, M_RIGHT_FORWARD);
                softServoWrite(M_LEFT, M_LEFT_BACKWARD);
            }
            else
            {
                std::cout << "BACKWARD" << std::endl;
                softServoWrite(M_RIGHT, M_RIGHT_BACKWARD);
                softServoWrite(M_LEFT, M_LEFT_BACKWARD);
            }
            delay(1000);
        #endif
    }

    return 0;
}
