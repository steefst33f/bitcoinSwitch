# Libraries used for the DIY Vending Machine NFC:

* AutoConnect (1.3.7)
* ArduinoJson (6.19.4)
* PageBuilder (1.5.3)
* PN532 (https://github.com/Seeed-Studio/PN532)Follow the Readme of this repo: 
    <br>Copy all the folders inside PN532-Arduino into the “../Arduino/libraries” folder or perform this in the terminal when in the “../Arduino/libraries” folder:
    <br>
    ```python
    git clone --recursive https://github.com/Seeed-Studio/PN532.git NFC
    ln -s NFC/PN532 ./
    ln -s NFC/PN532_SPI ./
    ln -s NFC/PN532_I2C ./
    ln -s NFC/PN532_HSU ./
    ln -s NFC/NDEF ./
    ```
	And remove the NFC folder when done.
* NfcAdapter(download from the link Don's NDEF library: https://github.com/don/NDEF)
* TFT_eSPI (2.4.79) (https://github.com/Bodmer/TFT_eSPI, get latest from master)
* QRcode(0.0.1)(https://github.com/ricmoo/QRCode)


