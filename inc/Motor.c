// Motor.c
// Runs on MSP432
// Provide mid-level functions that initialize ports and
// set motor speeds to move the robot. Lab 13 starter
// Daniel Valvano
// July 8, 2017

/* This example accompanies the books
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
       ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
       ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
       ISBN: 978-1466468863, , Jonathan Valvano, copyright (c) 2017
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2017, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// Sever VCCMD=VREG jumper on Motor Driver and Power Distribution Board and connect VCCMD to 3.3V.
//   This makes P3.7 and P3.6 low power disables for motor drivers.  0 to sleep/stop.
// Sever nSLPL=nSLPR jumper.
//   This separates P3.7 and P3.6 allowing for independent control
// Left motor direction connected to P1.7 (J2.14)
// Left motor PWM connected to P2.7/TA0CCP4 (J4.40)
// Left motor enable connected to P3.7 (J4.31)
// Right motor direction connected to P1.6 (J2.15)
// Right motor PWM connected to P2.6/TA0CCP3 (J4.39)
// Right motor enable connected to P3.6 (J2.11)

#include <stdint.h>
#include "msp.h"
#include "../inc/CortexM.h"
#include "../inc/SysTick.h"
#include "../inc/PWM.h"

// *******Lab 13 solution*******

// ------------Motor_Init------------
// Initialize GPIO pins for output, which will be
// used to control the direction of the motors and
// to enable or disable the drivers.
// The motors are initially stopped, the drivers
// are initially powered down, and the PWM speed
// control is uninitialized.
// Input: none
// Output: none
void Motor_Init(void){
    P5->DIR |= BIT5;
    P3->DIR |= BIT6;
    P2->DIR |= BIT6;

    // Initialize P5.4, P3.7, and P2.7 as outputs (LEFT)
    P5->DIR |= BIT4;
    P3->DIR |= BIT7;
    P2->DIR |= BIT7;

    // Put DRV8838 drivers to sleep
    P3->OUT &= ~0xC0;
}

// ------------Motor_Stop------------
// Stop the motors, power down the drivers, and
// set the PWM speed control to 0% duty cycle.
// Input: none
// Output: none
void Motor_Stop(void) {
    // Set the motor direction bits to 0
    P5->OUT &= ~(BIT5 | BIT4);
    P3->OUT &= ~(BIT6 | BIT7);

    // Set the PWM duty cycle to 0% for both motors
    TIMER_A0->CCR[1] = 0;
    TIMER_A0->CCR[2] = 0;
}

// ------------Motor_Forward------------
// Drive the robot forward by running left and
// right wheels forward with the given duty
// cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Forward(uint16_t leftDuty, uint16_t rightDuty) {
    // Drives both motors forward at the specified duty cycles (0 to 14,998)
    P5->OUT &= ~(BIT5 | BIT4); // Set motor direction for forward

    P3->OUT |= BIT6 | BIT7; // Enable motor drivers

    P2->OUT &= ~(BIT6 | BIT7); // Turn off motors before updating duty cycles

    // Update left motor duty cycle
    PWM_Duty4(leftDuty);

    // Update right motor duty cycle
    PWM_Duty3(rightDuty);

    P2->OUT |= BIT6 | BIT7; // Turn on motors after updating duty cycles
}


// ------------Motor_Right------------
// Turn the robot to the right by running the
// left wheel forward and the right wheel
// backward with the given duty cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Right(uint16_t leftDuty, uint16_t rightDuty){
    // Set motor direction bits
    P5->OUT &= ~BIT4; // Set left motor direction for forward
    P5->OUT |= BIT5; // Set right motor direction for backwards

    P3->OUT |= BIT6 | BIT7; // Enable motor drivers

    P2->OUT &= ~(BIT6 | BIT7); // Turn off motors before updating duty cycles

    // Update left motor duty cycle
    PWM_Duty4(leftDuty);
    // Update right motor duty cycle
    PWM_Duty3(rightDuty);

    P2->OUT |= BIT6 | BIT7; // Turn on motors after updating duty cycles
}

// ------------Motor_Left------------
// Turn the robot to the left by running the
// left wheel backward and the right wheel
// forward with the given duty cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Left(uint16_t leftDuty, uint16_t rightDuty){
    // Set motor direction bits
    P5->OUT |= BIT4; // Set left motor direction for forward
    P5->OUT &= ~BIT5; // Set right motor direction for backwards

    P3->OUT |= BIT6 | BIT7; // Enable motor drivers

  P2->OUT &= ~(BIT6 | BIT7); // Turn off motors before updating duty cycles

  // Update left motor duty cycle
  PWM_Duty4(leftDuty);
  // Update right motor duty cycle
  PWM_Duty3(rightDuty);

  P2->OUT |= BIT6 | BIT7; // Turn on motors after updating duty cycles
}

// ------------Motor_Backward------------
// Drive the robot backward by running left and
// right wheels backward with the given duty
// cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Backward(uint16_t leftDuty, uint16_t rightDuty){
  // Set motor direction bits
  P5->OUT |= BIT5 | BIT4;  // Both wheels backward

  P3->OUT |= BIT6 | BIT7; // Enable motor drivers

  P2->OUT &= ~(BIT6 | BIT7); // Turn off motors before updating duty cycles

  // Update left motor duty cycle
  PWM_Duty4(leftDuty);

  // Update right motor duty cycle
  PWM_Duty3(rightDuty);

  P2->OUT |= BIT6 | BIT7; // Turn on motors after updating duty cycles

}
