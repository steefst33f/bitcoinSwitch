#include "DisplayHandler.h"
#include "qrcoded.h"

#ifdef M5STACK
  #include <TFT_eSPI.h>
  
  TFT_eSPI ttf = TFT_eSPI();
  int qrScreenBrightness = 180;
  uint16_t qrScreenBgColour = ttf.color565(qrScreenBrightness, qrScreenBrightness, qrScreenBrightness);  
#endif

////////////////////////////////
///        Functions        ///
//////////////////////////////

void clearDisplay(uint16_t color) 
{
  #ifdef M5STACK
    ttf.fillScreen(color);
  #endif
}

void initDisplay() 
{
  #ifdef M5STACK
    ttf.init();
    ttf.setRotation(1);
    ttf.fillScreen(BLACK);
    ttf.invertDisplay(true);
    Serial.println("initDisplay() (for M5STACK)");
  #endif
}

void setDisplayText(String text, uint16_t textColor, uint16_t backgroundColor, int textSize /* = 3*/, int x /* = 0*/, int y /* = 0*/) {
  #ifdef M5STACK
    clearDisplay(backgroundColor);
    ttf.setCursor(x, y);
    ttf.setTextSize(textSize);
    ttf.setTextColor(textColor);
    ttf.setTextWrap(true);
    ttf.println(text);
    Serial.println(text);
  #endif
}

void displayQrCode(String string, int pixelsize /* = 2 */, int sidepadding /* = 10 */) 
{
  #ifdef M5STACK
    clearDisplay(qrScreenBgColour);
  
    int pixelSize;
    int sidePadding;
    
    pixelSize = 2;
    sidePadding = 10;
    
    String qrCodeData;
    qrCodeData = string;
    qrCodeData.toUpperCase();
    Serial.println(qrCodeData);
  
    const char *qrDataChar = qrCodeData.c_str();
    QRCode qrcoded;
    uint8_t qrcodeData[qrcode_getBufferSize(20)];
  
    qrcode_initText(&qrcoded, qrcodeData, 11, 0, qrDataChar);
    
    for (uint8_t y = 0; y < qrcoded.size; y++) {
      for (uint8_t x = 0; x < qrcoded.size; x++) {
        if (qrcode_getModule(&qrcoded, x, y)) {
          ttf.fillRect(sidePadding + pixelSize * x, sidePadding + pixelSize * y, pixelSize, pixelSize, BLACK);
        } else {
          ttf.fillRect(sidePadding + pixelSize * x, sidePadding + pixelSize * y, pixelSize, pixelSize, WHITE);
        }
      }
    }
   #endif
}

void debugDisplayText(String text) 
{
  #ifdef M5STACK
    #if DEBUG == 1
      setDisplayText(text, GREEN, BLACK);
    #endif
  #endif
}
