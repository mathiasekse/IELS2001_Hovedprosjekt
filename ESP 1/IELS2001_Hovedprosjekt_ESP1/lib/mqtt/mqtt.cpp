#include "mqtt.h"

const char* ssid        = "EspEkse";
const char* password    = "A3Dyi7qL";
const char* mqtt_server = "192.168.10.157";
const int   mqtt_port   = 1883;
const char* mqtt_topic  = "esp32/voice/command";
const char* mqtt_message= "Test_MQTT";

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
      client.subscribe("esp32/voice/command");
    } else {
      Serial.print("Feil, rc=");
      Serial.print(client.state());
      Serial.println(" prøver igjen om 5 sekunder...");
      delay(5000);
    }
  }
}



void mqtt_callback(char *topic, byte *payload, uint16_t length) {
  Serial.print("Melding mottatt på topic: ");
  Serial.println(topic);

  Serial.print("Innhold: ");
  for (uint16_t i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


// void mqtt_callback(char* topic, byte* payload, unsigned int length) {
//   String encoded_audio;
//   for (unsigned int i = 0; i < length; i++) {
//     encoded_audio += (char)payload[i];
//   }

//   int decoded_length = base64_dec_len(encoded_audio.c_str(), encoded_audio.length());
//   uint8_t* audio_data = (uint8_t*) malloc(decoded_length);

//   if (audio_data == nullptr) {
//     Serial.println("Ikke nok RAM!");
//     return;
//   }

//   base64_decode((char*)audio_data, encoded_audio.c_str(), encoded_audio.length());

//   size_t bytes_written;
//   i2s_write(I2S_DAC_PORT, audio_data, decoded_length, &bytes_written, portMAX_DELAY);

//   free(audio_data);
// }