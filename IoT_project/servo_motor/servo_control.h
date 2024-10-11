#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <ESP32Servo.h>

// 宣告 Servo 物件
extern Servo myservo_1;
extern Servo myservo_2;

// 宣告伺服控制函式
void setupServos();
void controlServo1(int angle);
void controlServo2(int angle);
void releaseServo1();
void releaseServo2();
void handleServoTimers();

#endif