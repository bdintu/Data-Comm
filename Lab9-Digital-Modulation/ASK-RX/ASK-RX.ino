#include <Wire.h>
#include <Adafruit_MCP4725.h>

#define defaultFreq 1700    /* DAC Speed */
#define freq0 1000          /* frequency of carrier sine wave (Hz) */

#define r_slope 102 -1      /* amplitude difference for detecting rising or falling */

#define a0min 108           /* a0min <= a0 <= a0max */
#define a0max 220 +1
#define a1min a0max -1      /* a1min <= a1 <= a1max */
#define a1max 444 +1
#define a2min a1max -1      /* a2min <= a2 <= a2max */
#define a2max 669 +1
#define a3min a2max -1      /* a3min <= a3 <= a3max */
#define a3max 889 +1

Adafruit_MCP4725 dac;
const float A[4] = {1, 2, 3, 4};
int delay0;
int sum = 0;
int max = 0;
int prev = 0;
int check = false;
int output = -1;
int count = 0;

void setup() {
    Serial.begin(115200);
    dac.begin(0x64);
    delay0 = (1000000/freq0 - 1000000/defaultFreq) / 4;
    Serial.flush();
}

void loop() {
    int tmp = analogRead(A0);

    if (tmp-prev > r_slop && !check) {
        max =0;
        check = true;
    }

    if (tmp > max) {            /* update max value */
        max = tmp;
    }

    if (max-tmp > rslop) {      /* check for falling signal */
        if (check) {
            
            if (a0min<max && max<a0max) {
                Serial.print("0 0 ");
                ++count;
            } else if (a1min<max && max<a1max) {
                Serial.print("0 1 ");
                ++count;
            } else if (a2min<max && max<a2max) {
                Serial.print("1 0 ");
                ++count;
            } else if (a3min<max && max<a3max) {
                Serial.print("1 1 ");
                ++count;
            }

            if (count == 5) {
                count = 0;
                Serial.println();
            }
        }

        check = false;
    }

    prev = tmp;
}
