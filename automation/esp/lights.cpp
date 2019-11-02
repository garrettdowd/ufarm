#include "lights.h"

Light::Light()
{
  id = 0;
  type = RLY;
  i2c_id = 0;
  pin = 0;
  pub = "";
  sub = "";
  // set external state
  // external state is changed in main(), internal functions will determine how to adjust internal state
  state = 0;
  brightness = 255;
  red = 0;
  green = 0;
  blue = 0;
  white = 255;
  color_temp = 327;
  transition = 1000;
  strcpy(effect,"none");
  
  // set internal state
  // internal state is what is written to a light/device
  _state = 0;
  _brightness = 255;
  _red = 0;
  _green = 0;
  _blue = 0;
  _white = 255;
  _color_temp = 327;
  _transition = 1000;
  strcpy(_effect,"none");
}
//
//Light::Light(uint8_t idd, PubSubClient mqttClient)
//{
//  id = idd;
//  mqtt = mqttClient;
//  type = RLY;
//  i2c_id = 0;
//  pin = 0;
//  pub = "";
//  sub = "";
//  state = 0;
//  brightness = 255;
//  red = 0;
//  green = 0;
//  blue = 0;
//  white = 0;
//  color_temp = 327;
//  transition = 1000;
//  strcpy(effect,"none");
//  
//  _state = 0;
//  _brightness = 255;
//  _red = 0;
//  _green = 0;
//  _blue = 0;
//  _white = 0;
//  _color_temp = 327;
//  _transition = 1000;
//  strcpy(_effect,"none");
//}

void Light::setRLY(uint8_t rly_pin)
{
  type = RLY;
  pin = rly_pin;
  pinMode(pin, OUTPUT);
}

void Light::setPWM(uint8_t pwm_pin)
{
  type = PWM;
  pin = pwm_pin;
  ledcSetup(CHANNEL, FREQUENCY, RESOLUTION);
  ledcAttachPin(pin, CHANNEL);
}

void Light::setI2C(uint8_t receiver)
{
  type = I2C;
  i2c_id = receiver;
}

void Light::setNEO(uint8_t neo_pin, uint16_t num_leds)
{
  type = NEO;
  pin = neo_pin;
  _num_leds = num_leds;

//  neo.setPin(pin);
//  neo.setNum(num_leds);
//  neo.setMaxBrightness(255);
  
}

void Light::turnOn()
{
  Serial.println("Light::turnOn() called");
  if (state == 1) {
    if (type == RLY) {
      digitalWrite(pin, HIGH);
    }
    else if (type == PWM) {
      if (transition == 0) {
        ledcWrite(CHANNEL, brightness);
      }
      else {
        // something here to fade lights
      }
    }
    else if (type == I2C) {
      Serial.print("Sending I2C message to ");
      Serial.print(i2c_id);
      Serial.print(" with message... ");
      Serial.print(id); Serial.print(" ");
      Serial.print(1); Serial.print(" ");
      Serial.print(gamma[brightness]); Serial.print(" ");
      Serial.print(transition);
      
      Wire.beginTransmission(i2c_id); // transmit to device 'i2c_id'
      Wire.write(id);              // sends light id
      Wire.write(1);              // 1==ON
      Wire.write(gamma[brightness]);     // 
      Wire.write(transition);     //
      Wire.endTransmission();    // stop transmitting

      _state = state;
      _brightness = brightness;
      _transition = transition;
    }
    else {
//      Serial.print("Setting neopixel ");
//      neo.setBrightness(brightness);
//      _brightness = brightness;
//      neo.offAllEffects();
//      strcpy(_effect,"none");
//      
//      if (!strcmp(effect, "flash")) {
//        // setFlash(uint32_t color1 = 255000000000, uint32_t = 000000000000, uint16_t led_array[] = {0}, uint16_t speed = 500, uint16_t num = 3, uint16_t length = 0);
//        neo.setFlash(neo.color(red,green,blue,white),000000000000, {0}, transition);
//        strcpy(_effect,effect);
//      }
//      else if (!strcmp(effect, "notify")) {
//        neo.setFlash(neo.color(_red,_green,_blue,_white),neo.color(_red/2,_green/2,_blue/2,_white/2), {0}, 300, 3);
//        strcpy(_effect,effect);
//      }
//      else if (!strcmp(effect, "alarm")) {
//        neo.setFlash(000255000000,000000000255, {0}, 300);
//        strcpy(_effect,effect);
//      }
//      else if (!strcmp(effect, "strobe")) {
//        neo.setFlash(000000255000,000000000000, {0}, 20);
//        strcpy(_effect,effect);
//      }
//      else if (!strcmp(effect, "colorfade")) {
//        neo.setColorFade({0},0,transition);
//        strcpy(_effect,effect);
//      }
//      else if (!strcmp(effect, "rainbow")) {
//        //void setRainbow(uint16_t led_array[] = {0}, uint16_t length = 0, uint16_t rainbow_speed = 10000, uint16_t white_length = 0, uint16_t white_speed = 5000);
//        neo.setRainbow();
//        strcpy(_effect,effect);
//      }
//      else if (!strcmp(effect, "rainbow_white")) {
//        //void setRainbow(uint16_t led_array[] = {0}, uint16_t length = 0, uint16_t rainbow_speed = 10000, uint16_t white_length = 0, uint16_t white_speed = 5000);
//        neo.setRainbow({0},0,10000,_num_leds/10);
//        strcpy(_effect,effect);
//      }
//      else if (!strcmp(effect, "rgb_off")) {
//        // void setStrip(uint32_t color = 000000000000, uint16_t led_array[] = {0}, bool gamma = 0, bool strip_show = 1);
//        // void setFade(uint32_t color = 000000000000, uint16_t led_array[] = {0}, uint16_t length = 1000);
//        neo.setFade(255000000000);
//        _red = 0;
//        _green = 0;
//        _blue = 0;
//        _white = 255;
//      }
//      else if (!(transition == 0)) {
//        neo.setFade(neo.color(red,green,blue,white),{0},transition);
//        _transition = transition;
//        _red = red;
//        _green = green;
//        _blue = blue;
//        _white = white;
//      }
//      else neo.setStrip(neo.color(red,green,blue,white,brightness));
//
//      _state = state;
    }
  }
  else {
    Serial.print("Light.turnOn() with id = ");
    Serial.print(id);
    Serial.println(" is already on. Ignoring request and publishing ON state");
  }
  mqttPublishState();
}


void Light::turnOff()
{
  Serial.println("Light::turnOff() called");
  if (state == 0) {
    if (type == RLY) {
      digitalWrite(pin, LOW);
    }
    else if (type == PWM) {
      if (transition == 0) {
        ledcWrite(CHANNEL, 0);
      }
      else {
        // something here to fade lights
      }
    }
    else if (type == I2C) {
      Serial.print("Sending I2C message to ");
      Serial.print(i2c_id);
      Serial.print(" with message... ");
      Serial.print(id); Serial.print(" ");
      Serial.print(0); Serial.print(" ");
      Serial.print(gamma[brightness]); Serial.print(" ");
      Serial.print(transition);
      
      Wire.beginTransmission(i2c_id); // transmit to device 'i2c'
      Wire.write(id);              // sends light id
      Wire.write(0);              // 0==OFF
      Wire.write(gamma[brightness]);     // 
      Wire.write(transition);     //
      Wire.endTransmission();    // stop transmitting

      _state = state;
      _brightness = brightness;
      _transition = transition;
    }
    else {
//      Serial.print("Setting neopixel ");
//      neo.setBrightness(brightness);
//      _brightness = brightness;
//      neo.offAllEffects();
//      strcpy(_effect,"none");
//
//      if (!(transition == 0)) {
//        neo.setFade(000000000000,{0},transition);
//        _transition = transition;
//        _red = red;
//        _green = green;
//        _blue = blue;
//        _white = white;
//      }
//      else neo.setStrip(000000000000);
//      
//      _state = state;
//      _brightness = brightness;
//      _transition = transition;
    }
  }
  else {
    Serial.print("Light.turnOFF() with id = ");
    Serial.print(id);
    Serial.println(" is already off. Ignoring request and publishing OFF state");
  }
  mqttPublishState();
}

void Light::update()
{
  Serial.print("Updating light.. ");
  Serial.println(id);

  Serial.print("Current light state is ");
  Serial.println(_state);
  Serial.print("New light state is ");
  Serial.println(state);
  if (state != _state) {
    if (state == true) turnOn(); 
    else turnOff();
  }
  else if (state == true) {
    if (brightness != _brightness) turnOn();
    else if (red != _red) turnOn();
    else if (green != _green) turnOn();
    else if (blue != _blue) turnOn();
    else if (white != _white) turnOn();
    else if (color_temp != _color_temp) turnOn();
    else if (!strcmp(_effect,effect)) turnOn();
  }
  

  else mqttPublishState();

//  
}

void Light::mqttPublishState()
{
  const int BUFFER_SIZE = JSON_OBJECT_SIZE(20);
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root["state"] = (_state) ? CONFIG_MQTT_PAYLOAD_ON : CONFIG_MQTT_PAYLOAD_OFF;
  
  root["brightness"] = _brightness;
  
  if (type == NEO) {
    JsonObject& color = root.createNestedObject("color");
    color["r"] = _red;
    color["g"] = _green;
    color["b"] = _blue;
  }

  root["color_temp"] = _color_temp;
  
  root["white_value"] = _white;

  root["transition"] = _transition;

//
//  if (colorfade) {
//    if (transitionTime == CONFIG_COLORFADE_TIME_SLOW * 1000) {
//      root["effect"] = "colorfade_slow";
//    }
//    else {
//      root["effect"] = "colorfade_fast";
//    }
//  }
//  else if (inRainbow) {
//    root["effect"] = "rainbow";
//  }
//  else if (array_state) {
//    root["effect"] = "array";
//  }
//  else if (music) {
//    root["effect"] = "music";
//  }
//  // add other custom effects here
////  else if (effect_name) {
////    root["effect"] = "effect_name";
////  }
//  else {
//    root["effect"] = "none";
//  }

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  if (mqtt.publish(pub.c_str(), buffer, true)) {
    Serial.println("");
    Serial.println("Published Light State ");
    root.printTo(Serial);
    Serial.println("");
  } 
  else {
    Serial.println("Something went wrong. Message possibly too large. Light state was not published ");
  }
  
}
