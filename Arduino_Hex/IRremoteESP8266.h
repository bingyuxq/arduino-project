 /***************************************************
 * IRremote for ESP8266
 * 
 * Based on the IRremote library for Arduino by Ken Shirriff 
 * Version 0.11 August, 2009
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 *
 * Edited by Mitra to add new controller SANYO
 *
 * Interrupt code based on NECIRrcv by Joe Knapp
 * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
 * Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
 *
 * JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
 * LG added by Darryl Smith (based on the JVC protocol)
 * Whynter A/C ARC-110WD added by Francesco Meschia
 *
 * Updated by markszabo (https://github.com/markszabo/IRremoteESP8266) for sending IR code on ESP8266
 * Updated by Sebastien Warin (http://sebastien.warin.fr) for receiving IR code on ESP8266
 *
 * GPL license, all text above must be included in any redistribution
 ****************************************************/

#ifndef IRremote_h
#define IRremote_h

// The following are compile-time library options.
// If you change them, recompile the library.
// If DEBUG is defined, a lot of debugging output will be printed during decoding.
// TEST must be defined for the IRtest unittests to work.  It will make some
// methods virtual, which will be slightly slower, which is why it is optional.
#define DEBUG
//#define TEST


// Results returned from the decoder
class decode_results {
public:
  unsigned long value; // Decoded value
  int bits; // Number of bits in decoded value
  volatile unsigned int *rawbuf; // Raw intervals in .5 us ticks
  int rawlen; // Number of records in rawbuf.
};

// main class for receiving IR
class IRrecv
{
public:
  IRrecv(int recvpin);
  int decode(decode_results *results);
  void enableIRIn();
  void disableIRIn();
  void resume();
  private:
  // These are called by decode
  int getRClevel(decode_results *results, int *offset, int *used, int t1);

  long decodeHash(decode_results *results);
  int compare(unsigned int oldval, unsigned int newval);
};

// Only used for testing; can remove virtual for shorter code
#ifdef TEST
#define VIRTUAL virtual
#else
#define VIRTUAL
#endif
class IRsend
{
public:
  IRsend(int IRsendPin);
  void begin();
  void sendRaw(unsigned int buf[], int len, int hz);
  void enableIROut(int khz);
  VIRTUAL void mark(int usec);
  VIRTUAL void space(int usec);
private:
  int halfPeriodicTime;
  int IRpin;
} ;

// Some useful constants
#define USECPERTICK 50  // microseconds per clock interrupt tick
#define RAWBUF 500 // Length of raw duration buffer

// Marks tend to be 100us too long, and spaces 100us too short
// when received due to sensor lag.
#define MARK_EXCESS 100

#endif
