/*
 * t25_sat.c
 *
 * Created: 11/28/2020 3:44:44 PM
 * Author : evincent
 *
 *
 * ATMEL ATTINY25
   Pin 1 is /RESET
   
              +-\/-+
   ADC0 PB5  1|    |8  Vcc
   ADC3 PB3  2|    |7  PB2 ADC1 
   ADC2 PB4  3|    |6  PB1 OCR0B
        GND  4|    |5  PB0 OCR0A
              +----+

   PB0 is pushing a PWM to LED1
   PB1 is pushing a PWM to LED2
   PB3 is for sampling voltage from solar panels
   
   8MHz internal RC oscillator, with Divide Clock by 8 internally enabled (1MHz):
   avrdude -F -c usbtiny -p t25 -U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
*/

#define  F_CPU 1000000 // Setting micro to 1MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#define PATTERN_1 0         // Pattern 1's index is 0
#define PATTERN_2 1         // Pattern 2's index is 1
#define PATTERN_3 2         // Pattern 3's index is 2
#define PATTERN_BOTH 3      // Pattern both's index is 3

#define fadeTablesize 17    // index size of the fadeTable array
#define fadeTablesize_2 13  // index size of the fadeTable_2 array
#define prefadeTablesize 6  // index size of the prefadeTable array

int prefadeTable[] = {0,121,184,218,235,245};	// array of PWM values for fading in an LED
int fadeTable[] = {255,217,185,158,134,115,98,83,71,60,51,44,37,32,27,23,20};	// array of PWM values for fading out an LED
int fadeTable_2[] = {255,255,255,134,98,83,71,51,44,37,32,23,20};	// another array of PWM values for fading out an LED
int wdt_delay = 9;	// initialize the watchdog timer to 9, aka 8 second duration
volatile bool f_wdt = 1;	// flag for when watchdog timer is kicked
int tick_counter = 0;	// this determines which blinking pattern to execute

// Prototype Functions
void analogWrite(uint8_t pin, int val);
void setup_watchdog(int ii);
void pattern_1(int pin);
void pattern_2(int pin);
void pattern_3(int pin);
void flashLed (int pattern);

/*
 * main
 *
 * The main micro-controller code
 * 
 */ 
int main(void)
{
	DDRB |= (1 << PORTB0);	// set PB0 to output mode for LED1
	DDRB |= (1 << PORTB1);	// set PB1 to output mode for LED2
	DDRB &= ~(1 << PORTB2);	// set PB2 to so we can pull-up internal resistor
	DDRB &= ~(1 << PORTB3);	// set PB3 to monitor solar cells
	DDRB &= ~(1 << PORTB4);	// set PB4 to so we can pull-up internal resistor
	
	PORTB &= ~(1 << 0);			// Set port PB0 to low to initialize LED as off
	PORTB &= ~(1 << 1);			// Set port PA1 to low to initialize LED as off
	
	PORTB |= (1 << 2);			// Set port PB2 to high to enable internal pull-up
	PORTB |= (1 << 4);			// Set port PB4 to high to enable internal pull-up
	
	// Setup for OC0A and OC0B
	TCCR0A |= (1 << WGM00);	// put timer 0 in 8-bit fast pwm
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << CS00);	// setup to prescaler 64
	TCCR0B |= (1 << CS01);
	
	setup_watchdog(9);          // approximately 8 seconds sleep

    while (1) {
	    // if watchdog flag has been set, it means the watchdog timeout has occurred
	    if (f_wdt == 1) {
		    // so, now that the timeout has occurred, we need to reset the flag
		    f_wdt = 0;

		    // if the solar cells are reading 0 volts, meaning, little-to-no light...
		    if(PINB & (1 << 3)) {
			    // otherwise, we have sunlight, so we don't need to blink
			    // so, in the name of saving power, increase the delay time back to 8 seconds
			    // so we don't wake up as often and waste juice.
			    wdt_delay=9;
			    } else {
			    // with no light, this is when we should be showing off the LEDs, so...
			    
			    // set the watchdog timer delay to only 4 seconds
			    wdt_delay=8;

			    // now that we are delaying only every 4 seconds, lets start doing our LED show
			    // whatever LED show we are on (set by the tick counter), let's do it
			    flashLed (tick_counter);

			    // increment the tick counter so we don't do 2 of the same LED show in a row
			    tick_counter++;

			    // if we've gotten to the end of our LED shows...
			    if(tick_counter > 3) {
				    // start back over again
				    tick_counter=0;
			    }
		    }

		    // and after all of that, go to sleep
		    setup_watchdog(wdt_delay);            // setup watchdog timer to delay amount
		    ADCSRA &= ~(1 << ADEN);               // switch Analog to Digital Converter OFF
		    set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // sleep mode is set here
		    sleep_enable();                       // sleep mode is enabled
		    sleep_mode();                         // system sleeps here
		    sleep_disable();                      // system continues execution here when watchdog timed out
		    ADCSRA |= (1 << ADEN);                // switch Analog to Digital Converter ON
	    }
    }
    
    return 1;
}

/*
 * setup_watchdog
 *
 * Function that sets up the watch dog timer and sets up its timing interval
 * 
 */ 
void setup_watchdog(int ii) {
	// integer values of 4-bit WDP[3:0]
	// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
	// 6=1 sec,7=2 sec, 8=4 sec, 9=8 sec
	uint8_t bb;
	
	// Disable all interrupts
	cli();

	// if watchdog prescale select is greater than 9, which doesn't exist...
	if (ii > 9 ) {
		// then hard set to WDP[3:0] of 9, or 8 second time-out
		ii = 9;
	}

	bb = ii & 7;

	// if watchdog interval is greater than 7, so greater than 1 second...
	if (ii > 7) {
		// then shift bit 5 to a 1 to bb.
		bb |= (1 << 5);
	}

	bb |= (1 << WDCE);

	// Set WatchDog Reset Flag bit to 0 on MCU Status Register
	MCUSR &= ~(1 << WDRF);
	
	// In the Watchdog Timer Control Register, start timed sequence
	WDTCR |= (1 << WDCE) | (1 << WDE);
	
	// In the Watchdog Timer Control Register, start timed sequence
	// WDTCR on tinyx5, WDTCSR on tinyx4
	// WDIE on tinyx4 and tinyx5, but on tiny13, WDTIE
	WDTCR = bb;
	WDTCR |= _BV(WDIE);
	
	// Enable all interrupts.
	sei();
}

// Interrupt Service Routine for the Watchdog Interrupt Service. Do this, when watchdog has timed out
// WDT_vect on tinyx5, WATCHDOG_vect on tinyx4
ISR(WDT_vect) {
	f_wdt = 1;  // set global flag
}

/*
 * analogWrite
 *
 * function to do an analog write to PWM pin
 * 
 */ 
void analogWrite(uint8_t pin, int val) {
	
	if(pin == 0) {
		DDRB |= (1 << 0); // Set PB0 to output/high
		
		if (val == 0) {
			PORTB &= ~(1 << 0);			// Set port PB0 to low
			DDRB &= ~(1 << PORTB0);		// Set PB0 to input/low
		} else {
			TCCR0A |= (1 << COM0A1);	// enable COM0A1 on TCCR0A
			OCR0A = val;				// set pwm duty
		}
	} else if (pin == 1) {
		DDRB |= (1 << 1); // Set PB1 to output/high
		
		if (val == 0) {
			PORTB &= ~(1 << 1);			// Set port PB1 to low
			DDRB &= ~(1 << PORTB1);		// Set PB1 to input/low
		} else {
			TCCR0A |= (1 << COM0B1);	// enable COM0B1 on TCCR0A
			OCR0B = val;				// set pwm duty
		}
	} 
}


/*
 * flashLed
 *
 * Function to flash the LEDs based off a pre-defined pattern set
 * 
 */ 
void flashLed (int pattern) {
	// we have 3 individual patterns and 1 we use on both LEDs
	switch (pattern) {
		case PATTERN_1:
		// set the pin that LED1 is out to output before we go to sleep
		DDRB |= (1 << PORTB0);	// set PB0 to output mode for LED1

		// do pattern 1 on LED1
		pattern_1(0);

		// once completed, turn off LED1
		analogWrite(0, 0);
		break;

		case PATTERN_2:
		// set the pin that LED2 is out to output before we go to sleep
		DDRB |= (1 << PORTB1);	// set PB1 to output mode for LED2

		// do pattern 2 on LED2
		pattern_2(1);

		// once completed, turn off LED2
		analogWrite(1, 0);
		break;

		case PATTERN_3:
		// set the pin that LED1 is out to output before we go to sleep
		DDRB |= (1 << PORTB0);	// set PB0 to output mode for LED1

		// do pattern 3 on LED1
		pattern_3(0);

		// once completed, turn off LED1
		analogWrite(0, 0);
		break;
		
		case PATTERN_BOTH:
		// set the pin that LED2 is out to output before we go to sleep
		DDRB |= (1 << PORTB1);	// set PB1 to output mode for LED2

		// do pattern 1 on LED2
		pattern_1(1);

		// once completed, turn off LED2
		analogWrite(1, 0);

		// set the pin that LED1 is out to output before we go to sleep
		DDRB |= (1 << PORTB0);	// set PB0 to output mode for LED1

		// do pattern 1 on LED1
		pattern_1(0);

		// once completed, turn off LED1
		analogWrite(0, 0);
		break;
	}
	
	DDRB &= ~(1 << PORTB0);	// set all used port to input to save power
	DDRB &= ~(1 << PORTB1);	// set all used port to input to save power
}

/*
 * pattern_1
 *
 * Function for flash pattern 1
 * 
 */ 
void pattern_1(int pin) {
	// loop through the fade in array
	for(int i = 0; i < prefadeTablesize; i++) {
		
		// set the PWM pin to the PWM value from the fade-in table
		analogWrite(pin, prefadeTable[i]);
		
		// wait 30 milliseconds
		_delay_ms(30);
	}
	
	// after we've faded in the LED, turn it to full on
	analogWrite(pin,255);
	
	// wait 100 milliseconds
	_delay_ms(100);
	
	// loop through the fade out array
	for(int i = 0; i < fadeTablesize; i++) {
		
		// set the PWM pin to the PWM value from the fade-out table
		analogWrite(pin, fadeTable[i]);
		
		// wait 30 milliseconds
		_delay_ms(30);
	}
}

/*
 * pattern_2
 *
 * Function for flash pattern 2
 * 
 */ 
void pattern_2(int pin) {
	
	// create a loop for this blinking pattern to make it repeat twice in a row
	for(int j = 0; j < 2; j++) {
		
		// loop through the fade in array
		for(int i = 0; i < prefadeTablesize; i++) {
			
			// set the PWM pin to the PWM value from the fade-in table
			analogWrite(pin, prefadeTable[i]);
			
			// wait 60 milliseconds
			_delay_ms(60);
		}
		
		// loop through the fade out array
		for(int i = 0; i < fadeTablesize; i++) {
			
			// set the PWM pin to the PWM value from the fade-out table
			analogWrite(pin, fadeTable[i]);
			
			// wait 10 milliseconds
			_delay_ms(10);
		}
	}
}

/*
 * pattern_3
 *
 * Function for flash pattern 3
 * 
 */ 
void pattern_3(int pin) {
	// loop through the fade in array
	for(int i = 0; i < prefadeTablesize; i++) {
		
		// set the PWM pin to the PWM value from the fade-in table
		analogWrite(pin, prefadeTable[i]);
		
		// wait 30 milliseconds
		_delay_ms(30);
	}
	
	// loop through the fade out array
	for(int i = 0; i < fadeTablesize_2; i++) {
		
		// set the PWM pin to the PWM value from the fade-out table
		analogWrite(pin, fadeTable_2[i]);
		
		// wait 35 milliseconds
		_delay_ms(35);
	}
	
	// loop through the fade in array
	for(int i = 0; i < prefadeTablesize; i++) {
		
		// set the PWM pin to the PWM value from the fade-in table
		analogWrite(pin, prefadeTable[i]/2);
		
		// wait 20 milliseconds
		_delay_ms(20);
	}
	
	// loop through the fade out array
	for(int i = 0; i < fadeTablesize_2; i++) {
		
		// set the PWM pin to the PWM value from the fade-out table
		analogWrite(pin, fadeTable_2[i]/2);
		
		// wait 25 milliseconds
		_delay_ms(25);
	}
}