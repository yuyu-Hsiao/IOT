#include "esp_timer.h"
#include "xbox.h"
#include "hcsr04.h"

#define A_1A 12 // A_1A控制A馬達的前進（HIGH）、後退（LOW）
#define A_1B 13 // A_1B控制A馬達的速度 0~255 ，LOW表示停止
#define B_1A 26 // B_1A控制B馬達的前進（HIGH）、後退（LOW）
#define B_1B 27 // B_1B控制B馬達的速度 0~255，LOW表示停止
#define Speed 255 //設定前進速度





volatile bool timebase_f = false;


float d;
int xbox_counter = 100;
boolean is_user_mode = true;
boolean is_xbox_connect = false;

int forward_button = 0;
boolean buttonY = false;
boolean buttonX = false;
boolean buttonA = false;
boolean buttonB = false;

esp_timer_handle_t timer_handle;  // 定義計時器變數

void IRAM_ATTR onTimer(void* arg) {
    // 執行中斷時要執行的代碼
    timebase_f = true;
}

void timer_setting(int micro_Second){
    // 計時器配置
    esp_timer_create_args_t timer_args = {
        .callback = &onTimer,      // 指定回調函數
        .name = "MyTimer"          // 計時器名稱
    };    
    esp_timer_create(&timer_args, &timer_handle); // 創建計時器
    esp_timer_start_periodic(timer_handle, micro_Second);  //啟動計時器（微秒）
}


void setup() {
  Serial.begin(115200);
  pinMode(A_1A, OUTPUT);
  pinMode(A_1B, OUTPUT);
  pinMode(B_1A, OUTPUT);
  pinMode(B_1B, OUTPUT);
  digitalWrite(A_1A, LOW);
  digitalWrite(A_1B, LOW);
  digitalWrite(B_1A, LOW);
  digitalWrite(B_1B, LOW);

  hcsr04_setup();
  xbox_setup();


  timer_setting(1000); // 設置 Timer1 每 1000 微秒觸發一次中斷



}

//車子停止
void stopcar() {
  analogWrite(A_1A, LOW);
  analogWrite(A_1B, LOW);
  analogWrite(B_1A, LOW);
  analogWrite(B_1B, LOW);
}

//車子向前走
void forward(int Spd = 255, float turn_rate = 1, boolean turn = false) {
  int spd_right;
  int spd_left;
  if(turn_rate >= 0.1){
    spd_right = Spd - Spd * turn_rate;
    spd_left = Spd;
  }
  if(turn_rate <= -0.1){
    spd_left = Spd + Spd * turn_rate;
    spd_right = Spd;
  }
  if(turn_rate<0.1 && turn_rate>-0.1){
    spd_right = Spd;
    spd_left = Spd;
  }
  if(Spd>=0){
    analogWrite(A_1A, spd_left);
    analogWrite(A_1B, LOW);
    analogWrite(B_1A, spd_right);
    analogWrite(B_1B, LOW);   
  }
  if(Spd<0){
    analogWrite(A_1A, LOW);
    analogWrite(A_1B, -spd_left);
    analogWrite(B_1A, LOW);
    analogWrite(B_1B, -spd_right);  
  }
  if(turn){
    analogWrite(A_1A, Spd);
    analogWrite(A_1B, LOW);
    analogWrite(B_1A, LOW);
    analogWrite(B_1B, Spd);
  }
}

float mapFloat(uint16_t x, uint16_t in_min, uint16_t in_max, float out_min, float out_max) {
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}


void loop() {
  
  if(timebase_f){   
    xbox_counter--;    
    if(xbox_counter == 0){
      is_xbox_connect = xbox_connect();
      xbox_counter = 100;
    }

    if(is_xbox_connect){
      if(xboxController.xboxNotif.btnY){
        is_user_mode = !is_user_mode;
        Serial.println(is_user_mode);
      }
      if(is_user_mode){
        forward_button = ((xboxController.xboxNotif.trigRT-xboxController.xboxNotif.trigLT)*255)/1023;
        float mappedValue = mapFloat(xboxController.xboxNotif.joyLHori, 0, 65535, -1, 1);
        forward(forward_button, mappedValue);
      }else{
        d = dis_cm();
        Serial.print(d,1);
        Serial.println("cm");
        if(d>15){
          forward(255, 0);
        }
        else{
          forward(255, 0, true);
        }
      }
    }
    timebase_f = false;
  }  
}


