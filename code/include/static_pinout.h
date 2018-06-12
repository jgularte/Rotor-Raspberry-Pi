#ifndef STATIC_PINOUT_H
#define STATIC_PINOUT_H

/**
\file       static_pinout.h
\brief      GPIO Pin file of the static RPi.

This file maps all GPIO pins used by the hardware and RPi of the static portion of the POV
*/

// GPIO Input Pins
/// Email-My-IP and utility switch "S1" (input pin 37, GPIO_25)
#define IN_SW1            25
/// Motor encoder A (input pin 18, GPIO_5)
#define IN_MT_ENC_A        5
/// Motor encoder B (input pin 22, GPIO_6)
#define IN_MT_ENC_B        6

// GPIO Output Pins
/// Motor enable signal (output pin 10, GPIO_16)
#define OUT_MT_EN         16
/// Motor PWM drive (output pin 12, PWM_0/GPIO_1)
#define OUT_PWM            1

#endif
