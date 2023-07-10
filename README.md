# BitcoinSwitch DIY Sticker Vending Machine with NFC (NTAG21*)

[![DIY vending machine demo](https://twitter.com/arcbtc/status/1581186711668678657)](https://twitter.com/arcbtc/status/1581186711668678657)

## This is a proof of concept of a Vending Machine that accept Bitcoin Lightning payments (with NFC). 

<br>

<p>The goal of this  project to get you and the people around you excited about the possibilities of Bitcoin lightning payments by using it and seeing it work in a real life application.</p>

This little DIY Sticker Vending Machine is based on [Ben Arcs](https://github.com/arcbtc) [BitcoinSwitch](https://youtu.be/FeoIwTjv3YM) (github project: [BitcoinSwitch](https://github.com/arcbtc/bitcoinSwitch)) and accepts Bitcoin Lightning payments in return for stickers. ;)

<p>You can pay for a sticker by scanning the QRCode on the display with your favourite lightning wallet app or tap to the NFC module with a NFC tag or card that contains a LNURL withdrawal link.</p>

<p>The Vending Machine will poll (check every x seconds) if a payment has been received.  When a confirmation of payment is received, the Vending Machine will activate the Sticker dispenser and a sticker will be pushed out of the Vending Machine. Customer happy, Vending Machine happy!<br></p>

### What you need:
- (TTGO) esp32 dev module (Can be any, but for this project I used the TTGO as it has a built in display which is nice for displaying some usage feedback)
- green LED
- Resistor 
- 360 degrees mini servo motor
- Some (copper) metal wire (1mm diameter)
- Karton (to make the boxes of the vending machine)
- 2 USB cables (C or mini usb, depending on your esp32 dev module)
- 2 powerbanks
- (Jumper) wires

### Tools:
- Scissors
- Flashlight (or something else that sturdy and tube shaped so you can use it to form the wire in shape)
- (Duck-)tape/glue
- Arduino IDE ([installation steps for Mac M1](docs/Arduino%20Install%20Steps%20M1.md))

### Setup:
1. Download the files from this github repository onto your computer.
2. Open the VendingMachine.ino sketch file in your Arduino IDE. 
3. Drag and drop all the libraries found in the libraries folder from the repository into your own Arduino Library folder on your computer ((used libraries)[])
5. Select the esp32 dev module as your board in the board manager.
6. Connect your esp32 dev module with a usb cable to your computer.
7. Compile the sketch.ino, continue if it succeeds. If its doesn’t succeed try these tips: [troubleshoot esp32](/docs/tips.md)
8. Now compile and flash the sketch to your esp32 dev module
9. Open the serial monitor output window in your Arduino IDE and set the baudrate to 115200.
10. Press the reset button on your esp32 dev module.
11. You should now be able to see the serial output of the esp32 in the serial monitor window

### Configure your Vending Machine:
To be able to configure the wifi and ports of your esp32 you will have to switch it in Access Point (Portal) mode:
1. Press the reset button of your esp32 dev module and hold the capacitive touch pin (the wire) until you see Portal Launch on your display or Serial Monitor output.
2. Get your mobile phone or another device with a browser and wifi. Search for a wifi network starting with “device-….” (This is your esp32) and connect to it.
3. A browser with a login page should appear now with a pop-up message to enter the device name of your esp32 and the password for accessing it. Name: “device-….” Password: “ToTheMoon” (For better security you can change this password later in the access point configuration)
4. Configure your ssid for the Vending Machine (set the wifi network and its password that your Vending Machine can use to connect to the internet).Choose configure new AP from the menu.
5. Here select the wifi network you want it to connect to.
6. Fill in the password for accessing the wifi network.
7. If all went well your wifi network credentials are now saved in the esp32. To check verify this you can now go to the show ssid??? Page. And your wifi network should be in the list there now. Select it and your esp32 will try to connect to it. If you’re redirected to the succes page it works. 
8. In the Portal menu go to configure acces point to configure the other Vending Machine parameters: LNBits server and wallet you use, the url of your static LNURLp, the GPIO pins of the esp32 you are going to connect the LED and the servo motor with, the high time of the LED.

### Connecting the components:
<p>Until now we only had the esp32 dev module connected to our pc via a usb cable, now we will connect the other components for our vending machine to it, following this schematic:

![Image of DIY Vending Machine Scheme](/docs/DIY%20Vending%20Machine%20NFC_schem.png)
￼
</P>

### Which can be connected on a breadboard like this:
<p>

![Image of DIY Vending Machine Scheme](/docs/DIY%20Vending%20Machine%20NFC_bb.png)	
 <br>
| TTGO\/ESP32 | NFC Module |
| :--- | :---|
| Pin 21 | Pin SDA |
| Pin 22 | Pin SCL |
| Pin  G | Pin GND |
| Pin 3V | Pin VCC |

 <br>

| TTGO\/ESP32 | Touch Wire |
| :--- | :---|
| Pin 2 | Wire end |

 <br>
 </p>

| TTGO\/ESP32 | Servo |
| :--- | :---|
| Pin 15 | Pin Pulse |
| Pin G | Pin GND |
| Pin 5V | Pin VCC |

 <br>

(Pull down) Resistor 220 Omh between TTGO pin 21 and GND <br>
(Pull down) Resistor 220 Ohm between TTGO pin 22 and GND <br>

 <br>

### Test if al plays nicely together:

1. Connect the esp32 dev module with a usb cable to your pc.
2. Open the serial monitor in Arduino. 
3. Reset the esp32 dev module. Now you should see the serial output of the sketch file on the serial monitor.
4. If you used the TTGO or connected a different screen you will see it startup with :  “BitcoinVendingMachine”, “Powered by LNBITS”, “and Steef”.
5. If everything goes well it will end up displaying a QRcode on the screen. This means the Vending Machine is ready to receive payments.

## Paying using the QRcode:

1. Scan the QRcode on the TTGO display with your phone. This should open your lighting wallet with an invoice to pay.
2. Pay the invoice.
3. In a few seconds the screen of the TTGO should go green and say: “PAID”
4. The LED will turn on (for x seconds)
5. The servo will turn for x degrees ( the the dispenser of the vending machine.)
6. The TTGO will display the QR code again.

## Paying with NFC:
1. Tap your NTAG21* card with a LNURLWithdrawl link on it against the NFC module.
2. Hold it against the module untill the url appears on the screen (if notthing happens after 10 seconds, try again).
3. If the vending machine could withdrawl the amount to pay succesfull, the screen oof the TTGO should go green and say: "PAID".
4. The LED will turn on (for x seconds)
5. The servo will turn for x degrees ( the the dispenser of the vending machine.)
6. The TTGO will display the QR code again.