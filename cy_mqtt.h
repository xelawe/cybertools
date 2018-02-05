#ifndef MQTT_TOOL_H
#define MQTT_TOOL_H

#include <PubSubClient.h>
#include <LinkedList.h>
#include "cy_mqtt_cfg.h"
#include <cy_serial.h>

const char* mqtt_clientname;
long lastReconnectAttempt = 0;

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

boolean reconnect_mqtt() {
  String lv_lwt;
  char lv_lwtc[20];

  DebugPrint("Attempting MQTT connection...");
  // Attempt to connect
  lv_lwt = mqtt_clientname;
  lv_lwt += "/LWT";
  lv_lwt.toCharArray(lv_lwtc, sizeof(lv_lwtc));

  if (client.connect(mqtt_clientname, mqtt_user, mqtt_pass, lv_lwtc, 0, true, "offline")) {
    DebugPrintln("connected");
    // Once connected, publish an announcement, retained
    client.publish(lv_lwtc, "online", true);

    MQTTSubTopic *lv_SubTopic;
    for (int i = 0; i < gv_SubTopicList.size(); i++) {

      // ... and resubscribe
      lv_SubTopic = gv_SubTopicList.get(i);
      client.subscribe(lv_SubTopic->topic);
	    DebugPrint("Subscribed: ");
    DebugPrintln(lv_SubTopic->topic);	
    }
    return true;
  } else {
    DebugPrint("failed, rc=");
    DebugPrintln(client.state());
    return false;
  }
}

boolean check_mqtt_conn() {
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect_mqtt())
        lastReconnectAttempt = 0;
    }
    return client.connected();
  } else {
    return true;
  }
}


void check_mqtt() {
  if (check_mqtt_conn()) {
	  //DebugPrintln("MQTT loop");
    client.loop();
  }
}

void init_mqtt(const char* iv_clientname) {
  DebugPrintln(F("init MQTT ...."));
  mqtt_clientname = iv_clientname;

  client.setServer(mqtt_server, 1883);
  client.setCallback(mqtt_callback);
  //check_mqtt_conn();
}

#endif
