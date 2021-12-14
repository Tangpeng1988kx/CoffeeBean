#ifndef _RESSURETRANSMITTER_H_
#define _RESSURETRANSMITTER_H_
#include "HardwareSerial.h"
#include <stdint.h>
class pressureTransmitter
{
    private:
    uint16_t _baud_rate;
    uint16_t _sensor_addr;
    public:
    static int16_t ReadPressureVal();
    static uint16_t ReadDecimalPlaces();
    static uint16_t ReadSensorAddr();
    static uint16_t ReadSensorBaudRate();
    static void WriteSensorAddr(uint16_t sensorAddr);
    static void WriteSensorBaudRate(uint16_t baudRate);
};
#endif