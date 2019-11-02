#include "Dimmer.h"
#include <Wire.h> // for I2C communication to receive commands

#define OUTPUT_PIN  3 // PWM pin for dimmer (see docs for available pins)
#define ZERO_CROSS  2 // zero-cross input pin (see docs for available pins)

#define I2C_ID     8
#define I2C_CLOCK_FREQ 100000

#define MINN       0 // minimum power setting for LEDs to light
#define MAXX       90 // maximum power setting for LEDs (not to exceed 100)

Dimmer lights[] = {
  Dimmer(3,DIMMER_RAMP),
  Dimmer(4,DIMMER_RAMP),
  Dimmer(5,DIMMER_RAMP),
  Dimmer(6,DIMMER_RAMP),
  Dimmer(7,DIMMER_RAMP)
//  Dimmer(8,DIMMER_RAMP)
//  Dimmer(9,DIMMER_RAMP)
//  Dimmer(10,DIMMER_RAMP)
//  Dimmer(11,DIMMER_RAMP)
//  Dimmer(12,DIMMER_RAMP)
//  Dimmer(13,DIMMER_RAMP)
};
const int NUM_LIGHTS = sizeof(lights) / sizeof(Dimmer);

//const uint8_t gamma[] = 
//{
//  
//}

void setup() {
  Serial.begin(115200);

  Serial.println("Starting up");
  Serial.print("There are ");
  Serial.print(NUM_LIGHTS);
  Serial.println(" light dimmers being configured");
  
  Wire.setClock(I2C_CLOCK_FREQ);
  Wire.begin(I2C_ID); // setup I2C
  Wire.onReceive(receiveEvent); // register event

  for (int i=0;i<NUM_LIGHTS;i++) {
    lights[i].begin();
    lights[i].setMinimum(MINN);
    lights[i].set(0);
  }
}



void receiveEvent(int num_bytes) {
  Serial.println("Receiving message");  

  int light = Wire.read();
  Serial.println(light);    
  bool onoff = Wire.read(); // receive byte
  Serial.println(onoff);    
  int power = Wire.read();    // receive byte
  Serial.println(power);    
  int time_b = Wire.read();   // receive byte
  Serial.println(time_b);   

  if (power < 0) power = 0;
  if (power > MAXX) power = MAXX;
  float time_s = (float) time_b/10;
  if (time_s > 20) time_s = 20;
  if (time_s < 1) time_s = 1;

  lights[light].setRampTime(time_s);
  
  if (!onoff) lights[light].set(0);
  else {
    lights[light].set(power);
  }
}



void loop() {

}
