#ifndef WEBSOCKET_CLIENT
#define WEBSOCKET_CLIENT

// === BIBLIOTEKER ===
#include <WiFi.h>
#include <ArduinoWebsockets.h>

// === Nettverkskonfigurasjoner fra tilhørende .cpp-fil ===
extern const char* ssid;
extern const char* password;
extern const char* websocket_server;

extern websockets::WebsocketsClient client;


// === FUNKSJONS PROTOTYPER ===
void setup_wifi(void);
void onEventsCallback(websockets::WebsocketsEvent event, String data);
void onMessageCallback(websockets::WebsocketsMessage message);
void websocket_init(void);


#endif