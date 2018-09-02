/* WifiMan */

#include <ESP8266WiFi.h>
//needed for Wifi Manager library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

String clientName;
char *gv_clientname;


String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    if (mac[i] < 0x10 ) {
      result += '0';
    }
    result += String(mac[i], 16);
    //if (i < 5)
    //  result += ':';
  }
  return result;
}

void get_clientname(const char *iv_hostname) {
  // Generate hostname based on MAC address

  clientName += iv_hostname;
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);

  gv_clientname = (char*) clientName.c_str();
}




void WifimanAPcb (WiFiManager *myWiFiManager) {
  DebugPrintln( "Wifi config mode");
  DebugPrintln( myWiFiManager->getConfigPortalSSID());
}


void wifi_init(const char *iv_APname) {
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
#ifdef serdebug
#else
  wifiManager.setDebugOutput(false);
#endif

  get_clientname( iv_APname );
  DebugPrint("clientname: ");
  DebugPrintln(gv_clientname);
	
  WiFi.hostname(iv_APname);
	
  //reset saved settings
  //wifiManager.resetSettings();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180);

  DebugPrintln("WiFi: wait to connect");

  //fetches ssid and pass and tries to connect
  // wifiManager.setAPCallback(WifimanAPcb);
  
  //if it does not connect it starts an access point
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect(iv_APname)) {
    DebugPrintln("failed to connect and hit timeout");
    delay(1000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  // Force to station mode because if device was switched off while in access point mode it will start up next time in access point mode.
  // see also https://github.com/tzapu/WiFiManager/issues/229#issuecomment-253360552
   WiFi.mode(WIFI_STA); 

  //if you get here you have connected to the WiFi
  DebugPrintln("connected to " + WiFi.SSID() + " ...yeey");

}

void check_wifi_conn() {
  while (WiFi.status() != WL_CONNECTED) {
    DebugPrintln("Wifi not connected -> resetting!");
    delay(3000);
    //reset and try again
    ESP.reset();
    delay(5000);
  }
}

