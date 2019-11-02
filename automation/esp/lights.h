#ifndef lights_h
#define lights_h

#include "Arduino.h"
#include <Wire.h> // for I2C communication to dim grow LEDs
#include <ArduinoJson.h>
//#include <Neomation.h>
#include "config.h"
#include "mqtt_support.h"

#define MAX_EFFECT_LENGTH   30

// For use with LEDC 'PWM' for ESP32 (analogWrite() not available)
// probably need to develop a library to make this easier to use
#define RESOLUTION  8
#define FREQUENCY   5000
#define CHANNEL     0

class Light
{
  public:
    enum TYP_LIGHT
      {
        RLY,
        PWM,
        I2C,
        NEO
      };
      
    uint8_t id;
    PubSubClient mqtt;
//    Neomation neo;
    TYP_LIGHT type;
    uint8_t i2c_id;
    uint8_t pin;
    String pub;
    String sub;
    bool state;
    uint8_t brightness;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
    int color_temp;
    uint8_t transition;
    char effect[MAX_EFFECT_LENGTH];

    Light();
//    Light(uint8_t idd, PubSubClient mqttClient);
    void setRLY(uint8_t rly_pin);
    void setPWM(uint8_t pwm_pin);
    void setI2C(uint8_t receiver);
    void setNEO(uint8_t neo_pin, uint16_t num_leds);
    void turnOn();
    void turnOff();
    void mqttPublishState();
    void update();


  private:
    bool _state;
    uint8_t _brightness;
    uint8_t _red;
    uint8_t _green;
    uint8_t _blue;
    uint8_t _white;
    int _color_temp;
    uint8_t _transition;
    char _effect[MAX_EFFECT_LENGTH];

    uint16_t _num_leds;

    const uint8_t PROGMEM gamma[256] = {
        0,  0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  4,  5,  5,  5,  6,
        6,  7,  7,  7,  8,  8,  9,  9,  9, 10, 10, 11, 11, 11, 12, 12,
       13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18,
       19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 24, 25,
       25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 31, 31,
       31, 32, 32, 33, 33, 33, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37,
       38, 38, 38, 39, 39, 40, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44,
       44, 44, 45, 45, 45, 46, 46, 47, 47, 47, 48, 48, 49, 49, 49, 50,
       50, 51, 51, 51, 52, 52, 53, 53, 53, 54, 54, 55, 55, 55, 56, 56,
       56, 57, 57, 58, 58, 58, 59, 59, 60, 60, 60, 61, 61, 62, 62, 62,
       63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 67, 67, 67, 68, 68, 69,
       69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 73, 74, 74, 75, 75,
       75, 76, 76, 76, 77, 77, 78, 78, 78, 79, 79, 80, 80, 80, 81, 81,
       82, 82, 82, 83, 83, 84, 84, 84, 85, 85, 85, 86, 86, 87, 87, 87,
       88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 93, 93, 93, 94,
       94, 95, 95, 95, 96, 96, 96, 97, 97, 98, 98, 98, 99, 99,100,100 };

    
};

#endif
