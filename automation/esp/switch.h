#ifndef switch_h
#define switch_h

#include "Arduino.h"
#include <Wire.h> // for I2C communication to dim grow LEDs
#include "config.h"
#include "mqtt_support.h"



class Switch
{
  public:
    enum TYP_SWITCH
      {
        RLY,
        I2C
      };
      
    uint8_t id;
    PubSubClient mqtt;
    TYP_SWITCH type;
    uint8_t i2c_id;
    uint8_t pin;
    bool high_trigger;
    String pub;
    String sub;
    bool state;

    Switch();
    Switch(uint8_t idd, PubSubClient mqttClient);
    void setRLY(uint8_t rly_pin, bool high_trig = 0);
    void setI2C(uint8_t receiver);
    void turnOn();
    void turnOff();
    void mqttPublishState();
    void update();


  private:
    bool _state;
    
};

#endif
