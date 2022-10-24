#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <Wire.h>

#include "DisplayHandler.h"

#ifdef M5STACK
  #include <JC_Button.h>
#endif

#include <AutoConnect.h>
#include <ArduinoJson.h>

#include <WebSocketsClient.h>

#define PARAM_FILE "/elements.json"

using WebServerClass = WebServer;
fs::SPIFFSFS &FlashFS = SPIFFS;
#define FORMAT_ON_FAIL true

/////////////////////////////////
///////////CHANGE////////////////
/////////////////////////////////

/* comment out #define M5STACK in DisplayHelpers.h if you don't use the M5Stack */        
bool format = false; // true for formatting SPIFFS, use once, then make false and reflash
int portalPin = 4;

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

// Access point variables
String password;
String serverFull;
String lnbitsServer;
String deviceId;
String highPin;
String timePin;
String lnurl;
String dataId;
String payReq;

// variables
int balance;
int oldBalance;

bool paid;
bool down = false;
bool triggerAp = false; 

String content = "<h1>Base Access-point</br>For easy variable and wifi connection setting</h1>";

// custom access point pages
static const char PAGE_ELEMENTS[] PROGMEM = R"(
{
  "uri": "/config",
  "title": "Access Point Config",
  "menu": true,
  "element": [
    {
      "name": "text",
      "type": "ACText",
      "value": "AP options",
      "style": "font-family:Arial;font-size:16px;font-weight:400;color:#191970;margin-botom:15px;"
    },
    {
      "name": "password",
      "type": "ACInput",
      "label": "Password",
      "value": "ToTheMoon"
    },
    {
      "name": "text",
      "type": "ACText",
      "value": "Project options",
      "style": "font-family:Arial;font-size:16px;font-weight:400;color:#191970;margin-botom:15px;"
    },
    {
      "name": "server",
      "type": "ACInput",
      "label": "LNbits LNURLDevice ws link",
      "value": ""
    },
    {
      "name": "pin",
      "type": "ACInput",
      "label": "Pin to turn on",
      "value": ""
    },
     {
      "name": "lnurl",
      "type": "ACInput",
      "label": "Using LNURL",
      "value": "true"
    },
    {
      "name": "load",
      "type": "ACSubmit",
      "value": "Load",
      "uri": "/config"
    },
    {
      "name": "save",
      "type": "ACSubmit",
      "value": "Save",
      "uri": "/save"
    },
    {
      "name": "adjust_width",
      "type": "ACElement",
      "value": "<script type='text/javascript'>window.onload=function(){var t=document.querySelectorAll('input[]');for(i=0;i<t.length;i++){var e=t[i].getAttribute('placeholder');e&&t[i].setAttribute('size',e.length*.8)}};</script>"
    }
  ]
 }
)";

static const char PAGE_SAVE[] PROGMEM = R"(
{
  "uri": "/save",
  "title": "Elements",
  "menu": false,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "format": "Elements have been saved to %s",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970"
    },
    {
      "name": "validated",
      "type": "ACText",
      "style": "color:red"
    },
    {
      "name": "echo",
      "type": "ACText",
      "style": "font-family:monospace;font-size:small;white-space:pre;"
    },
    {
      "name": "ok",
      "type": "ACSubmit",
      "value": "OK",
      "uri": "/config"
    }
  ]
}
)";

// instances
WebSocketsClient webSocket;
WebServerClass server;
AutoConnect portal(server);
AutoConnectConfig config;
AutoConnectAux elementsAux;
AutoConnectAux saveAux;

// setup

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  initDisplay();
  logoScreen();

  #ifdef M5STACK
    const byte BUTTON_PIN_A = 39;
    Button BTNA(BUTTON_PIN_A);
    BTNA.begin();
    pinMode (2, OUTPUT);
  #endif

  int timer = 0;
  while (timer < 2000)
  {
    digitalWrite(2, LOW);
    #ifdef M5STACK
      if (BTNA.read() == 1){
        Serial.println("Launch portal");
        triggerAp = true;
        timer = 5000;
      }
    #else
      Serial.println(touchRead(portalPin));
      if(touchRead(portalPin) < 60){
        Serial.println("Launch portal");
        triggerAp = true;
        timer = 5000;
      }
    #endif
    digitalWrite(2, HIGH);
    timer = timer + 100;
    delay(300);
  }
    
 // h.begin();
  FlashFS.begin(FORMAT_ON_FAIL);
  SPIFFS.begin(true);
  if(format == true){
    SPIFFS.format(); 
  }
  // get the saved details and store in global variables
  File paramFile = FlashFS.open(PARAM_FILE, "r");
  if (paramFile)
  {
    StaticJsonDocument<2500> doc;
    DeserializationError error = deserializeJson(doc, paramFile.readString());

    const JsonObject maRoot0 = doc[0];
    const char *maRoot0Char = maRoot0["value"];
    password = maRoot0Char;
    
    const JsonObject maRoot1 = doc[1];
    const char *maRoot1Char = maRoot1["value"];
    serverFull = maRoot1Char;
    lnbitsServer = serverFull.substring(5, serverFull.length() - 38);
    deviceId = serverFull.substring(serverFull.length() - 22);

    const JsonObject maRoot2 = doc[2];
    const char *maRoot2Char = maRoot2["value"];
    highPin = maRoot2Char;

    const JsonObject maRoot3 = doc[3];
    const char *maRoot3Char = maRoot3["value"];
    lnurl = maRoot3Char;
  }
  else{
    triggerAp = true;
  }
  paramFile.close();
    server.on("/", []() {
      content += AUTOCONNECT_LINK(COG_24);
      server.send(200, "text/html", content);
    });
    
    elementsAux.load(FPSTR(PAGE_ELEMENTS));
    elementsAux.on([](AutoConnectAux &aux, PageArgument &arg) {
      File param = FlashFS.open(PARAM_FILE, "r");
      if (param)
      {
        aux.loadElement(param, {"password", "server", "pin", "lnurl"});
        param.close();
      }

      if (portal.where() == "/config")
      {
        File param = FlashFS.open(PARAM_FILE, "r");
        if (param)
        {
          aux.loadElement(param, {"password", "server", "pin"});
          param.close();
        }
      }
      return String();
    });
    saveAux.load(FPSTR(PAGE_SAVE));
    saveAux.on([](AutoConnectAux &aux, PageArgument &arg) {
      aux["caption"].value = PARAM_FILE;
      File param = FlashFS.open(PARAM_FILE, "w");
      if (param)
      {
        // save as a loadable set for parameters.
        elementsAux.saveElement(param, {"password", "server", "pin", "lnurl"});
        param.close();
        // read the saved elements again to display.
        param = FlashFS.open(PARAM_FILE, "r");
        aux["echo"].value = param.readString();
        param.close();
      }
      else
      {
        aux["echo"].value = "Filesystem failed to open.";
      }
      return String();
    });
    config.auth = AC_AUTH_BASIC;
    config.authScope = AC_AUTHSCOPE_AUX;
    config.ticker = true;
    config.autoReconnect = true;
    config.apid = "Device-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    config.psk = password;
    config.menuItems = AC_MENUITEM_CONFIGNEW | AC_MENUITEM_OPENSSIDS | AC_MENUITEM_RESET;
    config.reconnectInterval = 1;

    if (triggerAp == true)
    {
      portalLaunched();
      config.immediateStart = true;
      portal.join({elementsAux, saveAux});
      portal.config(config);
      portal.begin();
      while (true)
      {
        portal.handleClient();
      }
      timer = 2000;
    }
    timer = timer + 200;
    delay(200);
  if (lnbitsServer != "")
  {
    portal.join({elementsAux, saveAux});
    config.autoRise = false;
    portal.config(config);
    portal.begin();
  }
  triggerAp = false;
  lnbitsScreen();
  delay(1000);
  pinMode(highPin.toInt(), OUTPUT);
  digitalWrite(highPin.toInt(), HIGH);
  delay(1000);
  digitalWrite(highPin.toInt(), LOW);
  webSocket.beginSSL(lnbitsServer, 443, "/lnurldevice/ws/" + deviceId);
  webSocket.onEvent(webSocketEvent);
}

// loop

void loop() {
  while(WiFi.status() != WL_CONNECTED){
    connectionError();
    Serial.println("Failed to connect");
    delay(500);
  }
  Serial.println(highPin.toInt());

  if(lnurl == "true"){
    qrdisplayScreen();
    paid = false;
    
    while(paid == false){
      webSocket.loop();
    }
    oldBalance = balance;
    Serial.println("Paid");
    paidScreen();
    digitalWrite(highPin.toInt(), HIGH);
    delay(timePin.toInt());
    digitalWrite(highPin.toInt(), LOW); 
    delay(2000);
  }
  else{
    getInvoice();
    if(down){
      errorScreen();
      getInvoice();
      delay(5000);
    }
    if(payReq != ""){
      qrdisplayScreen();
      delay(5000);
    }
    while(paid == false && payReq != ""){
      webSocket.loop();
      if(paid){
        completeScreen();
        digitalWrite(highPin.toInt(), HIGH);
        delay(timePin.toInt());
        digitalWrite(highPin.toInt(), LOW);
      }
    }
    payReq = "";
    dataId = "";
    paid = false;
    delay(4000);
  }
}

//////////////////DISPLAY///////////////////

void serverError()
{
  setDisplayText("Server connect fail", WHITE, RED, 3, 0, 80);
  Serial.println("Server connect fail");
}

void connectionError()
{
  setDisplayText("Wifi connect fail", WHITE, RED, 3, 0, 80);
  Serial.println("Wifi connect fail");
}

void connection()
{
  setDisplayText("Wifi connected", WHITE, RED, 3, 0, 80);
  Serial.println("Wifi connected");
}

void logoScreen()
{ 
  setDisplayText("bitcoinSwitch", WHITE, PURPLE, 4, 0, 80);
  Serial.println("bitcoinSwitch");
}

void portalLaunched()
{ 
  setDisplayText("PORTAL LAUNCH", WHITE, PURPLE, 4, 0, 80);
  Serial.println("PORTAL LAUNCH");
}

void processingScreen()
{ 
  setDisplayText("PROCESSING", BLACK, RED, 4, 40, 80);
  Serial.println("PROCESSING");
}

void lnbitsScreen()
{ 
  setDisplayText("POWERED BY LNBITS", WHITE, BLACK, 3, 10, 90);
  Serial.println("POWERED BY LNBITS");
}

void portalScreen()
{ 
  setDisplayText("PORTAL LAUNCHED", BLACK, WHITE, 3, 30, 80);
  Serial.println("PORTAL LAUNCHED");
}

void paidScreen()
{ 
  setDisplayText("PAID", WHITE, RED, 4, 110, 80);
  Serial.println("PAID");
}

void completeScreen()
{ 
  setDisplayText("COMPLETE", BLACK, WHITE, 4, 60, 80);
  Serial.println("COMPLETE");
}

void errorScreen()
{ 
  setDisplayText("ERROR", BLACK, WHITE, 4, 70, 80);
  Serial.println("ERROR");
}

void qrdisplayScreen()
{
  String qrCodeData;
  if(lnurl == "true"){
    qrCodeData = lnurl;
  }
  else{
    qrCodeData = payReq;
  }
  
  displayQrCode(qrCodeData);
  Serial.println("QRCode..");
}

//////////////////NODE CALLS///////////////////

void checkConnection(){
  WiFiClientSecure client;
  client.setInsecure();
  const char* lnbitsserver = lnbitsServer.c_str();
  if (!client.connect(lnbitsserver, 443)){
    down = true;
    serverError();
    return;   
  }
}

void getInvoice(){
  WiFiClientSecure client;
  client.setInsecure();
  const char* lnbitsserver = lnbitsServer.c_str();
  if (!client.connect(lnbitsserver, 443)){
    down = true;
    return;   
  }
  StaticJsonDocument<500> doc;
  DeserializationError error;
  char c;
  String line;
  String url = "/lnurldevice/api/v1/lnurl/";
  client.print(String("GET ") + url + deviceId +" HTTP/1.1\r\n" +
                "Host: " + lnbitsserver + "\r\n" +
                "User-Agent: ESP32\r\n" +
                "Content-Type: application/json\r\n" +
                "Connection: close\r\n\r\n");
  while (client.connected()) {
    line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  String callback;
  while (client.available()) {
    line = client.readStringUntil('\n');
    callback = line;
  }
  Serial.println(callback);
  delay(500);
  error = deserializeJson(doc, callback);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  const char* callbackChar = doc["callback"];
  String callbackStr = callbackChar;
  getCallback(callbackStr);
}

void getCallback(String callbackStr){
  WiFiClientSecure client;
  client.setInsecure();
  const char* lnbitsserver = lnbitsServer.c_str();
  if (!client.connect(lnbitsserver, 443)){
    down = true;
    return;   
  }
  StaticJsonDocument<500> doc;
  DeserializationError error;
  char c;
  String line;
  client.print(String("GET ") + callbackStr.substring(8 + lnbitsServer.length()) +" HTTP/1.1\r\n" +
                "Host: " + lnbitsserver + "\r\n" +
                "User-Agent: ESP32\r\n" +
                "Content-Type: application/json\r\n" +
                "Connection: close\r\n\r\n");
   while (client.connected()) {
   String line = client.readStringUntil('\n');
   Serial.println(line);
    if (line == "\r") {
      break;
    }
  }
  line = client.readString();
  Serial.println(line);
  error = deserializeJson(doc, line);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  const char* temp = doc["pr"];
  payReq = temp;
}
//////////////////WEBSOCKET///////////////////


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            {
                Serial.printf("[WSc] Connected to url: %s\n",  payload);
                

			    // send message to server when Connected
				webSocket.sendTXT("Connected");
            }
            break;
        case WStype_TEXT:
            timePin = (char*)payload;
            paid = true;
            
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
    }

}
