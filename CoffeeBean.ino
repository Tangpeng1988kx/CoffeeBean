#include "SCServo.h"
#include <M5Stack.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <SPI.h>
#include <Ethernet2.h>
#include "Motor.h"
#include "PressureTransmitter.h"
#include "WeighingSensor.h"
#include "esp32-hal-ledc.h"
#include "UNIT_ENV.h"

/*温湿度传感器相关定义*/
SMS_STS sm_; //舵机相关
SHT3X sht30;
QMP6988 qmp6988;
float tmp = 0.0;
float hum = 0.0;
float pressure = 0.0;
/********************/
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 26
#define COUNT 100
#define TIMES 50
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 187);
unsigned int localPort = 8888; // local port to listen on
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";       // a string to send back
EthernetUDP Udp;
SemaphoreHandle_t xMutex;
static uint8_t Aflag, Bflag, Cflag;
int gweight = 0;
float pressureval = 0;
static QueueHandle_t xqueuel = NULL;
int target_weight = 0; //要取豆子的重量
//界面展示参数
struct displayInfo
{
    float tmp;
    float hum;
    float pressure;
    float pressureval;
};
struct displayInfo displayMess;
void setup()
{
    char str[20] = "hello";
    M5.begin(true, false, true, true);
    M5.Power.begin();
    M5.lcd.setTextSize(2); // Set the text size to 2.  设置文字大小为1
    Wire.begin();          // Wire init, adding the I2C bus.  Wire初始化, 加入i2c总线
    qmp6988.init();
    SPI.begin(SCK, MISO, MOSI, -1);
    Ethernet.init(CS);
    Ethernet.begin(mac, ip);
    Udp.begin(localPort);
    // start the Ethernet connection and the server:
    Serial.begin(115200);
    Serial2.begin(38400);
    sm_.pSerial = &Serial2;
    pinMode(5, OUTPUT); //设置G5为输出模式
                        //  ledcSetup(0, 30000, 3);
                        //  ledcAttachPin(5, 0); //绑定G5与channel0
                        //  ledcWrite(0, 0);     //设置0通道pwm
    xMutex = xSemaphoreCreateMutex();

    gweight = WeighingSensor::readWeigtValue(); //上电先读取豆仓的重量
    //创建消息队列
    xqueuel = xQueueCreate(10, sizeof(displayInfo));
    if (xqueuel == 0)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }

    xTaskCreate(

        taskUNIT_ENV, /* Task function. */

        "TaskUNIT_ENV", /* String with name of task. */

        10000, /* Stack size in bytes. */

        NULL, /* Parameter passed as input of the task */

        1, /* Priority of the task. */

        NULL); /* Task handle. */

    xTaskCreate(

        taskPressureControl, /* Task function. */

        "taskPressureControl", /* String with name of task. */

        10000, /* Stack size in bytes. */

        NULL, /* Parameter passed as input of the task */

        1, /* Priority of the task. */

        NULL); /* Task handle. */
    xTaskCreate(

        TaskSteering, /* Task function. */

        "TaskSteering", /* String with name of task. */

        10000, /* Stack size in bytes. */

        NULL, /* Parameter passed as input of the task */

        1, /* Priority of the task. */

        NULL); /* Task handle. */

    xTaskCreate(

        utpTask, /* Task function. */

        "utpTask", /* String with name of task. */

        10000, /* Stack size in bytes. */

        NULL, /* Parameter passed as input of the task */

        1, /* Priority of the task. */

        NULL); /* Task handle. */
    xTaskCreate(

        buttonTask, /* Task function. */

        "buttonTask", /* String with name of task. */

        10000, /* Stack size in bytes. */

        NULL, /* Parameter passed as input of the task */

        1,     /* Priority of the task. */
        NULL); /* Task handle. */

    xTaskCreate(

        displayTask, /* Task function. */

        "displayTask", /* String with name of task. */

        10000, /* Stack size in bytes. */

        NULL, /* Parameter passed as input of the task */

        1,     /* Priority of the task. */
        NULL); /* Task handle. */
}
void loop()
{
    delay(1000);
}

void taskPressureControl(void *parameter)

{
    M5.Lcd.setCursor(0, 25);
    while (1)
    {
        pressureval = pressureTransmitter::ReadPressureVal();
        if (Bflag)
        {
            if (pressureval < -300)
            {
                motor::MotorStop();
            }
            else if (pressureval > -100)
            {
                motor::MotorStart();
            }
        }
        else
        {
            motor::MotorStop();
        }
        displayMess.pressureval = pressureval;
        if (pdTRUE != xQueueSend(xqueuel, (void *)&displayMess, TickType_t(10)))
        {
        }
        delay(1000);
    }
}

void taskUNIT_ENV(void *parameter)
{
    while (1)
    {
        pressure = qmp6988.calcPressure();
        if (sht30.get() == 0)
        {                         // Obtain the data of shT30.  获取sht30的数据
            tmp = sht30.cTemp;    // Store the temperature obtained from shT30.  将sht30获取到的温度存储
            hum = sht30.humidity; // Store the humidity obtained from the SHT30.  将sht30获取到的湿度存储
        }
        else
        {
            tmp = 0, hum = 0;
        }
        displayMess.tmp = tmp;
        displayMess.hum = hum;
        displayMess.pressure = pressure;
        // M5.lcd.fillRect(0, 20, 100, 60, BLACK); // Fill the screen with black (to clear the screen).  将屏幕填充黑色(用来清屏)
        if (pdTRUE != xQueueSend(xqueuel, (void *)&displayMess, TickType_t(10)))
        {
        }
        delay(2000);
    }
}
void TaskSteering(void *parameter)
{
    uint16_t p_weight = 0; //本次转动前读取重量值
    uint16_t b_weight = 0; //本次转动后读取重量值
    while (1)
    {
        if (Cflag)
        {
            if (target_weight > 0)
            {
                Serial.print("target_weight>0 ");
                Serial.println(target_weight);
                p_weight = WeighingSensor::readValWeight();
                while (!sm_.WritePosEx(3, 1093, 1000, 100)) //舵机(ID3)以最高速度V=80(50*80步/秒)，加速度A=100(100*100步/秒^2)，运行至P1=1945  位置
                {
                    Serial.print("write pos 1945 OK");
                }
                delay(2500);
                Serial.println("to-->3993");
                while (!sm_.WritePosEx(3, 3993, 1000, 100)) //舵机(ID3)以最高速度V=80(50*80步/秒)，加速度A=100(100*100步/秒^2)，运行至P0=0位置
                {
                    Serial.print("write pos 3993 OK");
                }
                Serial.print("pos:");
                Serial.println(sm_.ReadPos(3));
                delay(2500);
                b_weight = WeighingSensor::readValWeight();
                target_weight -= (p_weight - b_weight);
            }
        }
        delay(10);
        //  sm_.WheelMode(3);
        //  sm_.WriteSpe(3.400, 0);
    }
}
void buttonTask(void *parameter)
{
    while (1)
    {
        M5.update();
        if (M5.BtnA.pressedFor(1000))
        {
            Aflag = !Aflag;
            Serial.println("AAAA");
            delay(3000);
        }
        else if (M5.BtnB.pressedFor(1000))
        {
            Bflag = !Bflag;
            Serial.println("BBBB");
            delay(3000);
        }
        else if (M5.BtnC.pressedFor(1000))
        {
            Cflag = !Cflag;
            Serial.println("CCCC");
            delay(3000);
        }
        delay(10);
    }
}

void utpTask(void *parameter)
{
    char *token;
    while (1)
    {
        int packetSize = Udp.parsePacket();
        if (packetSize)
        {
            Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
            Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
            Udp.write("STOP");
            Udp.endPacket();
            if (strncmp("weight", packetBuffer, strlen("weight")) == 0) //对比字符串
            {
                token = strtok(packetBuffer, ":"); //字符串分割
                token = strtok(NULL, ":");         //字符串分割
                target_weight = atoi(token);
                xSemaphoreTake(xMutex, portMAX_DELAY);
                Serial.print("received:");
                Serial.println(target_weight);
                xSemaphoreGive(xMutex);
                //对比成功，首先读取秤的重量值
                Serial.print("tatal:");
                Serial.println(WeighingSensor::readValWeight());
            }
        }
        delay(10);
    }
}

void displayTask(void *parameter)
{
    while (1)
    {
        if (pdTRUE != xQueueReceive(xqueuel, (void *)(&displayMess), TickType_t(10)))
        {
        }
        xSemaphoreTake(xMutex, portMAX_DELAY);
        M5.lcd.setCursor(0, 0);
        M5.Lcd.printf("Temp: %2.1f  \r\nHumi: %2.0f%%  \r\nEnvPressure:%2.0fPa\r\nAirPressureVal:%2.0fKPa\r\n", displayMess.tmp, displayMess.hum, displayMess.pressure, displayMess.pressureval);
        xSemaphoreGive(xMutex);
        delay(1000);
    }
}