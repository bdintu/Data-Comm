#define PWM_PIN 6
#define N 4
char str[80];
float zeta[N];
float S[N];
uint16_t pwmDuty[N];

void setup() {
  Serial.begin(115200);
  pinMode(PWM_PIN, OUTPUT);
  for(int i=0; i<4; ++i) {
    zeta[i] = i*(360/N);
    S[i] = sin((PI/2)*(zeta[i]/360));
    pwmDuty[i] = S[i]*255;
    sprintf(str, "zeta:%6.3f, sin:%6.3f, pwmDuty:%6.3f", zeta[i], S[i], pwmDuty[i]);
    Serial.println(str);
  }
}; 

void loop() {
  for(int i=0; i<4; ++i) {
    analogWrite(PWM_PIN, pwmDuty[i]);
    delayMicroseconds(400);
  }
}
