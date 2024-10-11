#include <Stepper.h>
#include "esp_timer.h"
#include <WiFi.h>
#include <PubSubClient.h> //請先安裝PubSubClient程式庫

const int step1_PerRevolution = 2048; // 每圈步數
const int step2_PerRevolution = 2048; // 每圈步數
// ULN2003 Motor Driver Pins
//MOTOR 1
#define MOTOR1_IN1 19
#define MOTOR1_IN2 18
#define MOTOR1_IN3 5
#define MOTOR1_IN4 13

//MOTOR 2
#define MOTOR2_IN1 14
#define MOTOR2_IN2 27
#define MOTOR2_IN3 16
#define MOTOR2_IN4 17

Stepper Stepper_1(step1_PerRevolution, MOTOR1_IN1, MOTOR1_IN3, MOTOR1_IN2, MOTOR1_IN4);
Stepper Stepper_2(step1_PerRevolution, MOTOR2_IN1, MOTOR2_IN3, MOTOR2_IN2, MOTOR2_IN4);

// ------ 以下修改成你自己的WiFi帳號密碼 ------
char* ssid = "RVL122LAB";
char* password = "rvl122vision";
// ------ 以下修改成你MQTT設定 ------
char* MQTTServer = "broker.emqx.io";
int MQTTPort = 1883;//MQTT Port
char* MQTTUser = "";//不須帳密
char* MQTTPassword = "";//不須帳密

//訂閱主題
char* MQTTSubTopic1 = "rvlTopic/rvl122/step_motor122_1";
char* MQTTSubTopic2 = "rvlTopic/rvl122/step_motor122_2";

WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);



boolean step1_on_f = false;
boolean step1_off_f = false;
volatile int steps1_To_Move = 0; 
volatile int step1_Direction = 0; // 1: 順時針, -1: 逆時針
boolean step1_use_f = false;
boolean step1_is_back_f = false;
int tmp1_step;

boolean step2_on_f = false;
boolean step2_off_f = false;
volatile int steps2_To_Move = 0; 
volatile int step2_Direction = 0; // 1: 順時針, -1: 逆時針
boolean step2_use_f = false;
boolean step2_is_back_f = false;
int tmp2_step;

String wifi_buffer_1; 
String wifi_buffer_2; 
boolean wifi_flag1 = false;
boolean wifi_flag2 = false;
boolean timebase_f;


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
  
  Stepper_1.setSpeed(5);  // set the speed at 5 rpm
  Stepper_2.setSpeed(5); 

  Serial.begin(115200);
  timer_setting(1000);  //設置timer中斷
  
  WifiConnecte(); //開始WiFi連線
  MQTTConnecte(); //開始MQTT連線
}



void loop() {

  //如果WiFi連線中斷，則重啟WiFi連線
  if (WiFi.status() != WL_CONNECTED) { 
      WifiConnecte(); 
    }

  //如果MQTT連線中斷，則重啟MQTT連線
  if (!MQTTClient.connected()) { 
      MQTTConnecte(); 
    }

  
  if(timebase_f){
    MQTTClient.loop();//更新訂閱狀態

    //確定topic相同 //step 1
    if(wifi_flag1 == 1){
      //判斷mqtt傳來的值
      if(step1_use_f == false){
        if (wifi_buffer_1 == "0") {
          steps1_To_Move = step1_PerRevolution / 6; // 設定要轉動的步數（30度）
          tmp1_step = steps1_To_Move;
          step1_Direction = 1; // 設定順時針方向
          step1_on_f = true; // 啟動移動
          step1_use_f = true;
        }
        else if (wifi_buffer_1 == "1") {
          steps1_To_Move = step1_PerRevolution / 6;
          tmp1_step = steps1_To_Move; 
          step1_Direction = -1; 
          step1_off_f = true; 
          step1_use_f = true;
        }
      }
      wifi_flag1 = 0;
    }
    //確定topic相同 //step 2
    if(wifi_flag2 == 1){
      if(step2_use_f == false){
        if (wifi_buffer_2 == "0") {
          steps2_To_Move = step2_PerRevolution / 6; // 設定要轉動的步數（30度）
          tmp2_step = steps2_To_Move;
          step2_Direction = 1; // 設定順時針方向
          step2_on_f = true; // 啟動移動
          step2_use_f = true;
        }
        else if (wifi_buffer_2 == "1") {
          steps2_To_Move = step2_PerRevolution / 6;
          tmp2_step = steps2_To_Move; 
          step2_Direction = -1; 
          step2_off_f = true; 
          step2_use_f = true;
        }
      }
      wifi_flag2 = 0;
    }



     

    timebase_f = false;
  } 

  // step1 如果有步數需要移動且馬達正在移動
  if (step1_on_f) {
    if (steps1_To_Move > 0 && !step1_is_back_f){
      Stepper_1.step(step1_Direction); // 每次移動一步
      steps1_To_Move--; // 減少剩餘步數
    }
    if (steps1_To_Move == 0 && !step1_is_back_f) {
      Serial.println("1.正轉完成");
      step1_is_back_f = true;
    }
    if(step1_is_back_f && tmp1_step>0){
      Stepper_1.step(-step1_Direction); // 每次移動一步
      tmp1_step--; // 減少剩餘步數
      if(tmp1_step == 0){
        step1_is_back_f = false;
        step1_use_f = false;
        step1_on_f = false;
        Serial.println("1.步進完成");
        disableStepper(MOTOR1_IN1, MOTOR1_IN3, MOTOR1_IN2, MOTOR1_IN4);
      }
    }
  }
  if (step1_off_f) {
    if (steps1_To_Move > 0 && !step1_is_back_f){
      Stepper_1.step(step1_Direction); // 每次移動一步
      steps1_To_Move--; // 減少剩餘步數
    }
    if (steps1_To_Move == 0 && !step1_is_back_f) {
      Serial.println("2.正轉完成");
      step1_is_back_f = true;
    }
    if(step1_is_back_f && tmp1_step>0){
      Stepper_1.step(-step1_Direction); // 每次移動一步
      tmp1_step--; // 減少剩餘步數
      if(tmp1_step == 0){
        step1_is_back_f = false;
        step1_use_f = false;
        step1_off_f = false;
        Serial.println("2.步進完成");
        disableStepper(MOTOR1_IN1, MOTOR1_IN3, MOTOR1_IN2, MOTOR1_IN4);
      }
    }
  }


  // step2 如果有步數需要移動且馬達正在移動
  if (step2_on_f) {
    if (steps2_To_Move > 0 && !step2_is_back_f){
      Stepper_2.step(step2_Direction); // 每次移動一步
      steps2_To_Move--; // 減少剩餘步數
    }
    if (steps2_To_Move == 0 && !step2_is_back_f) {
      Serial.println("3.正轉完成");
      step2_is_back_f = true;
    }
    if(step2_is_back_f && tmp2_step>0){
      Stepper_2.step(-step2_Direction); // 每次移動一步
      tmp2_step--; // 減少剩餘步數
      if(tmp2_step == 0){
        step2_is_back_f = false;
        step2_use_f = false;
        step2_on_f = false;
        Serial.println("3.步進完成");
        disableStepper(MOTOR2_IN1, MOTOR2_IN3, MOTOR2_IN2, MOTOR2_IN4);
      }
    }
  }
  if (step2_off_f) {
    if (steps2_To_Move > 0 && !step2_is_back_f){
      Stepper_2.step(step2_Direction); // 每次移動一步
      steps2_To_Move--; // 減少剩餘步數
    }
    if (steps2_To_Move == 0 && !step2_is_back_f) {
      Serial.println("4.正轉完成");
      step2_is_back_f = true;
    }
    if(step2_is_back_f && tmp2_step>0){
      Stepper_2.step(-step2_Direction); // 每次移動一步
      tmp2_step--; // 減少剩餘步數
      if(tmp2_step == 0){
        step2_is_back_f = false;
        step2_use_f = false;
        step2_off_f = false;
        Serial.println("4.步進完成");
        disableStepper(MOTOR2_IN1, MOTOR2_IN3, MOTOR2_IN2, MOTOR2_IN4);
      }
    }
  }

}


// 禁用步進馬達的供電
void disableStepper(int in1, int in2, int in3, int in4) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}



//開始WiFi連線
void WifiConnecte() {
  //開始WiFi連線
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("WiFi連線成功");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());
}

//開始MQTT連線
void MQTTConnecte() {
  MQTTClient.setServer(MQTTServer, MQTTPort);
  MQTTClient.setCallback(MQTTCallback);
  while (!MQTTClient.connected()) {
    //以亂數為ClietID
    String MQTTClientid = "esp32-" + String(random(1000000, 9999999));
    if (MQTTClient.connect(MQTTClientid.c_str(), MQTTUser, MQTTPassword)) {
      //連結成功，顯示「已連線」。
      Serial.println("MQTT已連線");
      MQTTClient.subscribe(MQTTSubTopic1);  //訂閱SubTopic1主題
      MQTTClient.subscribe(MQTTSubTopic2);  //訂閱SubTopic2主題
    } else {
      //若連線不成功，則顯示錯誤訊息，並重新連線
      Serial.print("MQTT連線失敗,狀態碼=");
      Serial.println(MQTTClient.state());
      Serial.println("五秒後重新連線");
      delay(5000);
    }
  }
}

//接收到訂閱時
void MQTTCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print(topic); 
  Serial.print("訂閱通知:");

  //將接收的payload轉成字串
  String payloadString; 
  for (int i = 0; i < length; i++) {
    payloadString = payloadString + (char)payload[i];
  }
  Serial.println(payloadString);
  
  //比對主題是否為訂閱主題1
  if (strcmp(topic, MQTTSubTopic1) == 0) {
    Serial.println("pin：" + payload[0]);
    wifi_flag1 = 1;  //topic 相同，開啟 wifi 判定
    wifi_buffer_1 = payloadString;

  }
  // 比對主題是否為訂閱主題 2
  if (strcmp(topic, MQTTSubTopic2) == 0) {
    Serial.println("Topic 2 pin：" + payload[0]);
    wifi_flag2 = 1;  // topic 2 相同，開啟 wifi 判定
    wifi_buffer_2 = payloadString;
  }
}