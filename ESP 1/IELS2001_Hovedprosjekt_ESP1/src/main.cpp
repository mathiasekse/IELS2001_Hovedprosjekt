#include <Arduino.h>
#include <driver/i2s.h>
#include "microphone_data.h"
#include "speaker_data.h"
#include "mqtt.h"


// #define SAMPLE_RATE 16000
// #define TONE_FREQ 440
// #define BUFFER_SIZE (SAMPLE_RATE / TONE_FREQ)
// void generateTone(int16_t* buffer, size_t samples) {
//   for (size_t i = 0; i < samples; i++) {
//     float t = (float)i / SAMPLE_RATE;
//     float sample = sinf(2.0f * PI * TONE_FREQ * t);
//     buffer[i] = (int16_t)(sample * 32767); // 16-bit signed PCM
//   }
// }

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
  
  // // === INITIALISERING AV MQTT ===
  // setup_wifi();
  // client.setServer(mqtt_server, mqtt_port);
  // client.setCallback(mqtt_callback);
}


void loop() {
  // === MIKROFONDATA ===
  int16_t *mic_data = micData();
  uint16_t samples_received = getSamplesReceived();
    if (mic_data != nullptr) {
    // OPTIONAL: print peak amplitude for debugging
    int16_t peak = 0;
    for (int i = 0; i < samples_received; i++) {
      if (abs(mic_data[i]) > peak) {
        peak = abs(mic_data[i]);
      }
    }
    Serial.print("Peak: ");
    Serial.println(peak);

    // Send mic-data til DAC
    i2s_write(I2S_DAC_PORT, mic_data, samples_received * sizeof(int16_t), &bytes_written, portMAX_DELAY);
  }
// }
  // static int16_t buffer[BUFFER_SIZE];

  // generateTone(buffer, BUFFER_SIZE);

  // size_t bytesWritten;
  // i2s_write(I2S_DAC_PORT, buffer, sizeof(buffer), &bytesWritten, portMAX_DELAY);


  // // === MQTT===
  // if (!client.connected()) {
  //   reconnect_mqtt();
  // }
  // client.loop();

  // // Test-publisering én gang etter 5 sek
  // static bool sent = false;
  // if (!sent && millis() > 5000) {
  //   client.publish(mqtt_topic, mqtt_message);
  //   Serial.println("Melding sendt til MQTT!");
  //   sent = true;
  // }
}


