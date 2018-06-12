#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <pthread.h>

#include "motor.h"
#include "static_pinout.h"
#include "utils.h"

// Local function declaration
void* rpmCalculatorThrFunc (void* null_ptr);

int pulseCounter_A;
pthread_t rpmCalculatorThr;
static int init_motors_done = 0;
int rpm, count;


// ISR function for the encoder counter
void counterA_ISR (void)
{
    ++pulseCounter_A;
}

// Thread function to calculate RPM
void* rpmCalculatorThrFunc (void* null_ptr)
{
    int rpm_sampling_ms = 200;
    int diff_time_us;
    struct timeval start, end;

    while(1)
    {
        pulseCounter_A = 0;
        gettimeofday (&start, NULL);
        delay(rpm_sampling_ms); //Always keep a sleep or delay in infinite loop
        count = pulseCounter_A;
        gettimeofday (&end, NULL);
        diff_time_us = time_diff_us(end, start);

        // TODO:
        // Calculate RPM from count and diff_time_us (in microseconds)
        // Hint: look at fall quarter Lab 7 or Lab 8
        // rpm = ???
		// *** Note *** The line below is useless, it prevent a "Wunused-but-set-variable" warning.
		// To be removed once use to calculate rpm
		diff_time_us = diff_time_us;
    }
}


// init_motor: initializes motor pin (GPIO allocation and initial values of output)
// and initialize the elements of all motor control data structure
int initMotor(void)
{
    pulseCounter_A = 0;
    rpm = 0;
    count = 0;

    // Initialize GPIO pins
    pinMode(IN_MT_ENC_A, INPUT);
    pinMode(IN_MT_ENC_B, INPUT);
    pinMode(OUT_MT_EN, OUTPUT);
    pinMode(OUT_PWM, PWM_OUTPUT);
    digitalWrite(OUT_MT_EN, 0);
    pwmWrite(OUT_PWM, 0);

    if(!init_motors_done)
    {
        // Start counter ISR and RPM calculator
        wiringPiISR(IN_MT_ENC_A, INT_EDGE_FALLING, &counterA_ISR);
        int ret = pthread_create( &(rpmCalculatorThr), NULL, rpmCalculatorThrFunc, NULL);
        if( ret )
        {
            fprintf(stderr,"Error creating rpmCalculatorThr - pthread_create() return code: %d\n",ret);
            fflush(stderr);
            return ret;
        }
    }
    init_motors_done = 1;
    return 0;
}

// stopMotor: stop the motor
void stopMotor(void)
{
    digitalWrite(OUT_MT_EN, 0);
    pwmWrite(OUT_PWM, 0);
}


// getCount: accessor funtion of a motor encoder counter
int getCount(void)
{
    return count;
}


// getRPM: accessor funtion of a motor encoder counter
int getRPM(void)
{
    return rpm;
}


// setDutyCycle: sets the motor PWM duty-cycle
void setDutyCycle(int dutyCycle)
{
    int dc;
    if(dutyCycle <= 0)
    {
        stopMotor();
    }
    else
    {
        dc = dutyCycle <= 100 ? dutyCycle : 100;
        digitalWrite(OUT_MT_EN, 1);
        pwmWrite(OUT_PWM, (dc * 1024)/100);
    }
}
