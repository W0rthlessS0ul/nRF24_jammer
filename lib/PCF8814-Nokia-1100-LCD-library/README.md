# PCF8814 LCD LIBRARY

This is an arduino library for monochrome LCD based on PCF8814 driver. This library provides just hardware application layer functions. Most graphic features are provided by Adafruit GFX library.

## Overview

PCF8814 supports I²C, 4-wire and 3-wire SPI interfaces to communicate. 
In this library used 3-wire SPI (just software by bitbanging). 
My LCD version (from the legendary Nokia 1100) doesn't have 4-wire SPI and I²C outputs.

#### PCF8814:

    - resolution: 96x65 visible pixels
    - on-chip display data RAM (96x65 = 6240 bits)
    - lcd from Nokia 1100 pinout:

           1 RESET             1 2 3 4 5 6 7 8 9
           2 SCE              _|_|_|_|_|_|_|_|_|_
           3 GND             / _________________ \ 
           4 SDIN           | |                 | |
           4 SCLK           | |   PCF8814 LCD   | |
           5 VDDI           | |    from 1100    | |
           6 VDD            | |      96x65      | |
           7 LED+           | |_________________| |
           8 unused          \___________________/

## Compatibility 
| Tested with               | SPI       | fps      |
| ------------------------- |:---------:| --------:|
| Atmega328 based arduinos  | software  | about 14 |
| ESP8266 based platforms   | software  | about 39 |

## How to use it

PCF8814 library depends by Adafruit GFX library and you must install Adafruit GFX for your IDE.

If you use Arduino IDE add this, Adafruit GFX and Adafruit BusIO libraries via "Sketch -> Include Library -> Add .ZIP Library" in Arduino IDE.\
If you use Platformio IDE just add the following text in your platformio.ini:

    lib_deps=
        https://github.com/kashapovd/PCF8814-Nokia-1100-LCD-library
        https://github.com/adafruit/Adafruit-GFX-Library
        https://github.com/adafruit/Adafruit_BusIO

## Demonstration ([video](https://youtu.be/tWlndlFdpFo))
![PCF8814 LCD TEST](demo/demo.gif)

Kashapov Yaroslav, 2020\
feedback: [telegram](https://t.me/kashapovd), <iam@kashapovd.xyz>
