#ifndef config_h
#define config_h
#include "Arduino.h"

// WARNING ON FIRST USE OR WHEN UPDATING LIBRARIES!!!!!!!!
// for PubSubClient Library
// default MQTT_MAX_PACKET_SIZE is 128 which is too small for large JSON messages from HA
// this may not work due to a bug. In that case, manually define in PubSubClient.h.
#define MQTT_MAX_PACKET_SIZE 512 

/************************* WiFi Access Point *********************************/

#define CONFIG_WIFI_SSID        "darknet"
#define CONFIG_WIFI_PASS        "*4fJ6ZX6YA@&D&"
// give your ESP8266 a friendly hostname so it is easy to identify when 
//uploading firmware (ArduinoOTA)
#define CONFIG_DEVICE_ID    "EDEN1"                                      //<-------------------------------
#define CONFIG_OTA_PORT         8999
#define CONFIG_DEVICE_TOPIC    "EDEN1/" 

/*********************** Private MQTT Server ********************************/

#define CONFIG_MQTT_HOST        "192.168.0.4"
#define CONFIG_MQTT_PORT        1883  //not secure
#define CONFIG_MQTT_USER        "garrett"
#define CONFIG_MQTT_PASS        "xxbGxQuGYZCkBN?3ccCG$"
#define CONFIG_MQTT_QOS         1  // not implemented yet

#define CONFIG_MQTT_TOPIC_PREFIX       "clb/urban_eden/"   
#define CONFIG_MQTT_TOPIC_SUB          "/set"   
#define CONFIG_MQTT_TOPIC_PUB          "/stat"                                        

#define CONFIG_MQTT_PAYLOAD_ON  "ON"
#define CONFIG_MQTT_PAYLOAD_OFF "OFF"

#define CONFIG_MAX_TOPIC_LENGTH  10

/*********************** Light Configuration ********************************/
#define CONFIG_MQTT_TYPE1        "light/"    

// LED Type naming convention
// Ar = Array (multiple addressible types)
// FS = Full spectrum (white light)
// OS = Optimized Spectrum (Red+Blue)
// UV = Ultra-violet
// RB = Royal Blue
// Gr = Green
// Rd = Red
// FR = Far Red
// NEO = RGBW Neopixels
// Define the topics names for the different lights in your system
#define CONFIG_N_LIGHTS  6
const char CONFIG_LIGHT_MQTT_TOPICS[CONFIG_N_LIGHTS][CONFIG_MAX_TOPIC_LENGTH] = {"OS1", "OS2", "OS3", "OS4", "OS5","NEO"};

// list pin numbers in order based upon topics defined above
const uint16_t CONFIG_LIGHT_PINS[] = {1,1,1,1,1,14};
const uint16_t CONFIG_LIGHT_PIXELS[] = {1,1,1,1,1,60};


/*********************** Switch Configuration ********************************/
#define CONFIG_MQTT_TYPE2        "switch/"    

// switch naming convention
// S = sump or water pump
// A = air pump
// V = valve/solenoid
// GF = grow area fan
// LF = light heatsink fan
// H = heater
#define CONFIG_N_SWITCHES   8
const char CONFIG_SWITCH_MQTT_TOPICS[CONFIG_N_SWITCHES][CONFIG_MAX_TOPIC_LENGTH] = {"RLY8", "RLY7", "RLY6", "RLY5", "RLY4", "RLY3", "RLY2", "RLY1"};

// list pin numbers in order based upon topics defined above
// ensure that power is disconnected from relay board when manually uploading sketch
const uint16_t CONFIG_SWITCH_PINS[] = {15,2,4,16,17,5,18,19,13,12,14,27};



/*********************** Sensor Configuration ********************************/
#define CONFIG_MQTT_TYPE3        "sensor/"    

#define CONFIG_N_SENSORS   0
const char CONFIG_SENSOR_MQTT_TOPICS[CONFIG_N_SENSORS][CONFIG_MAX_TOPIC_LENGTH] = {};

const uint16_t CONFIG_SENSOR_PINS[] = {};

/************************** I2C with Arduino *************************************/
#define I2C_DEVICE_ID       8   // device ID of Arduino to send light commands

#define CONFIG_I2C_SDA_PIN            21
#define CONFIG_I2C_SCL_PIN            22
#define CONFIG_I2C_CLOCK_FREQ  100000  //clockFrequency: the value (in Hertz) of desired communication clock. Accepted values are 100000 (standard mode) and 400000 (fast mode). Some processors also support 10000 (low speed mode), 1000000 (fast mode plus) and 3400000 (high speed mode). Please refer to the specific processor documentation to make sure the desired mode is supported. 

/********************** Other Global Variables ******************************/
// turn debugging on or off (turn on or off serial communication)
#define CONFIG_DEBUG  1


#endif
