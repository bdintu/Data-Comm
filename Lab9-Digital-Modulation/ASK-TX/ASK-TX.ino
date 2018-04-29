#include <Wire.h>
#include <Adafruit_MCP4725.h>

#define defaultFreq 1700
#define freq0 500

Adafruit_MCP4725 dac;
const float A[4] = {_,_,_,_};
const uint16_t S_DAC[4] = {_,_,_,_};
int delay0;
char inData[20];

void setup() {
    Serial.begin(115200);
    dac.begin(0x64);
    delay0 = (1000000/freq0 - 1000000/defaultFreq) / 4;
    Serial.flush();
}

void loop() {
   if (Serial.available() > 0) {
    for (unsigned i=0; i<20; ++i) {
        inData[i] = Serial.read();
    }

    for (unsigned i=0; i<20; ++i) {
        for (unsigned k=7; k>=0; k-=2) {
            int tmp = inData[i] & 3;

            for (unsigned sl=0; sl<5; ++sl) {
                for (unsigned s=0; s<4; ++s) {
                    dac.setVoltage(S_DAC[s], false);
                    delayMicroseconds(dalay0);
                }
            }
            inData[i] >>= 2;
        }
    }

    dac.setVoltage(0, false);
}
