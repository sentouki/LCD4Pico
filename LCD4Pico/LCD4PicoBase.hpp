#pragma once
#include "pico/stdlib.h"
#include <string>
#include <bitset>

#define WRITE_ONLY UINT8_MAX

#define FUNCTION_SET 0b100000
#define DL_8BIT 0b10000
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

namespace lcd4pico
{
    template <uint8_t data_length>
    class LCD4PicoBase
    {
    protected:
        bool isFunctionSet = false;
        bool isInWriteMode = false;
        bool writeOnlyMode = true;
        bool firstInstruction = true; // flag indicating whether the instruction currently being executed is the first instruction; important for 4bit mode
    public:
        const uint8_t ENABLEPIN;
        const uint8_t RSPIN;
        const uint8_t RWPIN;
        const uint8_t (&DATAPINS)[data_length];

        // data pins order: D7,D6,D5,D4 (,D3,D2,D1,D0)
        LCD4PicoBase(uint8_t Enable_Pin,
                     uint8_t RS_Pin,
                     uint8_t RW_Pin,
                     const uint8_t (&Data_Pins)[data_length]) :

                                                                ENABLEPIN(Enable_Pin),
                                                                RSPIN(RS_Pin),
                                                                RWPIN(RW_Pin),
                                                                DATAPINS(Data_Pins)
        {
        }

        // ctor with the RW pin deactivated
        // data pins order: D7,D6,D5,D4 (,D3,D2,D1,D0)
        LCD4PicoBase(uint8_t Enable_Pin,
                     uint8_t RS_Pin,
                     const uint8_t (&Data_Pins)[data_length]) :

                                                                ENABLEPIN(Enable_Pin),
                                                                RSPIN(RS_Pin),
                                                                RWPIN(WRITE_ONLY),
                                                                DATAPINS(Data_Pins)
        {
        }

        void setup(uint8_t numOfdisplayLines = 2,
                   bool largeFont = false,
                   bool blinkingCursor = false,
                   bool cursorOn = true,
                   bool displayOn = true,
                   bool accompanyDisplayShift = false,
                   bool incrementCursor = true)
        {
            gpio_init(ENABLEPIN);
            gpio_init(RSPIN);
            if (RWPIN != WRITE_ONLY)
            {
                gpio_init(RWPIN);
                gpio_set_dir(RWPIN, GPIO_OUT);
                writeOnlyMode = false;
            }
            gpio_set_dir(ENABLEPIN, GPIO_OUT);
            gpio_set_dir(RSPIN, GPIO_OUT);
            gpio_put(ENABLEPIN, 0);
            gpio_put(RSPIN, 0);
            setFunctionMode(numOfdisplayLines, largeFont);
            displayControl(blinkingCursor, cursorOn, displayOn);
            setEntryMode(accompanyDisplayShift, incrementCursor);
        }

        void setFunctionMode(uint8_t numDisplayLines = 2, bool largeFont = false)
        {
            if (isFunctionSet)
                return;

            writeMode();
            gpio_put(RSPIN, 0);

            uint8_t data = FUNCTION_SET;
            if (data_length == 8)
                data |= DL_8BIT;
            else
            {
                writeData(data); // set operation mode to 4bit
            }
            firstInstruction = false;

            if (numDisplayLines == 2)
            {
                data |= TWO_DISPLAY_LINES;
            }
            if (largeFont && numDisplayLines == 1)
            {
                data |= FONT_5x10DOTS;
            }
            writeData(data);
            isFunctionSet = true;
        }

        void shiftDisplayOrCursor(std::string direction, bool display)
        {
            if (direction != "left" && direction != "right")
                return;

            writeMode();
            gpio_put(RSPIN, 0);

            uint8_t data = direction == "right" ? RIGHT_SHIFT : LEFT_SHIFT;
            if (display)
                data |= DISPLAY_SHIFT;

            writeData(data);
        }

        void setEntryMode(bool accompanyDisplayShift, bool incrementCursor)
        {
            writeMode();
            gpio_put(RSPIN, 0);

            uint8_t data = ENTRY_MODE_SET;
            if (accompanyDisplayShift)
                data |= ACCOMPANY_DISPLAY_SHIFT;
            if (incrementCursor)
                data |= INCREMENT_CURSOR;

            writeData(data);
        }

        void displayControl(bool blinkingCursor = true, bool cursorOn = true, bool displayOn = true)
        {
            writeMode();
            gpio_put(RSPIN, 0);

            uint8_t data = DISPLAY_CONTROL;
            if (blinkingCursor)
                data |= BLINKING_CURSOR;
            if (cursorOn)
                data |= CURSOR_ON;
            if (displayOn)
                data |= DISPLAY_ON;

            writeData(data);
        }

        // converts uint8 to its binary representation and returns it as string
        std::string intToBinString(uint8_t value)
        {
            std::string binary = std::bitset<8>(value).to_string();
            return binary;
        }

        void setCGRAM(uint8_t addr)
        {
            writeMode();
            gpio_put(RSPIN, 0);

            writeData(SET_CGRAM | addr);
        }

        void setDDRAM(uint8_t addr)
        {
            writeMode();
            gpio_put(RSPIN, 0);

            writeData(SET_DDRAM | addr);
        }

        void readMode()
        {
            if (!isInWriteMode)
                return;
            for (uint8_t pin : DATAPINS)
            {
                gpio_init(pin);
                gpio_set_dir(pin, GPIO_IN);
            }
            gpio_put(RWPIN, 1);
            isInWriteMode = false;
        }

        void writeMode()
        {
            if (isInWriteMode)
                return; // don't switch to write mode if it's alreay in it
            for (uint8_t pin : DATAPINS)
            {
                gpio_init(pin);
                gpio_set_dir(pin, GPIO_OUT);
            }
            gpio_put(RWPIN, 0);
            isInWriteMode = true;
        }

        void clockEnable(uint64_t pulseWidth_ns)
        {
            gpio_put(ENABLEPIN, 1);
            sleep_us(pulseWidth_ns);
            gpio_put(ENABLEPIN, 0);
        }

        void clockEnable()
        {
            gpio_put(ENABLEPIN, 1);
            sleep_us(50);
            gpio_put(ENABLEPIN, 0);
        }

        void setEnable(bool value)
        {
            gpio_put(ENABLEPIN, value);
        }

        // checks whether the LCD is busy processing instructions
        bool isBusy()
        {
            if (writeOnlyMode)
                return false;
            gpio_put(RSPIN, 0);
            readMode();

            setEnable(1);
            sleep_us(10);
            bool bf = gpio_get(DATAPINS[0]);
            setEnable(0);

            return bf;
        }

        // checks whether the LCD is busy processing instructions
        bool isBusy(uint8_t &addrCounter)
        {
            if (writeOnlyMode)
                return false;
            gpio_put(RSPIN, 0);

            uint8_t data;
            readData(data);
            bool bf = data & 0b10000000;
            return bf;
        }

        void readData(uint8_t &data)
        {
            if (writeOnlyMode)
                return;
            readMode();

            setEnable(1);
            sleep_us(10);
            std::string ac;
            for (uint8_t pin = 0; pin < data_length; pin++)
            {
                ac.append(gpio_get(DATAPINS[pin]) ? "1" : "0");
            }
            data = std::bitset<8>(ac).to_ulong();

            setEnable(0);
        }

        void writeData(uint8_t data)
        {
            if (!writeOnlyMode)
                waitWhileBusy(); // use busy flag checking if it's available as it's more safer

            writeMode();

            auto binData = intToBinString(data);
            for (uint8_t pin = 0; pin < data_length; pin++)
            {
                gpio_put(DATAPINS[pin], binData[pin] - '0');
            }
            clockEnable();

            if (data_length != 8 && !firstInstruction)
            {
                if (!writeOnlyMode)
                    waitWhileBusy();

                writeMode();
                for (uint8_t pin = 0, i = data_length; pin < data_length; pin++, i++)
                {
                    gpio_put(DATAPINS[pin], binData[i] - '0');
                }
                clockEnable();
            }
        }

    private:
        void waitWhileBusy()
        {
            if (isFunctionSet)
            {
                bool state = gpio_get(RSPIN); // save the current state of the RS pin
                while (isBusy())
                {
                    sleep_us(5);
                }
                gpio_put(RSPIN, state); // reset the state
            }
        }
    };
}
