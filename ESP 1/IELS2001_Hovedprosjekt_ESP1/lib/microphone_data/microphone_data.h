#ifndef __MICROPHONE_DATA_H__
#define __MICROPHONE_DATA_H__

#include <stdint.h>
#include <stddef.h>

// === MICROPHONE CONFIG ===
#ifndef MIC_SAMPLE_RATE
#define MIC_SAMPLE_RATE 16000
#endif

#ifndef BUFFER_SAMPLES
#define BUFFER_SAMPLES 512
#endif

#ifndef DIGITAL_AMP_CONSTANT
#define DIGITAL_AMP_CONSTANT 1
#endif


// === MIC I2S PORT/PINS ===
#ifndef I2S_MIC_PORT
#define I2S_MIC_PORT I2S_NUM_0
#endif

#ifndef I2S_MIC_SCK
#define I2S_MIC_SCK 26
#endif

#ifndef I2S_MIC_WS
#define I2S_MIC_WS 25
#endif

#ifndef I2S_MIC_SD
#define I2S_MIC_SD 35
#endif



// === FUNCTION PROTOTYPE ===
void microphone_INIT(void);
int16_t* microphone_data(void);
int get_sample_count(void);


// === VARIABLES ===
extern int sample_count;
extern int32_t buffer_in[BUFFER_SAMPLES];
extern int16_t buffer_out[BUFFER_SAMPLES];
extern size_t bytes_read, bytes_written;


#endif