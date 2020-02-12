#include <SPI.h>

volatile byte c;

void setup() {
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  //pinMode(MOSI, INPUT);
  //pinMode(SCK, INPUT);
  //pinMode(SS, INPUT);

  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
      
  // turn on interrupts
  SPI.attachInterrupt();

  Serial.begin(9600);

  Serial.println("Init done");
}

// SPI interrupt routine
ISR (SPI_STC_vect) {
  c = SPDR;
  SPDR = c+1;
}  
// end of interrupt service routine (ISR) for SPI

void loop () {
  if (c != 0 && c != 255)
    Serial.println(c);
}
