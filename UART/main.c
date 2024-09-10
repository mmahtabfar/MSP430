#include <msp430.h>
#define redLED BIT0
#define greenLED BIT7
#define FLAGS UCA1IFG
#define RXFLAG UCRXIFG
#define TXFLAG UCTXIFG
#define TXBUFFER UCA1TXBUF //Transmit buffer
#define RXBUFFER UCA1RXBUF //Receive buffer
void uart_write_uint16(unsigned int x){
if(x>=10000)
    uart_write_char('0'+(x/10000));
if(x>=1000)
    uart_write_char('0'+((x/1000)%10));
if(x>=100)
    uart_write_char('0'+((x/100)%10));
if(x>=10)
    uart_write_char('0'+((x/10)%10));
uart_write_char('0'+(x%10));
}
void Initialize_UART(void){
//Pins are Diverted to UART
P3SEL1 &= ~(BIT4 | BIT5);
P3SEL0 |= (BIT4 | BIT5);
UCA1CTLW0 |= UCSSEL_2;
//UCA1BRW gets 6
UCA1BRW = 6;
UCA1MCTLW = UCBRS5 | UCBRF3 | UCOS16;
//Red LED at P1.0 //Green LED at P9.7
//Contains the transmit & receive flag //Recieve flag
 //Transmit buffer

//Reset state is exited UCA1CTLW0 &= ~UCSWRST;
}
void uart_write_char(unsigned char ch){
// Wait for any ongoing transmission to complete
while ( (FLAGS & TXFLAG)==0 ) {}
// Copy the byte to the transmit buffer
TXBUFFER = ch; // Tx flag goes to 0 and Tx begins!
return;
}
unsigned char uart_read_char(void){
unsigned char hold; if((FLAGS & RXFLAG)==0)
return 0;
hold = RXBUFFER;
return hold;
}
void uart_newline(){
uart_write_char(10);
uart_write_char(13); }
void Initialize_ADC() {
// Divert the pins to analog functionality
// X-axis: A10/P9.2, for A10 (P9DIR=x, P9SEL1=1, P9SEL0=1) P9SEL1 |= BIT2;
P9SEL0 |= BIT2;
// Turn on the ADC module ADC12CTL0 |= ADC12ON;
// Turn off ENC (Enable Conversion) bit while modifying the configuration ADC12CTL0 &= ~ADC12ENC;
//*************** ADC12CTL0 ***************
// Set ADC12SHT0 (select the number of cycles that you determined) ADC12CTL0|=ADC12ON|ADC12SHT1_10;
//*************** ADC12CTL1 *************** // Set ADC12SHS (select ADC12SC bit as the trigger) // Set ADC12SHP bit
// Set ADC12DIV (select the divider you determined) // Set ADC12SSEL (select MODOSC)
ADC12CTL1= ADC12SHS_0|ADC12SHP|ADC12DIV_7|ADC12SSEL_0;
//*************** ADC12CTL2 *************** // Set ADC12RES (select 12-bit resolution)
// Set ADC12DF (select unsigned binary format)

ADC12CTL2|= ADC12RES_1;
//*************** ADC12CTL3 *************** // Leave all fields at default values
//*************** ADC12MCTL0 ***************
// Set ADC12VRSEL (select VR+=AVCC, VR-=AVSS)
// Set ADC12INCH (select channel A10)
ADC12MCTL0|= ADC12INCH_10|ADC12VRSEL_0;
// Turn on ENC (Enable Conversion) bit at the end of the configuration ADC12CTL0 |= ADC12ENC;
return; }
void main(){ int y;
WDTCTL = WDTPW | WDTHOLD; //Stops watch dog timer PM5CTL0 &= ~LOCKLPM5; // Enables GPIO pins
// Direct pins as the output
P1DIR |= redLED;
P9DIR |= greenLED;
P1OUT &= ~redLED; // Turns redLED Off P9OUT &= ~greenLED; // Turns greenLED OFF Initialize_UART();
Initialize_ADC();
for(;;){
ADC12CTL0 |= ADC12SC;
while ((ADC12CTL1 & ADC12BUSY) != 0){} {
uart_write_uint16(ADC12MEM0);
uart_newline(); }
P1OUT ^= redLED;
for( y=0; y<16383; y++){} }
}
