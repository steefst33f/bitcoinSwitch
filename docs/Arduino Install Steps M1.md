# Installing Arduino for esp32 from scratch Mac M1

1. Download Arduino IDE 2: https://www.arduino.cc/en/software#future-version-of-the-arduino-ide
2. Install Arduino IDE 2 by opening the .dmg package and drop the Arduino IDE into the Application folder as indicated.
3. Open the Arduino IDE 2
4. Open the sketch you want to use
5. Go to Arduino-> Preferences
6. Enter https://espressif.github.io/arduino-esp32/package_esp32_index.json into the “Additional Board Manager URLs” field and click OK.
7. Open the boards manager in the left pannel.
8. Enter esp32 on the search bar
9. Select the “esp32 by Espressif Systems” package and install the latest one.
10. If you have the esp32 (dev board) already connected Arduino will automatically detect and selected the correct board for you.
11. If this is not the case you can select it yourself in: Tools->Board->esp32->(your board, probably: esp32 Dev Module.
12. You can now upload your sketch to your board.
13. If you get errors that one of the libraries included by your sketch are not found, you will need to add these manually.
14. Go to the library manager  ( via the left side panel or Tools->Library manager)
15. Use the search bar to find an install the library(’s) you need.
16. If you can’t find them in the library manager you can also add a library more manually by adding the folder of the library pin finder into your Arduino Library folder. 
17. If you have issues with Arduino, kill it and restart from the terminal with: "/Applications/Arduino IDE.app/Contents/MacOS/Arduino IDE"