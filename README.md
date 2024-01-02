# RGB LED Clock 

## Features:
* Unique RGB Clock Display
* Clock is an Anlog display of hours, minutes and seconds
* Hour is Red
* Minutes is Green
* Seconds are Blue and (tick is white)
* Offers Advanced Web Interface for changing settings
* Select the UTC Offset for your time zone
* Control the brightness

## Required Parts:
* Wemos D1 Mini: https://amzn.to/2ImqD1n
* 12 RGB LED Ring 

Note: Using the links provided here help to support these types of projects. Thank you for the support.  

## Wiring for the Wemos D1 Mini to the RGB Ring:
RBB Pin -> D3  
5V Pin -> VCC
Ground -> Ground


## 3D Printed Case by Qrome:  

## Compiling and Loading to Wemos D1 Mini
It is recommended to use Arduino IDE.  You will need to configure Arduino IDE to work with the Wemos board and USB port and installed the required USB drivers etc.  
* USB CH340G drivers:  https://wiki.wemos.cc/downloads
* Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.  This will add support for the Wemos D1 Mini to Arduino IDE.
* Open Boards Manager from Tools > Board menu and install esp8266 platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).
* Select Board:  "WeMos D1 R2 & mini"
* Set 1M File System SPIFFS -- this project uses File System for saving and reading configuration settings on the chip.

## Loading Supporting Library Files in Arduino
Use the Arduino guide for details on how to installing and manage libraries https://www.arduino.cc/en/Guide/Libraries  
**Packages** -- the following packages and libraries are used (download and install):  
* WiFiManager.h --> https://github.com/tzapu/WiFiManager  
* Adafruit_NeoPixel.h --> Library Manager

## Initial Configuration
All settings may be managed from the Web Interface, however, you may update the **Settings.h** file manually -- but it is not required.   
NOTE: The settings in the Settings.h are the default settings for the first loading. After loading you will manage changes to the settings via the Web Interface. If you want to change settings again in the settings.h, you will need to erase the file system on the Wemos or use the “Reset Settings” option in the Web Interface.  

## Web Interface
This project uses the **WiFiManager** so when it can't find the last network it was connected to 
it will become a **AP Hotspot** -- connect to it with your phone and you can then enter your WiFi connection information.  
NOTE: The Wemos D1 Mini requires a 2.4Ghz network.  

After connected to your WiFi network it will display the IP addressed assigned to it and that can be 
used to open a browser to the Web Interface.  **Everything** can be configured there.

## Donate or Tip
Please do not feel obligated, but donations and tips are warmly welcomed.  I have added the donation button at the request of a few people that wanted to contribute and show appreciation.  Thank you, and enjoy the application and project.  

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=6VPMTLASLSKWE)



/* The MIT License (MIT)

Copyright (c) 2022 David Payne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
