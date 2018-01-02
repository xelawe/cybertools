#ifndef cy_serial_h
#define cy_serial_h

#include "Arduino.h"

#ifdef serdebug
#define DebugPrint(...) {  Serial.print(__VA_ARGS__); }
#define DebugPrintln(...) {  Serial.println(__VA_ARGS__); }
#else
#define DebugPrint(...) { }
#define DebugPrintln(...) { }
#endif

class cy_serial{
	public:
      static void init(char *iv_file);
};



#endif