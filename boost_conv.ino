#include <Automaton.h>
#include <Stepper.h>

///////////////////////////////////Calibration values///////////////////////////////////
const float VOLT_CENTER = 2.52;
const float VOLT_LOW = 2.48;
const float VOLT_HIGH = 2.57;
const int POT_ANGLE = 330;
const double STEPS_PER_REV = 64;
const float DEG_RATIO = 330*8/STEPS_PER_REV;
const int STEPPER_SPEED = 200;

float last_deg = 0;
float freq = 100;
float duty = 0.5;
Stepper myStepper(STEPS_PER_REV, 8,10,9,11);  // Pin inversion to make the library work

/////////////////////////////////////Analog FSMs////////////////////////////////////////
Atm_analog PWM_control;
Atm_analog wind_control;

void setup() {
  //last_deg = getDegrees(analogRead(A1) * (5 / 1023.0));
  
  //////////////////////////////////////Serial setup////////////////////////////////////
  Serial.begin(19200);

  ///////////////////////////////////////PWM setup//////////////////////////////////////
  pinMode(3, OUTPUT); // output pin for OCR2B
  //pinMode(5, OUTPUT); // output pin for OCR0B

  //SETUP TIMER1 for interrupt to control Boost Converter
  //TIMSK1 = (TIMSK1 & B11111110) | 0x01;
  //TCCR1B = (TCCR1B & B11111000) | 0x05;

  // Set up the 250KHz output
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20);
  OCR2A = freq;
  OCR2B = freq*duty;

  ////////////////////////////////////Stepper setup////////////////////////////////////
  myStepper.setSpeed(STEPPER_SPEED); 

  ////////////////////////////////Automaton callbacks//////////////////////////////////
  //PWM_control.begin(A5,50).onChange(PWM_callback,3); // Call PWM controller when analog pin 5 changes
  PWM_init();

  //Serial.println(voltage);
  
  wind_control.begin( A1, 50 ).range(0,64).onChange( wind_callback, 3 ); // Monitor wind vane for change in voltage

}

void loop() {
  automaton.run();
}

//callback function to keep boost converter outputting at 12 Volts
void PWM_callback(int index, int v, int up){
  float voltage = v * (5.0 / 1023.0);

  Serial.println(duty);

  if(!(VOLT_LOW <= voltage && voltage <= VOLT_HIGH)){
      if(voltage < VOLT_CENTER){
          if (duty < 1) {duty += 0.01;}
          else {duty = 0.5;}
        }else{
            if (duty > 0.0) {duty -= 0.01;}
            else {duty = 0.5;}
        }
        OCR2B = freq*duty;
    }
}

//callback to keep the motor lined up with the wind vane
void wind_callback(int index, int v, int up){
  float voltage = v * (5 / 1023.0);
  
  int degree = getDegrees(voltage);
  int steps = (degree - last_deg) * DEG_RATIO;

  //Serial.println(steps);
  if(abs(steps) > 4)
    myStepper.step(steps);
  last_deg = degree;
}

//quick function to get degrees from a voltage supplied by 3.3 volts
int getDegrees(float v)
{
    float degree = (v * POT_ANGLE) / 3.3; 
    return degree;
}

 void PWM_init() {
      PWM_control.begin(A0, 50).onChange(PWM_callback, 3);
 }

