#ifndef _MOTOR_H_
#define _MOTOR_H_
#include "esp32-hal-ledc.h"
class motor
{
    public:
    static void MotorStart();
    static void MotorStop();
};
#endif