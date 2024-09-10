#include <msp430fr6989.h>
#define redLED BIT0 // Red at P1.0
#define greenLED BIT7 // Green at P9.7
#define BUT1 BIT1   //S1
#define BUT2 BIT2   //S2
void Initialize_LCD();  // Call LCD function
void lcd_display_uint16(unsigned int n);
void config_ACLK_to_32KHz_crystal();
int c = 0;
int stpw;

// The array has the shapes of the digits (0 to 9)
const unsigned char LCD_Shapes[10] = {0xFC, 0x60, 0xDB, 0xF1, 0x67, 0xB7, 0xBF, 0xE0, 0xFF, 0xF7};
volatile unsigned char* LCDptr[5] = {&LCDM8, &LCDM15, &LCDM19, &LCDM4, &LCDM6};

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &=  ~LOCKLPM5; // Enable GPIO pins
    P1DIR |= redLED; // Pins as output
    P9DIR |= greenLED;
    P1OUT &= ~redLED; // Red off
    P9OUT &=  ~greenLED; // Green off

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

    // Initializes the LCD_C module
    Initialize_LCD();
    TA0CTL &= ~TAIFG;   // clear flag

    // Configure ACLK to 32 KHzrystal
    config_ACLK_to_32KHz_crystal();

    LCDM3 = 0x08;   //Timer symbol
    LCDM20 = 0x01;  // Decimal point

    lcd_display_uint16(c);  // Display 0

    TA0CCR0 = 32000 - 1;    // Cycle for 1 second
    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR | TAIE;     // Start Clock with interrupts
    // Enable interrupts
    _enable_interrupts();

    while((TA0CTL & TAIFG) == 0 ){} // Do nothing between interrupts
    while((P1IN & BUT1) == BUT1){}  // Do nothing between interrupts

  for(;;) {
  }
    return 0;
}



// Timer ISR
//******* Writing the ISR *******
#pragma vector = TIMER0_A1_VECTOR // Link the ISR to the vector
__interrupt void T0A1_ISR() {
        c++;                // count + 1
        lcd_display_uint16(c);  // Display current count value
        P1OUT ^= redLED;    // Toggle LED
        LCDM7 ^= 0x04;   // : symbol
        TA0CTL &= ~TAIFG;       // clear interrupt flag
}

// BUttons ISR
    //******* Writing the ISR *******
#pragma vector = PORT1_VECTOR // Link the ISR to the vector
__interrupt void Port1_ISR() {
    if((P1IN & BUT2) == 0){ // S2 pressed
        TA0CCR0 = 16;    // short interval cycle
        TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;     // Start Clock
        while((P1IN & BUT2) == 0){
            if((P1IN & BUT1) == 0){ // S1 pressed
                TA0CCR0 = 16;    // short interval cycle
                TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;     // Start Clock
                while((P1IN & BUT1) == 0){
                        if((TA0CTL & TAIFG) == 1){  // if flag is raised
                                   c--;          // sub 1 to count
                                   lcd_display_uint16(c);  // display count
                                   TA0CTL &= ~TAIFG;   // clear flag
                         }
                 }
            }
            if((TA0CTL & TAIFG) == 1){  // if flag is raised
                c += 1;          // add 1 to count
                lcd_display_uint16(c);  // display count
                TA0CTL &= ~TAIFG;   // clear flag
            }
        }
        TA0CCR0 = 32000 - 1;    // Cycle for 1 second
        TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR | TAIE;     // Start Clock with interrupts
        _delay_cycles(200000);  // Button bounce delay
    }

    if((P1IN & BUT1) == 0){ // S1 pressed
        stpw += 1;
        TA0CCR0 = 16000 - 1;    // Cycle for 0.5 second
        TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;     // Start Clock
        lcd_display_uint16(c);  // Display n

        while((P1IN & BUT1) == 0){  // While button is pressed
            if((TA0CTL & TAIFG) == 1){  // if 0.5 seconds elapses -> Flag is raised and clear timer
                LCDM3 = 0x01;   // ! symbol
                c = 0;          // set count to 0
                lcd_display_uint16(c);  // display 0
                TA0CTL &= ~TAIFG;   // clear flag
                stpw = 1;   // set to odd
            }
        }

        if((stpw % 2)!= 0){         // Odd stop clock
            LCDM3 = 0x01;   // ! symbol
            TA0CTL &= MC_0; // stop clock
        }

        if((stpw % 2)== 0){ // Even resume clock
            LCDM3 = 0x08;   //Timer symbol
            TA0CCR0 = 32000 - 1;    // Cycle for 1 second
            TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR | TAIE;     // Start Clock with interrupts
            lcd_display_uint16(c);  // Display n
        }
        _delay_cycles(200000);  // Button bounce delay
        P1IFG &= ~BUT1;     // clear button interrupt flag
    }
//
//    if((P1IN & BUT2) == 0){ // S2 pressed
//        TA0CCR0 = 1600;    // short interval cycle
//        TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;     // Start Clock
//        while((P1IN & BUT2) == 0){
//            if((TA0CTL & TAIFG) == 1){  // if flag is raised
//                c += 1;          // add 1 to count
//                lcd_display_uint16(c);  // display count
//                TA0CTL &= ~TAIFG;   // clear flag
//            }
//        }
//        lcd_display_uint16(c);  // Display count
//        P1IFG &= ~BUT1;     // clear button interrupt flag
//        _delay_cycles(200000);  // Button bounce delay
//    }
}

void lcd_display_uint16(unsigned int n){
    int digit, i = 0;
    do{
        digit = n % 10;
        *LCDptr[i] = LCD_Shapes[digit];
        n = n/10;
        i++;
    } while (n > 0);
     //    clear remaining digits
    while (i < 5){
        *LCDptr[i] = 0xFC;
        i++;
    }
    return;
}

//**********************************************************
// Initializes the LCD_C module
// *** Source: Function obtained from MSP430FR6989’s Sample Code ***
void Initialize_LCD() {
PJSEL0 = BIT4 | BIT5; // For LFXT
// Initialize LCD segments 0 - 21; 26 - 43
LCDCPCTL0 = 0xFFFF;
LCDCPCTL1 = 0xFC3F;
LCDCPCTL2 = 0x0FFF;
// Configure LFXT 32kHz crystal
CSCTL0_H = CSKEY >> 8; // Unlock CS registers
CSCTL4 &=  ~LFXTOFF; // Enable LFXT
do {
CSCTL5 &=  ~LFXTOFFG; // Clear LFXT fault flag
SFRIFG1 &=  ~OFIFG;
}while (SFRIFG1 & OFIFG); // Test oscillator fault flag
CSCTL0_H = 0; // Lock CS registers
// Initialize LCD_C

// ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;
// VLCD generated internally,
// V2-V4 generated internally, v5 to ground
// Set VLCD voltage to 2.60v
// Enable charge pump and select internal reference for it
LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;
LCDCCPCTL = LCDCPCLKSYNC; // Clock synchronization enabled
LCDCMEMCTL = LCDCLRM; // Clear LCD memory
//Turn LCD on
LCDCCTL0 |= LCDON;
return;
}

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

