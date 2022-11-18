#include "DisplayHandler.h"
#include "qrcode.h"

#ifdef TFT_DISPLAY
  #include <TFT_eSPI.h>
  
  TFT_eSPI ttf = TFT_eSPI();
  int qrScreenBrightness = 180;
  uint16_t qrScreenBgColour = ttf.color565(qrScreenBrightness, qrScreenBrightness, qrScreenBrightness);  
#endif

////////////////////////////////
///        Functions        ///
//////////////////////////////

void clearDisplay(uint16_t color){
  #ifdef TFT_DISPLAY
    ttf.fillScreen(color);
  #endif
}

void initDisplay() {
  #ifdef TFT_DISPLAY
    ttf.init();
    ttf.setRotation(1);
    ttf.fillScreen(BLACK);
    ttf.invertDisplay(true);
  #endif
}

void setDisplayText(String text, uint16_t textColor, uint16_t backgroundColor, int textSize /* = 3*/, int x /* = 0*/, int y /* = 0*/) {
  #ifdef TFT_DISPLAY
    clearDisplay(backgroundColor);
    ttf.setCursor(x, y);
    ttf.setTextSize(textSize);
    ttf.setTextColor(textColor);
    ttf.setTextWrap(true);
    ttf.println(text);
  #endif
  Serial.println(text);
}

void displayQrCode(String string, int pixelSize /* = 2 */, int sidePadding /* = 10 */) {
  #ifdef TFT_DISPLAY
    clearDisplay(qrScreenBgColour);
    
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

void setDisplayErrorText(String text) {
  #if defined(M5STACK)
      setDisplayText(text, RED, WHITE, 3, 0, 80);
  #elif defined(TTGO)
      setDisplayText(text, RED, WHITE, 2, 0, 70);
  #endif 
}

void debugDisplayText(String text) {
  // #if DEBUG == 1
    #ifdef TFT_DISPLAY
      setDisplayText(text, GREEN, BLACK);
    #endif
  // #endif
}
