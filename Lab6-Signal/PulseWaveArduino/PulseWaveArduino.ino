#define PWM_PIN 6
#define N 4

float zeta[N];              /* N=4, zeta : 0, 90, 180, 270 */
float S[N];                 /* Sinewave of zeta*/
uint16_t pwmDuty[N];        /* DutyCycle PWM of sinewave */
char str[80];

void setup() {
  Serial.begin(115200);
  pinMode(PWM_PIN, OUTPUT);

  for (unsigned i=0; i<N; ++i) {
    zeta[i] = i * (360 / N);                /* gen zeta (deg) */
    S[i] = sin(zeta[i] * (M_PI / 180));     /* covent zeta(deg) to sine (rad) */
    /* arduino uno pwm (0-255) */
    pwmDuty[i] = ((S[i] + 1) / 2) * 255;    /* gen DutyCycle PWM from sine */
    sprintf(str, "zeta:%6s, sine:%5s, pwmDuty:%3d", String(zeta[i]).c_str(), String(S[i]).c_str(), pwmDuty[i]);
    Serial.println(str);
  }
}

void loop() {
  for (unsigned i=0; i<N; ++i) {
    analogWrite(PWM_PIN, pwmDuty[i]);
    delayMicroseconds(400);
  }
}
