// MotorSimple.c

#include <stdint.h>
#include "msp.h"
#include "../inc/SysTick.h"
#include "../inc/Bump.h"

void Motor_InitSimple(void){
    // Initialize P5.5, P3.6, and P2.6 as outputs (RIGHT)
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

void Motor_StopSimple(void){
    // Stops both motors, puts driver to sleep

    // Turn off right motor
    P5->OUT &= ~BIT5; // Set P5.5 (DIRR) to low
    P2->OUT &= ~BIT6; // Set P2.6 (PWMR) to low
    P3->OUT |= BIT6; // Set P3.6 (nSLPR) to high

    // Turn off left motor
    P5->OUT &= ~BIT4; // Set P5.4 (DIRL) to low
    P2->OUT &= ~BIT7; // Set P2.7 (PWML) to low
    P3->OUT |= BIT7; // Set P3.7 (nSLPL) to high
}


void Motor_ForwardSimple(uint16_t duty, uint32_t time) {
    // Drives both motors forward at duty (100 to 9900)
    // Runs for time duration (units=10ms), and then stops
    // Stop the motors and return if any bumper switch is active
    // Returns after time*10ms or if a bumper switch is hit

    uint32_t endTime = time / 2; // Calculate the end time based on desired duration
    uint16_t H = duty;
    uint32_t count = 0; // Counter to keep track of time elapsed

    P5->OUT &= ~(BIT5 | BIT4); // Set motor direction for forward
    P3->OUT |= BIT6 | BIT7;

    uint8_t bumps = 0x3F;
    while (count < endTime) { // Continue running until time duration is reached
        bumps = Bump_Read();
        if (bumps != 0x3F) { // Check if any bumper switch is active
            break;
        }

        P2->OUT |= BIT6 | BIT7;
        SysTick_Wait1us(H);
        P2->OUT &= ~(BIT6 | BIT7);
        SysTick_Wait1us(10000 - H);
        count++; // Increment the counter for elapsed time
    }

    Motor_StopSimple(); // Stop the motors after reaching the desired duration
    return;
}

void Motor_BackwardSimple(uint16_t duty, uint32_t time){
// Drives both motors backward at duty (100 to 9900)
// Runs for time duration (units=10ms), and then stops
// Runs even if any bumper switch is active
// Returns after time*10ms

    uint32_t endTime = time / 2; // Calculate the end time based on desired duration
    uint16_t H = duty;
    uint32_t count = 0; // Counter to keep track of time elapsed

    P5->OUT |= BIT5 | BIT4; // Set motor direction for forward
    P3->OUT |= BIT6 | BIT7;

    uint8_t bumps = 0x3F;
    while (count < endTime) { // Continue running until time duration is reached
        bumps = Bump_Read();
        if (bumps != 0x3F) { // Check if any bumper switch is active
            break;
        }

        P2->OUT |= BIT6 | BIT7;
        SysTick_Wait1us(H);
        P2->OUT &= ~(BIT6 | BIT7);
        SysTick_Wait1us(10000 - H);
        count++; // Increment the counter for elapsed time
    }

    Motor_StopSimple(); // Stop the motors after reaching the desired duration
    return;
}
void Motor_LeftSimple(uint16_t duty, uint32_t time){
// Drives just the left motor forward at duty (100 to 9900)
// Right motor is stopped (sleeping)
// Runs for time duration (units=10ms), and then stops
// Stop the motor and return if any bumper switch is active
// Returns after time*10ms or if a bumper switch is hit

    uint32_t endTime = time / 2 ; // Calculate the end time based on desired duration
    uint16_t H = duty;
    uint32_t count = 0; // Counter to keep track of time elapsed

    P5->OUT &= ~BIT4; // Set left motor direction for forward
    P3->OUT |= BIT7; // Set nSLPL to high for left motor sleep

    uint8_t bumps = 0x3F;
    while (count < endTime) { // Continue running until time duration is reached
        bumps = Bump_Read();
        if (bumps != 0x3F) { // Check if any bumper switch is active
            break;
        }

        P2->OUT |= BIT6; // Set PWMR to high for left motor
        SysTick_Wait1us(H);
        P2->OUT &= ~BIT6; // Set PWMR to low for left motor
        SysTick_Wait1us(10000 - H);
        count++; // Increment the counter for elapsed time
    }

    Motor_StopSimple(); // Stop the motor after reaching the desired duration or if a bumper switch is hit
    return;
}
void Motor_RightPoint(uint16_t duty, uint32_t time){
// Drives left motor forward and right motor backwards at duty (100 to 9900)
// Runs for time duration (units=10ms), and then stops
// Stop the motors and return if any bumper switch is active
// Returns after time*10ms or if a bumper switch is hit

    uint32_t endTime = time / 2 ; // Calculate the end time based on desired duration
    uint16_t H = duty;
    uint32_t count = 0; // Counter to keep track of time elapsed

    P5->OUT &= ~BIT4; // Set left motor direction for forward
    P3->OUT |= BIT7; // Set nSLPL to high for left motor sleep
    P5->OUT |= BIT5; // Set right motor direction for backwards
    P3->OUT |= BIT6; // Set nSLPR to high for right motor sleep

    uint8_t bumps = 0x3F;
    while (count < endTime) { // Continue running until time duration is reached
        bumps = Bump_Read();
        if (bumps != 0x3F) { // Check if any bumper switch is active
            break;
        }

        P2->OUT |= BIT6 | BIT7; // Set PWMR to high for left motor
        SysTick_Wait1us(H);
        P2->OUT &= ~(BIT6 | BIT7); // Set PWMR to low for left motor
        SysTick_Wait1us(10000 - H);
        count++; // Increment the counter for elapsed time
    }

    Motor_StopSimple(); // Stop the motor after reaching the desired duration or if a bumper switch is hit
    return;
}
void Motor_RightSimple(uint16_t duty, uint32_t time){
// Drives just the right motor forward at duty (100 to 9900)
// Left motor is stopped (sleeping)
// Runs for time duration (units=10ms), and then stops
// Stop the motor and return if any bumper switch is active
// Returns after time*10ms or if a bumper switch is hit

    uint32_t endTime = time / 2; // Calculate the end time based on desired duration
    uint16_t H = duty;
    uint32_t count = 0; // Counter to keep track of time elapsed

    P5->OUT &= ~BIT5; // Set right motor direction for forward
    P3->OUT |= BIT6; // Set nSLPR to high for right motor sleep

    uint8_t bumps = 0x3F;
    while (count < endTime) { // Continue running until time duration is reached
        bumps = Bump_Read();
        if (bumps != 0x3F) { // Check if any bumper switch is active
            break;
        }

        P2->OUT |= BIT7; // Set PWML to high for right motor
        SysTick_Wait1us(H);
        P2->OUT &= ~BIT7; // Set PWML to low for right motor
        SysTick_Wait1us(10000 - H);
        count++; // Increment the counter for elapsed time
    }

    Motor_StopSimple(); // Stop the motor after reaching the desired duration or if a bumper switch is hit
    return;
}

void Motor_LeftPoint(duty, time) {
    uint32_t endTime = time / 2; // Calculate the end time based on desired duration
    uint16_t H = duty;
    uint32_t count = 0; // Counter to keep track of time elapsed

    P5->OUT &= ~BIT5; // Set right motor direction for forward
    P3->OUT |= BIT6; // Set nSLPR to high for right motor sleep
    P5->OUT |= BIT4; // Set left motor direction for backwards
    P3->OUT |= BIT7; // Set nSLPL to high for left motor sleep

    uint8_t bumps = 0x3F;
    while (count < endTime) { // Continue running until time duration is reached
        bumps = Bump_Read();
        if (bumps != 0x3F) { // Check if any bumper switch is active
            break;
        }

        P2->OUT |= BIT7 | BIT6; // Set PWML to high for left and right motor
        SysTick_Wait1us(H);
        P2->OUT &= ~(BIT7 | BIT6); // Set PWML to low for left and right motor
        SysTick_Wait1us(10000 - H);
        count++; // Increment the counter for elapsed time
    }

    Motor_StopSimple(); // Stop the motor after reaching the desired duration or if a bumper switch is hit
    return;
}
