/*
Importing the library for non-blocking finite state machine framework
Importing the library for non-blocking stepper motor functions
Antoine Torossian - 26977158
*/
#include <Automaton.h>
#include <AccelStepper.h>

///////////////////////////////////Calibration values///////////////////////////////////
//these values are hardware specific, and are easily accesible to keep the code function, even with the swapping of parts.
const float VOLT_CENTER = 2.48;
const float VOLT_LOW = 2.45;
const float VOLT_HIGH = 2.42;
const int POT_ANGLE = 330;
const float DEG_RATIO = 100;
const float I_SENSOR_RATIO = 5;

//This is the PWM settings, with the duty cycle starting at 50%. Freq 100 is for the timer (~150kHz)
float last_deg = 0;
float freq = 100;
float duty = 0.5;

float highest_power = 0;

////////////////////////////////Stepper initialization///////////////////////////////////
AccelStepper stepper_wind(AccelStepper::FULL4WIRE, 8,10,9,11);

/////////////////////////////////////Analog FSMs////////////////////////////////////////
Atm_analog PWM_control;
Atm_analog wind_control;
uint16_t avgbuffer[16];

void setup() {
  //////////////////////////////////////Serial setup////////////////////////////////////
  Serial.begin(19200);

  ///////////////////////////////////////PWM setup//////////////////////////////////////
  pinMode(3, OUTPUT); // output pin for OCR2B

  // Set up the 150KHz output
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20);
  OCR2A = freq;
  OCR2B = freq*duty;

  ////////////////////////////////////Stepper setup////////////////////////////////////
  stepper_wind.setMaxSpeed(500.0);
  stepper_wind.setAcceleration(500.0);
  stepper_wind.setSpeed(500.0); 

  ////////////////////////////////Automaton callbacks//////////////////////////////////
  PWM_init();
  //smooth out the readings from the voltage sensor and trigger state
  wind_control.begin( A1, 5 ).average( avgbuffer, sizeof( avgbuffer ) ).range(0,32).onChange( wind_callback, 3 ); // Monitor wind vane for change in voltage

}

void loop() {
  Serial.println(analogRead(A1)/32);
  automaton.run();
  stepper_wind.run();
}

//callback function to keep boost converter outputting at 12 Volts
void PWM_callback(int index, int v, int up){
  float voltage = v * (5.0 / 1023.0);

  //Serial.println(duty);

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
  
  //Serial.println(last_deg);
  if(abs(steps) > 4)
    stepper_wind.move(steps);
  last_deg = degree;
}

//callback to monitor if the power is still at a maximum
void power_callback(int index, int v, int up){
  float voltage = v * (5 / 1023.0);
  float current = voltage/I_SENSOR_RATIO;
  voltage = analogRead(A3);
  float power = voltage*current;
  
  if(highest_power <= power + 0.1){
      highest_power = power;
    }else{
      duty += 0.01;
      OCR2B = freq*duty;
    }
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

