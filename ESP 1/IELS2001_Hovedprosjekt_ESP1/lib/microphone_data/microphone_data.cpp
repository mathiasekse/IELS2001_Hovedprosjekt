#include "microphone_data.h"
#include <driver/i2s.h>
#include <Arduino.h>

// === DC BLOCK KOEFFESIENT ===
#define a 0.97                                    // Juster om nødvendig
#define A_Q15 ((int16_t)( (a) * 32768.0 + 0.5 ))  // Regner om a til Q15 fikspunktverdi


// === GLOBALE VARIABLER ===
// Til lyddata
uint16_t samples_in;
int32_t buffer_in[BUFFER_SAMPLES];
int16_t buffer_out[BUFFER_SAMPLES];
size_t bytes_read, bytes_written;

// Til høypassfilter
int16_t prev_input = 0;
int32_t prev_output_q15 = 0;


/**
 * @brief Høypassfilter for å kompansere for DC-OFFSET
 * Filtrerer basert på første ordens differenslikning: y[n] = x[n] - x[n-1] + a * y[n-1]
 * Filtreringen skjer i Q15-fikspunkt, altså 1 fortegnsbit og 15 bits til brøkdelen
 * @param input punktprøveverdien som skal filtreres
 * @return int16_t filtrert verdi basert på forrige verdi
 */
int16_t highpass_fixed(int16_t input) {
  int16_t delta = input - prev_input;
  prev_input = input;

  // Løser differenslikningen
  int32_t y_q15 = ((int32_t)delta << 15) + ((int32_t)A_Q15 * prev_output_q15 >> 15);
  
  prev_output_q15 = y_q15;

  // Skaler ned og returner i int16_t
  return (int16_t)(y_q15 >> 16);  // Ekstra demping for sikkerhetsmargin
}


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
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 4,
      .dma_buf_len = 512,
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

    result = i2s_set_clk(I2S_MIC_PORT, MIC_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_32BIT, I2S_CHANNEL_STEREO);
    if (result != ESP_OK) {
      return result;
    }
    
    return result;
}



/**
 * @brief Leser av mikrofondata med i2s-protokollen
 * Tar inn 32-bit lydsignal fra mikrofonen. Mikrofonen har 18 bit gyldige data, så forkaster
 * de 14 nullerene bakerst ved å høyreskifte med 14 og beholder fortegnsbitet. Vi er fornøyd med en
 * 16 bit oppløsning og ønsker derfor å lagre dataen som int16_t for å spare minne. Til slutt 
 * filtreres signalet for å kompensere for DC-offset.
 * @return int16_t* Lagrer dataen som en buffer
 */
int16_t* micData() {
  if (i2s_read(I2S_MIC_PORT, buffer_in, sizeof(buffer_in), &bytes_read, portMAX_DELAY) == ESP_OK) {
    samples_in = bytes_read / sizeof(int32_t); // totalt antall samples

    int out_index = 0;
    for (int i = 0; i < samples_in; i += 2) {
      int32_t raw = buffer_in[i];
      int32_t sample_18 = raw >> 14;
      int16_t sample_16 = (int16_t)(sample_18 >> 2);
      int16_t filtered_16 = highpass_fixed(sample_16);

      buffer_out[out_index++] = filtered_16;
    }

    samples_in = out_index;
    return buffer_out;
  }

  return nullptr;
}


/**
 * @brief Gjør det mulig for andre filer å hente mottatte samples fra mikrofonen
 * 
 * @return uint16_t samples lest fra mikrofon
 */
uint16_t getSamplesReceived(void) {
  return samples_in;
}