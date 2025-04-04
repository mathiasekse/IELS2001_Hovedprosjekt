#include <Arduino.h>
#include "speaker_data.h"
#include "microphone_data.h"



void setup() {
  Serial.begin(115200);
  delay(1000);
  microphone_INIT();
  speaker_INIT();
  i2s_set_clk(I2S_DAC_PORT, DAC_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
}



void loop() {
  int16_t* mic_data = microphone_data();
  if (mic_data != nullptr && get_sample_count() > 0) {
    size_t bytes_written = 0;
    i2s_write(I2S_DAC_PORT, mic_data, get_sample_count() * sizeof(int16_t), &bytes_written, portMAX_DELAY);
  }
}









