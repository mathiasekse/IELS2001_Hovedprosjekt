#ifndef __SPEAKER_DATA_H__
#define __SPEAKER_DATA_H__

#include <driver/i2s.h>
// === DAC CONFIG ===

#ifndef DAC_SAMPLE_RATE
#define DAC_SAMPLE_RATE 16000
#endif

// === DAC I2S PORT/PINS ===
#ifndef I2S_DAC_PORT
#define I2S_DAC_PORT I2S_NUM_1
#endif

#ifndef I2S_DAC_SCK
#define I2S_DAC_SCK 27
#endif

#ifndef I2S_DAC_WS
#define I2S_DAC_WS 22
#endif

#ifndef I2S_DAC_SD
#define I2S_DAC_SD 21
#endif



// === FUNCTION PROTOTYPE ===
void speaker_INIT(void);


#endif