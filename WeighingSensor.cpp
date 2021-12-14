/*称重传感器*/
#include "WeighingSensor.h"
#include <stdint.h>
#include <M5Stack.h>
uint16_t WeighingSensor::readWeigtValue()
{
    char sendBuf[] = {0x02, 0x04, 0x00, 0x00, 0x00, 0x01, 0x31, 0xF9};
    char recvBuf[20] = {0};
    uint16_t weight = 0;
    Serial2.write(sendBuf, sizeof(sendBuf));
    if (Serial2.read(recvBuf, 20) > 0)
    {
        if ((recvBuf[0] == 0x02) && (recvBuf[1] == 0x04))
        {
            weight = (recvBuf[3] << 8) | (recvBuf[4]);
            return weight;
        }
        else
        {
            Serial.println("weight: rcv data error...");
            return 0;
        }
    }
    else
    {
        Serial.println("no data ...");
        return 0;
    }
}

/*读取十次平均值*/
uint16_t WeighingSensor::readValWeight()
{
    uint32_t total_weight = 0;
    uint16_t weight = 0;
    for(int i=0;i<10;i++)
    {
        delay(50);
        weight = WeighingSensor::readWeigtValue();
        if(weight != 0)
        {
            total_weight += weight;
            Serial.print("weight:");
            Serial.println(weight);
        }
        else
        {
            i--;
        }
    }
    return total_weight/10;
}