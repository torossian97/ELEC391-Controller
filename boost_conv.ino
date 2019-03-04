#include <Automaton.h>
#include <Stepper.h>

double stepsPerRevolution = 2048;
int speed_val = 10;
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);  // Pin inversion to make the library work
// Toggle a blinking led (pin 5) with a button (pin 2)

const int VOLT_CENTER = 2.12;
const int VOLT_LOW = 2.09;
const int VOLT_HIGH = 2.15;

Atm_analog wind_sensor;

void setup() {

  ///////////////////////////////////////PWM setup//////////////////////////////////////
  pinMode(3, OUTPUT); // output pin for OCR2B
  pinMode(5, OUTPUT); // output pin for OCR0B

  pinMode(ledPin, OUTPUT);
  //SETUP TIMER1 for interrupt to control Boost Converter
  TIMSK1 = (TIMSK1 & B11111110) | 0x01;
  TCCR1B = (TCCR1B & B11111000) | 0x05;

  // Set up the 250KHz output
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20);
  OCR2A = freq;
  OCR2B = freq*duty;

  ////////////////////////////////////Stepper setup////////////////////////////////////
  myStepper.setSpeed(0); 

  ////////////////////////////////Automaton callbacks//////////////////////////////////
  wind_sensor.begin( A0, 50 )
    .onChange( wind_callback, 3 ); // Toggle the led when button pressed

}

void loop() {
  automaton.run();
}

void wind_callback(int index, int v, int up){
  voltage = v * (5.0 / 1023.0);

  if(!(VOLT_LOW <= voltage && voltage <= VOLT_HIGH)){
      if(voltage < VOLT_CENTER){
          duty += 0.01;
        }else{
            duty -= 0.01;
        }
        OCR2B = freq*duty;
    }
}


