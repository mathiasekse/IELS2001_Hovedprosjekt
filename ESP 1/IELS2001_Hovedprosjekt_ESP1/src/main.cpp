#include <Arduino.h>
#include <driver/i2s.h>

/*------------------*/
/*---DEFINE BEGIN---*/
/*------------------*/
// Mikrofon (SPH0645)
// WS = LRCLK = GPIO 25
// SCK = BCLK = GPIO 26
// SD = BOUT = GPIO 35
// PORT = KANAL = 0
#define I2S_MIC_WS      25
#define I2S_MIC_SCK     26
#define I2S_MIC_SD      35
#define I2S_MIC_PORT    I2S_NUM_0

// DAC (MAX98357A)
// WS = LRCLK = GPIO 22
// SCK = BCLK = GPIO 27
// SD = BIN = GPIO 31
// PORT = KANAL = 1
#define I2S_DAC_WS      22
#define I2S_DAC_SCK     27
#define I2S_DAC_SD      21
#define I2S_DAC_PORT    I2S_NUM_1


// Tale er typisk 0-8kHz, bruker derfor sample rate på 16kHz for å få representert dette området
#define SAMPLE_RATE     16000
#define DIGITAL_AMP_CONSTANT 1

/*----------------*/
/*---DEFINE END---*/
/*----------------*/


/*------------------------------*/
/*---FUNCTION PROTOTYPE BEGIN---*/
/*------------------------------*/
void microphone_INIT(void);
void speaker_INIT(void);
void listen_to_microphone(void);
/*----------------------------*/
/*---FUNCTION PROTOTYPE END---*/
/*----------------------------*/



/*---------------------*/
/*---VARIABLES BEGIN---*/
/*---------------------*/
const int samples = 512;
int32_t buffer_in[samples];
int16_t buffer_out[samples];
size_t bytes_read, bytes_written;
/*-------------------*/
/*---VARIABLES END---*/
/*-------------------*/



void setup() {
  Serial.begin(115200);
  delay(1000);
  microphone_INIT();
  speaker_INIT();
}



void loop() {
  listen_to_microphone();
}



void listen_to_microphone(void) {
   // LESER AV MIKROFONDATA
  if (i2s_read(I2S_MIC_PORT, buffer_in, sizeof(buffer_in), &bytes_read, portMAX_DELAY) == ESP_OK) {
    int count = bytes_read / sizeof(int32_t);
    
    // Kun gyldig data på venstre kanal, lagrer derfor kun partallsindex i rådataen
    for (int i = 0; i < count; i += 2) {
      int32_t raw = (buffer_in[i] << 8)>>8;  // Konverterer 32-bit datapakken til 24-bit lesbar data.
      int16_t out = constrain((raw >> 8)*DIGITAL_AMP_CONSTANT, -32768, 32767);  // konverter 24-bit til 16-bit og tvinger den innenfor int16_t sitt område
                                                             // Multipliserer med 6 for å forsterke signalet
      buffer_out[i] = out;       // Lagrer bufferen i venstre kanal
      buffer_out[i + 1] = out;   // Kopierer samme buffer i høyre kanal for stereo effekt.
    }
    // SEND BUFFER UT TIL DAC
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
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
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



void speaker_INIT(void) {
 // === DAC (I2S TX) ===
  i2s_config_t i2s_out_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
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
