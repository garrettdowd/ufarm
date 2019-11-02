
#include "mqtt_support.h"


// Max message size calculated by PubSubClient is (MQTT_MAX_PACKET_SIZE < 5 + 2 + strlen(topic) + plength)
#if (MQTT_MAX_PACKET_SIZE) < MIN_MESSZ  // If the max message size is too small, throw an error at compile time. See PubSubClient.cpp line 359
  #error "MQTT_MAX_PACKET_SIZE is too small in libraries/PubSubClient/src/PubSubClient.h,"
#endif

/****************############# FUNCTIONS #############*******************/


void reconnect(PubSubClient &mqttClient, char* device_id , const char* user, const char* password, String subscriptions[], int num_subs) {
  // Try connecting
  delay(50);
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (mqttClient.connect(device_id, user, password)) {
    Serial.println("connected");
    for(int i=0;i<num_subs;i++) {
      mqttClient.subscribe(subscriptions[i].c_str());
    }
  } else {
    Serial.print("failed, rc=");
    Serial.print(mqttClient.state());
    Serial.println(" try again next loop");
  }
}

String getFullTopic(String prefix, String device, String type, String topic, String suffix) {
  String full_topic;
  full_topic = prefix + device + type + topic + suffix;
  return full_topic;
}
