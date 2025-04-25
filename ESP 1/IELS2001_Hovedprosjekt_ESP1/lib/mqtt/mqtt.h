#ifndef MQTT_H
#define MQTT_H

// ======== MQTT ==========
#include <WiFi.h>
#include <PubSubClient.h>
extern const char* ssid;
extern const char* password;
extern const char* mqtt_server;
extern const int   mqtt_port;
extern const char* mqtt_topic;
extern const char* mqtt_message;


// === OBJECT DECLARATION ===
extern WiFiClient espClient;
extern PubSubClient client;


// === FUNCTION PROTOTYPE ===
void setup_wifi(void);
void reconnect_mqtt(void);


#endif