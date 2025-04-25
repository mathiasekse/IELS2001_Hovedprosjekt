#include <Arduino.h>
#include <driver/i2s.h>
#include "microphone_data.h"
#include "speaker_data.h"
#include "mqtt.h"


void setup() {
  Serial.begin(115200);

  // === INITIALISERING AV MIKROFON ===
  esp_err_t micResult = micInit();

  if (micResult != ESP_OK) {
    Serial.println("Feil ved initialisering av mikrofon, starter på nytt om 3 sekunder...");
    delay(3000);
    ESP.restart();
  } else {
    Serial.println("Mikrofon initialisert!");
  }

  // === INITIALISERING AV HØYTTALER/DAC ===
  esp_err_t dacResult = dacInit();

  if (dacResult != ESP_OK) {
    Serial.println("Feil ved initialisering av DAC/høyttaler, starter på nytt om 3 sekunder...");
    delay(3000);
    ESP.restart();
  } else {
    Serial.println("Høyttaler initialisert!");
  }
  
  // === INITIALISERING AV MQTT ===
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  // === MIKROFONDATA ===
  int16_t *mic_data = micData();

  // if (microphone_data != nullptr) {
  //   i2s_write(I2S_DAC_PORT, microphone_data, samples_received * sizeof(int16_t), &bytes_written, portMAX_DELAY);
  // }


  // === MQTT===
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();

  // Test-publisering én gang etter 5 sek
  static bool sent = false;
  if (!sent && millis() > 5000) {
    client.publish(mqtt_topic, mqtt_message);
    Serial.println("Melding sendt til MQTT!");
    sent = true;
  }
}


