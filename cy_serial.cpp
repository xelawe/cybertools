#include <cy_serial.h>

void cy_serial::init(char *iv_file){
#ifdef serdebug
  Serial.begin(115200);
#endif
  DebugPrintln("\n" + String(__DATE__) + ", " + String(__TIME__) + " " + String(iv_file) ); //String(__FILE__));
}