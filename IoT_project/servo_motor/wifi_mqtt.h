#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>

extern WiFiClient WifiClient;
extern PubSubClient MQTTClient;
extern char* ssid;
extern char* password;
extern char* MQTTServer;
extern int MQTTPort;
extern char* MQTTUser;
extern char* MQTTPassword;
extern char* MQTTSubTopic1;
extern String wifi_buffer;
extern boolean wifi_flag;

// 函式宣告
void WifiConnecte();
void MQTTConnecte();
void MQTTCallback(char* topic, byte* payload, unsigned int length);

#endif