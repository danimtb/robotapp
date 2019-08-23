#include <iostream>

#include <gopigo-wrapper.h>

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
    gopigo::init();

    while(1)
    {
        std::cout << "BEAT" << std::endl;
    }

    return 0;
}
