#define PWM_PIN 6
#define N_BIT 4

char str[80];
float zeta[N_BIT];
float S[N_BIT];
uint16_t pwmDuty[N_BIT];

void setup() {
  Serial.begin(115200);
  pinMode(PWM_PIN, OUTPUT);

  for (int i = 0; i < N_BIT; ++i) {
    zeta[i] = i * (360 / N_BIT);
    S[i] = sin(zeta[i] * (M_PI / 180));
    pwmDuty[i] = ((S[i] + 1) / 2) * 255;
    sprintf(str, "zeta:%6s, sine:%5s, pwmDuty:%3d", String(zeta[i]).c_str(), String(S[i]).c_str(), pwmDuty[i]);
    Serial.println(str);
  }
}

void loop() {
  for (int i = 0; i < N_BIT; ++i) {
    analogWrite(PWM_PIN, pwmDuty[i]);
    delayMicroseconds(400);
  }
}
