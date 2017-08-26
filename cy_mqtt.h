#ifndef MQTT_TOOL_H
#define MQTT_TOOL_H

#include <PubSubClient.h>
#include <LinkedList.h>
#include "cy_mqtt_cfg.h"

const char* mqtt_clientname;

WiFiClient espClient;
PubSubClient client(espClient);

// Class for SubTopics
class MQTTSubTopic {
  public:
    const char *topic;
    MQTT_CALLBACK_SIGNATURE;
};

// List for SubTopics
LinkedList<MQTTSubTopic*> gv_SubTopicList = LinkedList<MQTTSubTopic*>();


// add Subtopic to internal linked list
void add_subtopic(const char* iv_subtopic, MQTT_CALLBACK_SIGNATURE) {
  // Create a SubTopic
  MQTTSubTopic *lv_SubTopic = new MQTTSubTopic();
  // set Values
  lv_SubTopic->topic = iv_subtopic;
  lv_SubTopic->callback = callback;
  // add to list
  gv_SubTopicList.add(lv_SubTopic);

}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  DebugPrint("Message arrived [");
  DebugPrint(topic);
  DebugPrint("] ");
  for (int i = 0; i < length; i++) {
    DebugPrint((char)payload[i]);
  }
  DebugPrintln();

  MQTTSubTopic *lv_SubTopic;
  for (int i = 0; i < gv_SubTopicList.size(); i++) {

    // is topic called?
    lv_SubTopic = gv_SubTopicList.get(i);
    DebugPrintln(lv_SubTopic->topic);
    if (strcmp(lv_SubTopic->topic, topic) == 0) {
      DebugPrint("start Callback ");
      DebugPrintln(i);
      lv_SubTopic->callback(topic, payload, length);
    }
    else {
      DebugPrintln("no Callback ");
    }

  }

}

void reconnect_mqtt() {
  // Loop until we're reconnected
  //while (!client.connected()) {
  DebugPrint("Attempting MQTT connection...");
  // Attempt to connect
  if (client.connect(mqtt_clientname)) {
    DebugPrintln("connected");
    // Once connected, publish an announcement, retained
    //client.publish(mqtt_pubtopic, "hello world");

    MQTTSubTopic *lv_SubTopic;
    for (int i = 0; i < gv_SubTopicList.size(); i++) {

      // ... and resubscribe
      lv_SubTopic = gv_SubTopicList.get(i);
      client.subscribe(lv_SubTopic->topic);
    }
  } else {
    DebugPrint("failed, rc=");
    DebugPrintln(client.state());
  }
}

void init_mqtt(const char* iv_clientname) {
  DebugPrintln(F("init MQTT ...."));
  mqtt_clientname = iv_clientname;

  client.setServer(mqtt_server, 1883);
  client.setCallback(mqtt_callback);
}

void check_mqtt() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();
}

#endif
