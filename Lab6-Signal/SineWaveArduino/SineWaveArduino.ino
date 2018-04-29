#include <Wire.h>
#include <Adafruit_MCP4725.h>

#define defaultFreq 1700
#define freq0 500
#define N_BIT 4

Adafruit_MCP4725 dac;
int delay0;
char str[80];
float zeta[N_BIT];
float S[N_BIT];
uint16_t S_DAC[N_BIT];

void setup() {

  Serial.begin(115200);
  dac.begin(0x64);
  delay0 = (1000000 / freq0 - 1000000 / defaultFreq) / N_BIT;
  sprintf(str, "delay0:", delay0);
  Serial.println(str);

  for (int i = 0; i < N_BIT; ++i) {
    zeta[i] = i * (360 / N_BIT);
    S[i] = sin(zeta[i] * (M_PI / 180));
    S_DAC[i] = ((S[i] + 1) / 2) * 255;
    sprintf(str, "zeta:%6s, sine:%5s, S_DAC:%3d", String(zeta[i]).c_str(), String(S[i]).c_str(), S_DAC[i]);
    Serial.println(str);
  }
}

void loop() {
  for (int i = 0; i < N_BIT; ++i) {
    dac.setVoltage(S_DAC[i], false);
    delayMicroseconds(delay0);
  }
}
