// === NØDVENDIGE BIBLIOTEKER ===
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
#define SILENCE_THRESHOLD 400
#define SECOND_TO_MILLISECOND 1000

// === FUNKSJONSPROTOTYPING ===
void recordAndStreamAudio();
bool isChunkSilent(int16_t* buffer, size_t samples);



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
    
    // Setter sampling-rate
    esp_err_t sampleResult = i2s_set_sample_rates(I2S_NUM_0, I2S_SAMPLE_RATE);
    if (sampleResult != ESP_OK) {
        Serial.println("Feil ved initialisering av SAMPLE RATE, restart...");
        delay(3000);
        ESP.restart();
    }
    
    // Initialiserer WiFi tilkobling og WebSocket klient
    setup_wifi();
    websocket_init();
    
    Serial.println("Setup ferdig");
}



// === Kontinuerlig kommunikasjon over WebSocket ===
void loop() {
    client.poll();
    recordAndStreamAudio();
}



/**
 * @brief Sjekker om peaken fra siste lyd-chunk er over en terskelverdi
 * 
 * @param buffer Peker til hvilken buffer som skal sjekkes
 * @param samples Størrelsen til bufferen
 * @return true hvis lydnivået er under terskelverdien (altså stille)
 * @return false ellers
 */
bool isChunkSilent(int16_t* buffer, size_t samples) {
    int16_t peak = 0;
    for (size_t i = 0; i < samples; i++) {
        int16_t v = abs(buffer[i]);
        if (v > peak) peak = v;
    }
    return (peak < SILENCE_THRESHOLD);
}



/**
 * @brief Funksjon som tar seg av opptak og streaming til NodeRED over WebSocket
 * 
 */
void recordAndStreamAudio() {
    // Begynner ikke å streame dersom vi ikke har kobling
    if (!client.available()) {
      Serial.println("Websocket ikke tilkoblet!");
      return;
    }

    // Dersom det er oppdaget lyd, og vi har gyldige data, start opptak i N antall sekunder
    if (!isChunkSilent(micData(), getSamplesReceived())) {
        unsigned long startTime = millis();
        size_t totalBytesSent = 0;
        unsigned loopCounter = 0;
        Serial.println("Starter lydopptak");
        client.send("Start");                                 // Gir beskjed til NodeRED om start på lydopptak

        while (millis() - startTime < MAX_RECORDING_SECONDS * SECOND_TO_MILLISECOND) {
          int16_t* audioBuffer = micData();
          size_t samplesRead = getSamplesReceived();
          size_t bytes = samplesRead * sizeof(int16_t);
          totalBytesSent += bytes;
          client.sendBinary((const char*)audioBuffer, bytes);   // Sender lyddata over WebSocket
          client.poll();
        }

    client.send("End");                                       // Gir beskjed til NodeRED at lydopptaket er slutt
    Serial.println("Lydopptak ferdig");
    // Debug print
    Serial.print("Bytes sendt: ");
    Serial.println(totalBytesSent);                   
    }
}

