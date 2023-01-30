#pragma once
#include "pico/stdlib.h"
#include "../Enums.hpp"

#define WRITE_ONLY UINT8_MAX

#ifndef INSTRUCTION_WAITING_TIME
#define INSTRUCTION_WAITING_TIME 50
#endif

#define INSTRUCTION_REGISTER 0
#define DATA_REGISTER 1

#define FUNCTION_SET 0b100000
#define _8BIT_MODE 0b10000
#define TWO_DISPLAY_LINES 0b1000
#define FONT_5x10DOTS 0b100

#define DISPLAY_SHIFT 0b11000
#define RIGHT_SHIFT 0b10100
#define LEFT_SHIFT 0b10000

#define ENTRY_MODE_SET 0b100
#define ACCOMPANY_DISPLAY_SHIFT 0b1
#define INCREMENT_CURSOR 0b10

#define DISPLAY_CONTROL 0b1000
#define BLINKING_CURSOR 0b1
#define CURSOR_ON 0b10
#define DISPLAY_ON 0b100

#define SET_CGRAM 0b1000000
#define SET_DDRAM 0b10000000

#define BUSY_FLAG 0b10000000
#define ADDRESS_COUNTER 0b01111111

namespace lcd4pico
{
    template <const Bit_Mode bit_mode>
    class LCD4PicoBase
    {
    protected:
        bool isFunctionSet = false;
        bool isInWriteMode = false;
        bool writeOnlyMode = true;

    public:
        const uint8_t ENABLEPIN;
        const uint8_t RSPIN;
        const uint8_t RWPIN;
        const uint8_t (&DATAPINS)[bit_mode];

        /**
         * @brief Construct a new LCD4PicoBase object.
         * 
         * @param Data_Pins Data pins order: (D0,D1,D2,D3,) D4,D5,D6,D7 .
         */
        LCD4PicoBase(uint8_t Enable_Pin,
                     uint8_t RS_Pin,
                     uint8_t RW_Pin,
                     const uint8_t (&Data_Pins)[bit_mode]);

        /**
         * @brief Construct a new object without the RW pin (write only mode; not recommended).
         * 
         * @param Data_Pins Data pins order: (D0,D1,D2,D3,) D4,D5,D6,D7 .
         */
        LCD4PicoBase(uint8_t Enable_Pin,
                     uint8_t RS_Pin,
                     const uint8_t (&Data_Pins)[bit_mode]);

        /**
         * @brief 
         * 
         * @param numOfdisplayLines Number of display lines, 1 or 2, default is 2.
         * @param largeFont Large font (5x10; 1 Line mode only) or small font (5x8; default).
         * @param blinkingCursor Should the cursor blink (default is false).
         * @param cursorOn Show cursor (default is true).
         * @param displayOn Turn the display on or off (default is true).
         * @param accompanyDisplayShift If true, text on the display shifts left or right (depends on `incerementCursor` setting) 
         *                              while the cursor stands still (default is false).
         * @param incrementCursor Increment (cursor moves right) or decrement (cursor moves left) the cursor (default is true).
         */
        void setup(uint8_t numOfdisplayLines = 2,
                   bool largeFont = false,
                   bool blinkingCursor = false,
                   bool cursorOn = true,
                   bool displayOn = true,
                   bool accompanyDisplayShift = false,
                   bool incrementCursor = true);

        void setFunctionMode(uint8_t numDisplayLines = 2, bool largeFont = false);

        void shiftDisplayOrCursor(Direction direction, bool display);

        void setEntryMode(bool accompanyDisplayShift, bool incrementCursor);

        void displayControl(bool blinkingCursor, bool cursorOn, bool displayOn);

        /**
         * @brief Sets the CGRAM address.
         * 
         */
        void setCGRAM(uint8_t addr);

        /**
         * @brief Sets the DDRAM address.
         * 
         * @param addr 
         */
        void setDDRAM(uint8_t addr);

        void readMode();

        void writeMode();

        void pulseEnable(uint64_t pulseWidth_us);

        void pulseEnable();

        void setEnable(bool value);

        /**
         * @brief Set the Register either to `INSTRUCTION_REGISTER` or `DATA_REGISTER`.
         * 
         * @param reg `INSTRUCTION_REGISTER` (0) or `DATA_REGISTER` (1).
         */
        void setRegister(bool reg);

        /**
         * @brief Checks whether the LCD is busy processing instructions.
         * 
         * @return true The display is busy processing current instruction and cannot accept any new instruction now.
         * @return false The display accepts new instructions
         */
        bool isBusy();

        /**
         * @brief Checks whether the LCD is busy processing instructions.
         * 
         * @param addrCounter Address Counter
         * @return true The display is busy processing current instruction and cannot accept any new instruction now.
         * @return false The display accepts new instructions
         */
        bool isBusy(uint8_t &addrCounter);

        uint8_t readData();

        void writeData(uint8_t data);

    private:
        void waitWhileBusy();

        // For 4bit mode only
        void writeUpperNibble(uint8_t data);
    };
}

#include "LCD4PicoBase.cpp"
