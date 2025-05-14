#ifndef WEBSOCKET_CLIENT
#define WEBSOCKET_CLIENT
#include <WiFi.h>
#include <ArduinoWebsockets.h>


extern const char* ssid;
extern const char* password;
extern const char* websocket_server;

extern websockets::WebsocketsClient client;



// ==== Lydkonfig ====
// const int sample_rate = 16000;        // 16kHz
// const int duration_sec = 5;            // 5 sekunder
// const int total_samples = sample_rate * duration_sec;

// const int chunk_samples = 512;         // samples per WebSocket-send (tilpass om nødvendig)

void setup_wifi(void);
void onEventsCallback(websockets::WebsocketsEvent event, String data);
void onMessageCallback(websockets::WebsocketsMessage message);
void websocket_init(void);

#endif