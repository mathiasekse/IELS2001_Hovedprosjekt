#ifndef MICROPHONE_DATA_H
#define MICROPHONE_DATA_H

#include <stdint.h>
#include <stddef.h>
#include <esp_err.h>
// === MICROPHONE CONFIG ===
static const uint16_t MIC_SAMPLE_RATE = 16000;
static const uint16_t BUFFER_SAMPLES = 512;
static const uint8_t MIC_AMP_CONSTANT = 1;

// === MIC I2S PORT/PINS ===
static const uint8_t I2S_MIC_BCLK = 26;
static const uint8_t I2S_MIC_LRCLK = 25;
static const uint8_t I2S_MIC_DIN = 34;
#define I2S_MIC_PORT I2S_NUM_0


// === 16 BIT CONSTRAINS ===
static const int16_t CONSTRAIN_MIN_VALUE = -32768;
static const int16_t CONSTRAIN_MAX_VALUE = 32767;


// === FUNCTION PROTOTYPE ===
esp_err_t micInit(void);
int16_t* micData(void);
uint16_t getSamplesReceived(void);


// === VARIABLES ===
extern uint16_t samples_in;
extern int32_t buffer_in[BUFFER_SAMPLES];
extern int16_t buffer_out[BUFFER_SAMPLES];
extern size_t bytes_read, bytes_written;


#endif