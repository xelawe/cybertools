#ifndef MQTT_TOOL_H
#define MQTT_TOOL_H

#include <PubSubClient.h>
#include <LinkedList.h>
#include "cy_mqtt_cfg.h"
#include <cy_serial.h>
 
const char* mqtt_clientname;
long lastReconnectAttempt = 0;
boolean gv_mqtt_conn_ok = true;
boolean gv_reconnect_fail = false;

WiFiClient espClient;
PubSubClient client(espClient);


const char MQTT_TOKEN_PREFIX[] PROGMEM = "%prefix%";  // To be substituted by mqtt_prefix[x]
const char MQTT_TOKEN_TOPIC[] PROGMEM = "%topic%";    // To be substituted by mqtt_topic, mqtt_grptopic, mqtt_buttontopic, mqtt_switchtopic

const uint16_t MQTT_TOPSZ = 151;                 // Max number of characters in topic string
const uint16_t MQTT_CMDSZ = 24;                  // Max number of characters in command

char* mqtt_GetTopic_P(char *stopic, uint32_t prefix, char *topic, const char* subtopic)
{
  /* prefix 0 = Cmnd
     prefix 1 = Stat
     prefix 2 = Tele
     prefix 4 = Cmnd fallback
     prefix 5 = Stat fallback
     prefix 6 = Tele fallback
     prefix 8 = Cmnd topic
     prefix 9 = Stat topic
     prefix 10 = Tele topic
  */
  char romram[MQTT_CMDSZ];
  String fulltopic;

  snprintf_P(romram, sizeof(romram), subtopic);

  fulltopic = F("%topic%/%prefix%/"); //SettingsText(SET_MQTT_FULLTOPIC);

  switch (prefix) {
    case 0:
      fulltopic.replace(FPSTR(MQTT_TOKEN_PREFIX), F("cmnd") );//SettingsText(SET_MQTTPREFIX1 + prefix));
      break;
    case 1:
      fulltopic.replace(FPSTR(MQTT_TOKEN_PREFIX), F("stat") );//SettingsText(SET_MQTTPREFIX1 + prefix));
      break;
    case 2:
      fulltopic.replace(FPSTR(MQTT_TOKEN_PREFIX), F("tele") );//SettingsText(SET_MQTTPREFIX1 + prefix));
      break;
  }

  fulltopic.replace(FPSTR(MQTT_TOKEN_TOPIC), topic);

  fulltopic.replace(F("#"), "");
  fulltopic.replace(F("//"), "/");

  if (!fulltopic.endsWith("/")) {
    fulltopic += "/";
  }

  snprintf_P(stopic, MQTT_TOPSZ, PSTR("%s%s"), fulltopic.c_str(), romram);
  return stopic;

}


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
  DebugPrint(F("Message arrived ["));
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
      DebugPrint(F("start Callback "));
      DebugPrintln(i);
      lv_SubTopic->callback(topic, payload, length);
    }
    else {
      DebugPrintln(F("no Callback "));
    }
  }
}

boolean reconnect_mqtt() {
  String lv_lwt;
  char lv_lwtc[40];

  DebugPrint(F("Attempting MQTT connection..."));
  // Attempt to connect
  lv_lwt = mqtt_clientname;
  lv_lwt += "/LWT";
  lv_lwt.toCharArray(lv_lwtc, sizeof(lv_lwtc));

  if (client.connect(mqtt_clientname, mqtt_user, mqtt_pass, lv_lwtc, 0, true, "offline")) {
    DebugPrintln(F("connected"));
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
    gv_reconnect_fail = false;
	  // Last connection attempt more than 5 secs in past?  
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect_mqtt()){
		    // if connection lost later: try immediately to connect
        lastReconnectAttempt = 0;
      } else {
        gv_reconnect_fail = true;
      }
    }
    return client.connected();
  } else {
    return true;
  }
}


void check_mqtt() {
  if (check_mqtt_conn()) {
	  gv_mqtt_conn_ok = true;
	  //DebugPrintln("MQTT loop");
    client.loop();
  } else {
	  gv_mqtt_conn_ok = false;
  }
}


void check_mqtt_reset() {
  check_mqtt();
  // connsection lost an reconnect after 5 seconds failed?
	 if ( !gv_mqtt_conn_ok && gv_reconnect_fail ){
     // restart
     ESP.restart();
     delay(2000);
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
