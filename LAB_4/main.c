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

int L = 0;
unsigned int Blink = 32000;


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

    //Ensure Flag is cleared
    TA0CTL &= ~TAIFG;

// Configure ACLK to 32 KHz crystal
    config_ACLK_to_32KHz_crystal();

  //  Use ACLK, divide by 1, up mode, TAR cleared, enable interrupt  for rollback-to-zero event
    TA0CCR0 = Blink;
    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR | TAIE;

    // Enable interrupts
    _enable_interrupts();

    //4.2
    while((TA0CTL & TAIFG) == 0 ){}
    //4.3
    while((P1IN & BUT1) == BUT1){}
    while((P1IN & BUT2) == BUT2){}

//for(;;){}
_low_power_mode_2();
}

// Lights ISR
    //******* Writing the ISR *******
    #pragma vector = TIMER0_A1_VECTOR // Link the ISR to the vector
    __interrupt void T0A1_ISR() {
        if(L > 0){                  // L is greater than zero
            P1OUT ^= redLED;        // Toggle Red
            P9OUT &= ~greenLED;     // Green off
            TA0CTL &= ~TAIFG;       //Clear Flag
        }

        else if(L == 0){          // L is zero
            P1OUT ^= redLED;        // Toggle Red
            P9OUT ^= greenLED;      // Toggle Green
            TA0CTL &= ~TAIFG;       //Clear Flag
        }

        else if(L < 0){                         // L is less than zero
                    P9OUT ^= greenLED;        // Toggle green
                    P1OUT &= ~redLED;     // red off
                    TA0CTL &= ~TAIFG;       //Clear Flag
          }
    }

// BUttons ISR
    //******* Writing the ISR *******
    #pragma vector = PORT1_VECTOR // Link the ISR to the vector
    __interrupt void Port1_ISR() {

        if((P1IN & BUT1) == 0){         // S1 Button Pressed
            L++;                        // L + 1;

            if(L > 0 && L <= 3){                 // If L is greater than zero and less than or equal to 3
                Blink = Blink/2;        // Make blink faster
            }

            else if(L <= 0){                 // If L is less than or equal to 0
                Blink = Blink*2;        // Make blink slower
            }

            if(L >= -3 && L <= 3){          // If mode is changed reset clock, LEDs, and set new interval
                TA0CCR0 = Blink;
                TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR | TAIE;     // Reset clock to new conditions
                P9OUT &= ~greenLED;
                P1OUT &= ~redLED;
                _delay_cycles(200000);      // Button bounce delay
            }

            else if(L > 3){         // If goes over 3 set to 3 again
                L = 3;
            }

            P1IFG &= ~BUT1;             // Clear interrupt flag

        }

        if((P1IN & BUT2) == 0){         // S2 Button Pressed
            L--;
            if(L > 0){                  // If L is greater than or equal to 0
                Blink = Blink * 2;      // Make blink slower
            }

            else if(L == 0){
                Blink = Blink * 2;      // Make blink slower
            }

            else if (L < 0 && L >= -3){                 // If L is less than zero
                Blink = Blink/2;        // Make blink faster
            }

            if(L >= -3 && L <= 3){
                TA0CCR0 = Blink;
                TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR | TAIE;     // Reset clock to new conditions
                P9OUT &= ~greenLED;
                P1OUT &= ~redLED;
                _delay_cycles(200000);      // Button bounce delay
            }

            else if (L < -3){
                           L = -3;
                       }
                       P1IFG &= ~BUT2;             // Clear interrupt flag
        }
    }

