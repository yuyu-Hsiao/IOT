#include "wifi_mqtt.h"

WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);

// WiFi/MQTT相關設定
char* ssid = "";
char* password = "";

char* MQTTServer = "";
int MQTTPort = 1883;                                    //MQTT Port
char* MQTTUser = "";
char* MQTTPassword = "";
char* MQTTSubTopic1 = "rvlTopic/rvl122/servo122_3";     //訂閱主題

String wifi_buffer;
boolean wifi_flag = false;



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
    wifi_flag = 1;  //topic 相同，開啟 wifi 判定
    wifi_buffer = payloadString;
  }
}