#include <Arduino.h>

#pragma once

#ifndef DisplayHandler_h
  #define DisplayHandler_h
  
  //  #define M5STACK // comment out if not using M5Stack
  #define TTGO // comment out if not using TTGO

  #if defined(M5STACK) || defined(TTGO)
    #define TFT_DISPLAY
  #endif
  
  #ifdef TFT_DISPLAY
    #include <TFT_eSPI.h>

    #define BLACK TFT_BLACK
    #define WHITE TFT_WHITE
    #define RED TFT_RED
    #define GREEN TFT_GREEN
    #define PURPLE TFT_PURPLE
    #define YELLOW TFT_YELLOW
  #else
    #define BLACK 0x0000
    #define PURPLE 0x780F
    #define GREEN 0x07E0
    #define RED 0xF800
    #define WHITE 0xFFFF
    #define YELLOW 0xFFE0
  #endif
  
  void clearDisplay(uint16_t color);
  void initDisplay();
  void setDisplayText(String text, uint16_t textColor, uint16_t backgroundColor, int textSize = 2, int x = 0, int y = 0);
  void displayQrCode(String string, int pixelsize = 2 , int sidepadding = 10);
  void debugDisplayText(String text);  
  void setDisplayErrorText(String text);
#endif
