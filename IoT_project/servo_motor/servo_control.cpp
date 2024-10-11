#include "servo_control.h"
#include <Arduino.h>  // For Serial

Servo myservo_1;
Servo myservo_2;

const byte servoPin_1 = 17;
const byte servoPin_2 = 16;

int sg90_counter_1 = 500;
boolean sg90_on_f1 = false;
boolean sg90_off_f1 = false;

int sg90_counter_2 = 500;
boolean sg90_on_f2 = false;
boolean sg90_off_f2 = false;


void setupServos() {
    myservo_1.attach(servoPin_1);
    myservo_2.attach(servoPin_2);
    myservo_1.write(90);  // 設定伺服器初始位置
    myservo_2.write(90);
    delay(500);
    myservo_1.detach();
    myservo_2.detach();
}


void controlServo1(int angle) {
    myservo_1.attach(servoPin_1);
    sg90_counter_1 = 500;
    sg90_on_f1 = true;
    myservo_1.write(angle);
    Serial.print("Servo 1 set to angle: ");
    Serial.println(angle);
}

void controlServo2(int angle) {
    myservo_2.attach(servoPin_2);
    sg90_counter_2 = 500;
    sg90_on_f2 = true;
    myservo_2.write(angle);
    Serial.print("Servo 2 set to angle: ");
    Serial.println(angle);
}

void releaseServo1() {
    myservo_1.detach();  // 釋放伺服器
    Serial.println("Servo 1 released");
}

void releaseServo2() {
    myservo_2.detach();  // 釋放伺服器
    Serial.println("Servo 2 released");
}



void handleServoTimers() {
    if (sg90_on_f1) {
        sg90_counter_1--;
        if (sg90_counter_1 == 0) {
            myservo_1.write(90);
            Serial.println("Servo 1 放開");
        }
        if (sg90_counter_1 == -500) {
            releaseServo1();
            sg90_on_f1 = false;
        }
    }
    if (sg90_on_f2) {
        sg90_counter_2--;
        if (sg90_counter_2 == 0) {
            myservo_2.write(90);
            Serial.println("Servo 2 放開");
        }
        if (sg90_counter_2 == -500) {
            releaseServo2();
            sg90_on_f2 = false;
        }
    }
}