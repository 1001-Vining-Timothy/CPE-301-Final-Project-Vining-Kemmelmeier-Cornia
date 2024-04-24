// I modified Tim's starter txt

// My focus for 4/23 work - outlining what we need for the motor components (the fan and the vent) as they seem like the most unfamiliar

/*
What the fan has to do:
- Main idea: Start and stop a fan motor as needed when the temperature falls out of a specified range (high or low).
- Stop button should turn fan motor off (if on) and system should go to DISABLED state
- When running, fan motor should be on
- The kit motor and fan blade must be used for the fan motor.
- Must use separate power supply for the fan
*/

/*
What has to be done for opening/closing the vent (stepper motor):
- Main idea: Allow a user to use a control to adjust the angle of an output vent from the system
- The vent direction control must be implemented using the stepper motor. We can use either buttons or a potentiometer to control the direction of the vent
  + We may use the Arduino libraries for the stepper motor
- All states except DISABLED: System should respond to changes in vent position control
- Vent position should be adjustable at all states except error
- Side note: – The realtime clock must be used to report (via the Serial port) the time of each state transition, and any changes to the stepper motor position for the vent.
*/

/*
- Use separate power supply for both motors
- About the 293D motor driver IC we'll be using to control the speed and direction of the fan:
    + IN1 and IN2 pins control the spinning direction of motor A
    + IN3 and IN4 control the spinning direction of motor B.
- For the stepper motor, I think we will be using the ULN2003-based driver board (see motors slides)

ULN2003-based driver board :
- IN1 – IN4 are motor control input pins. Connect them to the Arduinoʼs digital output pins.
- Motor Connector = where the motor plugs in. The connector is keyed, so it will only go in one way.

I am not sure about what the code will look like for the fan motor yet:
- It looks like for the stepper (vent) but not the fan, we can use library functions
- We will need to take a look at slide 26 of the motor functions and make it so it is GPIO instead
- Might be a good idea to test the fan with the example code though (?) 
*/

#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Stepper.h> //LK - this is needed for the vent

#define RDA 0x80
#define TBE 0x20 

// LK- defines the number of steps per rotation (for the fan)
const int stepsPerRevolution = 2038;

// LK - creates an instance of stepper class
// LK - pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
// LK - see slides for L293D motor driver IC in motors lectures
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11); // this is pulled from the slides, needed for the vent



const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {48, 46, 44, 42}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {40, 38, 36, 34}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// LCD pins <--> Arduino pins
const int RS = 11, EN = 12, D4 = 2, D5 = 3, D6 = 4, D7 = 5;
int right=0,up=0;
int dir1=0,dir2=0;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// UART Pointers
volatile unsigned char *myUCSR0A  = (unsigned char *) 0x00C0;
volatile unsigned char *myUCSR0B  = (unsigned char *) 0x00C1;
volatile unsigned char *myUCSR0C  = (unsigned char *) 0x00C2;
volatile unsigned int  *myUBRR0   = (unsigned  int *) 0x00C4;
volatile unsigned char *myUDR0    = (unsigned char *) 0x00C6;
// GPIO Pointers
volatile unsigned char *portB     = (unsigned char *) 0x25;
volatile unsigned char *portDDRB  = (unsigned char *) 0x24;
// Timer Pointers
volatile unsigned char *myTCCR1A  = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B  = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C  = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1  = (unsigned char *) 0x6F;
volatile unsigned char *myTIFR1   = (unsigned char *) 0x36;
volatile unsigned int  *myTCNT1   = (unsigned  int *) 0x84;


void setup()
{
  //setup the UART
  U0int(9600);
  //setup the ADC
  adc_init();
}

void loop() 
{


}

//UART FUNCTIONS
void U0Init(int U0baud)
{
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}
unsigned char kbhit()
{
  return *myUCSR0A & RDA;
}
unsigned char getChar()
{
  return *myUDR0;
}
void putChar(unsigned char U0pdata)
{
  while((*myUCSR0A & TBE)==0);
  *myUDR0 = U0pdata;
}

//DELAY FUNCTION - REGULAR
void my_delay(unsigned int freq)
{
  // calc period
  double period = 1.0/double(freq);
  // 50% duty cycle
  double half_period = period/ 2.0f;
  // clock period def
  double clk_period = 0.0000000625;
  // calc ticks
  unsigned int ticks = half_period / clk_period;
  // stop the timer
  *myTCCR1B &= 0xF8;
  // set the counts
  *myTCNT1 = (unsigned int) (65536 - ticks);
  // start the timer
  * myTCCR1A = 0x0;
  * myTCCR1B |= 0b00000001;
  // wait for overflow
  while((*myTIFR1 & 0x01)==0); // 0b 0000 0000
  // stop the timer
  *myTCCR1B &= 0xF8;   // 0b 0000 0000
  // reset TOV           
  *myTIFR1 |= 0x01;
}

//ADC FUNCTIONS
void adc_init()
{
  // setup the A register
  *my_ADCSRA |= 0b10000000; // set bit   7 to 1 to enable the ADC
  *my_ADCSRA &= 0b11011111; // clear bit 6 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0b11110111; // clear bit 5 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0b11111000; // clear bit 0-2 to 0 to set prescaler selection to slow reading
  // setup the B register
  *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0b11111000; // clear bit 2-0 to 0 to set free running mode
  // setup the MUX Register
  *my_ADMUX  &= 0b01111111; // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
}
unsigned int adc_read(unsigned char adc_channel_num)
{
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX  &= 0b11100000;
  // clear the channel selection bits (MUX 5)
  *my_ADCSRB &= 0b11110111;
  // set the channel number
  if(adc_channel_num > 7)
  {
    // set the channel selection bits, but remove the most significant bit (bit 3)
    adc_channel_num -= 8;
    // set MUX bit 5
    *my_ADCSRB |= 0b00001000;
  }
  // set the channel selection bits
  *my_ADMUX  += adc_channel_num;
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register
  return *my_ADC_DATA;
}

// in the motor lecture, this code is inside the loop function
// LK - I am thinking we can use a simple modification of this for the vent
void rotateCode(){
  // Rotate CW slowly at 5 RPM
  myStepper.setSpeed(5);
  myStepper.step(stepsPerRevolution);
  delay(1000);
  // Rotate CCW quickly at 10 RPM
  myStepper.setSpeed(10);
  myStepper.step(-stepsPerRevolution);
  delay(1000);
}

