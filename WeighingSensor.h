/*称重传感器*/
#ifndef _WEIGHING_SENSOR_H_
#define _WEIGHING_SENSOR_H_
#include "stdint.h"
class WeighingSensor
{
public:
    static uint16_t readWeigtValue();
    static uint16_t readValWeight();
};

#endif
