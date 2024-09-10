#include <msp430.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1   // S1 is button 1
#define BUT2 BIT2   // S2 is button 2


void config_ACLK_to_32KHz_crystal() {
// By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
// Reroute pins to LFXIN/LFXOUT functionality
PJSEL1 &=  ~BIT4;
PJSEL0 |= BIT4;
// Wait until the oscillator fault flags remain cleared
CSCTL0 = CSKEY; // Unlock CS registers
do {
CSCTL5 &=  ~LFXTOFFG; // Local fault flag
SFRIFG1 &=  ~OFIFG; // Global fault flag
} while((CSCTL5 & LFXTOFFG) != 0);
CSCTL0_H = 0; // Lock CS registers
return;
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    // Configure and initialize LEDs
    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &=  ~redLED; // Turn LED Off
    P9OUT &= ~greenLED;// Turn LED Off

    // Configure and initialize the Buttons
    //S1
    P1DIR &= ~BUT1; // direct output pin
    P1REN |= BUT1;  // enable built in resistor
    P1OUT |= BUT1;  // resistor set to pull-up

    //S2
    P1DIR &= ~BUT2; // direct output
    P1REN |= BUT2;  // enable resistor
    P1OUT |= BUT2;  // resistor pullup

    unsigned int count = 1; // Variable for time button pressed
    unsigned int over;  // Variable if button pressed time goes over

    //Ensure Flag is cleared
    TA0CTL &= ~TAIFG;

// Configure ACLK to 32 KHz crystal
    config_ACLK_to_32KHz_crystal();

for(;;){

    if((P1IN & BUT1)==0){   //  If S1 is pressed, start timer
        TA0CTL = TASSEL_1 | ID_0 | MC_2 | TACLR;
        while ((P1IN & BUT1)==0){
            count = TA0R;
            if((TA0CTL & TAIFG) == TAIFG){         // Flag triggered Pressed button for too long
                    P9OUT |= greenLED;        // turn on green LED
                    TA0CTL = 0; // stop clock completely, set everything to zero
                    count = 0;  // set count to 0
                    over = 1;   // When button released will go to while loop
                }
        }
    }

    while (over == 1){  // Do nothing and leave LED on until button is pressed

        if ((P1IN & BUT2) == 0){    // If S2 button pressed
                    P9OUT &= ~greenLED; // turn off green LED
                    over = 0;   // Exit while loop
                }
    }

    while (count > 0){
        TA0CCR0 = count;                      // count is duration of button press now
        TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;    // Start UP clock and clear flag
        P1OUT |= redLED;                            // turn redLED on
        count = TA0R;                   // count is set to tar value and will reiterate until it goes back to zero
        // When count reaches zero it will exit this loop
        }

    if((TA0CTL & TAIFG) == TAIFG){         // Flag triggered
        P1OUT &= ~redLED;        // turn off redLED
        TA0CTL = 0; // stop clock completely, set everything to zero
        count = 0;  // set count to 0
    }

    }
}
/*
                                                    // 3.2
#include <msp430.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7

void config_ACLK_to_32KHz_crystal() {

// By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
// Reroute pins to LFXIN/LFXOUT functionality
PJSEL1 &=  ~BIT4;
PJSEL0 |= BIT4;

// Wait until the oscillator fault flags remain cleared
CSCTL0 = CSKEY; // Unlock CS registers
do {
CSCTL5 &=  ~LFXTOFFG; // Local fault flag
SFRIFG1 &=  ~OFIFG; // Global fault flag
} while((CSCTL5 & LFXTOFFG) != 0);
CSCTL0_H = 0; // Lock CS registers
return;
}

void main(void) {

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    // Configure and initialize LEDs
    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &=  ~redLED; // Turn LED Off
    P9OUT &= ~greenLED;// Turn LED Off

    //3.2 Up Mode
        TA0CCR0 = 31999;
        TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;

// Configure ACLK to 32 KHz crystal

    config_ACLK_to_32KHz_crystal();

for(;;){

    if((TA0CTL & TAIFG) == TAIFG){  // If flag raised
        P1OUT ^= redLED;    // toggle led
        TA0CTL &= ~TAIFG;   // Clear the flag
    }

}
}
*/
