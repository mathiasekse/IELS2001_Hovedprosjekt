#include "mqtt.h"

const char* ssid        = "BV5";
const char* password    = "Bragevegen57035";
const char* mqtt_server = "host.docker.internal";
const int   mqtt_port   = 1881;
const char* mqtt_topic  = "esp32/voice/command";
const char* mqtt_message= "turn_on_light";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi(void) {
  WiFi.begin(ssid, password);
  Serial.println("Kobler til WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }
  Serial.println("WiFi tilkoblet!");
}


void reconnect_mqtt(void) {
  while (!client.connected()) {
    Serial.println("Kobler til MQTT...");
    if (client.connect("ESP32Client")){
      Serial.println("Tilkoblet");
    } else {
      Serial.print("Feil, rc=");
      Serial.print(client.state());
      Serial.println(" prøver igjen om 5 sekunder...");
      delay(5000);
    }
  }
}