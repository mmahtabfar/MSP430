//
//#include "msp430fr6989.h"
//#include "Grlib/grlib/grlib.h"          // Graphics library (grlib)
//#include "LcdDriver/lcd_driver.h"       // LCD driver
//#include <stdio.h>
//#define redLED BIT0
//#define greenLED BIT7
//#define S1 BIT1
//#define S2 BIT2
//#define BUT1 BIT1   // S1 is button 1
//#define BUT2 BIT2   // S2 is button 2
//
//void config_ACLK_to_32KHz_crystal();
//void Initialize_Clock_System();
//int i2c_read_word(unsigned char, unsigned char, unsigned int*); //I2C read
//int i2c_write_word(unsigned char, unsigned char, unsigned int); // I2C write
//void Initialize_I2C(void);  //I2C initialization
//int i = 0;
//
//unsigned int Result, Configuration = 0x7604, count, seconds;
//
//// ****************************************************************************
//void main(void) {
//    char mystring[20];
//    extern tImage UCF_Logo;
//    // Configure WDT & GPIO
//    WDTCTL = WDTPW | WDTHOLD;
//    PM5CTL0 &= ~LOCKLPM5;
//
//    // Configure LEDs
//    P1DIR |= redLED;
//    P9DIR |= greenLED;
//    P1OUT &= ~redLED;
//    P9OUT &= ~greenLED;
//
//    // Configure and initialize the Buttons
//    //S1
//    P1DIR &= ~BUT1; // direct output pin
//    P1REN |= BUT1;  // enable built in resistor
//    P1OUT |= BUT1;  // resistor set to pull-up
//    P1IES |= BUT1; // interrupt on falling edge
//    P1IFG &= ~BUT1; // clear interrupt flags
//    P1IE |= BUT1;   // enable the interrupt
//
//    //S2
//    P1DIR &= ~BUT2; // direct output
//    P1REN |= BUT2;  // enable resistor
//    P1OUT |= BUT2;  // resistor pullup
//    P1IES |= BUT2; // interrupt on falling edge
//    P1IFG &= ~BUT2; // clear interrupt flags
//    P1IE |= BUT2;   // enable the interrupt
//
//    // Configure pins to I2C functionality
//    P4SEL1 |= (BIT1|BIT0);
//    P4SEL0 &=  ~(BIT1|BIT0);
//
//    // Configure ACLK to 32 KHz crystal
//        config_ACLK_to_32KHz_crystal();
//
//    // Set the LCD backlight to highest level
//    //P2DIR |= BIT6;
//    //P2OUT |= BIT6;
//
//    // Enable interrupts
//        _enable_interrupts();
//
//    // Configure clock system
//    Initialize_Clock_System();
//
//    Initialize_I2C();   // Initialize I2C
//
////    //Set timer to 0.25 second period and enable interrupts
////    TA0CCR0 = 8000;    // Cycle for 1 seconds
////    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR | TAIE;
////    TA0CTL &= ~TAIFG;   // clear flag
//
//    // Read and write the Configuration Register
//    i2c_write_word(0x44, 0x01, Configuration);
//
//    ////////////////////////////////////////////////////////////////////////////////////////////
//    // Graphics functions
//    Graphics_Context g_sContext;        // Declare a graphic library context
//    Crystalfontz128x128_Init();         // Initialize the display
//
//    // Set the screen orientation
//    Crystalfontz128x128_SetOrientation(0);
//
//    // Initialize the context
//    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
//
//    // Set background and foreground colors
//    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
//    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
//
//    // Set the default font for strings
//    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
//
//    //Clear the screen
//    Graphics_clearDisplay(&g_sContext);
//
//for(;;){
//    int h;
//    i2c_read_word(0x44, 0x00, &Result); //read the lux value from register
//    Result = Result * 1.28;  // correct lux value
//    char Lux[6];
//    for(h = 5; h > 0; h--){     // This for loop takes the integer value of Lux and turns it to array of characters
//       Lux[h] = (Result % 10) + '0';
//       Result = Result / 10;
//    }
//    Graphics_drawStringCentered(&g_sContext, "Lux: ", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);   //Print The string before the lux value
//    Graphics_drawStringCentered(&g_sContext, Lux + 1, AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);   //Print Lux value
//    _delay_cycles(200000);
//    P1OUT ^= redLED;
//}
//
//}
//
//// BUttons ISR
//    //******* Writing the ISR *******
//    #pragma vector = PORT1_VECTOR // Link the ISR to the vector
//    __interrupt void Port1_ISR() {
//        if((P1IN & BUT1) == 0){
//            P1OUT ^= redLED;
//            i++;        // Add one to value of i
//            _delay_cycles(200000);      // Button bounce delay
//            P1IFG &= ~BUT1;             // Clear interrupt flag
//        }
//
//        if((P1IN & BUT2) == 0){
//            i = 0;
//            _delay_cycles(200000);      // Button bounce delay
//            P1IFG &= ~BUT2;             // Clear interrupt flag
//        }
//    }
//
//// *****************************
//void Initialize_Clock_System() {
//    // DCO frequency = 8 MHz (default value)
//    // MCLK = fDCO/2 = 4 MHz
//    // SMCLK = fDCO/1 = 8 MHz
//    CSCTL0 = CSKEY;                         // Unlock clock module config registers
//    CSCTL3 &= ~(BIT2|BIT1|BIT0);            // DIVM = 000
//    CSCTL3 |= BIT0;                         // DIVM = 001 = /2
//    CSCTL3 &= ~(BIT6|BIT5|BIT4);            // DIVS = 000 = /1
//    CSCTL0_H = 0;                           // Relock clock module config registers
//
//    return;
//}
//
//void Initialize_I2C(void) {
//// Configure the MCU in Master mode
//// Configure pins to I2C functionality
//// (UCB1SDA same as P4.0) (UCB1SCL same as P4.1)
//// (P4SEL1=11, P4SEL0=00) (P4DIR=xx)
//P4SEL1 |= (BIT1|BIT0);
//P4SEL0 &=  ~(BIT1|BIT0);
//// Enter reset state and set all fields in this register to zero
//UCB1CTLW0 = UCSWRST;
//// Fields that should be nonzero are changed below
//// (Master Mode: UCMST) (I2C mode: UCMODE_3) (Synchronous mode: UCSYNC)
//// (UCSSEL 1:ACLK, 2,3:SMCLK)
//UCB1CTLW0 |= UCMST | UCMODE_3 | UCSYNC | UCSSEL_3;
//// Clock frequency: SMCLK/8 = 1 MHz/3 = 325 KHz
//UCB1BRW = 3;
//// Chip Data Sheet p. 53 (Should be 400 KHz max)
//// Exit the reset mode at the end of the configuration
//UCB1CTLW0 &= ~UCSWRST;
//}
//
//// Read a word (2 bytes) from I2C (address, register)
//int i2c_read_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int * data) {
//unsigned char byte1, byte2;
//// Initialize the bytes to make sure data is received every time
//byte1 = 111;
//byte2 = 111;
////********** Write Frame #1 ***************************
//UCB1I2CSA = i2c_address; // Set I2C address
//UCB1IFG &= ~UCTXIFG0;
//UCB1CTLW0 |= UCTR; // Master writes (R/W bit = Write)
//UCB1CTLW0 |= UCTXSTT; // Initiate the Start Signal
//while ((UCB1IFG & UCTXIFG0) ==0) {}
//UCB1TXBUF = i2c_reg; // Byte = register address
//while((UCB1CTLW0 & UCTXSTT)!=0) {}
//if(( UCB1IFG & UCNACKIFG )!=0) return -1;
//UCB1CTLW0 &= ~UCTR; // Master reads (R/W bit = Read)
//UCB1CTLW0 |= UCTXSTT; // Initiate a repeated Start Signal
////****************************************************
////********** Read Frame #1 ***************************
//while ( (UCB1IFG & UCRXIFG0) == 0) {}
//byte1 = UCB1RXBUF;
////****************************************************
////********** Read Frame #2 ***************************
//while((UCB1CTLW0 & UCTXSTT)!=0) {}
//UCB1CTLW0 |= UCTXSTP; // Setup the Stop Signal
//while ( (UCB1IFG & UCRXIFG0) == 0) {}
//byte2 = UCB1RXBUF;
//while ( (UCB1CTLW0 & UCTXSTP) != 0) {}
////****************************************************
//// Merge the two received bytes
//*data = ( (byte1 << 8) | (byte2 & 0xFF) );
//return 0;
//}
//
//// Write a word (2 bytes) to I2C (address, register)
//
//int i2c_write_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int data) {
//unsigned char byte1, byte2;
//byte1 = (data >> 8) & 0xFF; // MSByte
//byte2 = data & 0xFF; // LSByte
//UCB1I2CSA = i2c_address; // Set I2C address
//UCB1CTLW0 |= UCTR; // Master writes (R/W bit = Write)
//UCB1CTLW0 |= UCTXSTT; // Initiate the Start Signal
//while ((UCB1IFG & UCTXIFG0) ==0) {}
//UCB1TXBUF = i2c_reg; // Byte = register address
//while((UCB1CTLW0 & UCTXSTT)!=0) {}
//while ((UCB1IFG & UCTXIFG0) ==0) {}
////********** Write Byte #1 ***************************
//UCB1TXBUF = byte1;
//while ( (UCB1IFG & UCTXIFG0) == 0) {}
////********** Write Byte #2 ***************************
//UCB1TXBUF = byte2;
//while ( (UCB1IFG & UCTXIFG0) == 0) {}
//UCB1CTLW0 |= UCTXSTP;
//while ( (UCB1CTLW0 & UCTXSTP) != 0) {}
//return 0;
//}
//
//
//void config_ACLK_to_32KHz_crystal() {
//// By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
//// Reroute pins to LFXIN/LFXOUT functionality
//PJSEL1 &=  ~BIT4;
//PJSEL0 |= BIT4;
//// Wait until the oscillator fault flags remain cleared
//CSCTL0 = CSKEY; // Unlock CS registers
//do {
//CSCTL5 &=  ~LFXTOFFG; // Local fault flag
//SFRIFG1 &=  ~OFIFG; // Global fault flag
//} while((CSCTL5 & LFXTOFFG) != 0);
//CSCTL0_H = 0; // Lock CS registers
//return;
//}

#include "msp430fr6989.h"
#include "Grlib/grlib/grlib.h"          // Graphics library (grlib)
#include "LcdDriver/lcd_driver.h"       // LCD driver
#include <stdio.h>

#define redLED BIT0
#define greenLED BIT7
#define S1 BIT1
#define S2 BIT2
#define BUT1 BIT1   // S1 is button 1
#define BUT2 BIT2   // S2 is button 2

void config_ACLK_to_32KHz_crystal();
void Initialize_Clock_System();
int i = 0;

// ****************************************************************************
void main(void) {
    char mystring[20];
    extern tImage UCF_Logo;
    // Configure WDT & GPIO
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    // Configure LEDs
    P1DIR |= redLED;
    P9DIR |= greenLED;
    P1OUT &= ~redLED;
    P9OUT &= ~greenLED;

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

    // Configure ACLK to 32 KHz crystal
        config_ACLK_to_32KHz_crystal();

    // Set the LCD backlight to highest level
    //P2DIR |= BIT6;
    //P2OUT |= BIT6;

    // Enable interrupts
        _enable_interrupts();

    // Configure clock system
    Initialize_Clock_System();

    ////////////////////////////////////////////////////////////////////////////////////////////
    // Graphics functions
    Graphics_Context g_sContext;        // Declare a graphic library context
    Crystalfontz128x128_Init();         // Initialize the display

    // Set the screen orientation
    Crystalfontz128x128_SetOrientation(0);

    // Initialize the context
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);

    // Set background and foreground colors
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);

    // Set the default font for strings
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

    //Clear the screen
    Graphics_clearDisplay(&g_sContext);

    tRectangle myRectangle1 = { 40, 100, 50, 80};   // For outline Rectangle
    tRectangle myRectangle2 = { 40, 100, 50, 80};   // For fill Rectangle
for(;;){

    if(i == 0){
        Graphics_drawImage(&g_sContext, &UCF_Logo,0,0);
    }
    else if(i == 1){
        //Clear the screen
        Graphics_clearDisplay(&g_sContext);
    }

    else if(i == 2){
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK); // Set foreground to black
        GrContextFontSet(&g_sContext, &g_sFontFixed6x8);    // Set first font
        GrCircleDraw(&g_sContext, 100, 100, 20);  // circle outline

        Graphics_drawStringCentered(&g_sContext, "Outline. Press Again", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);  // Message
        GrContextFontSet(&g_sContext, &g_sFontlucidasans8x15);    // Second font
        sprintf(mystring, "To Fill");   // Message

        Graphics_drawStringCentered(&g_sContext, mystring, AUTO_STRING_LENGTH, 64, 55, OPAQUE_TEXT);
        GrRectDraw(&g_sContext, &myRectangle1 );    // Draw rectangle outline
        Graphics_drawLineH(&g_sContext, 20, 100, 62);
    }

    else if(i == 3){
        Graphics_setForegroundColorTranslated(&g_sContext, GRAPHICS_COLOR_BLUE);
        GrCircleFill(&g_sContext, 100, 100, 20);  // fill circle
        GrRectFill(&g_sContext, &myRectangle2); // fill rectangle
    }

    else if(i == 4){
        Graphics_clearDisplay(&g_sContext); // clear screen
    }
}

}
// Timer ISR
//******* Writing the ISR *******
#pragma vector = TIMER0_A1_VECTOR // Link the ISR to the vector
__interrupt void T0A1_ISR() {
    TA0CTL &= ~TAIFG;       //Clear Flag
}



// BUttons ISR
    //******* Writing the ISR *******
    #pragma vector = PORT1_VECTOR // Link the ISR to the vector
    __interrupt void Port1_ISR() {
        if((P1IN & BUT1) == 0){
            P1OUT ^= redLED;
            i++;        // Add one to value of i
            _delay_cycles(200000);      // Button bounce delay
            P1IFG &= ~BUT1;             // Clear interrupt flag
        }

        if((P1IN & BUT2) == 0){
            i = 0;
            _delay_cycles(200000);      // Button bounce delay
            P1IFG &= ~BUT2;             // Clear interrupt flag
        }
    }



// *****************************
void Initialize_Clock_System() {
    // DCO frequency = 8 MHz (default value)
    // MCLK = fDCO/2 = 4 MHz
    // SMCLK = fDCO/1 = 8 MHz
    CSCTL0 = CSKEY;                         // Unlock clock module config registers
    CSCTL3 &= ~(BIT2|BIT1|BIT0);            // DIVM = 000
    CSCTL3 |= BIT0;                         // DIVM = 001 = /2
    CSCTL3 &= ~(BIT6|BIT5|BIT4);            // DIVS = 000 = /1
    CSCTL0_H = 0;                           // Relock clock module config registers

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
