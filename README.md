#  Air Sampler on PIO
## Controls and monitors a High Volume Air Sampler
## Uses BMP280P to monitor pressures
## Stores data on an external database via WiFi
## Contents
- [Notes](#notes)
- [Installation Instructions](#installation-instructions)
- [Decoding Exceptions](#decoding-exceptions)
- [Issue/Bug report template](#issuebug-report-template)
- [WEMOS LOLIN32 with OLED Board](#WEMOS-LOLIN32)
- [WeMos d1 Mini Pro](#d1_Mini_Pro)
- [Heltec WiFi LORA 32 V1](#Heltec-WiFi-LORA-32-V1)
- [Heltec WiFi LORA 32 V2](#Heltec-WiFi-LORA-32-V2)

## Notes

You may have to push the button to upload to the board
## Installation Instructions
- Use VS Code and PlatformIO

#### Decoding exceptions

I have no idea how to do this or what they are talking about.

#### Issue/Bug report template

Finally, if you're sure no one else had the issue, it's probably you.

## WEMOS LOLIN32
Board name WEMOS LOLIN32.<br/>
<ul>
    <li>OLED Connections:</li>
        <li>CLOCK 4</li>
        <li>DATA 5</li>
        <li>RESET 16</li>
</ul>

![Pin Functions](docs/WemosESP32OLEDTop.jpg)
![Pin Functions](docs/WemosESP32OLEDBottom.jpg)
![Pin Functions](docs/WemosESP32OLEDPinout.jpg)
## - #WEMOS LOLIN32V100
Board name WeMos LOLIN32 V1.0.0<br/>

![Pin Functions](docs/ESP32WeMosLOLIN32Top.jpg)
![Pin Functions](docs/ESP32WeMosLOLIN32Bottom.jpg)
![Pin Functions](docs/ESP32WeMosLOLIN32Pinout.png)
## - #d1_Mini_Pro
Board name WeMos d1 Mini Pro<br/>

![Pin Functions](docs/wemos_d1_mini_pro_pinout.png)
## Heltec WiFi LORA 32 V1
Heltec WiFi LORA 32 V1 – ESP32 with OLED and LORA Board<br/>
To program you need to hold button labeled PRG near coil antenna.<br/>
<ul>
    <li>I2C Connections:</li>
        <li>SCL 22</li>
        <li>SDA 21</li>
    <li>OLED Connections:</li>
        <li>CLOCK 15</li>
        <li>DATA 4</li>
        <li>RESET 16</li>
</ul>

![Pin Functions](docs/WiFi-LORA-32-pinout-Diagram.png)
## Heltec WiFi LORA 32 V2
Heltec WiFi LORA 32 V2 – ESP32 with OLED and LORA Board<br/>
To program you need to hold button labeled PRG near coil antenna.<br/>
I have unresolved issues using a second I2C device with the OLED<br/>
<ul>
    <li>I2C Connections:</li>
        <li>SCL 22</li>
        <li>SDA 21</li>
    <li>OLED Connections:</li>
        <li>CLOCK 15</li>
        <li>DATA 4</li>
        <li>RESET 16</li>
</ul>

![Pin Functions](docs/WIFI_LoRa_32_V2PinDiagram.png)

## Hint

Keep at it.