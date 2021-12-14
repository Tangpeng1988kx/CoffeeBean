#include "PressureTransmitter.h"
#include <M5Stack.h>
int16_t pressureTransmitter::ReadPressureVal()
{
    int16_t pressureVal = 0;
    char send_buf[8] = {0x01, 0x03, 0x00, 0x04, 0x00, 0x01, 0xC5, 0xCB};
    char receive_buf[50] = {0};
    int data1 = 0;
    int data2 = 0;
    Serial2.write(send_buf, sizeof(send_buf));
    if (0 == Serial2.read(receive_buf, sizeof(receive_buf)))
    {
       // Serial.println("no data ......");
        return 0;
    }
    else
    {
        if ((receive_buf[0] == 0x01) && (receive_buf[1] == 0x03))
        {
            pressureVal = ((receive_buf[3] << 8) | (receive_buf[4]));
            Serial.print("pressureVal = :");
            Serial.println(pressureVal);
            return pressureVal;
        }
        else
        {
            data1 = receive_buf[0];
            data2 = receive_buf[1];
            Serial.println("data error");
            Serial.println(data1);
            Serial.println(data2);
            return 0;
        }
    }
    return 1;
}
uint16_t pressureTransmitter::ReadDecimalPlaces()
{
    uint16_t decimalplaces = 0;
    char send_buf[8] = {0x01, 0x03, 0x00, 0x03, 0x00, 0x01, 0x74, 0x0A};
    char receive_buf[50] = {0};
    Serial2.write(send_buf, sizeof(send_buf));
    if (0 == Serial2.read(receive_buf, sizeof(receive_buf)))
    {
        return -1;
    }
    else
    {
        if ((receive_buf[0] == 0x01) && (receive_buf[1] == 0x03))
        {
            decimalplaces = ((receive_buf[3] << 8) | (receive_buf[4]));
            return decimalplaces;
        }
        else
        {
            return -1;
        }
    }
    return 0;
}
uint16_t pressureTransmitter::ReadSensorAddr()
{
    uint16_t sensoraddr = 0;
    char send_buf[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};
    char receive_buf[50] = {0};
    Serial2.write(send_buf, sizeof(send_buf));
    if (0 == Serial2.read(receive_buf, sizeof(receive_buf)))
    {
        return -1;
    }
    else
    {
        if ((receive_buf[0] == 0x01) && (receive_buf[1] == 0x03))
        {
            sensoraddr = ((receive_buf[3] << 8) | (receive_buf[4]));
            return sensoraddr;
        }
        else
        {
            return -1;
        }
    }
    return 0;
}
uint16_t pressureTransmitter::ReadSensorBaudRate()
{
    uint16_t LocalBaudRate = 0;
    char send_buf[8] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA};
    char receive_buf[50] = {0};
    Serial2.write(send_buf, sizeof(send_buf));
    if (0 == Serial2.read(receive_buf, sizeof(receive_buf)))
    {
        return -1;
    }
    else
    {
        if ((receive_buf[0] == 0x01) && (receive_buf[1] == 0x03))
        {
            LocalBaudRate = ((receive_buf[3] << 8) | (receive_buf[4]));
            return LocalBaudRate;
        }
        else
        {
            return -1;
        }
    }
    return 0;
}
void pressureTransmitter::WriteSensorAddr(uint16_t sensorAddr)
{
}
void pressureTransmitter::WriteSensorBaudRate(uint16_t baudRate)
{
}