#include "speaker_data.h"


/**
 * @brief Initialisering av DAC/HØYTTALER med I2S-protokollen
 * 
 * @return esp_err_t ESP_OK ved vellykket initialisering av DAC. Feilmelding ellers.
 */
esp_err_t dacInit(void) {
  esp_err_t result;
 // === DAC (I2S TX) ===
  i2s_config_t DAC_I2S_CONFIG = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = DAC_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 256,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t DAC_I2S_PINS = {
    .bck_io_num = I2S_DAC_BCLK,
    .ws_io_num = I2S_DAC_LRCLK,
    .data_out_num = I2S_DAC_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  result = i2s_driver_install(I2S_DAC_PORT, &DAC_I2S_CONFIG, 0, NULL);
  if (result != ESP_OK) {
    return result;
  }

  result = i2s_set_pin(I2S_DAC_PORT, &DAC_I2S_PINS);
  if (result != ESP_OK) {
    return result;
  }

  result = i2s_set_clk(I2S_DAC_PORT, DAC_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
  if (result != ESP_OK) {
    return result;
  }

  return result;
}
