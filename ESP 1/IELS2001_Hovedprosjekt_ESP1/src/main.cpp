#include <Arduino.h>
#include "speaker_data.h"
#include "microphone_data.h"



void setup() {
  Serial.begin(115200);
  delay(1000);
  microphone_INIT();
  speaker_INIT();
}



void loop() {
  int16_t* mic_data = microphone_data();
  if (mic_data != nullptr && get_sample_count() > 0) {
    size_t bytes_written = 0;
    i2s_write(I2S_DAC_PORT, mic_data, get_sample_count() * sizeof(int16_t), &bytes_written, portMAX_DELAY);
  }
}









