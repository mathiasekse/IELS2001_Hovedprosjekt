#include "microphone_data.h"
#include <driver/i2s.h>
#include <Arduino.h>

int sample_count;
int32_t buffer_in[BUFFER_SAMPLES];
int16_t buffer_out[BUFFER_SAMPLES];
size_t bytes_read, bytes_written;


/**
 * @brief Konfigurasjon av I2S-mikrofon med ESP32
 * 
 */
void microphone_INIT(void) {
  // Konfigurasjon for å bruke I2S-protokollen
  // I vårt tilfelle: RX for å hente data. Bruker kun venstre kanal, ordlengde på 32 bit, I2S-format  
  i2s_config_t i2s_in_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = MIC_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 256,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  // Definerer hvilke I2S-pinner vi bruker. Kan endres i microphone_data.h
  i2s_pin_config_t mic_pins = {
    .bck_io_num = I2S_MIC_SCK,
    .ws_io_num = I2S_MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SD
  };

  // Funksjoner hentet fra i2s.h for implementasjon med ESP32, slik at riktig klokkesignal blir sendt ut
  i2s_driver_install(I2S_MIC_PORT, &i2s_in_config, 0, NULL);
  i2s_set_pin(I2S_MIC_PORT, &mic_pins);
  i2s_set_clk(I2S_MIC_PORT, MIC_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_32BIT, I2S_CHANNEL_STEREO);
}



/**
 * @brief Denne funksjonen leser av data med i2s-protokollen
 * 
 * @return int16_t* Lagrer dataen som en buffer
 */
int16_t* microphone_data() {
  if (i2s_read(I2S_MIC_PORT, buffer_in, sizeof(buffer_in), &bytes_read, portMAX_DELAY) == ESP_OK) {
    int samples_in = bytes_read / sizeof(int32_t); // hver sample = 32 bit
    int j = 0;

    for (int i = 0; i < samples_in; i += 2) { // kun venstre kanal
      // Fjern ubrukt topp og bunnbit → behold 24-bit og skaler ned
      int32_t raw = (buffer_in[i] << 8) >> 8; // 24-bit justering
      int16_t out = constrain(raw >> 8, -32768, 32767); // ned til 16-bit

      buffer_out[j++] = out; // venstre
      buffer_out[j++] = out; // høyre
    }

    sample_count = j; // antall int16_t verdier (stereo)
    return buffer_out;
  }

  return nullptr;
}


int get_sample_count(void) {
  return sample_count;
}
