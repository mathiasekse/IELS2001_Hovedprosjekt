#include "microphone_data.h"
#include <driver/i2s.h>
#include <Arduino.h>

int samples_in;
int32_t buffer_in[BUFFER_SAMPLES];
int16_t buffer_out[BUFFER_SAMPLES];
size_t bytes_read, bytes_written;


/**
 * @brief Konfigurerer mikrofonen til bruk med I2S-protokollen
 * 
 * @return esp_err_t ESP_OK ved vellykket initialisering, feilkode ellers
 */
esp_err_t micInit(void) {
    esp_err_t result;

    i2s_driver_config_t MIC_I2S_CONFIG = {
      .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = MIC_SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 4,
      .dma_buf_len = 256,
      .use_apll = false,
      .tx_desc_auto_clear = false,
      .fixed_mclk = 0,
      .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT
    };

    i2s_pin_config_t MIC_I2S_PINS = {
      .bck_io_num = I2S_MIC_BCLK,
      .ws_io_num = I2S_MIC_LRCLK,
      .data_out_num = I2S_PIN_NO_CHANGE,
      .data_in_num = I2S_MIC_DIN
    };

    result = i2s_driver_install(I2S_MIC_PORT, &MIC_I2S_CONFIG, 0, NULL);
    if (result != ESP_OK) {
      return result;
    }

    result = i2s_set_pin(I2S_MIC_PORT, &MIC_I2S_PINS);
    if (result != ESP_OK) {
      return result;
    }

    result = i2s_set_clk(I2S_MIC_PORT, MIC_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_32BIT, I2S_CHANNEL_MONO);
    if (result != ESP_OK) {
      return result;
    }

    return result;
}




/**
 * @brief Denne funksjonen leser av data med i2s-protokollen
 * 
 * @return int16_t* Lagrer dataen som en buffer
 */
int16_t* micData() {
  if (i2s_read(I2S_MIC_PORT, buffer_in, sizeof(buffer_in), &bytes_read, portMAX_DELAY) == ESP_OK) {
    samples_in = bytes_read / sizeof(int32_t); // hver sample = 32 bit

    for (int i = 0; i < samples_in; i ++) { 
      // Fjern ubrukt topp og bunnbit → behold 24-bit og skaler ned
      buffer_out[i] = (int16_t) (buffer_in[i] >> 16); // 24-bit justering, fjerner padding (ubrukte bits)
      
      if (MIC_AMP_CONSTANT != 1) {
        int32_t temp = buffer_out[i] * MIC_AMP_CONSTANT;
        buffer_out[i] = constrain(temp, CONSTRAIN_MIN_VALUE, CONSTRAIN_MAX_VALUE);
      }
    }

    return buffer_out;
  }

  return nullptr;
}


int getSamplesReceived(void) {
  return samples_in;
}
