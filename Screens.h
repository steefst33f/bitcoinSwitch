#include "DisplayHandler.h"

#pragma once

#ifndef Screens_h
  #define Screens_h

   void serverError() {
      #ifdef TFT_DISPLAY
         setDisplayErrorText("Server connect fail");
      #endif
   }

   void connectionError() {
      #ifdef TFT_DISPLAY
         setDisplayErrorText("Wifi connect fail");
      #endif
   }

   void connection() {
      #ifdef TFT_DISPLAY
         setDisplayErrorText("Wifi connected");
      #endif
   }

   void nfcModuleConnectionError() {
      #ifdef TFT_DISPLAY
         setDisplayErrorText("Sorry, NFC payment option not available.");
      #endif
   }

   void logoScreen() {
      #if defined(M5STACK)
         setDisplayText("bitcoinSwitch", PURPLE, WHITE, 4, 0, 80);
      #elif defined(TTGO)
         setDisplayText("bitcoinSwitch", PURPLE, WHITE, 2, 0, 70);
      #endif 
   }

   void portalLaunched() {
      #if defined(M5STACK)
         setDisplayText("PORTAL LAUNCH", PURPLE, WHITE, 4, 0, 80);
      #elif defined(TTGO)
         setDisplayText("PORTAL LAUNCH", PURPLE, WHITE, 2, 0, 70);
      #endif 
   }

   void vendorMode() {
      #if defined(M5STACK)
         setDisplayText("VENDOR REFILL MODE", PURPLE, WHITE, 4, 0, 80);
      #elif defined(TTGO)
         setDisplayText("VENDOR REFILL MODE", PURPLE, WHITE, 2, 0, 70);
      #endif 
   }

   void processingScreen() {
      #if defined(M5STACK)
         setDisplayText("PORTAL LAUNCH", PURPLE, WHITE, 4, 0, 80);
      #elif defined(TTGO)
         setDisplayText("PORTAL LAUNCH", PURPLE, WHITE, 2, 0, 70);
      #endif
   }

   void lnbitsScreen() {
      #if defined(M5STACK)
         setDisplayText("POWERED BY LNBITS", WHITE, BLACK, 3, 10, 90);
      #elif defined(TTGO)
         setDisplayText("POWERED BY LNBITS", WHITE, BLACK, 2, 10, 90);
      #endif
   }

   void portalScreen() {
      #if defined(M5STACK)
         setDisplayText("PORTAL LAUNCHED", BLACK, WHITE, 3, 30, 80);
      #elif defined(TTGO)
         setDisplayText("PORTAL LAUNCHED", BLACK, WHITE, 2, 30, 80);
      #endif
   }

   void paidScreen() {
      #if defined(M5STACK)
         setDisplayText("PAID", BLACK, GREEN, 4, 110, 80);
      #elif defined(TTGO)
         setDisplayText("PAID", BLACK, GREEN, 3, 110, 80);
      #endif
   }

   void completeScreen() {
      #if defined(M5STACK)
         setDisplayText("COMPLETE", BLACK, WHITE, 4, 60, 80);
      #elif defined(TTGO)
         setDisplayText("COMPLETE", BLACK, WHITE, 2, 60, 80);
      #endif
   }

   void errorScreen() {
      #ifdef TFT_DISPLAY 
         setDisplayErrorText("ERROR");
      #endif
   }

   void qrdisplayScreen(String qrCodeData) {
      #ifdef TFT_DISPLAY 
         displayQrCode(qrCodeData, 2, 10);
      #endif
   }
#endif
