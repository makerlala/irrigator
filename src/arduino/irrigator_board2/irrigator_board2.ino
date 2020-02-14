/**
 * Dumi Loghin - 2020
   Irrigator - Arduino Board 2
*/
#include <SPI.h>

// SPI
volatile byte new_cmd = 0;  // command got from SPI
volatile byte spi_ret;      // response to send to SPI

#define hc04_1_trig_pin   4
#define hc04_1_echo_pin   5
#define hc04_2_trig_pin   6
#define hc04_2_echo_pin   7

float dist1 = 0.0;
float dist2 = 0.0;

#define enc_left_pin      2
#define enc_right_pin     3

volatile long enc_left_val = 1;
volatile long enc_right_val = 1;

// timer
#define TTH 10
byte t = 0;

byte byte_idx = 0;

void inc_left_encoder() {
  enc_left_val++;
}

void inc_right_encoder() {
  enc_right_val++;
}

void setup() {
  
  // sonars
  pinMode(hc04_1_trig_pin, OUTPUT);
  pinMode(hc04_1_echo_pin, INPUT);
  pinMode(hc04_2_trig_pin, OUTPUT);
  pinMode(hc04_2_echo_pin, INPUT);

  // encoders
  attachInterrupt(digitalPinToInterrupt(enc_left_pin), inc_left_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(enc_right_pin), inc_right_encoder, CHANGE);

  // SPI
  pinMode(MISO, OUTPUT);
  //pinMode(MOSI, INPUT);
  //pinMode(SCK, INPUT);
  //pinMode(SS, INPUT);
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
  // turn on interrupts
  SPI.attachInterrupt();

  // serial - for debug
  Serial.begin(9600);
  Serial.println("Setup done.");
}

byte send_value(byte idx, byte start) {
  if (start == 1)
    byte_idx = 0;
  else
    byte_idx++;
  if (byte_idx >= 4)
    return 255;
  long* ptr;  
  switch (idx) {
    case '0': // left encoder
    ptr = (long*)&enc_left_val;
    break;
    case '1': // right encoder
    ptr = (long*)&enc_right_val;
    break;
    case '2': // sonar 1
    ptr = (long*)&dist1;    
    break;
    case '3': // sonar 2
    ptr = (long*)&dist2;
    break;
    default:
    return 0;
  }
  return (byte)((*ptr) >> (8 * byte_idx)); 
}

// SPI interrupt routine
ISR (SPI_STC_vect) {
  byte c = SPDR;
  if (c == 'r') {
    // continue sending float
    SPDR = send_value(new_cmd, 0);
  }
  else {
    new_cmd = c;
    if (c >= '0' && c <= '9') {
      SPDR = send_value(new_cmd, 1);
    }
    else {
      if (c == 'z') {
        enc_left_val = 0;
        enc_right_val = 0;
        SPDR = 0;
      }
      else
        SPDR = spi_ret;
    }
  }
}

void read_sonar1() {
  digitalWrite(hc04_1_trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(hc04_1_trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(hc04_1_trig_pin, LOW);
  long pulseWidth = pulseIn(hc04_1_echo_pin, HIGH);
  dist1 = 0.75 * dist1 + 0.00425 * pulseWidth;  
}

void read_sonar2() {
  digitalWrite(hc04_2_trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(hc04_2_trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(hc04_2_trig_pin, LOW);
  long pulseWidth = pulseIn(hc04_2_echo_pin, HIGH);
  dist2 = 0.75 * dist2 + 0.00425 * pulseWidth;  
}

void print_sensors() {
  Serial.println(enc_left_val);
  Serial.println(enc_right_val);
  Serial.println(dist1);
  Serial.println(dist1);  
  Serial.println("----");  
}

void loop() {
  read_sonar1();
  read_sonar2();
  
  delay(100);
  t++;
  if (t == TTH) {
    t = 0;
    print_sensors();
    Serial.println(new_cmd);
  }
  
/*
  
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
