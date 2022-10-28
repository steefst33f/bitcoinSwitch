#include "DisplayHandler.h"

#pragma once

#ifndef Screens_h
  #define Screens_h

  #define TTGO

   void serverError() {
      #ifdef TFT_DISPLAY
         setDisplayText("Server connect fail", WHITE, RED, 3, 0, 80);
         Serial.println("Server connect fail");
      #endif;

   }

   void connectionError() {
      #ifdef TFT_DISPLAY
         setDisplayText("Wifi connect fail", WHITE, RED, 3, 0, 80);
         Serial.println("Wifi connect fail");
      #endif;
   }

   void connection() {
      #ifdef TFT_DISPLAY
         setDisplayText("Wifi connected", WHITE, RED, 3, 0, 80);
         Serial.println("Wifi connected");
      #endif;
   }

   void logoScreen() {
      #ifdef TFT_DISPLAY 
         setDisplayText("bitcoinSwitch", WHITE, PURPLE, 4, 0, 80);
         Serial.println("bitcoinSwitch");
      #endif;
   }

   void portalLaunched() {
      #ifdef TFT_DISPLAY 
         setDisplayText("PORTAL LAUNCH", WHITE, PURPLE, 4, 0, 80);
         Serial.println("PORTAL LAUNCH");
      #endif;
   }

   void processingScreen() {
      #ifdef TFT_DISPLAY 
         setDisplayText("PROCESSING", BLACK, RED, 4, 40, 80);
         Serial.println("PROCESSING");
      #endif;
   }

   void lnbitsScreen() {
      #ifdef TFT_DISPLAY 
         setDisplayText("POWERED BY LNBITS", WHITE, BLACK, 3, 10, 90);
         Serial.println("POWERED BY LNBITS");
      #endif;
   }

   void portalScreen() {
      #ifdef TFT_DISPLAY 
         setDisplayText("PORTAL LAUNCHED", BLACK, WHITE, 3, 30, 80);
         Serial.println("PORTAL LAUNCHED");
      #endif;
   }

   void paidScreen() {
      #ifdef TFT_DISPLAY 
         setDisplayText("PAID", WHITE, RED, 4, 110, 80);
         Serial.println("PAID");
      #endif;
   }

   void completeScreen() {
      #ifdef TFT_DISPLAY 
         setDisplayText("COMPLETE", BLACK, WHITE, 4, 60, 80);
         Serial.println("COMPLETE");
      #endif;
   }

   void errorScreen() {
      #ifdef TFT_DISPLAY 
         setDisplayText("ERROR", BLACK, WHITE, 4, 70, 80);
         Serial.println("ERROR");
      #endif;
   }

   void qrdisplayScreen(String qrCodeData) {
      #ifdef TFT_DISPLAY 
         displayQrCode(qrCodeData, 2, 10);
         Serial.println("QRCode..");
      #endif;
   }
#endif
