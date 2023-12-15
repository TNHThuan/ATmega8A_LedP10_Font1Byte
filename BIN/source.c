#include <mega8.h>

// Declare your global variables here
#include <delay.h>
#include <ledfont_size9_1byte.h>
#include <atmega8_ledp10_dispfnc.h>

unsigned char flagFrameDataValid = 0;
// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void)
{
	unsigned char dataRec=UDR;        //data recieved from UART

    if(flagFrameDataValid){
        switch(dataRec){
            case '#':
                flagFrameDataValid=0;
                break;

            case '&':
                Matrix_SelectPos(3,17);
                break;

            default:
                Matrix_PrintText(dataRec);
                break;
        }
    }

    if(dataRec == '$' && !flagFrameDataValid){
        Matrix_ClearFromTo(0,0,64,32);
        Matrix_SelectPos(3,1);
        flagFrameDataValid=1;
    }
}

// Timer 0 overflow interrupt service routine
unsigned char interCheck = 0;
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
// Reinitialize Timer 0 value
//TCNT0=0x83;     //2ms    //2*2ms timer + (4 line * 2ms pwm) = 12ms => 80Hz
TCNT0=0x06;    //4ms    //2*4ms timer + (4 line * 2ms pwm) = 16ms => 60Hz
//TCNT0=0x83;    //8ms        => 40Hz
// Place your code here
    interCheck++;
    if(interCheck == 2)     
    {
        TimerHandlerShowLed();
        interCheck=0;
    }
}

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port B initialization
// Function: Bit7=In Bit6=In Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRB=(0<<DDB7) | (0<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
// State: Bit7=T Bit6=T Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit6=In Bit5=Out Bit4=Out Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC6) | (1<<DDC5) | (1<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit6=T Bit5=T Bit4=0 Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=Out Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(1<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 62.500 kHz
TCCR0=(1<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x06;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (1<<TOIE0);

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: Off
// USART Mode: Asynchronous
// USART Baud Rate: 600 (Double Speed Mode)
UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (1<<U2X) | (0<<MPCM);
UCSRB=(1<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (0<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
UBRRH=0x0D;
UBRRL=0x04;

// Global enable interrupts
#asm("sei")

Matrix_SelectPos(3,1);
Matrix_PrintString("0");
Matrix_SelectPos(3,17);
Matrix_PrintString("0");

while (1)
      {
      // Place your code here

      }
}