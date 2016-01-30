#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager
//
#include <IRremoteESP8266.h>      // https://github.com/markszabo/IRremoteESP8266
//
#include <ESP8266mDNS.h>
//
//#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson

// pin  2 is D4 on nodemcu
// pin  5 is D1 on nodemcu
// pin 12 is D6 on nodemcu
// pin 13 is D7 on nodemcu
// pin 14 is D5 on nodemcu
// pin 15 is D8 on nodemcu
// pin 16 is D0 on nodemcu

int RESET_AP_SETTING_PIN = 5;
int LED_RED  = 15;
int LED_GRN  = 13;
int RECV_PIN = 14;

IRrecv irrecv(RECV_PIN);
IRsend irsend(12);
decode_results  results;        // Somewhere to store the results

unsigned int irBufSize = 300;
unsigned int irBuf[300];

String jsonStr;

//String myName = "esp8266";
String myName = "irweb";

ESP8266WebServer webserver ( 80 );

byte mac[6];
char szMac[18];

void blink()
{
  for (int i = 0 ; i < 2; i++) {
    digitalWrite(LED_GRN, LOW);
    delay(60);
    digitalWrite(LED_GRN, HIGH);
    delay(60);
  }
}

void handleRoot() {
  blink();
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf ( temp, 400,

             "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
    <br>\
    <pre></pre>\
  </body>\
</html>",

             hr, min % 60, sec % 60
           );
  webserver.send ( 200, "text/html", temp );
  //  digitalWrite ( led, 0 );
}

void handleMessages() {
  Serial.print("handleMessage : begin  at ");
  Serial.println(millis());
  blink();
  if ( webserver.method() == HTTP_GET ) {
    webserver.send ( 200, "application/json", jsonStr );
    jsonStr = String("");
  }
  else if ( webserver.method() == HTTP_POST ) {
    Serial.println("POST recieved");
    for (int i = 0; i < webserver.args(); i++) {
      String s = String(webserver.argName(i));
      s += String(" -> ");
      s += String(webserver.arg(i));
      if (String(webserver.argName(i)) == String("plain")) {
        jsonStr = String(webserver.arg(i));
      }
    }
    Serial.println(jsonStr);

    unsigned int fromIdx  = jsonStr.indexOf("\"data\":", 0) + 7;
    unsigned int fromIdx2 = jsonStr.indexOf("[", fromIdx) + 1;
    unsigned int toIdx    = jsonStr.indexOf("]", fromIdx2);
    String sigStr = jsonStr.substring(fromIdx2, toIdx);
    Serial.print("data : ");
    Serial.println(sigStr);

    fromIdx = 0;
    toIdx = 0;
    unsigned int sigIdx = 0;
    while (sigStr.length() > 0) {
      toIdx = fromIdx;
      while (toIdx < sigStr.length() && sigStr.charAt(toIdx) != ',') {
        toIdx ++;
      }
      String buf = sigStr.substring(fromIdx, toIdx);
      buf.trim();
      irBuf[sigIdx] = buf.toInt();
      sigStr = sigStr.substring(toIdx+1, sigStr.length());
      sigIdx++;
    }
    irBuf[sigIdx] = sigStr.substring(fromIdx, sigStr.length()).toInt();
    irsend.sendRaw(irBuf, sigIdx, 38);
    webserver.send ( 200, "text/plain", "" );
  }
  Serial.print("handleMessage : finish at ");
  Serial.println(millis());

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Serial.begin(230400);

  pinMode(RESET_AP_SETTING_PIN, INPUT_PULLUP);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GRN, OUTPUT);

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GRN, HIGH);

  WiFi.macAddress(mac);
  //sprintf(szMac, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  //sprintf(szMac, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  sprintf(szMac, "%02x%02x", mac[4], mac[5]);
  myName += String("_");
  myName += String(szMac);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  Serial.print("GPIO 16 is ");
  Serial.print(digitalRead(RESET_AP_SETTING_PIN));
  //reset settings
  // 電源投入時に GPIO5 がLOWならばWIFIの設定を削除する
  if ( digitalRead(RESET_AP_SETTING_PIN) == 0 ) {
    wifiManager.resetSettings();
  }

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "esp8266-" followed by MAC Address
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect(myName.c_str())) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GRN, LOW);

    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GRN, HIGH);

  irrecv.enableIRIn();  // Start the receiver
  irsend.begin();
  if ( MDNS.begin ( myName.c_str() , WiFi.localIP(), 10) ) {
    
    Serial.println ( "MDNS responder started" );
    Serial.print ( "  My name is " );
    Serial.println ( myName );
  }
  // Add service to MDNS-SD
  // ex. MDNS.addService("http", "tcp", 80);
  // OSXからは dns-sd -B _irweb._tcp として探せるようになる
  MDNS.addService("irweb", "tcp", 80);
  
  webserver.on ( "/", handleRoot );
  webserver.on ( "/messages", handleMessages );
  webserver.begin();
  Serial.println ( "HTTP server started" );
}

//+=============================================================================
// Dump out the decode_results structure.
//
String  dumpJson (decode_results *results)
{
  String jsonStr = "";
  if ((results->rawbuf[0] * USECPERTICK) < 1000) {
    return jsonStr;
  }

  jsonStr = "{\"format\":\"raw\",\"freq\":38,\"len\":";
  jsonStr += String(results->rawlen - 1);
  jsonStr += ",\"data\":[";

  // Dump data
  for (int i = 0; i < irBufSize; i++) {
    irBuf[i] = 0;
  }
  for (int i = 1;  i < results->rawlen;  i++) {
    unsigned long  x = results->rawbuf[i] * USECPERTICK;
    irBuf[i - 1] = x;
    jsonStr += String(x);
    if ( i < results->rawlen - 1 )
      jsonStr += (","); // ',' not needed on last one
  }
  jsonStr += "]}";

  //Serial.println(json);
  return jsonStr;
}

void  sendRawCode (decode_results *results)
{
  unsigned int irlen = results->rawlen - 1;
  for (int i = 1;  i < results->rawlen;  i++) {
    unsigned long  x = results->rawbuf[i] * USECPERTICK;
    irBuf[i - 1] = x;
  }
  irsend.sendRaw(irBuf, irlen, 38);
  Serial.println("");
  blink();
}

void loop() {
  webserver.handleClient();

  if (irrecv.decode(&results)) {  // Grab an IR code
    String tmpStr = dumpJson(&results);
    // if recieved IR is a remote signal, first signal will last atleast 2400 uSec
    // assuming the signal is one of NEC, AEHA, SONY
    // http://elm-chan.org/docs/ir_format.html
    if (irBuf[0] > 2000) {
      jsonStr = tmpStr;
      Serial.println(jsonStr);
      blink();
      Serial.println("-----");
    }
    irrecv.resume();              // Prepare for the next value
  }

}
