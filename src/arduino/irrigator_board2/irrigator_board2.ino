/**
 * Dumi Loghin - 2020
 */

#define hc04_trig_pin   13
#define hc04_echo_pin   12
int dist = 100;

#define sl_pin      A0
#define sr_pin      A1
int sl_val = 0;
int sr_val = 0;

int rspeed = 128;
int lspeed = 128;


void setup() {
  // put your setup code here, to run once:

  // sonar
  pinMode(hc04_trig_pin, OUTPUT);
  pinMode(hc04_echo_pin, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

/*
  digitalWrite(hc04_trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(hc04_trig_pin, LOW);
  int pulseWidth = pulseIn(hc04_echo_pin, HIGH);
  dist = (3 * dist + pulseWidth / 58.2) / 4;
  sl_val = (3 * sl_val + analogRead(sl_pin)) / 4;
  sr_val = (3 * sl_val + analogRead(sr_pin)) / 4;
  */
  /*
  t++;
  if (t == 512) {
    t = 0;
  Serial.print(" Distance: ");
  Serial.println(dist);
  Serial.print("Proximity: ");
  Serial.print(sl_val);
  Serial.print(" ");
  Serial.println(sr_val);
  }
  */
}
