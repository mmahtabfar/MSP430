#include <msp430.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1   // S1 is button 1
#define BUT2 BIT2   // S2 is button 2

unsigned int L;

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
    P1IES |= BUT1; // interrupt on falling edge
    P1IFG &= ~BUT1; // clear interrupt flags
    P1IE |= BUT1;   // enable the interrupt

    //S2
    P1DIR &= ~BUT2; // direct output
    P1REN |= BUT2;  // enable resistor
    P1OUT |= BUT2;  // resistor pullup
    P1IES |= BUT2; // interrupt on falling edge
    P1IFG &= ~BUT2; // clear interrupt flags
    P1IE |= BUT2;   // enable the interrupt


    // Configure Channel 1
    TA1CCR1 = 5000 - 1; // 0.5 Seconds
    TA1CCTL1 |= CCIE;   // Enable interrupt bit
    TA1CCTL1 &= ~CCIFG; // Clear flag

    // Configure Channel 2
    TA1CCR2 = 30000 - 1;    // 3 Seconds
    TA1CCTL2 &= ~CCIE;   // Disable Interrupt Bit
    TA1CCTL2 &= ~CCIFG; // Clear Flag

    TA1CTL = TASSEL_1 | ID_2 | MC_2 | TACLR;    // Start 3 Second Timer

// Enable interrupts
_enable_interrupts();


_low_power_mode_3();        // Leave ACLK on

while(1){}

}

//******* Writing the ISR *******// ISR for Channel 1

    #pragma vector = TIMER1_A1_VECTOR // Link the ISR to the vector
    __interrupt void T1A1_ISR() {
        if((TA1CCTL1 & CCIFG) != 0){
            P9OUT ^= greenLED;        // Toggle green
            TA1CCR1 += 5000;
            TA1CCTL1 &= ~CCIFG;     // Clear Flag
        }
        if((TA1CCTL2 & CCIFG) != 0 && (TA1CCTL2 & CCIE) != 0){
            P1OUT ^= redLED;                // Toggle red LED
            TA1CCR2 += 30000;
            TA1CCTL2 &= ~CCIE;   // Disable Interrupt Bit
            TA1CCTL2 &= ~CCIFG;             // Clear Flag
        }
    }

// BUttons ISR
    //******* Writing the ISR *******
    #pragma vector = PORT1_VECTOR // Link the ISR to the vector
    __interrupt void Port1_ISR() {
        if((P1IN & BUT1) == 0){ // S1 Button Pressed
            TA1CCTL2 &= ~CCIFG; // Clear Flag

            // Configure Channel 1
            TA1CCR1 = 5000 - 1; // 0.5 Seconds
            TA1CCTL1 |= CCIE;   // Enable interrupt bit
            TA1CCTL1 &= ~CCIFG; // Clear flag

            // Configure Channel 2
            TA1CCR2 = 30000 - 1;    // 3 Seconds
            TA1CCTL2 &= ~CCIFG; // Clear Flag
            TA1CCTL2 |= CCIE;   // Enable Interrupt Bit

            TA1CTL = TASSEL_1 | ID_2 | MC_2 | TACLR;    // Start 3 Second Timer
            P1IFG &= ~BUT1;             // Clear interrupt flag
        }
    }
