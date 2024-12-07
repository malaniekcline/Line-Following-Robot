// CPE 312 - Final Project
// Frank Bonanno & Malanie Cline

#include "msp.h"
#include "..\inc\bump.h"
#include "..\inc\Clock.h"
#include "..\inc\SysTick.h"
#include "..\inc\LaunchPad.h"
#include "..\inc\MotorSimple.h"
#include "..\inc\Motor.h"
#include "../inc/PWM.h"
#include "../inc/PWM.c"
#include "../inc/Reflectance.c"
#include "stdbool.h"

#define SPEED 2250
#define RIGHT_TURN_THRESHOLD 50
#define LEFT_TURN_THRESHOLD -50

// Driver test
void Start(void){
  while(LaunchPad_Input()==0);  // wait for touch
  while(LaunchPad_Input());     // wait for release
}

void adjust_speed(uint16_t *leftDuty, uint16_t *rightDuty, uint8_t sensorData, int32_t position) {
    int32_t SHARP_RIGHT_THRESHOLD = 60;
    int32_t SHARP_LEFT_THRESHOLD = -60;

    if (sensorData == 0x00) { // Completely off the line
        Motor_Backward(SPEED, SPEED);
        Clock_Delay1ms(500);
        Motor_Stop();
    }

    if(sensorData == 0x3F || sensorData == 0x1F || sensorData == 0xFF || sensorData == 0x7F) {
        Clock_Delay1ms(50);
        Motor_Right(SPEED*1.70, SPEED*1.70);
        Clock_Delay1ms(550);
        Motor_Stop();
        Clock_Delay1ms(50);
    } else if (sensorData == 0xF8 || sensorData == 0xF0) {
        Clock_Delay1ms(50);
        Motor_Left(SPEED*1.70, SPEED*1.70);
        Clock_Delay1ms(550);
        Motor_Stop();
        Clock_Delay1ms(50);
    } else if (position > SHARP_RIGHT_THRESHOLD) { // Adjust speed for a sharp right turn
        *leftDuty = SPEED * 2.5;
        *rightDuty = SPEED / 3;
    } else if (position > RIGHT_TURN_THRESHOLD) { // Right turn detected, adjust speed for a smooth turn
        *leftDuty = SPEED * 1.55;
        *rightDuty = SPEED / 2;
    } else if (position < SHARP_LEFT_THRESHOLD) { // Adjust speed for a sharp left turn
        *leftDuty = SPEED / 3;
        *rightDuty = SPEED * 2.5;
    } else if (position < LEFT_TURN_THRESHOLD) { // Left turn detected, adjust speed for a smooth turn
        *leftDuty = SPEED / 2;
        *rightDuty = SPEED * 1.55;
    } else { // Stay centered
        *leftDuty = SPEED;
        *rightDuty = SPEED;
    }
}

int main(void){
    Clock_Init48MHz();
    LaunchPad_Init();   // built-in switches and LEDs
    SysTick_Init();
    Motor_Init();
    PWM_Init34(15000, 0, 0);
    Reflectance_Init();
    Start();

    // Set initial duty cycles
    uint16_t leftDuty = SPEED;
    uint16_t rightDuty = SPEED;

    while(1){
        uint8_t sensorData = Reflectance_Read(1000); // Read sensor data
        int32_t position = Reflectance_Position(sensorData); // Get position relative to line

        adjust_speed(&leftDuty, &rightDuty, sensorData, position);
        Motor_Forward(leftDuty, rightDuty);

        // Add a small delay for sensor reading stability
        Clock_Delay1ms(10);
    }
}


