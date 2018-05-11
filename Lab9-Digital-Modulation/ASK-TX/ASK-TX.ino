#include <Wire.h>
#include <Adafruit_MCP4725.h>

#define defaultFreq 1700            /* DAC Speed */
#define freq0 1000                  /* frequency of carrier sine wave (Hz) */
#define N 1                         /* Number of Data (byte) */

Adafruit_MCP4725 dac;
const float A[4] = {1 ,2 , 3, 4};   /* ASK Amplitude (0, 5] (V) */
const uint16_t S_DAC[4] = {2047 ,4095 ,2047 , 0};   /* Amplitude (12bit) of sinewave at 0, 90, 180, 270 */
int delay0;
char inData[N];

void setup() {
    Serial.begin(115200);
    dac.begin(0x64);
    delay0 = (1000000/freq0 - 1000000/defaultFreq) / 4;
    Serial.flush();
}

void loop() {
   if (Serial.available()) {
    for (unsigned i=0; i<N; ++i) {
        inData[i] = Serial.read();
    }

    /* use a cycle loop i for sebd data 8 bits */
    for (unsigned i=0; i<N; ++i) {
        /* use a cycle loop k for 1 ASK signal element (2 bit) LSB-> MSB */
        for (unsigned k=7; k>=0; k-=2) {
            int tmp = inData[i] & 0x03;                 /* 00 01 10 11 */
            /* use a cycle loop sl to send 5 cycle/baud */
            for (unsigned sl=0; sl<5; ++sl) {
                /* use a cycle loop s to send 4 sample/cycle */
                for (unsigned s=0; s<4; ++s) {
                    float outData = A[tmp]*S_DAC[s] /4;
                    Serial.println(outData);
                    dac.setVoltage(outData, false);     /* (mV) */
                    delayMicroseconds(dalay0);
                }
            }
            inData[i] >>= 0x02;
        }
    }

    dac.setVoltage(0, false);
}
