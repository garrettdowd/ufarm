#include "Switch.h"

Switch::Switch()
{
  id = 0;
  type = RLY;
  i2c_id = 0;
  pin = 0;
  high_trigger = 0;
  pub = "";
  sub = "";
  // set external state
  // external state is changed in main(), internal functions will determine how to adjust internal state
  state = 0;

  // set internal state
  // internal state is what is written to a Switch/device
  _state = 0;

}

Switch::Switch(uint8_t idd, PubSubClient mqttClient)
{
  id = idd;
  mqtt = mqttClient;
  type = RLY;
  i2c_id = 0;
  pin = 0;
  high_trigger = 0;
  pub = "";
  sub = "";
  state = 0;
  
  _state = 0;
}

void Switch::setRLY(uint8_t rly_pin, bool high_trig)
{
  type = RLY;
  pin = rly_pin;
  high_trigger = high_trig;
  pinMode(pin, OUTPUT);
  if (high_trigger) digitalWrite(pin, LOW);
  else digitalWrite(pin, HIGH);
}


void Switch::setI2C(uint8_t receiver)
{
  type = I2C;
  i2c_id = receiver;
}

void Switch::turnOn()
{
  Serial.println("Switch::turnOn() called");
  if (state == 1) {
    if (type == RLY) {
      if (high_trigger) digitalWrite(pin, HIGH);
      else digitalWrite(pin, LOW);
      
    }
    else if (type == I2C) {
      Serial.print("Sending I2C message to ");
      Serial.print(i2c_id);
      Serial.print(" with message... ");
      Serial.print(id); Serial.print(" ");
      Serial.print(1);
      
      Wire.beginTransmission(i2c_id); // transmit to device 'i2c_id'
      Wire.write(id);              // sends Switch id
      Wire.write(1);              // 1==ON
      Wire.endTransmission();    // stop transmitting
    }

    _state = state;
  }
  else {
    Serial.print("Switch.turnOn() with id = ");
    Serial.print(id);
    Serial.println(" is already on. Ignoring request and publishing ON state");
  }
  mqttPublishState();
}


void Switch::turnOff()
{
  Serial.println("Switch::turnOff() called");
  if (state == 0) {
    if (type == RLY) {
      if (high_trigger) digitalWrite(pin, LOW);
      else digitalWrite(pin, HIGH);
    }
    else if (type == I2C) {
      Serial.print("Sending I2C message to ");
      Serial.print(i2c_id);
      Serial.print(" with message... ");
      Serial.print(id); Serial.print(" ");
      Serial.print(0);
      
      Wire.beginTransmission(i2c_id); // transmit to device 'i2c'
      Wire.write(id);              // sends Switch id
      Wire.write(0);              // 0==OFF
      Wire.endTransmission();    // stop transmitting
    }
  
    _state = state;
  }
  else {
    Serial.print("Switch.turnOFF() with id = ");
    Serial.print(id);
    Serial.println(" is already off. Ignoring request and publishing OFF state");
  }
  mqttPublishState();
}

void Switch::update()
{
  Serial.print("Updating Switch.. ");
  Serial.println(id);

  Serial.print("Current Switch state is ");
  Serial.println(_state);
  Serial.print("New Switch state is ");
  Serial.println(state);
  if (state != _state) {
    if (state == true) turnOn(); 
    else turnOff();
  }
  else mqttPublishState();
}

void Switch::mqttPublishState()
{
  // In Home Assistant, switch states are published directly to the topic. JSON not accepted (unless you make a custom component)
  const char* pub_msg = (_state) ? CONFIG_MQTT_PAYLOAD_ON : CONFIG_MQTT_PAYLOAD_OFF;

  if (mqtt.publish(pub.c_str(), pub_msg, true)) {
    Serial.println("");
    Serial.println("Published Switch State ");
    Serial.println(pub_msg);
    Serial.println("");
  } 
  else {
    Serial.println("Something went wrong. Message possibly too large. Switch state was not published ");
  }
  
}
