#include "websocket_client.h"

using namespace websockets;

const char* ssid        = "REALWIFI-6511";
const char* password    = "123spill";
const char* websocket_server = "ws://192.168.1.1:1880/endpoint/ws/audio";

WebsocketsClient client;

void setup_wifi(void) {
  WiFi.begin(ssid, password);
  Serial.println("Kobler til WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }
  Serial.println("WiFi tilkoblet!");
  delay(100);
}



void websocket_init(void) {
  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  bool connected = client.connect(websocket_server);
  if (connected) {
    Serial.println("Koblet til WebSocket server!");
  } else {
    Serial.println("Kunne ikke koble til WebSocket server!");
  }
}



void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("WebSocket tilkoblet!");
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("WebSocket frakoblet!");
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Ping mottatt!");
  }
}


void onMessageCallback(WebsocketsMessage message) {
  if (message.isText()) {
    Serial.println("Mottatt tekst: " + message.data());
  } else {
    Serial.println("Mottatt binær data");
  }
}