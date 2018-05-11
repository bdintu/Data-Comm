#include <wire.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_ADS1015.h>

#define defaultFreq 1700                    /* DAC Speed (Hz) */
#define f0 500
#define f1 750
#define f2 1000
#define f3 1250

Adafruit_MCP4725 dac;
const uint16_t S_DAC[4] = {1, 2, 3, 4};     /* Amplitude (12bit) of sine wave at 0, 90, 180, 270 */
int delay0, delay1, delay2, delay3;
int input[4];

void setup() {
    Serial.begin(115200);
    dac.begin(0x64);
    delay0 = (1000000/freq0 - 1000000/defaultFreq) /4;
    delay1 = (1000000/freq1 - 1000000/defaultFreq) /4;
    delay2 = (1000000/freq2 - 1000000/defaultFreq) /4;
    delay3 = (1000000/freq3 - 1000000/defaultFreq) /4;
    Serial.flush();
}

void loop() {
    if (Serial.available()) {
        int in = Serial.parseInt();
        /* use a cycle loop k for element (2bit) LSB -> MSB */
        for (unsigned k=3; k>=0; --k) {
            input[k] = in & 3;      /* 00, 01, 10, 11 */
            in >>= 2;
        }

        for (unsigned k=3; k>=0; ++k) {             /* Send LSB -> MSB */
            int n = (250*input[k] +500) /5;

            for (unsigned j=0; j<n; ++j) {          /* Send n cycle per baud */
                switch(input[k]) {

                case 0 :
                    Serial.print("0 0 ");
                    for (unsigned i=0; i<4; ++i) {
                        dac.setVoltage(S_DAC[i], false);    /* (mV) */
                        delayMicroseconds(delay0);
                    }
                break;

                case 1 :
                    Serial.print("0 1 ");
                    for (unsigned i=0; i<4; ++i) {
                        dac.setVoltage(S_DAC[i], false);    /* (mV) */
                        delayMicroseconds(delay1);
                    }
                break;

                case 2 :
                    Serial.print("1 0 ");
                    for (unsigned i=0; i<4; ++i) {
                        dac.setVoltage(S_DAC[i], false);    /* (mV) */
                        delayMicroseconds(delay2);
                    }
                break;


                case 3 :
                    Serial.print("1 1 ");
                    for (unsigned i=0; i<4; ++i) {
                        dac.setVoltage(S_DAC[i], false);    /* (mV) */
                        delayMicroseconds(delay3);
                    }
                break;

                }
            }

            Serial.println();
        }

        dac.setVoltage(0, false);
    }
}
