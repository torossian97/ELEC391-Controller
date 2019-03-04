#include <Automaton.h>
#include <Stepper.h>

Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);  // Pin inversion to make the library work

///////////////////////////////////Calibration values///////////////////////////////////
const float VOLT_CENTER = 2.12;
const float VOLT_LOW = 2.09;
const float VOLT_HIGH = 2.15;
const int POT_ANGLE = 330;
const double STEPS_PER_REV = 2048;
const float DEG_RATIO = 360/STEPS_PER_REV;
const int STEPPER_SPEED = 10;

float last_deg = POT_ANGLE/2;

/////////////////////////////////////Analog FSMs////////////////////////////////////////
Atm_analog PWM_control;
Atm_analog wind_control;

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
  myStepper.setSpeed(STEPPER_SPEED); 

  ////////////////////////////////Automaton callbacks//////////////////////////////////
  PWM_control.begin( A5, 50 )
    .onChange( PWM_callback, 3 ); // Call PWM controller when analog pin 5 changes
  
  wind_control.begin( A0, 50 )
    .onChange( wind_callback, 3 ); // Monitor wind vein for change in voltage

}

void loop() {
  automaton.run();
}

void PWM_callback(int index, float v, int up){
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

void wind_callback(int index, int v, int up){
  float voltage = v * (3.3 / 1023.0);
  
  int degree = getDegrees(voltage);
  int steps = (degree - last_degree) * DEG_RATIO;

  myStepper.step(steps);
  last_degree = degree;
}

int getDegrees(float v)
{
    int sensor_value = v;

    float degree = (voltage * POT_ANGLE) / 3.3; 
    return degree;
}


