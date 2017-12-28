#include "Arduino.h"
#include "cy_wunderg.h"

// Routines to post Data to Weather underground

cy_wunderg::cy_wunderg(const char *iv_host, const char *iv_sid, const char *iv_pwd){
	_gv_host = iv_host;
		_gv_sid = iv_sid;
		_gv_pwd = iv_pwd;
}

void cy_wunderg::send_val( char *iv_field, float iv_value ){
  String url;

  DebugPrint("connecting to ");
  DebugPrintln(_gv_host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(_gv_host, httpPort)) {
    DebugPrintln("connection failed");
    return;
  }

  // We now create a URI for the request
  url = "/weatherstation/updateweatherstation.php?ID=";
  url += _gv_sid;
  url += "&PASSWORD=";
  url += _gv_pwd;
  url += "&dateutc=now&";
  url += iv_field;
  url += "=";
  url += iv_value;
  url += "&action=updateraw";


  DebugPrint("Requesting URL: ");
  DebugPrintln(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + _gv_host + "\r\n" +
               //"Authorization: Basic " + wunderg_auth + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    DebugPrint(line);
  }

  DebugPrintln();
  DebugPrintln("closing connection");
	
}


void cy_wunderg::send_temp_f( float iv_tempf ){
	send_val( "tempf", iv_tempf);
}

void cy_wunderg::send_temp_c( float iv_tempc ){
	  DebugPrintln(iv_tempc);
	float lv_tempf = ( iv_tempc * 1.8 ) + 32;
		  DebugPrintln(lv_tempf);
	send_temp_f( lv_tempf );
}

void cy_wunderg::send_hum( float iv_hum){
		send_val( "humidity", iv_hum);
}
