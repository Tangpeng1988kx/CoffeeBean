#include "Motor.h"
#include "esp32-hal-gpio.h"
void motor::MotorStart()
{
    digitalWrite(5,HIGH);
}
void motor::MotorStop()
{
    digitalWrite(5,LOW);
}
