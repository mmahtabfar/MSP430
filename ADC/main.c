#include <msp430.h> 
#define FLAGS UCA1IFG // Contains the transmit & receive flags
#define RXFLAG UCRXIFG // Receive flag
#define TXFLAG UCTXIFG // Transmit flag
#define TXBUFFER UCA1TXBUF // Transmit buffer
#define RXBUFFER UCA1RXBUF // Receive buffer
#define BUT1 BIT1
#define BUT2 BIT2
#define redLED BIT0         // redLED to bit 0

void config_ACLK_to_32KHz_crystal();
void uart_write_uint16(unsigned int n);     // print number function
void uart_write_char(unsigned char ch);     // print character function
unsigned char uart_read_char(void);         // read character function
void Initialize_UART(void);                 // initialize UART function
void Initialize_ADC(void);                  // initialize ADC


int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    P1DIR |= redLED;   // Red LED as output

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

    Initialize_UART();  // Initialize UART
    Initialize_ADC();   // Initialize ADC
    for(;;) {
        ADC12CTL0 |= ADC12SC; //starting conversion
        //waiting for ADC12SC to clear
        while ((ADC12CTL1 & ADC12BUSY) != 0){}
         {
             uart_write_uint16(ADC12MEM0); //write x value
             uart_write_char(',');  // comma to seperate x and y values
             uart_write_uint16(ADC12MEM1); //write y value
             uart_write_char('\n');      //new line
             uart_write_char('\r');      // Part of new line^
         }
         P1OUT ^= redLED; //flash red led so we know code is running
         _delay_cycles(200000);
}
}
void Initialize_ADC() {
    // Divert the pins to analog functionality
    // X-axis: A10/P9.2, for A10 (P9DIR=x, P9SEL1=1, P9SEL0=1)
    P9SEL1 |= BIT2;
    P9SEL0 |= BIT2;

    P8SEL1 |= BIT7;
    P8SEL0 |= BIT7;

    // Turn on the ADC module
    ADC12CTL0 |= ADC12ON;

    // Turn off ENC (Enable Conversion) bit while modifying the configuration
    ADC12CTL0 &= ~ADC12ENC;

    //*************** ADC12CTL0 ***************
    // Set the bit ADC12MSC (Multiple Sample and Conversion)
    ADC12CTL0|=ADC12MSC|ADC12SHT0_2;

    //*************** ADC12CTL1 ***************
    // Set ADC12CONSEQ (select sequence-of-channels)
    ADC12CTL1= ADC12SHS_0|ADC12SHP|ADC12DIV_7|ADC12SSEL_0;
    ADC12CTL1 |= ADC12CONSEQ_1;

    //*************** ADC12CTL2 ***************
    // Set ADC12RES (select 12-bit resolution)
    // Set ADC12DF (select unsigned binary format)
    ADC12CTL2|= ADC12RES_2;

    //*************** ADC12CTL3 ***************
    // Set ADC12CSTARTADD to 0 (first conversion in ADC12MEM0)
    ADC12CTL3 = ADC12CSTARTADD_0;

    //*************** ADC12MCTL1 ***************
    // Set ADC12VRSEL (select VR+=AVCC, VR-=AVSS)
    // Set ADC12INCH (select the analog channel that you found)
    // Set ADC12EOS (last conversion in ADC12MEM1)
    ADC12MCTL0|= ADC12INCH_10|ADC12VRSEL_0;
    ADC12MCTL1 |= ADC12INCH_4 | ADC12VRSEL_0 | ADC12EOS;

    // Turn on ENC (Enable Conversion) bit at the end of the configuration
    ADC12CTL0 |= ADC12ENC;

    return;
}
void Initialize_UART(void){
// Configure pins to UART functionality
P3SEL1 &=  ~(BIT4|BIT5);
P3SEL0 |= (BIT4|BIT5);
// Main configuration register
UCA1CTLW0 = UCSWRST; // Engage reset; change all the fields to zero
// Most fields in this register, when set to zero, correspond to the
// popular configuration
UCA1CTLW0 |= UCSSEL_2; // Set clock to SMCLK
// Configure the clock dividers and modulators (and enable oversampling)
UCA1BRW = 6; // divider
// Modulators: UCBRF = 8 = 1000 --> UCBRF3 (bit #3)
// UCBRS = 0x20 = 0010 0000 = UCBRS5 (bit #5)
UCA1MCTLW = UCBRF3 | UCBRS5 | UCOS16;
// Exit the reset state
UCA1CTLW0 &=  ~UCSWRST;
}

void uart_write_char(unsigned char ch){
// Wait for any ongoing transmission to complete
while ( (FLAGS & TXFLAG)==0 ) {}
// Copy the byte to the transmit buffer
TXBUFFER = ch; // Tx flag goes to 0 and Tx begins!
return;
}

unsigned char uart_read_char(void){
unsigned char temp;
// Return null character (ASCII=0) if no byte was received
if( (FLAGS & RXFLAG) == 0)
return 0;
// Otherwise, copy the received byte (this clears the flag) and return it
temp = RXBUFFER;
return temp;
}

void uart_write_uint16(unsigned int n){
   int j;

   if (n==0){
           return;
   }
   uart_write_uint16(n/10);
   j= n%10;
   uart_write_char('0'+j);

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
