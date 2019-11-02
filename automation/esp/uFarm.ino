/*
 * Main Controller for uFarm
 *
 * Supports:
 * Home Assistant
 * MQTT JSON Lights and Sensors
 * RGBW LEDs (SK6812/WS2812/Neopixel) with Adafruit Library
 * Motion Sensor (AM312)
 * DHT Temperature/Humidity Sensor
 *
 * Copy the included `config-example.h` file to `config.h` and update
 * accordingly for your setup.
 *
 * See XXXXXXXXXXXXXXXX for more information.
 */


/* TO DO
 *  Fix mqtt loop - gets stuck in loop publishing stat (RLY1,RLY2,...) If RLY is turned on then it turns on and off when its stuck in this loop
 *  
 *  Enable the grow system to function autonomously without a wifi/mqtt connection
 *   - make reconnect() non-blocking
 *   - local copy of grow recipies
 *   
 *   Define data structures, both for local storage and communication
 *   Improve class Light to handle lights other than I2C
 *   Add guards and checks for messages/initialization/etc
 *   Make config more automated for setting up different types of lights
 *   Implement I2C handshake to ensure lights are in correct state
 *   Implement MQTT LWT
 *   Add Sensors
 */

/****************############# GLOBAL #############*******************/
#include "Arduino.h"

// https://github.com/bblanchon/ArduinoJson
#include <ArduinoJson.h>

// Set configuration options for LED type, pins, WiFi, and MQTT in the following file:
#include "config.h"
#include "wifi_support.h"
#include "mqtt_support.h"
#include "i2c_support.h"
#include "lights.h"
#include "switch.h"

/**************** Function Prototypes *******************/
void callback(char* topic, byte* payload, unsigned int length);
bool processJson(char* message);
//// end prototypes


/**************** Global Variables *******************/
// define what topics should be subscribed to
const int NUM_SUB_TOPICS = 1;
String subscriptions[NUM_SUB_TOPICS] = {String(CONFIG_MQTT_TOPIC_PREFIX CONFIG_DEVICE_TOPIC "#")}; // wildcard subscription for all sub topics
//const char* MQTT_LWT_TOPIC = CONFIG_MQTT_TOPIC_PREFIX CONFIG_DEVICE_TOPIC "stat"; // last will and testament topic for this device
//MQTT_LWT_TOPIC, MQTT_LWT_QOS, MQTT_LWT_RETAIN, MQTT_LWT_MSG  need to find a good way to implement

// create an array of Lights so that we can easily loop through them
Light lights[CONFIG_N_LIGHTS]; // MUST INITIALIZE with MQTT client!!!
Switch switches[CONFIG_N_SWITCHES]; // MUST INITIALIZE with MQTT client!!!

const int     BUFFER_SIZE       = JSON_OBJECT_SIZE(20);

bool          relay             = 0;

// Globals for sensors
char          *motion           = "OFF";
float         temperature       = 0.0;
float         humidity          = 0.0;
uint32_t      lastDHTtime       = 0;
bool          publish_flag      = 0;


/**************** Instantiate Objects *******************/
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


/****************############# SETUP #############*******************/
void setup() {
  if (CONFIG_DEBUG) {
    Serial.begin(115200);
  }

  setupWifi(CONFIG_WIFI_SSID,CONFIG_WIFI_PASS);
  
  // Setup arduinoOTA for ability to update firmware over wifi on the same LAN
  setupOTA(CONFIG_OTA_PORT,CONFIG_DEVICE_ID);
  
  mqttClient.setServer(CONFIG_MQTT_HOST, CONFIG_MQTT_PORT);
  mqttClient.setCallback(callback);

  Serial.println();
  Serial.println("Here are the topics I am subscribing to... ");
  for (int i=0;i<NUM_SUB_TOPICS;i++) {
    Serial.println(subscriptions[i]);
  }
  Serial.println();

//  reconnect(mqttClient,CONFIG_DEVICE_ID,CONFIG_MQTT_USER,CONFIG_MQTT_PASS,subscriptions,NUM_SUB_TOPICS);
  
  // initialize the components
  // Ned to make this more automated to take into account what type of device it is
  for(int i=0;i<CONFIG_N_LIGHTS;i++) {
    lights[i].id = i;  // give a unique ID
    lights[i].mqtt = mqttClient;  //pass it the mqtt client
    lights[i].sub = getFullTopic(String(CONFIG_MQTT_TOPIC_PREFIX), String(CONFIG_DEVICE_TOPIC), String(CONFIG_MQTT_TYPE1), String(CONFIG_LIGHT_MQTT_TOPICS[i]), String(CONFIG_MQTT_TOPIC_SUB));
    lights[i].pub = getFullTopic(String(CONFIG_MQTT_TOPIC_PREFIX), String(CONFIG_DEVICE_TOPIC), String(CONFIG_MQTT_TYPE1), String(CONFIG_LIGHT_MQTT_TOPICS[i]), String(CONFIG_MQTT_TOPIC_PUB));
    lights[i].setI2C(I2C_DEVICE_ID); // set light type as I2C
//    lights[i].mqttPublishState();
  }
  lights[5].setNEO(CONFIG_LIGHT_PINS[5],CONFIG_LIGHT_PIXELS[5]);
  
  for(int i=0;i<CONFIG_N_SWITCHES;i++) {
    switches[i].setRLY(CONFIG_SWITCH_PINS[i]);
    switches[i].id = i;  // give a unique ID
    switches[i].mqtt = mqttClient;  //pass it the mqtt client
    switches[i].sub = getFullTopic(String(CONFIG_MQTT_TOPIC_PREFIX), String(CONFIG_DEVICE_TOPIC), String(CONFIG_MQTT_TYPE2), String(CONFIG_SWITCH_MQTT_TOPICS[i]), String(CONFIG_MQTT_TOPIC_SUB));
    switches[i].pub = getFullTopic(String(CONFIG_MQTT_TOPIC_PREFIX), String(CONFIG_DEVICE_TOPIC), String(CONFIG_MQTT_TYPE2), String(CONFIG_SWITCH_MQTT_TOPICS[i]), String(CONFIG_MQTT_TOPIC_PUB));
  }



//  Wire.begin(CONFIG_I2C_SDA_PIN,CONFIG_I2C_SCL_PIN,CONFIG_I2C_CLOCK_FREQ);
  Wire.setClock(CONFIG_I2C_CLOCK_FREQ);
  Wire.begin();
  
}




// switches do not use JSON. The directly publish the strings "ON" and "OFF" to the mqtt channel
  /*
  SAMPLE SWITCH PAYLOAD:
    {
      "state": "ON"
    }
  */
  
  /*
  SAMPLE SENOR PAYLOAD:
    {
      "motion": "ON",            ON/OFF
      "temperature": "77.2",     float
      "humidity": "63.1",        float(0-100)
      "pH": "6.35",              float(0-14)
      "EC": "2.44",              float
      "shroud": "10"             int(0-100)
    }
  */

  /*
  SAMPLE LIGHT PAYLOAD:
    {
      "state": "ON",
      "brightness": 120,
      "color": {
        "r": 255,
        "g": 100,
        "b": 100
      },
      "white_value": 255,
      "flash": 2000,
      "transition": 5, // in seconds for fades and flashes, in ms for animations
      "effect": "colorfade_fast"
    }
  */

  


/****************############# LOOP #############*******************/
void loop() {
  // Check to see if someone is trying to upload new firmware to me
  ArduinoOTA.handle();

  // for all neopixel lights, update their animations
  // need better way of doing this automatically
//  lights[5].neo.update();

  // Make sure we are still connected to the MQTT Server
  if (!mqttClient.connected()) {
    reconnect(mqttClient,CONFIG_DEVICE_ID,CONFIG_MQTT_USER,CONFIG_MQTT_PASS,subscriptions,NUM_SUB_TOPICS);
  }

  /************ SUBSCRIPTIONS ***************/
  
  mqttClient.loop();
  

//  /**************** PUBLISHING *******************/
//  // this is where we check the attached sensors and determine whether to publish the values
//  if (strcmp(motionONOFF,"ON") == 0) {
//    char *motiont;
//    if (digitalRead(CONFIG_MOTION_PIN) == 1) {
//      motiont = "ON";
//    } else {
//      motiont = "OFF";
//    }
//    if (strcmp(motion,motiont) != 0) {
//      if (strcmp(motiont,"ON") == 0) {
//        motion = "ON";
//      } else {
//        motion = "OFF";
//      }
//      publish_flag = 1;
//    }
//  }
//  if (strcmp(envONOFF,"ON") == 0) {
//    // some code to read and publish temperature and humidity data
//    if(millis() - lastDHTtime > CONFIG_DELAY_DHT) {
//      Serial.println("Time to update update temp and humidity readings");
//      // reset lastDHTtime
//      lastDHTtime = millis();
//  
//      sensors_event_t event;  
//      // Get temperature event and print its value.
//      dht.temperature().getEvent(&event);
//      if (isnan(event.temperature)) {
//        Serial.println("Error reading temperature!");
//      }
//      else {
//        Serial.print("Temperature: ");
//        Serial.print(event.temperature);
//        Serial.println(" *C");
//        if (abs(event.temperature - temperature) > CONFIG_TEMP_THRESH) {
//          temperature = event.temperature;
//          publish_flag = 1;
//        }
//        else {
//          Serial.println("Temperature checked. Change does not exceed threshold. Not publishing");
//        }
//      }
//      // Get humidity event and print its value.
//      dht.humidity().getEvent(&event);
//      if (isnan(event.relative_humidity)) {
//        Serial.println("Error reading humidity!");
//      }
//      else {
//        Serial.print("Humidity: ");
//        Serial.print(event.relative_humidity);
//        Serial.println("%");
//        if (abs(event.relative_humidity - humidity) > CONFIG_HUMID_THRESH) {
//          humidity = event.relative_humidity;
//          publish_flag = 1;
//        }
//        else {
//          Serial.println("Humidity checked. Change does not exceed threshold. Not publishing");
//        }
//      }
//    }
//  }
//  if (publish_flag) {
//    sendSensorState();
//    publish_flag = 0;
//  }
  

  


}





/****************############# FUNCTIONS #############*******************/

/********************** MQTT STUFF *************************/
void callback(char* topic, byte* payload, unsigned int length) {  // easier to have callback here where it can access global states
  Serial.print("Hey it's the callback. Received the topic... ");
  Serial.println(topic);
  //**************  Handle Lights  ********************//
  for (int i=0;i<CONFIG_N_LIGHTS;i++) {
    if (!strcmp(topic,lights[i].sub.c_str())) {
      char message[length + 1];
      for (int i = 0; i < length; i++) {
        message[i] = (char)payload[i];
      }
      message[length] = '\0';
      Serial.print("With the message.... ");
      Serial.println(message);

      StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(message);
      if (!root.success()) {
        Serial.println("Processing JSON Failed...");
      }

      if (root.containsKey("state")) {
        Serial.println("Callback setting state");
        if (!strcmp(root["state"], CONFIG_MQTT_PAYLOAD_ON)) {
          lights[i].state = true;
        }
        else if (!strcmp(root["state"], CONFIG_MQTT_PAYLOAD_OFF)) {
          lights[i].state = false;
        }
      }
      if (root.containsKey("brightness")) {
        Serial.println("Callback setting brightness");
        lights[i].brightness = root["brightness"];
      }
      if (root.containsKey("color")) {
        Serial.println("Callback setting RGB");
        lights[i].red = root["color"]["r"];
        lights[i].green = root["color"]["g"];
        lights[i].blue = root["color"]["b"];
      }
      if (root.containsKey("white_value")) {
        Serial.println("Callback setting white");
        lights[i].white = root["white_value"];
      }
      if (root.containsKey("transition")) {
        Serial.println("Callback setting transition");
        lights[i].transition = root["transition"];
      }
      if (root.containsKey("color_temp")) {
        Serial.println("Callback setting color_temp");
        lights[i].color_temp = root["color_temp"];
      }
      if (root.containsKey("effect")) {
        Serial.println("Callback setting effect");
        strcpy(lights[i].effect,root["transition"]);
      }

      lights[i].update();
    }
  }

  //**************  Handle Switches  ********************//
  for (int i=0;i<CONFIG_N_SWITCHES;i++) {
    if (!strcmp(topic,switches[i].sub.c_str())) {
      char message[length + 1];
      for (int i = 0; i < length; i++) {
        message[i] = (char)payload[i];
      }
      message[length] = '\0';
      Serial.print("With the message.... ");
      Serial.println(message);

      if (!strcmp(message,CONFIG_MQTT_PAYLOAD_ON)) switches[i].state = true;
      else if (!strcmp(message,CONFIG_MQTT_PAYLOAD_OFF)) switches[i].state = false;
      else {
        Serial.println("");
        Serial.println("Message is not ON or OFF payload. Cannot set switch state ");
        Serial.println("");
      }
      switches[i].update();
    }
  }
      
}
