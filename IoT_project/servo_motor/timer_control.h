#ifndef TIMER_CONTROL_H
#define TIMER_CONTROL_H

#include "esp_timer.h"

// 宣告與 timer 相關的變數
extern bool timebase_f;
extern esp_timer_handle_t timer_handle;

// 宣告 Timer 的相關函式
void onTimer(void* arg);
void timerInit(int microSeconds);


#endif
