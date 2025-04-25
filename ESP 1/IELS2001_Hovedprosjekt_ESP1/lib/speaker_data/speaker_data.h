#ifndef SPEAKER_DATA_H
#define SPEAKER_DATA_H

#include <driver/i2s.h>
#include <esp_err.h>



// === DAC CONFIG ===
static const uint16_t DAC_SAMPLE_RATE = 16000;



// === DAC I2S PORT/PINS ===
#define I2S_DAC_PORT I2S_NUM_1
static const uint8_t I2S_DAC_BCLK = 27;
static const uint8_t I2S_DAC_LRCLK = 22;
static const uint8_t I2S_DAC_DOUT = 21;



// === FUNCTION PROTOTYPE ===
esp_err_t dacInit(void);


#endif