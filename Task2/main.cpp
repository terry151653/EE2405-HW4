#include "mbed.h"
#include "bbcar.h"
#include <cstdio>
BufferedSerial pc(USBTX, USBRX);

Ticker servo_ticker;
Ticker encoder_ticker;
Timer TimeCnt;
PwmOut pin5(D12), pin6(D13);
DigitalIn encoder(D10);
BusInOut pattern(D4, D5, D6, D7);
BBCar car(pin5, pin6, servo_ticker);
volatile int steps = 0;
volatile int last = 0;
bool Status = 0;
float Distance1 = 0.0, Distance2 = 0.0;

void encoder_control()
{
    int value = encoder;
    if (!last && value)
        steps++;
    last = value;
}

void readpattern()
{
    pattern.output();
    pattern = 0b1111; wait_us(230);
    pattern.input(); wait_us(230);
}
void carcontrol()
{
    readpattern();
    switch (pattern)
    {
        case 0b1000: car.turn(10, 0.1);      /*printf("1\n"); */break;
        case 0b1100: car.turn(10, 0.3);      /*printf("2\n"); */break;
        case 0b0100: car.turn(10, 0.6);      /*printf("3\n"); */break;
        case 0b0110: car.goStraight(10);     /*printf("4\n"); */break;
        case 0b0010: car.turn(10, -0.6);     /*printf("5\n"); */break;
        case 0b0011: car.turn(10, -0.3);     /*printf("6\n"); */break;
        case 0b0001: car.turn(10, -0.1);     /*printf("7\n"); */break;
        case 0b0000:                         /*printf("9\n"); */break;
        case 0b1111: car.stop(); Status = 1; /*printf("10\n");*/break;
        default:car.goStraight(10);          /*printf("8\n"); */
    }
}
int main()
{
    pc.set_baud(9600);
    encoder_ticker.attach(&encoder_control, 10ms);
    steps = 0;
    last = 0;

    printf("start\n");
    car.goStraight(15);
    while (!Status)
    {
        carcontrol();
        ThisThread::sleep_for(10ms);
    }

    printf("1st line\n");
    ThisThread::sleep_for(1s);
    while (1)
    {
        readpattern();
        if (pattern == 0b1111)
            car.goStraight(7);
        else
        {
            car.stop();
            break;
        }
        ThisThread::sleep_for(10ms);
    }
    ThisThread::sleep_for(1s);

    steps = 0;
    Status = 0;
    while (!Status)
    {
        ThisThread::sleep_for(10ms);
        carcontrol();
    }
    Distance1 = steps * 6.5 * 3.1415 / 32;

    printf("2nd line, steps = %d\n", steps);
    ThisThread::sleep_for(1s);
    while (1)
    {
        readpattern();
        if (pattern == 0b1111)
            car.goStraight(7);
        else
        {
            car.stop();
            break;
        }
            
        ThisThread::sleep_for(10ms);
    }
    ThisThread::sleep_for(1s);

    steps = 0;
    Status = 0;
    while (!Status)
    {
        ThisThread::sleep_for(10ms);
        carcontrol();
    }
    Distance2 = steps * 6.5 * 3.1415 / 32;
    printf("3rd line, steps = %d\n", steps);

    car.stop();
    printf("Distance1 = %f, Distance2 = %f\n", Distance1, Distance2);
}