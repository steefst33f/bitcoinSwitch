#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <Wire.h>

#ifdef M5STACK
  #include <JC_Button.h>
#endif

#include <AutoConnect.h>
#include <ArduinoJson.h>

//NFC
#include <PN532_I2C.h>
#include <NfcAdapter.h>

//My headers
#include "UriComponents.h"
#include "DisplayHandler.h"
#include "Screens.h"

///defines
#define PARAM_FILE "/elements.json"
#define FORMAT_ON_FAIL true
#define DEBUG 1 //set to 1 to show DEBUG Serial print debug logging, if not set to 0

fs::SPIFFSFS &FlashFS = SPIFFS;
using WebServerClass = WebServer;

/////////////////////////////////
///////////CHANGE////////////////
/////////////////////////////////

/* comment out #define M5STACK in DisplayHelpers.h if you don't use the M5Stack */        
bool format = false; // true for formatting SPIFFS, use once, then make false and reflash
int portalPin = 2;

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

// Access point variables
String password;
String lnbitsServer;
String lnurlP;
String invoiceKey;
String highPin;
String timePin;
String amount;
String dataId;
String description;
String payReq;

//nfc variables
bool isScanningForNfcTag = true;
int scanCount = 0; 

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
      "value": "ToTheMoon1"
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
      "label": "LNbits server",
      "value": "legend.lnbits.com"
    },
    {
      "name": "lnurl",
      "type": "ACInput",
      "label": "LNURLPay optional for static QR",
      "value": "LNURL1DP68GURN8GHJ7MR9VAJKUEPWD3HXY6T5WVHXXMMD9AKXUATJD3CZ7CTSDYHHVVF0D3H82UNV9U6RVVPK5TYPNE"
    },
    {
      "name": "invoicekey",
      "type": "ACInput",
      "label": "LNbits invoice key",
      "value": "4b76adae4f1a4dc38f93e892a8fba8b2"
    },
    {
      "name": "pin",
      "type": "ACInput",
      "label": "Pin to turn on",
      "value": "12"
    },
    {
      "name": "time",
      "type": "ACInput",
      "label": "Time to turn on pin",
      "value": "10000"
    },
    {
      "name": "amount",
      "type": "ACInput",
      "label": "Amount to check for",
      "value": "210"
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
WebServerClass server;
AutoConnect portal(server);
AutoConnectConfig config;
AutoConnectAux elementsAux;
AutoConnectAux saveAux;

//I2C Communnucation with the PN532 module
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

///structs
struct Withdrawal {
    String tag;
    String callback;
    String k1;
    int minWithdrawable;
    int maxWithdrawable;
    String defaultDescription;
};

struct CreateInvoiceRequest {
  bool out;
  int amount;
  String memo;
  String unit;
  String description_hash;
  String unhashed_description;
  String lnurl_callback;
  String lnurl_balance_check;
  String webhook;
  bool internal;
  String bolt11;
};

struct Invoice {
  String paymentHash;
  String paymentRequest;
  String checkingId;
  String lnurlResponse;
};

// setup
void setup() {
  Serial.begin(115200);

  // Print some useful debug output - the filename and compilation time
  Serial.println(__FILE__);
  Serial.println("Compiled: " __DATE__ ", " __TIME__);

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
  while(timer < 2000) {
    #ifdef M5STACK
      digitalWrite(2, LOW);
      if(BTNA.read() == 1) {
        Serial.println(F("Launch portal"));
        triggerAp = true;
        timer = 5000;
      }
      digitalWrite(2, HIGH);
    #else
      Serial.println(touchRead(portalPin));
      if(touchRead(portalPin) < 60) {
        Serial.println(F("Launch portal"));
        portalLaunched();
        triggerAp = true;
        timer = 5000;
      }
    #endif
    timer = timer + 100;
    delay(300);
  }
    
  //Initialise Flash and SPIFFS memory
  FlashFS.begin(FORMAT_ON_FAIL);
  SPIFFS.begin(true);
  if(format == true) {
    debugSerialPrintln(F("Formating Flash...:"));
    SPIFFS.format();
    debugSerialPrintln(F("Formated Flash!"));
  }

  // get the saved details and store in global variables
  File paramFile = FlashFS.open(PARAM_FILE, "r");
  if(paramFile) {
    StaticJsonDocument<2500> doc;
    DeserializationError error = deserializeJson(doc, paramFile.readString());

    const JsonObject maRoot0 = doc[0];
    const char *maRoot0Char = maRoot0["value"];
    password = maRoot0Char;
    
    const JsonObject maRoot1 = doc[1];
    const char *maRoot1Char = maRoot1["value"];
    lnbitsServer = maRoot1Char;

    const JsonObject maRoot2 = doc[2];
    const char *maRoot2Char = maRoot2["value"];
    lnurlP = maRoot2Char;
    lnurlP.toUpperCase();

    const JsonObject maRoot3 = doc[3];
    const char *maRoot3Char = maRoot3["value"];
    invoiceKey = maRoot3Char;

    const JsonObject maRoot4 = doc[4];
    const char *maRoot4Char = maRoot4["value"];
    highPin = maRoot4Char;
    
    const JsonObject maRoot5 = doc[5];
    const char *maRoot5Char = maRoot5["value"];
    timePin = maRoot5Char;

    const JsonObject maRoot6 = doc[6];
    const char *maRoot6Char = maRoot6["value"];
    amount = maRoot6Char;
  } else {
    //If there is no stored configuration than trigger start the AP
    triggerAp = true;
  }
  paramFile.close();

  //start server
  server.on("/", []() {
    content += AUTOCONNECT_LINK(COG_24);
    server.send(200, "text/html", content);
  });
  
  //Load stored configuration into portal config page
  elementsAux.load(FPSTR(PAGE_ELEMENTS));
  elementsAux.on([](AutoConnectAux &aux, PageArgument &arg) {
    File param = FlashFS.open(PARAM_FILE, "r");
    if(param) {
      aux.loadElement(param, {"password", "server", "lnurl", "invoicekey", "pin", "time", "amount"});
      param.close();
    }

    if(portal.where() == "/config") {
      File param = FlashFS.open(PARAM_FILE, "r");
      if(param) {
        aux.loadElement(param, {"password", "server", "lnurl", "invoicekey", "pin", "time", "amount"});
        param.close();
      }
    }
    return String();
  });

  //load save page
  saveAux.load(FPSTR(PAGE_SAVE));
  saveAux.on([](AutoConnectAux &aux, PageArgument &arg) {
    aux["caption"].value = PARAM_FILE;
    File param = FlashFS.open(PARAM_FILE, "w");
    if(param) {
      // save as a loadable set for parameters.
      elementsAux.saveElement(param, {"password", "server", "lnurl", "invoicekey", "pin", "time", "amount"});
      param.close();
      // read the saved elements again to display.
      param = FlashFS.open(PARAM_FILE, "r");
      aux["echo"].value = param.readString();
      param.close();
    } else {
      aux["echo"].value = "Filesystem failed to open.";
    }
    return String();
  });

  //configure AP
  config.auth = AC_AUTH_BASIC;
  config.authScope = AC_AUTHSCOPE_AUX;
  config.ticker = true;
  config.autoReconnect = true;
  config.apid = "Device-" + String((uint32_t)ESP.getEfuseMac(), HEX);
  config.psk = password;
  config.menuItems = AC_MENUITEM_CONFIGNEW | AC_MENUITEM_OPENSSIDS | AC_MENUITEM_RESET;
  config.reconnectInterval = 1;

  //Launch Portal
  if(triggerAp == true) {
    portalLaunched();
    config.immediateStart = true;
    portal.join({elementsAux, saveAux});
    portal.config(config);
    portal.begin();
    while(true) {
      portal.handleClient();
    }
    timer = 2000;
  }
  timer = timer + 200;
  delay(200);

  if(invoiceKey != "") {
    portal.join({elementsAux, saveAux});
    config.autoRise = false;
    portal.config(config);
    portal.begin();
  }
  triggerAp = false;
  lnbitsScreen();
  delay(1000);
  pinMode(highPin.toInt(), OUTPUT);
  digitalWrite(highPin.toInt(), LOW);
  printSwitchPinStatus();

  //start nfc scanner
  nfc.begin();
  isScanningForNfcTag = false;
  // delay(1000);
}

// loop
void loop() {
  while(WiFi.status() != WL_CONNECTED) {
    connectionError();
    Serial.println(F("Failed to connect"));
    digitalWrite(highPin.toInt(), LOW);
    printSwitchPinStatus();
    delay(5000);
  }

  debugSerialPrintln("switchPin = " + highPin);
  debugSerialPrintln("switchOnTimeInMs = " + timePin);
  debugSerialPrintln("LNURLp = " + lnurlP.substring(0, 5));

  printSwitchPinStatus();
  
  if(lnurlP.substring(0, 5) == "LNURL") {
    debugSerialPrintln(F("lnurlP.substring(0, 5) == \"LNURL\""));
    qrdisplayScreen(lnurlP);
    checkBalance();
    oldBalance = balance;
    isScanningForNfcTag = true;

    while((oldBalance + amount.toInt()) > balance) {
      checkBalance();
      debugSerialPrintln(F("Scanning for nfc tag with LNURLw....."));
      scanForNfcTagWithLNURL();
      delay(2000);
    }

    scanCount = 0;
    isScanningForNfcTag = false;
    oldBalance = balance;
    paidScreen();

    //turn on switch
    digitalWrite(highPin.toInt(), HIGH);
    delay(timePin.toInt());
    digitalWrite(highPin.toInt(), LOW); 
    delay(2000);
    paid = false;
  } else {
    debugSerialPrintln(F("When no LNURLp yet, create one flow..."));
    getInvoice("BitcoinSwitch");
    
    if(down) {
      errorScreen();
      getInvoice("BitcoinSwitch");
      delay(5000);
    }

    if(payReq != "") {
      qrdisplayScreen(payReq);
      delay(5000);
    }

    while(paid == false && payReq != "") {
      checkInvoice(dataId);
      if(paid) {
        completeScreen();
        digitalWrite(highPin.toInt(), HIGH);
        delay(timePin.toInt());
        digitalWrite(highPin.toInt(), LOW); 
        printSwitchPinStatus();
      }
      delay(2000);
    }
    payReq = "";
    dataId = "";
    paid = false;
    delay(4000);
  }
}

/////NFC scanning/////
void scanForNfcTagWithLNURL() {
  scanCount++;
  debugSerialPrintln("scanning... try number  " + String(scanCount));
  if(nfc.tagPresent() && isScanningForNfcTag) {
    NfcTag tag = nfc.read();
    debugSerialPrintln(tag.getTagType());
    debugSerialPrintln("UID: " + tag.getUidString());
    debugDisplayText(tag.getUidString());
    
    if(tag.hasNdefMessage()) {
      NdefMessage message = tag.getNdefMessage();

      // If you have more than 1 Message then it wil cycle through them
      int recordCount = message.getRecordCount();
      for(int i = 0; i < recordCount; i++) {
        debugSerialPrintln("\nNDEF Record " + String(i+1));
        NdefRecord record = message.getRecord(i);

        int payloadLength = record.getPayloadLength();
        byte payload[record.getPayloadLength()];
        record.getPayload(payload);
        String string = convertToStringFromBytes(payload, payloadLength);
        
        debugSerialPrintln("Payload Length = " + String(payloadLength));
        debugSerialPrintln("  Information (as String): " + string);

        String lnUrl = getLNURL(string);

        if(lnUrl != "") { 
          debugSerialPrintln(F("decode scanned LNURL...."));
          String decodedLnUrl = decode(lnUrl);
          debugDisplayText(decodedLnUrl);
          
          debugSerialPrintln(F("request withrawal callback details...."));
          Withdrawal withdrawal = getWithdrawal(decodedLnUrl);
          debugSerialPrintln("withdrawal.tag = " + withdrawal.tag); 
          debugSerialPrintln("withdrawal.callback = " + withdrawal.callback); 
          debugSerialPrintln("withdrawal.k1 = " + withdrawal.k1); 
          debugSerialPrintln("withdrawal.minWithdrawable = " + String(withdrawal.minWithdrawable)); 
          debugSerialPrintln("withdrawal.maxWithdrawable = " + String(withdrawal.maxWithdrawable)); 
          debugSerialPrintln("withdrawal.defaultDescription = " + withdrawal.defaultDescription);
          debugSerialPrintln(""); 

          if(withdrawal.tag != "withdrawRequest") {
            debugSerialPrintln(F("Scanned tag is not LNURL withdraw"));
            debugSerialPrintln(F("Present a tag with a LNURL withdraw on it"));
            debugDisplayText(F("Scanned tag is not LNURL withdraw"));
            return;
          }

          debugSerialPrintln(F("Scanned tag is a LNURL withdrawal request!"));
          if(!isAmountInWithdrawableBounds(amount.toInt(), withdrawal.minWithdrawable,  withdrawal.maxWithdrawable)) {
            debugSerialPrintln("The requested amount: " + amount + " is not within this LNURL withdrawal bounds");
            debugSerialPrintln(F("Amount not in bounds, can't withdraw from presented voucher."));
            debugDisplayText(F("Amount not in bounds, can't withdraw from presented voucher."));
            return;
          }

          debugSerialPrintln("The requested amount: " + amount);
          debugSerialPrintln(F(" is within this LNURL withdrawal bounds"));
          debugSerialPrintln(F("Continue payment flow by creating invoice"));

          debugSerialPrintln(F("Will create invoice to request withrawal..."));
          debugDisplayText("Creating invoice..");
          Invoice invoice = getInvoice("BitcoinSwitch QR");
          debugSerialPrintln("invoice.paymentHash = " + invoice.paymentHash); 
          debugSerialPrintln("invoice.paymentRequest = " + invoice.paymentRequest);
          debugSerialPrintln("invoice.checkingId = " + invoice.checkingId);  
          debugSerialPrintln("invoice.lnurlResponse = " + invoice.lnurlResponse); 
          debugSerialPrintln(F("")); 

          if(invoice.paymentRequest == "") {
            debugSerialPrintln(F("Failed to create invoice"));
            debugDisplayText(F("Failed to create invoice")); 
            return;
          }

          bool success = withdraw(withdrawal.callback, withdrawal.k1, invoice.paymentRequest);
          if(!success) {
            debugSerialPrintln(F("Failed to request withdrawalfor invoice request with memo: ")); // + invoice.memo);
            debugDisplayText(F("Failed to request withdrawal invoice"));
            return;
          }

          debugSerialPrintln(F("Withdrawal request successfull!"));
          //TODO: Check if open invoice is paid
          bool isPaid = checkInvoice(invoice.checkingId);

          int numberOfTries = 1;
          while(!isPaid && (numberOfTries < 3)) {
            delay(2000);
            isPaid = checkInvoice(invoice.checkingId);
            numberOfTries++;
          }

          if(!isPaid) {
            debugSerialPrintln(F("Could not confirm withdrawal, the invoice has not been payed in time"));
            debugDisplayText("Could not confirm withdrawal, transaction cancelled");
            return;
          }
          debugSerialPrintln(F("Withdrawal successfull, invoice is payed!"));
          debugDisplayText(F("Withdrawal succeeded!! Thank you!"));
          paid = true;
        }
      }
    }
  }
}

//////////////////NODE CALLS///////////////////

void checkBalance() {
  WiFiClientSecure client;
  client.setInsecure();
  const char* lnbitsserver = lnbitsServer.c_str();
  const char* invoicekey = invoiceKey.c_str();
  down = false;

  if(!client.connect(lnbitsserver, 443)) {
    down = true;
    return;   
  }

  String url = "/api/v1/wallet";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                "Host: " + lnbitsserver + "\r\n" +
                "X-Api-Key: "+ invoicekey +" \r\n" +
                "User-Agent: ESP32\r\n" +
                "Content-Type: application/json\r\n" +
                "Connection: close\r\n\r\n");
   while(client.connected()) {
   String line = client.readStringUntil('\n');
    if(line == "\r") {
      break;
    }
  }
  String line = client.readString();
  Serial.println(line);
  StaticJsonDocument<500> doc;
  DeserializationError error = deserializeJson(doc, line);
  if(error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  int charBalance = doc["balance"];
  balance = charBalance;
}

Invoice getInvoice(String description) 
{
  WiFiClientSecure client;
  client.setInsecure();
  down = false;

  if(!client.connect(lnbitsServer.c_str(), 443)) {
    down = true;
    return {};   
  }

  String topost = "{\"out\": false,\"amount\" : " + String(amount) + ", \"memo\" :\""+ String(description) + String(random(1,1000)) + "\"}";
  String url = "/api/v1/payments";
  String request = (String("POST ") + url +" HTTP/1.1\r\n" +
                "Host: " + lnbitsServer + "\r\n" +
                "User-Agent: ESP32\r\n" +
                "X-Api-Key: "+ invoiceKey +" \r\n" +
                "Content-Type: application/json\r\n" +
                "Connection: close\r\n" +
                "Content-Length: " + topost.length() + "\r\n" +
                "\r\n" + 
                topost + "\n");
  client.print(request);
  debugSerialPrintln(request);
  
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if(line == "\r") {
      break;
    }
  }
  
  String line = client.readString();
  StaticJsonDocument<1000> doc;
  DeserializationError error = deserializeJson(doc, line);
  if(error) {
    debugSerialPrintln("deserializeJson() failed: " + String(error.f_str()));
    return {};
  }

  //TODO: remove these user struct instsead
  const char* payment_hash = doc["checking_id"];
  const char* payment_request = doc["payment_request"];
  payReq = payment_request;
  dataId = payment_hash;

  return {
    doc["payment_hash"],
    doc["payment_request"],
    doc["checking_id"],
    doc["lnurl_response"]
  };
}


bool checkInvoice(String invoiceId) {
  WiFiClientSecure client;
  client.setInsecure();
  down = false;

  if(!client.connect(lnbitsServer.c_str(), 443)) {
    down = true;
    return false;   
  }

  String url = "/api/v1/payments/";
  String request = (String("GET ") + url + invoiceId +" HTTP/1.1\r\n" +
                "Host: " + lnbitsServer + "\r\n" +
                "User-Agent: ESP32\r\n" +
                "Content-Type: application/json\r\n" +
                "Connection: close\r\n\r\n");
  client.print(request);
  debugSerialPrintln(request);
  
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if(line == "\r") {
      break;
    }
  }
  String line = client.readString();
  debugSerialPrintln(line);
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, line);
  if(error) {
    debugSerialPrintln("deserializeJson() failed: " + String(error.f_str()));
    return false;
  }
  bool isPaid = doc["paid"];
//  paid = charPaid;
  return isPaid;
}

/// LNURLw functions
String getLNURL(String string) {
  debugSerialPrintln("bool islLNURL(" + string + ")");
//  
//  string.trim();
//  string.toUpperCase();
//
//  if(string.startsWith("LNURL")) {
//    debugSerialPrint("found LNURL");
//    return string;
//  
//  } else if(string.startsWith("LIGHTNING:LNURL")) {
    debugSerialPrintln("found lightning URI with LNURL: " + string);
    string.remove(0,11);
    return string;
//      
//  } else {
//    debugSerialPrintln(F("string is no LNURL");
//    return "";
//  }
//  return string;
}

String decode(String lnUrl) {
  WiFiClientSecure client;
  client.setInsecure();
  down = false;

  if(!client.connect(lnbitsServer.c_str(), 443)) {
    down = true;
    return "";   
  }

  String body = "{\"data\": \"" + lnUrl + "\"}";
  String url = "/api/v1/payments/decode";
  String request = String("POST ") + url + " HTTP/1.1\r\n" +
                "Host: " + lnbitsServer + "\r\n" +
                "User-Agent: ESP32\r\n" +
                "Content-Type: application/json\r\n" +
                "Connection: close\r\n" +
                "Content-Length: " + body.length() + "\r\n" +
                "\r\n" + 
                body + "\n";
  client.print(request);
  debugSerialPrintln(request);
   
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if(line == "\r") {
      break;
    }
  }
  String line = client.readString();
  debugSerialPrintln(line);
  const size_t capacity = JSON_OBJECT_SIZE(2) + 800;
  DynamicJsonDocument doc(capacity);  
  
  DeserializationError error = deserializeJson(doc, line);
  if(error) {
    debugSerialPrintln("deserializeJson() failed: " + String(error.f_str()));
    return "";
  }
  
  const char* domain = doc["domain"]; 
  debugSerialPrintln("decodedLNURL: " + String(domain));
  return domain;
}

Withdrawal getWithdrawal(String uri) {
  WiFiClientSecure client;
  client.setInsecure();
  down = false;
  UriComponents uriComponents = UriComponents::Parse(uri.c_str());
  String host = uriComponents.host.c_str();

  if(!client.connect(host.c_str(), 443)) {
    down = true;
    return {};   
  }
  
  debugSerialPrintln(F("xxxxx!!!!!!"));
  String request = String("GET ") + uri + " HTTP/1.0\r\n" +
    "User-Agent: ESP32\r\n" +
    "accept: text/html\r\n" +
    "Connection: close\r\n\r\n";
  debugSerialPrintln(request);
  client.print(request);

  while(client.connected()) {
   String line = client.readStringUntil('\n');
   if(line == "\r") {
     break;
    }
  }
  String line = client.readString();
  debugSerialPrintln(line);
  debugDisplayText(line);
  
  const size_t capacity = JSON_OBJECT_SIZE(2) + 800;
  DynamicJsonDocument doc(capacity);  
  DeserializationError error = deserializeJson(doc, line);
  if(error) {
    debugSerialPrintln(("deserializeJson() failed: ") + String(error.f_str()));
    return {};
  }

  return {
    doc["tag"],
    doc["callback"],
    doc["k1"],
    doc["minWithdrawable"],
    doc["maxWithdrawable"],
    doc["defaultDescription"]
  };
}

bool isAmountInWithdrawableBounds(int amount, int minWithdrawable, int maxWithdrawable) {
  int amountInMilliSats = amount * 1000;
  debugSerialPrintln("((" + String(amountInMilliSats) + " >= " + String(minWithdrawable) + ") && (" + String(amountInMilliSats) + " <= " + String(maxWithdrawable) + "))");
  return ((amountInMilliSats >=minWithdrawable) && (amountInMilliSats <= maxWithdrawable));
}

bool withdraw(String callback, String k1, String pr) {
  WiFiClientSecure client;
  client.setInsecure();
  UriComponents uriComponents = UriComponents::Parse(callback.c_str());
  String host = uriComponents.host.c_str();
  down = false;

  if(!client.connect(host.c_str(), 443)) {
    down = true;
    return {};   
  }
  String requestParameters = ("k1=" + k1 + "&pr=" + pr);
  String request = (String("GET ") + callback + "?" + requestParameters + " HTTP/1.0\r\n" +
                "User-Agent: ESP32\r\n" +
                "accept: text/plain\r\n" +
                "Connection: close\r\n\r\n");
  client.print(request);
  debugSerialPrintln(request);
  
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if(line == "\r") {
      break;
    }
  }
  String line = client.readString();
  debugSerialPrintln(line);
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, line);
  if(error) {
    debugSerialPrintln("deserializeJson() failed: " + String(error.f_str()));
    return false;
  }
  bool isOk = doc["status"];
  return isOk;
}

//////// Helper functions /////////////
String convertToStringFromBytes(byte dataArray[], int sizeOfArray) {
  String stringOfData = "";
  for(int byteIndex = 0; byteIndex < sizeOfArray; byteIndex++) {
    stringOfData += (char)dataArray[byteIndex];
  }
  return stringOfData;
}

/////////////////Serial Print //////////////
void debugSerialPrintln(const String &string) {
  #if DEBUG == 1
    Serial.println(string);
  #endif
}

void printSwitchPinStatus() {
  int highPinState = digitalRead(highPin.toInt());
  debugSerialPrintln("State of switch-pin: "+ highPin + " = " + String(highPinState));
  debugSerialPrintln("Switch ON time configuration = " + timePin + " ms");
}
