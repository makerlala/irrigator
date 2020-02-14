/**
   Dumi Loghin - 2020
   Irrigator - Arduino Board 1
*/
#include <SPI.h>

// SPI
volatile byte new_cmd = 0;  // command got from SPI
volatile byte spi_ret;      // response to send to SPI

byte cmd = 0;

// Motors
#define mrin1_pin   2
#define mrin2_pin   4
#define mren_pin    3 // PWM

#define mlin3_pin   5
#define mlin4_pin   7
#define mlen_pin    6 // PWM

// Light and Pump
#define light_pin   8
#define pump_pin    9

// current sensors
#define current_all_pin A0
#define current_jetson_pin A1
#define current_motors_pin A2
#define current_pump_pin A3
#define water_level_pin A4

float current_all = 0.0;
float current_motors = 0.0;
float current_jetson = 0.0;
float current_pump = 0.0;
float water_level = 0.0;

byte byte_idx = 0;

// timer
#define TTH 10
byte t = 0;


void setup() {

  // motors
  pinMode(mrin1_pin, OUTPUT);
  pinMode(mrin2_pin, OUTPUT);
  pinMode(mren_pin, OUTPUT);
  pinMode(mlin3_pin, OUTPUT);
  pinMode(mlin4_pin, OUTPUT);
  pinMode(mlen_pin, OUTPUT);

  // pump
  pinMode(pump_pin, OUTPUT);
  digitalWrite(pump_pin, HIGH);

  // light
  pinMode(light_pin, OUTPUT);
  digitalWrite(light_pin, HIGH);

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
  if (start)
    byte_idx = 0;
  else
    byte_idx++;
  if (byte_idx >= 4)
    return 255;
  long* ptr;
  switch (idx) {
    case '0': 
      ptr = (long*)&current_all;
      break;
    case '1': 
      ptr = (long*)&current_motors;
      break;
    case '2': 
      ptr = (long*)&current_jetson;
      break;
    case '3': 
      ptr = (long*)&current_pump;
      break;
    case '4': 
      ptr = (long*)&water_level;
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
    // continue sending value
    SPDR = send_value(new_cmd, 0);
  }
  else {
    new_cmd = c;
    if (c >= '0' && c <= '9') {
      SPDR = send_value(new_cmd, 1);
    }
    else
      SPDR = spi_ret;
  }  
}

void stop() {
  digitalWrite(mrin1_pin, LOW);
  digitalWrite(mrin2_pin, LOW);
  digitalWrite(mlin3_pin, LOW);
  digitalWrite(mlin4_pin, LOW);
}

void go_fwd() {
  stop();
  digitalWrite(mrin1_pin, HIGH);
  digitalWrite(mrin2_pin, LOW);
  digitalWrite(mlin3_pin, HIGH);
  digitalWrite(mlin4_pin, LOW);
}

void go_back() {
  stop();
  digitalWrite(mrin1_pin, LOW);
  digitalWrite(mrin2_pin, HIGH);
  digitalWrite(mlin3_pin, LOW);
  digitalWrite(mlin4_pin, HIGH);
}

void go_left() {
  stop();
  digitalWrite(mrin1_pin, HIGH);
  digitalWrite(mrin2_pin, LOW);
  // digitalWrite(mlin3_pin, HIGH);
  // digitalWrite(mlin4_pin, LOW);
}

void go_right() {
  stop();
  // digitalWrite(mrin1_pin, HIGH);
  // digitalWrite(mrin2_pin, LOW);
  digitalWrite(mlin3_pin, HIGH);
  digitalWrite(mlin4_pin, LOW);
}

void on_light() {
  digitalWrite(light_pin, LOW);
}

void off_light() {
  digitalWrite(light_pin, HIGH);
}

void on_pump() {
  digitalWrite(pump_pin, LOW);
}

void off_pump() {
  digitalWrite(pump_pin, HIGH);
}

void read_sensors() {
  word current = analogRead(current_all_pin);
  current_all = 0.75 * current_all + 0.25 * current / 1024;

  current = analogRead(current_jetson_pin);
  current_jetson = 0.75 * current_jetson + 0.25 * current / 1024;

  current = analogRead(current_motors_pin);
  current_motors = 0.75 * current_motors + 0.25 * current / 1024;

  current = analogRead(current_pump_pin);
  current_pump = 0.75 * current_pump + 0.25 * current / 1024;

  current = analogRead(water_level_pin);
  water_level = 0.75 * water_level + 0.25 * current / 1024;
}

void print_sensors() {
  Serial.println(current_all);
  Serial.println(current_jetson);
  Serial.println(current_motors);
  Serial.println(current_pump);
  Serial.println(water_level);
  Serial.println("----");
}

void loop() {

  // if using serial to get the command -> uncomment
  /*
    if (Serial.available() > 0) {
    new_cmd = Serial.read();
    Serial.println(cmd);
    }
  */

  // SPI command
  if (new_cmd != cmd) {
    cmd = new_cmd;
    spi_ret = 0;
    switch (cmd) {
      case 'w':
        /*
          if (dist > 20) {
          go_fwd();
          }
        */
        go_fwd();
        break;
      case 'a':
        go_left();
        break;
      case 'd':
        go_right();
        break;
      case 's':
        go_back();
        break;
      case 'q':
        stop();
        break;
      case 'l':
        on_light();
        break;
      case 'k':
        off_light();
        break;
      case 'm':
        on_pump();
        break;
      case 'n':
        off_pump();
        break;
      case 'r':
        break;
      default:
        Serial.print("Unknown command: ");
        Serial.println(cmd);
        spi_ret = 1;
    }
  }

  read_sensors();

  delay(100);
  t++;
  if (t == TTH) {
    t = 0;
    print_sensors();
  }



  /*
    if ((cmd == 'w' || cmd == 'a' || cmd == 'd') && dist <= 20) {
    stop();
    }
  */
}
