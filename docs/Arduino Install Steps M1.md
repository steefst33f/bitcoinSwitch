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

<br>
<br>

# Uninstalling and installing Arduino IDE

https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-mac-and-linux-instructions/
<br>
And did this, don’t know if still effective:
<br>
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html
<br>
<br>

# Installing Visual Code for Arduino development

Advice start clean and remove everything from Visual code if you already had an install that doesn’t play nicely with Arduino yet:

rm -fr ~/Library/Preferences/com.microsoft.VSCode.helper.plist 
rm -fr ~/Library/Preferences/com.microsoft.VSCode.plist 
rm -fr ~/Library/Caches/com.microsoft.VSCode
rm -fr ~/Library/Caches/com.microsoft.VSCode.ShipIt/
rm -fr ~/Library/Application\ Support/Code/
rm -fr ~/Library/Saved\ Application\ State/com.microsoft.VSCode.savedState/
rm -fr ~/.vscode/

Removes all extensions: 
rm -rf ~/.vscode*

See: https://stackoverflow.com/questions/42603103/how-to-completely-uninstall-vs-code-on-mac


1. Download the latest version of Visual Studio Code: https://code.visualstudio.com/docs/?dv=osx
2. Move the downloaded app to the Applications folder
3. Open the VScode app
4. Install the Arduino Extension: 
    1. (Cmd + Shift + X) search for Arduino Extension
    2. Press Install ( you need to have Arduino IDE already installed)
5. Install Reload:
    1. (Cmd + Shift + X) search for Reload (reload button for statusbar)
    2. Press Install
6. https://forum.arduino.cc/t/visual-code-studio-and-integrating-arduino-2-0/1032241
7. Open the Arduino Sketch file (.ino) you like to work on
8. Select a board (fn + F1) search for board