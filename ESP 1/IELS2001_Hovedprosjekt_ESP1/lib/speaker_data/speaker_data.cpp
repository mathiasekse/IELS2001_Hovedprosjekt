#include "speaker_data.h"

void speaker_INIT(void) {
 // === DAC (I2S TX) ===
  i2s_config_t i2s_out_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = DAC_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 256,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t dac_pins = {
    .bck_io_num = I2S_DAC_SCK,
    .ws_io_num = I2S_DAC_WS,
    .data_out_num = I2S_DAC_SD,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_DAC_PORT, &i2s_out_config, 0, NULL);
  i2s_set_pin(I2S_DAC_PORT, &dac_pins);
  i2s_set_clk(I2S_DAC_PORT, DAC_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
}
