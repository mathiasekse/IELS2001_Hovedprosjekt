#include <Arduino.h>
#include <driver/i2s.h>

// Mikrofon (SPH0645)
// WS = LRCLK and 
#define I2S_MIC_WS      25
#define I2S_MIC_SD      35
#define I2S_MIC_SCK     26
#define I2S_MIC_PORT    I2S_NUM_0

// DAC (MAX98357A)
#define I2S_DAC_WS      22
#define I2S_DAC_SD      21
#define I2S_DAC_SCK     27
#define I2S_DAC_PORT    I2S_NUM_1

#define SAMPLE_RATE     16000


void microphone_INIT();
void speaker_INIT();


void setup() {
  Serial.begin(115200);
  delay(1000);
  microphone_INIT();
  speaker_INIT();
}

void loop() {
  const int samples = 512;
  int32_t buffer_in[samples];
  int16_t buffer_out[samples];
  size_t bytes_read, bytes_written;

  if (i2s_read(I2S_MIC_PORT, buffer_in, sizeof(buffer_in), &bytes_read, portMAX_DELAY) == ESP_OK) {
    int count = bytes_read / sizeof(int32_t);

    for (int i = 0; i < count; i += 2) {
      int32_t raw = buffer_in[i] >> 8;  // venstre kanal
      int16_t out = constrain((raw >> 8)*8, -32767, 32767);  // konverter 24-bit til 16-bit

      buffer_out[i] = out;       // venstre kanal
      buffer_out[i + 1] = out;   // høyre kanal
    }

    i2s_write(I2S_DAC_PORT, buffer_out, sizeof(int16_t) * count, &bytes_written, portMAX_DELAY);
  }
}



void microphone_INIT(void) {
  /**
   * @brief 
   * 
   */
   i2s_config_t i2s_in_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
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


  i2s_pin_config_t mic_pins = {
    .bck_io_num = I2S_MIC_SCK,
    .ws_io_num = I2S_MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SD
  };

  i2s_driver_install(I2S_MIC_PORT, &i2s_in_config, 0, NULL);
  i2s_set_pin(I2S_MIC_PORT, &mic_pins);
  i2s_set_clk(I2S_MIC_PORT, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_32BIT, I2S_CHANNEL_STEREO);
}



void speaker_INIT() {
 // === DAC (I2S TX) ===
  i2s_config_t i2s_out_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
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
  i2s_set_clk(I2S_DAC_PORT, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
}



/*-------------------------------*/
/*---RELEVANTE SAKER FRA i2s.h---*/
/*-------------------------------*/

// typedef struct {
//     int mck_io_num;     /*!< MCK in out pin. Note that ESP32 supports setting MCK on GPIO0/GPIO1/GPIO3 only*/
//     int bck_io_num;     /*!< BCK in out pin*/
//     int ws_io_num;      /*!< WS in out pin*/
//     int data_out_num;   /*!< DATA out pin*/
//     int data_in_num;    /*!< DATA in pin*/
// } i2s_pin_config_t;


// typedef struct {

//     i2s_mode_t              mode;                       /*!< I2S work mode */
//     uint32_t                sample_rate;                /*!< I2S sample rate */
//     i2s_bits_per_sample_t   bits_per_sample;            /*!< I2S sample bits in one channel */
//     i2s_channel_fmt_t       channel_format;             /*!< I2S channel format.*/
//     i2s_comm_format_t       communication_format;       /*!< I2S communication format */
//     int                     intr_alloc_flags;           /*!< Flags used to allocate the interrupt. One or multiple (ORred) ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info */
//     int                     dma_buf_count;              /**< The total number of DMA buffers to receive/transmit data.
//                                                           * A descriptor includes some information such as buffer address,
//                                                           * the address of the next descriptor, and the buffer length.
//                                                           * Since one descriptor points to one buffer, therefore, 'dma_desc_num' can be interpreted as the total number of DMA buffers used to store data from DMA interrupt.
//                                                           * Notice that these buffers are internal to'i2s_read' and descriptors are created automatically inside of the I2S driver.
//                                                           * Users only need to set the buffer number while the length is derived from the parameter described below.
//                                                           */
//     int                     dma_buf_len;                /**< Number of frames in a DMA buffer.
//                                                           *  A frame means the data of all channels in a WS cycle.
//                                                           *  The real_dma_buf_size = dma_buf_len * chan_num * bits_per_chan / 8.
//                                                           *  For example, if two channels in stereo mode (i.e., 'channel_format' is set to 'I2S_CHANNEL_FMT_RIGHT_LEFT') are active,
//                                                           *  and each channel transfers 32 bits (i.e., 'bits_per_sample' is set to 'I2S_BITS_PER_CHAN_32BIT'),
//                                                           *  then the total number of bytes of a frame is 'channel_format' * 'bits_per_sample' = 2 * 32 / 8 = 8 bytes.
//                                                           *  We assume that the current 'dma_buf_len' is 100, then the real length of the DMA buffer is 8 * 100 = 800 bytes.
//                                                           *  Note that the length of an internal real DMA buffer shouldn't be greater than 4092.
//                                                           */

//     bool                    use_apll;                   /*!< I2S using APLL as main I2S clock, enable it to get accurate clock */
//     bool                    tx_desc_auto_clear;         /*!< I2S auto clear tx descriptor if there is underflow condition (helps in avoiding noise in case of data unavailability) */
//     int                     fixed_mclk;                 /*!< I2S using fixed MCLK output. If use_apll = true and fixed_mclk > 0, then the clock output for i2s is fixed and equal to the fixed_mclk value. If fixed_mclk set, mclk_multiple won't take effect */
//     i2s_mclk_multiple_t     mclk_multiple;              /*!< The multiple of I2S master clock(MCLK) to sample rate */
//     i2s_bits_per_chan_t     bits_per_chan;              /*!< I2S total bits in one channel， only take effect when larger than 'bits_per_sample', default '0' means equal to 'bits_per_sample' */

// #if SOC_I2S_SUPPORTS_TDM
//     i2s_channel_t           chan_mask;                  /*!< I2S active channel bit mask, set value in `i2s_channel_t` to enable specific channel, the bit map of active channel can not exceed (0x1<<total_chan). */
//     uint32_t                total_chan;                 /*!< I2S Total number of channels. If it is smaller than the biggest active channel number, it will be set to this number automatically. */
//     bool                    left_align;                 /*!< Set to enable left alignment */
//     bool                    big_edin;                   /*!< Set to enable big edin */
//     bool                    bit_order_msb;              /*!< Set to enable msb order */
//     bool                    skip_msk;                   /*!< Set to enable skip mask. If it is enabled, only the data of the enabled channels will be sent, otherwise all data stored in DMA TX buffer will be sent */
// #endif // SOC_I2S_SUPPORTS_TDM

// } i2s_driver_config_t;

// typedef i2s_driver_config_t i2s_config_t;   