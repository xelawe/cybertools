#ifndef CY_OTA_TOOL_H
#define CY_OTA_TOOL_H

#include <ArduinoOTA.h>
#include "cy_ota_cfg.h"

void init_ota(const char *iv_hostname) {

  ArduinoOTA.onStart([]() {
    Serial.println("Start OTA");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.setHostname(iv_hostname);
  ArduinoOTA.setPassword(ota_pass); //WA181231
  ArduinoOTA.begin();

}

void check_ota(){
    //ota loop
  ArduinoOTA.handle();
}

#endif