#ifndef cy_wunderg_h
#define cy_wunderg_h
// Routines to post Data to Weather underground


#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define serdebug
#ifdef serdebug
#define DebugPrint(...) {  Serial.print(__VA_ARGS__); }
#define DebugPrintln(...) {  Serial.println(__VA_ARGS__); }
#else
#define DebugPrint(...) { }
#define DebugPrintln(...) { }
#endif

class cy_wunderg
{
	
  public:
    cy_wunderg(const char *iv_host, const char *iv_sid, const char *iv_pwd);
	  void send_val( char *iv_field, float iv_value );
	  void send_temp_f( float iv_tempf );
	  void send_temp_c( float iv_tempc );
    void send_hum( float iv_hum);
    void send_press_in( float iv_press );
    void send_press_hpa( float iv_press );
  private:
    const char *_gv_host;
    const char *_gv_sid;
    const char *_gv_pwd;
};

#endif