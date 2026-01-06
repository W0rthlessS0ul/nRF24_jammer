/**
 * @file PCF8814.cpp
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

#include "PCF8814.h"
#include <SPI.h>
#include <Wire.h> // just for adafruit gfx lib [damn it], don't pay attention

#define PCF8814_BYTES_CAPACITY  PCF8814_WIDTH * (PCF8814_HEIGHT + 7) / 8
#define COLUMNS                 PCF8814_WIDTH
#define PAGES                   PCF8814_BYTES_CAPACITY / COLUMNS

#if defined(ARDUINO) && ARDUINO >= 100  
    // faster than standard digitalWrite(), estimated +7-8 fps 
    #define SETPIN(pin)         *(portOutputRegister(digitalPinToPort(pin))) |= digitalPinToBitMask(pin)
    #define CLRPIN(pin)         *(portOutputRegister(digitalPinToPort(pin))) &= ~digitalPinToBitMask(pin)
    #define DC(pin, type)       (type) ? SETPIN(pin):CLRPIN(pin);
#else // for ESP
    #define SETPIN(pin)         digitalWrite(pin, HIGH)
    #define CLRPIN(pin)         digitalWrite(pin, LOW)
    #define DC(pin, type)       digitalWrite(pin, type)
#endif
#define SETRST(pin)             SETPIN(pin)
#define CLRRST(pin)             CLRPIN(pin)
#define SETMOSI(pin)            SETPIN(pin) 
#define CLRMOSI(pin)            CLRPIN(pin)
#define SETCS(pin)              SETPIN(pin) 
#define CLRCS(pin)              CLRPIN(pin)
#define SETCLK(pin)             SETPIN(pin) 
#define CLRCLK(pin)             CLRPIN(pin)  
#define TGLCLK(pin)             CLRCLK(pin);\
                                SETCLK(pin)

uint8_t _buffer[PCF8814_BYTES_CAPACITY];

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//                        CONSTRUCTOR                           //   
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

PCF8814::PCF8814(const uint8_t sce, const uint8_t sclk, 
                      const uint8_t sdin, const uint8_t rst) : 
                      Adafruit_GFX (PCF8814_WIDTH, PCF8814_HEIGHT) {
    _sce = sce;
    _sclk = sclk;
    _sdin = sdin;
    _rst = rst;
}

/**************************************************************/
/** @brief This method makes hardware reset. 
*/
/**************************************************************/
inline void PCF8814::hardreset() {

    CLRRST(_rst);            // Bring reset low
    delay(10);                           // Wait 10ms
    SETRST(_rst);         // Bring out of reset
    delay(10);                           // Wait 10ms
}

/**************************************************************/
/** @brief This method makes bitbang spi write 
    @param type byte is data or command
    @param byte byte to be written
*/
/**************************************************************/
inline void PCF8814::spiwrite(uint8_t type, uint8_t byte) {

    CLRCS(_sce);
    DC(_sdin, type);  // next byte is data or cmd << see datasheet. it is necessary for each byte
	TGLCLK(_sclk);
    for (uint8_t bit = 0; bit < 8; bit++) {

        (byte & 0x80) ? SETMOSI(_sdin):CLRMOSI(_sdin);
        byte<<=1;
        TGLCLK(_sclk);
    }
    SETCS(_sce);
}

/**************************************************************/
/** @brief This method sets x[0...96](columns) and 
           y[0...9](pages) address of RAM.
*/
/**************************************************************/
void PCF8814::setXY(const uint8_t x, const uint8_t y) {
    
    spiwrite(PCF8814_CMD,PCF8814_XADDR_L | (x & 0b00001111));
    spiwrite(PCF8814_CMD,PCF8814_XADDR_H | (5 >> (x & 0b11100000)));
    spiwrite(PCF8814_CMD,PCF8814_YADDR | y);
}

/**************************************************************/
/** @brief This method makes initial display setup
*/
/**************************************************************/
void PCF8814::begin() {

    pinMode(_sdin, OUTPUT);
    pinMode(_sclk, OUTPUT);
    pinMode(_sce, OUTPUT);
    pinMode(_rst, OUTPUT);

    SETCS(_sce);
    SETCLK(_sclk);

    clearDisplay(); // clear the whole buffer
    hardreset();
    spiwrite(PCF8814_CMD,PCF8814_POWER_CONTROL|CHARGE_PUMP_ON);
    spiwrite(PCF8814_CMD,PCF8814_ADDRESSING_MODE | HORIZONTAL);
    spiwrite(PCF8814_CMD,PCF8814_TCOMPENSATION | COMP_EN); // enable temerature compensation
    spiwrite(PCF8814_CMD,PCF8814_FRAME_FREQ_H); // high word
    spiwrite(PCF8814_CMD,FREQ_L_H80);   // low word. 80Hz frame frequency
    spiwrite(PCF8814_CMD,PCF8814_INTERNAL_OSCILL); // use internal oscillator
    spiwrite(PCF8814_CMD,PCF8814_CHARGE_PUMP_H);
    spiwrite(PCF8814_CMD,PUMP_L_x2);
    spiwrite(PCF8814_CMD,PCF8814_SYSTEM_BIAS);
    spiwrite(PCF8814_CMD,PCF8814_VOP_H|0x04); // default
    spiwrite(PCF8814_CMD,PCF8814_VOP_L|0x10); // contrast  
    //spiwrite(PCF8814_CMD,PCF8814_YADDR);
    //spiwrite(PCF8814_CMD,PCF8814_XADDR_L);
    //spiwrite(PCF8814_CMD,PCF8814_XADDR_H);
    display(); // write empty buffer
    spiwrite(PCF8814_CMD,PCF8814_DISPLAY_MODE | ON);
    spiwrite(PCF8814_CMD,PCF8814_DISPLAY_MODE | ALLON);
}

/**************************************************************/
/** @brief Just display off, not power down
*/
/**************************************************************/
void PCF8814::displayOff(void) {
    spiwrite(PCF8814_CMD,PCF8814_DISPLAY_MODE | OFF);
}

void PCF8814::displayOn(void) {
    spiwrite(PCF8814_CMD,PCF8814_DISPLAY_MODE | ON);
}

void PCF8814::setContrast(const uint8_t value) {

    spiwrite(PCF8814_CMD,PCF8814_VOP_H | 0x04);
    spiwrite(PCF8814_CMD,PCF8814_VOP_L | (value & 0b00001111));
}

/****************************************************************/
/** @brief  Activate or deactivate the inverse video mode. 
            If true, your pixel will be black, if bit in 
            RAM is zero. And vice versa
    @param  state   true - mode is on, false - off
*/
/****************************************************************/
void PCF8814::invertDisplay(bool state) {
    spiwrite(PCF8814_CMD,PCF8814_DISPLAY_MODE|NORMAL|state);
}

/**************************************************************/
/** @brief  This method sets all framebuffer bits to zero
*/
/**************************************************************/
void PCF8814::clearDisplay(void) { 
    memset(_buffer, 0x00, PCF8814_BYTES_CAPACITY); 
}

void PCF8814::display() {

    for (uint8_t page = 0; page < PAGES; page++) {

        for (uint8_t column = 0; column < PCF8814_WIDTH; column++) {

            spiwrite(PCF8814_DATA, _buffer[page*COLUMNS+column]);
        }   
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//                     FEEDBACK FUNCTIONS                       //   
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

uint8_t PCF8814::getPixel(const uint8_t x, const uint8_t y) {

    // Apply rotation transformation
    int16_t rotated_x = x;
    int16_t rotated_y = y;
    
    switch(rotation) {
        case 1:  // 90 degrees clockwise
            rotated_x = PCF8814_HEIGHT - 1 - y;
            rotated_y = x;
            break;
        case 2:  // 180 degrees
            rotated_x = PCF8814_WIDTH - 1 - x;
            rotated_y = PCF8814_HEIGHT - 1 - y;
            break;
        case 3:  // 270 degrees clockwise
            rotated_x = y;
            rotated_y = PCF8814_WIDTH - 1 - x;
            break;
        case 0:  // No rotation
        default:
            break;
    }

    if ((rotated_x >= 0 && rotated_x < PCF8814_WIDTH) 
        && (rotated_y >= 0 && rotated_y < PCF8814_HEIGHT)) {

        return ((1 >> rotated_y%8) & _buffer[rotated_x + (rotated_y/8) * PCF8814_WIDTH]);
    }
    
    return 0;
}
/**************************************************************/
/** @brief Get memory pointer to the framebuffer
    @return Pointer to the first framebuffer's element 
*/
/**************************************************************/
uint8_t *PCF8814::getBuffer(void) { 
    return _buffer;
}

/**************************************************************/
/** @brief Get size of the framebuffer in bytes
    @return Size in bytes
*/
/**************************************************************/
uint16_t PCF8814::getBufferSize(void) {
    return PCF8814_BYTES_CAPACITY; 
}

/***************************************************************/
/** @brief Push another buffer
*/
/***************************************************************/
void PCF8814::pushBuffer(uint8_t *buffer, const uint16_t size) { 
    memmove(_buffer, buffer, size); 
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//                      DRAWING FUNCTIONS                       //   
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/****************************************************************/
/** @brief  Draw one pixel to the framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  color 
*/
/****************************************************************/
void PCF8814::drawPixel (int16_t x, int16_t y, 
                     uint16_t color) {

    // Apply rotation transformation
    int16_t rotated_x = x;
    int16_t rotated_y = y;
    
    switch(rotation) {
        case 1:  // 90 degrees clockwise
            rotated_x = PCF8814_HEIGHT - 1 - y;
            rotated_y = x;
            break;
        case 2:  // 180 degrees
            rotated_x = PCF8814_WIDTH - 1 - x;
            rotated_y = PCF8814_HEIGHT - 1 - y;
            break;
        case 3:  // 270 degrees clockwise
            rotated_x = y;
            rotated_y = PCF8814_WIDTH - 1 - x;
            break;
        case 0:  // No rotation
        default:
            break;
    }

    if ((rotated_x >= 0 && rotated_x < PCF8814_WIDTH) 
    && (rotated_y >= 0 && rotated_y < PCF8814_HEIGHT)) {

        color ?
        _buffer[rotated_x + (rotated_y/8) * PCF8814_WIDTH] |= (1 << rotated_y%8)
        :
        _buffer[rotated_x + (rotated_y/8) * PCF8814_WIDTH] &= ~(1 << rotated_y%8);
    }
}

/****************************************************************/
/** @brief  Draw rectangle to the framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  w   rectangle width
    @param  h   rectangle height
    @param  color 
*/
/****************************************************************/
void PCF8814::drawRect(int16_t x, int16_t y, 
                      int16_t w, int16_t h, 
                      uint16_t color) {
    
    for (int i = x; i < w+x; i++) {

        drawPixel(i, y, color);
        drawPixel(i, y+h-1, color);
    }
    for (int i = y; i < h+y; i++) {

        drawPixel(x, i, color);
        drawPixel(x+w-1, i, color);
    }
}

/****************************************************************/
/** @brief  Draw a filled rectangle to the framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  w   rectangle width
    @param  h   rectangle height
    @param  color 
*/
/****************************************************************/
void PCF8814::fillRect(int16_t x, int16_t y, 
                      int16_t w, int16_t h, 
                      uint16_t color) {

    for (int i = y; i < h+y; i++) {

        for (int j = x; j < w+x; j++) {

            drawPixel(j, i, color);
        }
    }
}

/****************************************************************/
/** @brief  Draw a square to the framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  a   square width
    @param  color 
*/
/****************************************************************/
void PCF8814::drawSquare(int16_t x, int16_t y, 
                      int16_t a, uint16_t color) {
    drawRect(x, y, a, a, color); 
}

/****************************************************************/
/** @brief  Draw a filled square to the framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  a   square width
    @param  color 
*/
/****************************************************************/
void PCF8814::fillSquare(int16_t x, int16_t y, 
                      int16_t a, uint16_t color) {
     fillRect(x, y, a, a, color); 
}

/***************************************************************/
/** @brief Fill all framebuffer 
*/
/***************************************************************/
void PCF8814::fillScreen(int16_t color) {
     memset(_buffer, color ? 0xFF : 0x00, PCF8814_BYTES_CAPACITY); 
}