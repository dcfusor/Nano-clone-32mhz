/*
 This is meant to become a set of demo/examples
 for this LG chip on the WeMos board

*/

#include <fastio_digital.h>
#include <MsTimer2.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
/////////////////////////////////// defines
/* @@@ DOES NOT WORK with fastio
#define TOGGLE_PIN D10
too many layers of define?
*/
#define ONE_SEC 1000
#define TENTH_SEC 100
// a/d sample time to get in 4 per 100ms or so
#define SAMP_TIME 11
// above so wwe can add 8 samples/shannel - takes around half cpu, loop runs at 146khz*2


////////////////////////////////// variables

/* "opsys" seheduling stuff */
unsigned long loop_millis; // so only needs to call once per loop
unsigned long sec_z;
unsigned long tenthsec_z;
unsigned long samptime_z;
bool did_something; // set false at loop begin, true if something was done (time control)

// "app" stuff
int analog_0; // test a/d converter
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


// testing analog read and speed
byte ch;
byte numsamples;
unsigned int channels[4];

volatile unsigned long c1counts;    // total counts from (hardware) counter 1, d5 pin
// top half incremented by timer overflow
volatile unsigned long c1z; // previous count for subtraction
volatile unsigned long curcount; // what we'll report, this - last total

unsigned long benchs; // for benchmarking
unsigned long benche;
unsigned long bencht; // total




unsigned long seconds;
////////////////////////////////////////////////
void t1set() // setup timer1 as a counter that interrupts on overflow
{
  TCCR1A = 0; // turn off all the weird compare/capture modes etc
  TCCR1B = 6; // clock on falling edge (7 for rising)
  TIFR1 = _BV(TOV1); // clear initial overflow - shouldn't need to, but do.
  TCNT1 = 0; //zero out timer
  TIMSK1 = _BV(TOIE1);// sets the timer overflow interrupt enable bit
  }
////////////////////////////////////////////////
void readCounts()
{ // just fast hardware counter (d5 pin), interrupts us only every 65k counts
/* Disable interrupts */
noInterrupts();  // probably meaningless, counts still come into the pin
/* Read TCNT1 into c1counts low part */
c1counts &= 0xffff0000;
c1counts |= TCNT1; // from the hardware register @@@ there's a better capture register
curcount = c1counts - c1z; // just the new ones for this time interval
c1z = c1counts; // for next time
/* Restore global interrupt flag */
interrupts(); 
}
////////////////////////////////////////////////
//////// manual isr setup, you bet, saves a whole library
// actual isr for hardware counter 1
ISR(TIMER1_OVF_vect)
{
 c1counts += 0x00010000; // add one to top half of longword
}
//////////////////////////////////
void init_ram ()
{
  // I group types together for faster init, like a=b=c=0
  did_something = false;
  c1counts = seconds = 0;
   for (ch=0;ch < 4;ch++)
  {
    channels[ch] = 0;

  }
  numsamples = 0;
  loop_millis = millis();
}

//////////////////////////////////
void setup() {
  // run-once code:
  fastioMode(D10, OUTPUT); // this is a new mode for this board
  fastioWrite(D10, LOW); // and it is indeed very fast
 Serial.begin(115200);
 Serial.println ("In setup...");
 init_ram(); // gets its own routine for clarity

//  fastioMode(D12,OUTPUT); // ok, this is our led
 // MsTimer2::set(200, flash); // 500ms period
//  MsTimer2::start(); 
u8g2.begin();
 
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  u8g2.drawStr(0,10,"Hello World!10");	// write something to the internal memory
  u8g2.drawStr(0,20,"Hello World! 20");	// write something to the internal memory
  
  u8g2.drawStr(0,30,"Hello World!34567890");	// write something to the internal memory
  u8g2.drawStr(0,40,"Hello World! 40");	// write something to the
  
  u8g2.drawStr(0,50,"Hello World! 50");	// write something to the internal memory
  u8g2.drawStr(0,62,"Hello World! 63");	// write something to the
  u8g2.sendBuffer();					// transfer internal memory to the display
 t1set();
}
//////////////////////////////////
void sample_ad()
{
  int ch;
  // get one sample from all 4 channels 0-3 and add to running sum
  for (ch=0;ch < 4;ch++) channels[ch] += analogRead(ch);
  numsamples++;
  did_something = true; // let rest of scheduler know we ate some time
}
//////////////////////////////////
void report_data()
{
  //send our our stuff on serial port

  did_something = true; // serial takes time
  Serial.print (loop_millis); // time of observations
  Serial.print ("\t");
  for (ch = 0; ch <4; ch++)
  {
   Serial.print (channels[ch]);
   channels[ch] = 0; // clear them for next time
   Serial.print ("\t");
  }
  numsamples = 0; // ready to start a/d again
  readCounts();
  Serial.print (curcount);
  
  Serial.println();
}
//////////////////////////////////
void flash()
{
//  static bool output;
  fastioToggle(D12);
}
//////////////////////////////////

/* @@@ bare fast toggle runs 3.95 mhz on scope (real times are twice that fast)
 *  but with Serial.gegin runs slower - 1.44 mhz,
 *  it must check every roundy of loop()
 *  Adding 1 and 1/th sec millis check slows to 247 khz loop
 *
 
 */
 
void loop() {
 fastioToggle(D10); // how fast is this loop?
 loop_millis = millis(); // what time is it?
 did_something = false; // because we haven't yet on this roundy

 if (loop_millis - samptime_z >= SAMP_TIME) // SAMP_TIME has to let us get in 4 every time
 { // this one's high priority, so we just do it, but do set 
   //  "did_something" to wave off other cpu use this go around
   // 8 samples is right around 1/2 of the cpu time
  samptime_z = loop_millis;
  if (numsamples < 8) sample_ad(); // took 678 us/call in other tests
 }

 if (loop_millis - tenthsec_z >= TENTH_SEC)
  {
   tenthsec_z = loop_millis;  // restart timing
   // schedule 10 per second stuff here
   report_data(); // not optional, this is our timebase for data acq
  }
 
 if (loop_millis - sec_z >= ONE_SEC)
  {
   sec_z = loop_millis;  // restart timing
   // schedule once per second stuff here
   seconds++;
 //  flash();
///////////////////////////////////
  }

} // end of loop()
 
