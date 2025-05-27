#include <Arduino.h>
#include <driver/i2s.h>
#include "microphone_data.h"
#include "speaker_data.h"
#include "ArduinoJson.h"
#include "websocket_client.h"

// === LYDOPPTAK KONFIGURASJON ===
#define I2S_SAMPLE_RATE 16000
#define BUFFER_SAMPLES 512
#define MAX_RECORDING_SECONDS 5
#define SILENCE_THRESHOLD 0
#define SECOND_TO_MILLISECOND 1000



void setup() {
    Serial.begin(115200);

    // Initialiser mikrofon
    esp_err_t micResult = micInit();
    if (micResult != ESP_OK) {
        Serial.println("Feil ved initialisering av mikrofon, restart...");
        delay(3000);
        ESP.restart();
    }
    
    // Initialiser DAC
    esp_err_t dacResult = dacInit();
    if (dacResult != ESP_OK) {
        Serial.println("Feil ved initialisering av DAC, restart...");
        delay(3000);
        ESP.restart();
    }
    
    // Sett sample rate
    esp_err_t sampleResult = i2s_set_sample_rates(I2S_NUM_0, I2S_SAMPLE_RATE);
    if (sampleResult != ESP_OK) {
        Serial.println("Feil ved initialisering av SAMPLE RATE, restart...");
        delay(3000);
        ESP.restart();
    }
    
    setup_wifi();
    websocket_init();
    
    Serial.println("Setup ferdig. Send 'r' for å starte opptak.");
}

// bool isChunkSilent(int16_t* buffer, size_t samples) {
//     int16_t peak = 0;
//     for (size_t i = 0; i < samples; i++) {
//         int16_t v = abs(buffer[i]);
//         if (v > peak) peak = v;
//     }
//     return (peak < SILENCE_THRESHOLD);
// }

void recordAndStreamAudio() {
    if (!client.available()) {
      Serial.println("Websocket ikke tilkoblet!");
      return;
    }

    Serial.println("Starter lydopptak");
    client.send("Start");
    delay(100);
    
    unsigned long startTime = millis();
    static size_t totalBytesSent = 0;
    unsigned loopCounter = 0;
    while (millis() - startTime < MAX_RECORDING_SECONDS * SECOND_TO_MILLISECOND) {
        int16_t* audioBuffer = micData();
        size_t samplesRead = getSamplesReceived();

        size_t bytes = samplesRead * sizeof(int16_t);
        totalBytesSent += bytes;
        client.sendBinary((const char*)audioBuffer, bytes);
        client.poll();
    }

    client.send("End");
    Serial.println("Lydopptak ferdig");
    Serial.print("Bytes sendt: ");
    Serial.println(totalBytesSent);
}

void loop() {
    client.poll();
    if (Serial.available() && Serial.read() == 'r') {
        recordAndStreamAudio();
    }
}