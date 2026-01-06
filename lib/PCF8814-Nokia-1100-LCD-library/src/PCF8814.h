/**
 * @file PCF8814.h
 *
 * @mainpage Arduino library for monochrome LCD (from the phone Nokia 1100) based on PCF8814 driver.
 *
 * @section Introduction
 *
 *  PCF8814 supports I²C, 4-wire and 3-wire SPI interfaces to communicate. 
 *  In this library used 3-wire SPI (just software, hardware in the future). 
 *  My LCD version (from the phone Nokia 1100) doesn't have 4-wire SPI and I²C.
 *
 *  PCF8814:
 *      - resolution: 96x65 
 *      - on-chip display data RAM (96x65 = 6240 bits)
 *      - pinout:
 * 
 *          1 RESET             1 2 3 4 5 6 7 8 9
 *          2 SCE              _|_|_|_|_|_|_|_|_|_
 *          3 GND             / _________________ \ 
 *          4 SDIN           | |                 | |
 *          4 SCLK           | |   PCF8814 LCD   | |
 *          5 VDDI           | |    from 1100    | |
 *          6 VDD            | |      96x65      | |
 *          7 LED+           | |_________________| |
 *          8 unused          \___________________/             
 * 
 * @section Dependencies
 *
 *  Adafruit GFX library
 *
 * @section Author
 *
 *  Written by Yaroslav @kashapovd Kashapov just for fun, may 2020
 *
 * @section License
 *
 *  GNU GENERAL PUBLIC LICENSE ver. 3
 * 
 */

#ifndef PCF8814_H
#define PCF8814_H

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif
#include <Adafruit_GFX.h>

#define PCF8814_WIDTH   96  
#define PCF8814_HEIGHT  65 

#define BLACK 1
#define WHITE 0

#define PCF8814_DATA    HIGH
#define PCF8814_CMD     LOW   

#define PCF8814_SOFTRST                 0b11100010

#define PCF8814_RAM_DATA_ORDER          0b10101000
    #define SWAP_ORDER                  0b00000000

#define PCF8814_ADDRESSING_MODE         0b10101010
    #define VERTICAL                    0b00000001
    #define HORIZONTAL                  0b00000000

#define PCF8814_BIAS                    0b00110000

#define PCF8814_TCOMPENSATION           0b11101010
    #define COMP_EN                     0b00000001
    #define COMP_DIS                    0b00000000           

#define PCF8814_POWER_CONTROL           0b00101000
    #define CHARGE_PUMP_ON              0b00000111    
    #define CHARGE_PUMP_OFF             0b00000000
    
// H - high word, L - low word    
#define PCF8814_CHARGE_PUMP_H           0b00111101
    #define PUMP_L_x2                   0b00000000
    #define PUMP_L_x3                   0b00000001
    #define PUMP_L_x4                   0b00000010
    #define PUMP_L_x5                   0b00000011

#define PCF8814_FRAME_FREQ_H            0b11101111
    #define FREQ_L_H80                  0b00000000
    #define FREQ_L_H70                  0b00000001
    #define FREQ_L_H60                  0b00000010
    #define FREQ_L_H40                  0b00000011

#define NOP                             0b11100011 
#define CONTROL_BYTE                    0b00000000

#define PCF8814_INTERNAL_OSCILL         0b00111010

#define PCF8814_MIRRORY                 0b11000010

#define PCF8814_DISPLAY_MODE            0b10100000
    #define ALLON                       0b00000100
    #define _NORMAL                     0b00000101
    #define OFF                         0b00001110
    #define ON                          0b00001111 
    #define INVERT                      0b00000111 
    #define NORMAL                      0b00000110

#define PCF8814_YADDR                   0b10110000
#define PCF8814_XADDR_L                 0b00000000  
#define PCF8814_XADDR_H                 0b00010000 

#define PCF8814_SYSTEM_BIAS              0b00110000
#define PCF8814_VOP_H                    0b00100000 
#define PCF8814_VOP_L                    0b10000000 

class PCF8814 : public Adafruit_GFX
{
    private:

        void spiwrite(uint8_t type, uint8_t byte);
        void hardreset(void);
        void setXY (const uint8_t x, const uint8_t y);
        // pins:
        int8_t _sce, _sclk, _sdin, _rst;
            
    public:

        // software 3-wire SPI
        PCF8814 (const uint8_t sce, const uint8_t sclk, const uint8_t sdin, const uint8_t rst);
        // // hardware 3-wire SPI: sce - 10, sclk - 13, sdin - 11
        // PCF8814 (const uint8_t rst);
        // // harware 3-wire SPI, but manual the sce pin.
        // PCF8814 (const uint8_t sce, const uint8_t rst);
        void begin(void);
        void displayOff(void);
        void displayOn(void);
        void setContrast(const uint8_t value);
        void invertDisplay(bool state);
        void clearDisplay(void);
        void display(void);
        uint8_t *getBuffer(void);
        uint16_t getBufferSize(void);
        uint8_t getPixel(const uint8_t x, const uint8_t y);
        void pushBuffer(uint8_t *buffer, uint16_t size);
        void drawPixel (int16_t x, int16_t y, 
                     uint16_t color); 
        void drawRect(int16_t x, int16_t y, 
                      int16_t w, int16_t h, 
                      uint16_t color);
        void fillRect(int16_t x, int16_t y, 
                      int16_t w, int16_t h, 
                      uint16_t color);
        void drawSquare(int16_t x, int16_t y, 
                      int16_t a, uint16_t color);
        void fillSquare(int16_t x, int16_t y, 
                      int16_t a, uint16_t color);
        void fillScreen(int16_t color);
};  
#endif

