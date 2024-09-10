#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1 // Button S1 at P1.1
#define BUT2 BIT2 // Button S2 at P1.2
void main(void) {
WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

// Configure and initialize LEDs

P1DIR |= redLED; // Direct pin as output
P9DIR |= greenLED; // Direct pin as output
P1OUT &=  ~redLED; // Turn LED Off
P9OUT &= ~greenLED;// Turn LED Off
// Configure buttons
// S1
P1DIR &= ~BUT1; // Direct pin as input
P1REN |= BUT1; // Enable built-in resistor
P1OUT |= BUT1;  // Set resistor as pull-up
// S2
P1DIR &= ~BUT2; // Direct pin as input
P1REN |= BUT2; // Enable built-in resistor
P1OUT |= BUT2;  // Set resistor as pull-up

// Polling the button in an infinite loop

for(;;) {
//2.2

/*
if ( (P1IN & BUT1) == 0 )   // IF S1 Button Pressed
        P1OUT |= redLED;    // Turn redLED on
else
    P1OUT &= ~redLED;   // Turn redLED off

if ( (P1IN & BUT2) == 0 )   // IF S2 Button Pressed
       P9OUT |= greenLED;    // Turn greenLED on
else
    P9OUT &= ~greenLED;   // Turn greenLED off
*/

// 2.3
/*
if((P1IN & BUT1)==0){   // S1 Button Pressed Red on Green off
    P1OUT |= redLED;
    P9OUT &= ~greenLED;
}
else if ((P1IN & BUT2)==0){ // S2 Button Pressed Green on Red off
    P9OUT |= greenLED;
    P1OUT &= ~redLED;
}
else{                       // No Button pressed both lights off
    P1OUT &= ~redLED;
    P9OUT &= ~greenLED;
}
*/
    //2.4
    if (((P1IN & BUT1) == 0) && ((P1IN & BUT2) == 0)){
        P1OUT &= ~redLED;
        P9OUT &= ~greenLED;
        while(((P1IN & BUT1) == 0) || ((P1IN & BUT2) == 0)){
            //Do nothing until both released
        }
    }
    else if((P1IN & BUT1)==0){   // S1 Button Pressed Red on Green off
        P1OUT |= redLED;
        P9OUT &= ~greenLED;
    }
    else if ((P1IN & BUT2)==0){ // S2 Button Pressed Green on Red off
        P9OUT |= greenLED;
        P1OUT &= ~redLED;
    }
    else{                       // No Button pressed both lights off
        P1OUT &= ~redLED;
        P9OUT &= ~greenLED;
    }
}
}



