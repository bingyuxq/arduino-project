 /***************************************************
 * IRremote for ESP8266
 *
 * Based on the IRremote library for Arduino by Ken Shirriff 
 * Version 0.11 August, 2009
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 *
 * Modified by Paul Stoffregen <paul@pjrc.com> to support other boards and timers
 * Modified  by Mitra Ardron <mitra@mitra.biz> 
 * Added Sanyo and Mitsubishi controllers
 * Modified Sony to spot the repeat codes that some Sony's send
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
 *  GPL license, all text above must be included in any redistribution
 ****************************************************/

#include "IRremoteESP8266.h"
#include "IRremoteInt.h"

// These versions of MATCH, MATCH_MARK, and MATCH_SPACE are only for debugging.
// To use them, set DEBUG in IRremoteInt.h
// Normally macros are used for efficiency

int MATCH(int measured, int desired) {return measured >= TICKS_LOW(desired) && measured <= TICKS_HIGH(desired);}
int MATCH_MARK(int measured_ticks, int desired_us) {return MATCH(measured_ticks, (desired_us + MARK_EXCESS));}
int MATCH_SPACE(int measured_ticks, int desired_us) {return MATCH(measured_ticks, (desired_us - MARK_EXCESS));}

// IRsend -----------------------------------------------------------------------------------

IRsend::IRsend(int IRsendPin)
{
	IRpin = IRsendPin;
}

void IRsend::begin()
{
	pinMode(IRpin, OUTPUT);
}

void IRsend::sendRaw(unsigned int buf[], int len, int hz)
{
  enableIROut(hz);
  for (int i = 0; i < len; i++) {
    if (i & 1) {
      space(buf[i]);
    } 
    else {
      mark(buf[i]);
    }
  }
  space(0); // Just to be sure
}

void IRsend::mark(int time) {
  // Sends an IR mark for the specified number of microseconds.
  // The mark output is modulated at the PWM frequency.
  long beginning = micros();
  while(micros() - beginning < time){
    digitalWrite(IRpin, HIGH);
    delayMicroseconds(halfPeriodicTime);
    digitalWrite(IRpin, LOW);
    delayMicroseconds(halfPeriodicTime); //38 kHz -> T = 26.31 microsec (periodic time), half of it is 13
  }
}

/* Leave pin off for time (given in microseconds) */
void IRsend::space(int time) {
  // Sends an IR space for the specified number of microseconds.
  // A space is no output, so the PWM output is disabled.
  digitalWrite(IRpin, LOW);
  if (time > 0) delayMicroseconds(time);
}

void IRsend::enableIROut(int khz) {
  // Enables IR output.  The khz value controls the modulation frequency in kilohertz.
  halfPeriodicTime = 500/khz; // T = 1/f but we need T/2 in microsecond and f is in kHz
}

// ---------------------------------------------------------------
  

//IRRecv------------------------------------------------------

extern "C" {
	#include "user_interface.h"
	#include "gpio.h"
}

static ETSTimer timer;
volatile irparams_t irparams;

static void ICACHE_FLASH_ATTR read_timeout(void *arg) {
    os_intr_lock();
    if (irparams.rawlen) {
		irparams.rcvstate = STATE_STOP;
    }
	os_intr_unlock();
}

static void ICACHE_FLASH_ATTR gpio_intr(void *arg) {
    uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

	if (irparams.rcvstate == STATE_STOP) {
		return;
	}	
	if (irparams.rawlen >= RAWBUF) {
        irparams.rcvstate = STATE_STOP;  // Buffer overflow
        #ifdef DEBUG
        Serial.println("RawData overflow");
        #endif
    }
    static uint32_t start = 0;
    uint32_t now = system_get_time();
	if (irparams.rcvstate == STATE_IDLE) {
		irparams.rcvstate = STATE_MARK;	
	}
    else if (irparams.rawlen < RAWBUF) {
		irparams.rawbuf[irparams.rawlen++] = (now - start) / USECPERTICK + 1;
    }
    start = now;

    os_timer_disarm(&timer);
    os_timer_arm(&timer, _GAP, 0);
}

IRrecv::IRrecv(int recvpin) {
  irparams.recvpin = recvpin;
}

// initialization
void IRrecv::enableIRIn() {
	
  // initialize state machine variables
  irparams.rcvstate = STATE_IDLE;
  irparams.rawlen = 0;

  // set pin modes  
  //PIN_FUNC_SELECT(IR_IN_MUX, IR_IN_FUNC);
  GPIO_DIS_OUTPUT(irparams.recvpin);
  
  // Initialize timer
  os_timer_disarm(&timer);
  os_timer_setfn(&timer, (os_timer_func_t *)read_timeout, &timer);
  
  // ESP Attach Interrupt
  ETS_GPIO_INTR_DISABLE();
  ETS_GPIO_INTR_ATTACH(gpio_intr, NULL);
  gpio_pin_intr_state_set(GPIO_ID_PIN(irparams.recvpin), GPIO_PIN_INTR_ANYEDGE);
  ETS_GPIO_INTR_ENABLE();
  //ETS_INTR_UNLOCK();  
  #ifdef DEBUG
  Serial.println("enableIRIn end");
  #endif
  //attachInterrupt(irparams.recvpin, readIR, CHANGE);  
  //irReadTimer.initializeUs(USECPERTICK, readIR).start();
  //os_timer_arm_us(&irReadTimer, USECPERTICK, 1);
  //ets_timer_arm_new(&irReadTimer, USECPERTICK, 1, 0);
}

void IRrecv::disableIRIn() {
  //irReadTimer.stop();
  //os_timer_disarm(&irReadTimer);   
   ETS_INTR_LOCK();
   ETS_GPIO_INTR_DISABLE();
}

void IRrecv::resume() {
  irparams.rcvstate = STATE_IDLE;
  irparams.rawlen = 0;
}

// Decodes the received IR message
// Returns 0 if no data ready, 1 if data ready.
// Results of decoding are stored in results
int IRrecv::decode(decode_results *results) {
  results->rawbuf = irparams.rawbuf;
  results->rawlen = irparams.rawlen;
  if (irparams.rcvstate != STATE_STOP) {
    return ERR;
  }
  // decodeHash returns a hash on any input.
  // Thus, it needs to be last in the list.
  // If you add any decodes, add them before this.
  if (decodeHash(results)) {
    return DECODED;
  }
  // Throw away and start over
  resume();
  return ERR;
}

// Gets one undecoded level at a time from the raw buffer.
// The RC5/6 decoding is easier if the data is broken into time intervals.
// E.g. if the buffer has MARK for 2 time intervals and SPACE for 1,
// successive calls to getRClevel will return MARK, MARK, SPACE.
// offset and used are updated to keep track of the current position.
// t1 is the time interval for a single bit in microseconds.
// Returns -1 for error (measured time interval is not a multiple of t1).
int IRrecv::getRClevel(decode_results *results, int *offset, int *used, int t1) {
  if (*offset >= results->rawlen) {
    // After end of recorded buffer, assume SPACE.
    return SPACE;
  }
  int width = results->rawbuf[*offset];
  int val = ((*offset) % 2) ? MARK : SPACE;
  int correction = (val == MARK) ? MARK_EXCESS : - MARK_EXCESS;

  int avail;
  if (MATCH(width, t1 + correction)) {
    avail = 1;
  } 
  else if (MATCH(width, 2*t1 + correction)) {
    avail = 2;
  } 
  else if (MATCH(width, 3*t1 + correction)) {
    avail = 3;
  } 
  else {
    return -1;
  }

  (*used)++;
  if (*used >= avail) {
    *used = 0;
    (*offset)++;
  }
#ifdef DEBUG
  if (val == MARK) {
    Serial.println("MARK");
  } 
  else {
    Serial.println("SPACE");
  }
#endif
  return val;   
}


/* -----------------------------------------------------------------------
 * hashdecode - decode an arbitrary IR code.
 * Instead of decoding using a standard encoding scheme
 * (e.g. Sony, NEC, RC5), the code is hashed to a 32-bit value.
 *
 * The algorithm: look at the sequence of MARK signals, and see if each one
 * is shorter (0), the same length (1), or longer (2) than the previous.
 * Do the same with the SPACE signals.  Hszh the resulting sequence of 0's,
 * 1's, and 2's to a 32-bit value.  This will give a unique value for each
 * different code (probably), for most code systems.
 *
 * http://arcfn.com/2010/01/using-arbitrary-remotes-with-arduino.html
 */

// Compare two tick values, returning 0 if newval is shorter,
// 1 if newval is equal, and 2 if newval is longer
// Use a tolerance of 20%
int IRrecv::compare(unsigned int oldval, unsigned int newval) {
  if (newval < oldval * .8) {
    return 0;
  } 
  else if (oldval < newval * .8) {
    return 2;
  } 
  else {
    return 1;
  }
}

// Use FNV hash algorithm: http://isthe.com/chongo/tech/comp/fnv/#FNV-param
#define FNV_PRIME_32 16777619
#define FNV_BASIS_32 2166136261

/* Converts the raw code values into a 32-bit hash code.
 * Hopefully this code is unique for each button.
 * This isn't a "real" decoding, just an arbitrary value.
 */
long IRrecv::decodeHash(decode_results *results) {
  // Require at least 6 samples to prevent triggering on noise
  if (results->rawlen < 6) {
    return ERR;
  }
  long hash = FNV_BASIS_32;
  for (int i = 1; i+2 < results->rawlen; i++) {
    int value =  compare(results->rawbuf[i], results->rawbuf[i+2]);
    // Add value into the hash
    hash = (hash * FNV_PRIME_32) ^ value;
  }
  results->value = hash;
  results->bits = 32;
  return DECODED;
}

// ---------------------------------------------------------------
