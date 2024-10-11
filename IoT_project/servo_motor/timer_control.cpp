#include "timer_control.h"
#include <Arduino.h>  // For Serial

// 初始化變數
bool timebase_f = false;
esp_timer_handle_t timer_handle;

// Timer 中斷處理函式
void IRAM_ATTR onTimer(void* arg) {
    timebase_f = true;  // 中斷觸發時將 timebase_f 設為 true
}

// Timer 初始化函式
void timerInit(int microSeconds) {
    esp_timer_create_args_t timer_args = {
        .callback = &onTimer,  // 中斷觸發時要呼叫的函式
        .name = "MyTimer"      // Timer 名稱
    };
    
    // 創建 Timer
    esp_timer_create(&timer_args, &timer_handle);
    
    // 啟動周期性 Timer，單位為微秒
    esp_timer_start_periodic(timer_handle, microSeconds);

    Serial.println("Timer initialized");
}
