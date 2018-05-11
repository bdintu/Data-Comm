/*
 * by Dollapat CE #59
*/

#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_ADS1015.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi 
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define defaultFreq 1700	/* DAC speed (Hz) */
#define freq0 1000			/* frequency of carrier sine wave (Hz)*/

#define p0 50
#define p1 34
#define p2 25
#define p3 20

#define r_slope 102-1		/* amplitude difference for detecting rising or falling signal */

int sum = 0;
int max = 0;
int prev = 0;
int check = false;			/* check for true rising and falling signal (ignore noise) */
int output = -1;
int count = 0;

void setup() {
	Serial.begin(115200);
	sbi(ADCSRA,ADPS2);		/* this for increase analogRead Speed */
	cbi(ADCSRA,ADPS1); 
	cbi(ADCSRA,ADPS0);
}

void loop() {
	int tmp = analogRead(A0);

 	if (tmp-prev > r_slope && !check){	/* check for rising signal */
		max = 0;
		check = true;
	}
	 
	if (tmp > max) {					/* update max value */
		max=tmp;
	}
	
 	if (max-tmp > r_slope) {			/* check for falling signal */
		if (check) {
      
			if(p0-1<=period && period<=p0+1) {
				Serial.print("0 0 "); 
				count++; 
      		} else if (p1-1<=period && period<=p1+1) {
				Serial.print("1 ");
				count++; 
			} else if (p2-1<=period && period<=p2+1) {
				Serial.print("2 "); 
				count++; 
			} else if (p3-1<=period && period<=p3+1) {
				Serial.print("3 "); 
				count++; 
			}

			if (count == 5) {
				count = 0;
				Serial.println();
			}

		    check = false;
		}	
 	}

	prev = tmp;			/* assign temp value to previous */
}
