#include <msp430.h> 
#define FLAGS UCA1IFG // Contains the transmit & receive flags
#define RXFLAG UCRXIFG // Receive flag
#define TXFLAG UCTXIFG // Transmit flag
#define TXBUFFER UCA1TXBUF // Transmit buffer
#define RXBUFFER UCA1RXBUF // Receive buffer

void config_ACLK_to_32KHz_crystal();
int i2c_read_word(unsigned char, unsigned char, unsigned int*); //I2C read
int i2c_write_word(unsigned char, unsigned char, unsigned int); // I2C write
void Initialize_I2C(void);  //I2C initialization
void uart_print_string(unsigned char s[], int length);  // print string
void uart_write_char(unsigned char ch);     // print character function
unsigned char uart_read_char(void);         // read character function
void Initialize_UART(void);                 // initialize UART function
void uart_write_uint16(unsigned int n);     // print number function



int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

	// Configure pins to I2C functionality
	// (UCB1SDA same as P4.0) (UCB1SCL same as P4.1)
	// (P4SEL1=11, P4SEL0=00) (P4DIR=xx)
	P4SEL1 |= (BIT1|BIT0);
	P4SEL0 &=  ~(BIT1|BIT0);

	_enable_interrupts();
    Initialize_I2C();   // Initialize I2C
    Initialize_UART();  // Initialize UART

	// Light sensor is connected to these pins
	    // Serial clock (SCL): I2C_SCL at J1.9
	    // Serial data (SDA): I2C_SDA at J1.10
	
    // Reading two bytes from register 0x50 on I2C device 0x22
    unsigned int ManufacturerID, DeviceID;

    i2c_read_word(0x44, 0x7E, &ManufacturerID);
    i2c_read_word(0x44, 0x7F, &DeviceID);

    // Configure ACLK to 32 KHzrystal
//    config_ACLK_to_32KHz_crystal();
//
//	TA0CTL &= ~TAIFG;   // clear flag
//	TA0CCR0 = 32000;    // Set TAR for UP mode
//	TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR | TAIE;

	for(;;){
	    i2c_read_word(0x44, 0x7E, &ManufacturerID);
	    i2c_read_word(0x44, 0x7F, &DeviceID);
	    _delay_cycles(200000);
	    uart_write_uint16(ManufacturerID);
	    uart_write_uint16(DeviceID);
	    _delay_cycles(200000);

	}

}

////Timer ISR
////******* Writing the ISR *******
//#pragma vector = TIMER0_A1_VECTOR // Link the ISR to the vector
//__interrupt void T0A1_ISR() {
//        uart_write_char('1');
//        uart_write_uint16(ManufacturerID);
//        TA0CTL &= ~TAIFG;       // clear flag
//}


// Read a word (2 bytes) from I2C (address, register)
int i2c_read_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int * data) {
unsigned char byte1, byte2;
// Initialize the bytes to make sure data is received every time
byte1 = 111;
byte2 = 111;
//********** Write Frame #1 ***************************
UCB1I2CSA = i2c_address; // Set I2C address
UCB1IFG &= ~UCTXIFG0;
UCB1CTLW0 |= UCTR; // Master writes (R/W bit = Write)
UCB1CTLW0 |= UCTXSTT; // Initiate the Start Signal
while ((UCB1IFG & UCTXIFG0) ==0) {}
UCB1TXBUF = i2c_reg; // Byte = register address
while((UCB1CTLW0 & UCTXSTT)!=0) {}
if(( UCB1IFG & UCNACKIFG )!=0) return -1;
UCB1CTLW0 &= ~UCTR; // Master reads (R/W bit = Read)
UCB1CTLW0 |= UCTXSTT; // Initiate a repeated Start Signal
//****************************************************
//********** Read Frame #1 ***************************
while ( (UCB1IFG & UCRXIFG0) == 0) {}
byte1 = UCB1RXBUF;
//****************************************************
//********** Read Frame #2 ***************************
while((UCB1CTLW0 & UCTXSTT)!=0) {}
UCB1CTLW0 |= UCTXSTP; // Setup the Stop Signal
while ( (UCB1IFG & UCRXIFG0) == 0) {}
byte2 = UCB1RXBUF;
while ( (UCB1CTLW0 & UCTXSTP) != 0) {}
//****************************************************
// Merge the two received bytes
*data = ( (byte1 << 8) | (byte2 & 0xFF) );
return 0;
}


////////////////////////////////////////////////////////////////////
// Write a word (2 bytes) to I2C (address, register)
int i2c_write_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int data) {
unsigned char byte1, byte2;
byte1 = (data >> 8) & 0xFF; // MSByte
byte2 = data & 0xFF; // LSByte
UCB1I2CSA = i2c_address; // Set I2C address
UCB1CTLW0 |= UCTR; // Master writes (R/W bit = Write)
UCB1CTLW0 |= UCTXSTT; // Initiate the Start Signal
while ((UCB1IFG & UCTXIFG0) ==0) {}
UCB1TXBUF = i2c_reg; // Byte = register address
while((UCB1CTLW0 & UCTXSTT)!=0) {}
while ((UCB1IFG & UCTXIFG0) ==0) {}
//********** Write Byte #1 ***************************
UCB1TXBUF = byte1;
while ( (UCB1IFG & UCTXIFG0) == 0) {}
//********** Write Byte #2 ***************************
UCB1TXBUF = byte2;
while ( (UCB1IFG & UCTXIFG0) == 0) {}
UCB1CTLW0 |= UCTXSTP;
while ( (UCB1CTLW0 & UCTXSTP) != 0) {}
return 0;
}

void Initialize_I2C(void) {
// Configure the MCU in Master mode
// Configure pins to I2C functionality
// (UCB1SDA same as P4.0) (UCB1SCL same as P4.1)
// (P4SEL1=11, P4SEL0=00) (P4DIR=xx)
P4SEL1 |= (BIT1|BIT0);
P4SEL0 &=  ~(BIT1|BIT0);
// Enter reset state and set all fields in this register to zero
UCB1CTLW0 = UCSWRST;
// Fields that should be nonzero are changed below
// (Master Mode: UCMST) (I2C mode: UCMODE_3) (Synchronous mode: UCSYNC)
// (UCSSEL 1:ACLK, 2,3:SMCLK)
UCB1CTLW0 |= UCMST | UCMODE_3 | UCSYNC | UCSSEL_3;
// Clock frequency: SMCLK/8 = 1 MHz/8 = 125 KHz
UCB1BRW = 8;
// Chip Data Sheet p. 53 (Should be 400 KHz max)
// Exit the reset mode at the end of the configuration
UCB1CTLW0 &= ~UCSWRST;
}

void uart_print_string(unsigned char s[], int length){
    int i;
    for(i = 0; i < length; i++){        // Printing for loop
        uart_write_char(s[i]);                   // Print current character
        if(s[i] == '\0'){               // if null character detected
            uart_write_char('\r');      // after string printed start new line
            uart_write_char('\n');
            return;
        }
    }
    uart_write_char('\r');      // after string printed start new line
    uart_write_char('\n');

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
    // Wait for any ongoing transmission to complete
    while ( (FLAGS & TXFLAG)==0 ) {}
    // Transmit the low byte
    uart_write_char((unsigned char)(n & 0xFF));
    // Wait for transmission to complete
    while ( (FLAGS & TXFLAG)==0 ) {}
    // Transmit the high byte
    uart_write_char((unsigned char)(n >> 8));
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
