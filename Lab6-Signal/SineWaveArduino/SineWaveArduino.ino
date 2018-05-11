#include <Wire.h>
#include <Adafruit_MCP4725.h>

#define defaultFreq 1700        /* DAC Speed */
#define freq0 500               /* frequency of carrier size wave (Hz) */
#define N 4                     /* number of angle */

Adafruit_MCP4725 dac;
float zeta[N_BIT];              /* N=4, zeta : 0, 90, 180, 270 */
float S[N_BIT];                 /* Sinewave of zeta */
uint16_t S_DAC[N_BIT];          /* DutyCycle PWM of sizewave */
int delay0;                     /* delay for period of sizewave */
char str[80];

void setup() {
  Serial.begin(115200);
  dac.begin(0x64);
  delay0 = (1000000 / freq0 - 1000000 / defaultFreq) / N;
  sprintf(str, "delay0:", delay0);
  Serial.println(str);

  for (unsigned i=0; i<N; ++i) {
    zeta[i] = i * (360 / N);                /* geb zeta (deg) */
    S[i] = sin(zeta[i] * (M_PI / 180));     /* covent zeta(deg) to sin(rad) */
    /* MCP4725 DAC pwm (0-4095) */
    S_DAC[i] = ((S[i] + 1) / 2) * 4095;      /* gen DutyCycle PWM from sine */ 
    sprintf(str, "zeta:%6s, sine:%5s, S_DAC:%3d", String(zeta[i]).c_str(), String(S[i]).c_str(), S_DAC[i]);
    Serial.println(str);
  }
}

void loop() {
  for (unsigned i=0; i<N; ++i) {
    dac.setVoltage(S_DAC[i], false);
    delayMicroseconds(delay0);
  }
}
