#ifndef mqtt_support_h
#define mqtt_support_h

#include "config.h"
/* MQTT Basics
 *  http://www.steves-internet-guide.com/mqtt-publish-subscribe/
 *  http://www.steves-internet-guide.com/understanding-mqtt-topics/
 */  

// http://pubsubclient.knolleary.net/
#include <PubSubClient.h>

#define MIN_MESSZ  512  // mqtt packet size


/**************** Function Prototypes *******************/
void reconnect(PubSubClient &mqttClient, char* DEVICE_ID, const char* user, const char* password, String subscriptions[], int num_subs);
String getFullTopic(String prefix, String device, String type, String topic, String suffix);

#endif
