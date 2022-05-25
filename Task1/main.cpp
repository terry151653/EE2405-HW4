#include "mbed.h"
#include "bbcar.h"
#include <cstdio>
BufferedSerial pc(USBTX, USBRX);

Ticker servo_ticker;
Ticker encoder_ticker;
Timer TimeCnt;
PwmOut pin5(D12), pin6(D13);
DigitalInOut LaserPin(D11);

BBCar car(pin5, pin6, servo_ticker);

volatile int PulseRecord = 0, MinPulse = 0, MaxPulse = 0;
int cnt = 0;

void laserpin()
{
    ThisThread::sleep_for(50ms);
    TimeCnt.reset();
    LaserPin.output();
    LaserPin = 0; wait_us(100);
    LaserPin = 1; wait_us(2);
    LaserPin = 0; wait_us(100);
    
    LaserPin.input();
    while (!LaserPin);
    TimeCnt.start();
    while (LaserPin);
    TimeCnt.stop();
    PulseRecord = TimeCnt.read_us();
}

int main()
{
    printf("start\n");
    laserpin();
    MinPulse = MaxPulse = PulseRecord;
    pc.set_baud(9600);
    printf("%d\n", MinPulse);
    ThisThread::sleep_for(1s);
    car.servo0.set_speed(10);
    car.servo1.set_speed(10);
    while (PulseRecord < 13000)
    {
        laserpin();
        if (PulseRecord - MaxPulse > 30)
        {
            cnt++;
            if (cnt > 10)
                break;
            continue;
        }
        MinPulse = min(PulseRecord, MinPulse);
        MaxPulse = max(PulseRecord, MaxPulse);
        printf("%d\n", MaxPulse);
    }
    printf("%d, %d, %d\n", MinPulse, MaxPulse, PulseRecord);
    car.stop();
    printf("end\n");
    float w = 2 * sqrt((MaxPulse * 0.1715)*(MaxPulse * 0.1715) - (MinPulse * 0.1715)*(MinPulse * 0.1715));
    printf("W is %f mm\n", w);
}