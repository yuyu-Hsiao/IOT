#include <WiFi.h>
#include <PubSubClient.h> 
#include <Ticker.h>
#include "esp_timer.h"


char* ssid = "";
char* password = "";

char* MQTTServer = "192.168.1.196";
int MQTTPort = 1883;		//MQTT Port
char* MQTTUser = "";
char* MQTTPassword = "";

//訂閱主題
char* MQTTSubTopic1 = "myTopic/relay";

String wifi_buffer; 
boolean wifi_flag;
boolean timebase_f;

#define relay1_output 16
#define relay2_output 17
#define relay3_output 18

boolean relay1_f=false;
boolean relay2_f=false;
boolean relay3_f=false;
int relay1_counter; 
int relay2_counter;
int relay3_counter;

Ticker timer_once;

WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);





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
 
  pinMode(relay1_output, OUTPUT);
  pinMode(relay2_output, OUTPUT);
  pinMode(relay3_output, OUTPUT);
  digitalWrite(relay3_output, HIGH);

  //設置timer中斷
  timer_setting(1000);
  
  //開始WiFi連線
  WifiConnecte();

  //開始MQTT連線
  MQTTConnecte();
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

  //1ms執行一圈
  if(timebase_f){
     MQTTClient.loop();//更新訂閱狀態

     //確定topic相同
     if(wifi_flag == 1){
       //判斷mqtt傳來的值
       if (wifi_buffer == "0") {
        relay1_counter = 1000;
        relay1_f = true;
        digitalWrite(relay1_output, HIGH);
      }
      if (wifi_buffer == "1") {
        relay2_counter = 1000;
        relay2_f = true;
        digitalWrite(relay2_output, HIGH);
      }
      if (wifi_buffer == "2") {
        relay3_counter = 1000;
        relay3_f = true;
        digitalWrite(relay3_output, HIGH);
      }
      wifi_flag = 0;
    }


   //開始計算1秒
   if(relay1_f == true){
      relay1_counter--;
      if(relay1_counter == 0){
        digitalWrite(relay1_output, LOW);
        relay1_f = false;
      }
   }
   if(relay2_f == true){
      relay2_counter--;
      if(relay2_counter == 0){
        digitalWrite(relay2_output, LOW);
        relay2_f = false;
      }
   }  
   if(relay3_f == true){
      relay3_counter--;
      if(relay3_counter == 0){
        digitalWrite(relay3_output, LOW);
        relay3_f = false;
      }
   }  
  timebase_f = false;
  }
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
      //訂閱SubTopic1主題
      MQTTClient.subscribe(MQTTSubTopic1);
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
    wifi_flag = 1;  //topic 相同，開啟 wifi 判定
    wifi_buffer = payloadString;

  }
}
