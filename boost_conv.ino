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

  myStepper.setSpeed(0); 

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


