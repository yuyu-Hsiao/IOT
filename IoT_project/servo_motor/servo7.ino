#include "esp_timer.h"
#include "wifi_mqtt.h"
#include "servo_control.h"
#include "timer_control.h"

//boolean sg90_is_use_f = false;

void setup() {
  setupServos();    // 初始化伺服器
  timerInit(1000);  // 設置 Timer 中斷，並啟動 Timer（單位：微秒）
  WifiConnecte();   // 開始WiFi連線
  MQTTConnecte();   // 開始MQTT連線
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) { 
        WifiConnecte(); 
    }
    if (!MQTTClient.connected()) { 
        MQTTConnecte(); 
    }

    if (timebase_f) {
        MQTTClient.loop(); // 更新訂閱狀態

        // 根據 MQTT 接收的訊息控制伺服器
        if (wifi_flag == 1) {
            if (wifi_buffer == "0") {
                controlServo1(118);  // Servo1
            }
            if (wifi_buffer == "1") {
                controlServo1(57);  // Servo1
            }
            if (wifi_buffer == "2") {
                controlServo2(120);  // Servo2
            }
            if (wifi_buffer == "3") {
                controlServo2(53);  // Servo2
            }
            wifi_flag = 0;
        }

        handleServoTimers();  // 處理伺服器計時器邏輯
        timebase_f = false;
    }
}