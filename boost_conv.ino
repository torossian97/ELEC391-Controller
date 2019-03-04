#include <Wire.h>
#include <Stepper.h>

float voltage = 0;
float freq = 100;
float duty = 0.5;
const int ledPin = 13;
volatile byte state = LOW;

void setup() { 
  
  //myStepper.setSpeed(speed_val);  
  Serial.begin(9600);

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

} 
void loop() {  
  
  //dutyValue = analogRead(A0);
 //dutyValue = dutyValue * (5.0 / 1023.0);
  // print out the value you read:
 /* if(duty >0){
    duty -= 0.1;
    }else{duty = 1;}
  
  OCR2B = freq*duty;*/
  /*if(freq > 20){
    freq -= 10;
    }else{freq = 255;}

  OCR2A = freq;
  OCR2B = freq*duty;
  */
  //delay(3000);
}

ISR(TIMER1_OVF_vect){
   state = !state;
   digitalWrite(ledPin, state);
   
  voltage = analogRead(A5) * (5.0 / 1023.0);
  
  if(!(2.09 <= voltage && voltage <= 2.15)){
      if(voltage < 2.12){
          duty += 0.01;
        }else{
            duty -= 0.01;
        }
        OCR2B = freq*duty;
    }
   
   Serial.println(voltage);
}


