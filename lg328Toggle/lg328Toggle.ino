
#include <fastio_digital.h>
#include <MsTimer2.h>
void setup() {
  // put your setup code here, to run once:
  //set D10 to output
  fastioMode(D10, OUTPUT);

  // driver D10 to low level
  fastioWrite(D10, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  // toggle D10
  fastioToggle(D10);
}
