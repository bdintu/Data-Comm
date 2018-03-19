#define PWM_PIN 6
#define N 4
char str[80];
float zeta[N];
float S[N];
uint16_t pwmDuty[N];

void setup() {
  Serial.begin(115200);
  pinMode(PWM_PIN, OUTPUT);
  for (int i = 0; i < N; ++i) {
    zeta[i] = i * (360 / N);
    S[i] = sin(zeta[i] * (M_PI / 180));
    pwmDuty[i] = ((S[i] + 1) / 2) * 255;
    sprintf(str, "zeta:%6s, sin:%5s, pwmDuty:%3d", String(zeta[i]).c_str(), String(S[i]).c_str(), pwmDuty[i]);
    Serial.println(str);
  }
}

void loop() {
  for (int i = 0; i < N; ++i) {
    analogWrite(PWM_PIN, pwmDuty[i]);
    delayMicroseconds(400);
  }
}
