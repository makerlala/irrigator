/**
 * Dumi Loghin - 2020
 */
#include <SPI.h>

// SPI
volatile byte new_cmd = 0;  // command got from SPI
volatile byte spi_ret;  // response to send to SPI

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
#define all_current_pin A0
#define jetson_current_pin A1
#define motor_current_pin A2
#define pump_current_pin A3
#define water_level_pin A4

byte cmd = 0;

byte t = 0;

#define TTH 10

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

  // current sensors
  pinMode(motor_current_pin, INPUT);

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

// SPI interrupt routine
ISR (SPI_STC_vect) {
  new_cmd = SPDR;
  SPDR = spi_ret;
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
        default:
          Serial.print("Unknown command: ");
          Serial.println(cmd);
          spi_ret = 1;
      }
    }

  delay(100);
  t++;
  if (t == TTH) {
    t = 0;
    int all_curr = analogRead(all_current_pin);
    int water_level = analogRead(water_level_pin);
    Serial.println(all_curr + " | " + water_level);    
  }

  
  
  /*
  if ((cmd == 'w' || cmd == 'a' || cmd == 'd') && dist <= 20) {
    stop();
  }
  */
}
