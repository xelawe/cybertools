#ifndef cy_serial_h
#define cy_serial_h
/* test cy_serial */

#include "Arduino.h"

#ifdef serdebug
/* serdebug defined h*/
#define DebugPrint(...) {  Serial.print(__VA_ARGS__); }
#define DebugPrintln(...) {  Serial.println(__VA_ARGS__); }
#else
/* serdebug not defined h*/
#define DebugPrint(...) { }
#define DebugPrintln(...) { }
#endif



class cy_serial{
  public:
   // static  void start(HardwareSerial* iv_ser, char *iv_file){
    static  void start(char *iv_file){
		#ifdef serdebug
	/* serdebug defined cpp*/
		Serial.begin(115200);
  //iv_ser->begin(115200);
  Serial.println("\n\n" + String(__DATE__) + ", " + String(__TIME__) + " " + String(iv_file) + "\n");
  //iv_ser->println("\n\n" + String(__DATE__) + ", " + String(__TIME__) + " " + String(iv_file) + "\n");		
	#else
/* serdebug not defined cpp*/
#endif
}

};
#endif