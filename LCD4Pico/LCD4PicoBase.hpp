#pragma once
#include "pico/stdlib.h"
#include <vector>
#include <string>
#include <bitset>
#include <algorithm>

#define DATABITSIZE 8

namespace lcd4pico
{
    class LCD4PicoBase
    {
    public:
        const uint8_t ENABLEPIN;
        const uint8_t RSPIN;
        const uint8_t RWPIN;
        const uint8_t (&DATAPINS)[8];
        bool isFunctionSet = false;

        LCD4PicoBase(uint8_t Enable_Pin,
                     uint8_t RS_Pin,
                     uint8_t RW_Pin,
                     const uint8_t (&Data_Pins)[8]) : ENABLEPIN(Enable_Pin),
                                                      RSPIN(RS_Pin),
                                                      RWPIN(RW_Pin),
                                                      DATAPINS(Data_Pins) {}

        void setup(uint8_t displayLines = 2,
                   bool blinkingCursor = true,
                   bool cursorOn = true,
                   bool displayOn = true,
                   bool accompanyDisplayShift = false,
                   bool incrementCursor = true)
        {
            gpio_init(ENABLEPIN);
            gpio_init(RSPIN);
            gpio_init(RWPIN);
            gpio_set_dir(ENABLEPIN, GPIO_OUT);
            gpio_set_dir(RSPIN, GPIO_OUT);
            gpio_set_dir(RWPIN, GPIO_OUT);
            gpio_put(ENABLEPIN, 0);
            gpio_put(RSPIN, 0);
            setFunctionMode(displayLines);
            displayControl(blinkingCursor, cursorOn, displayOn);
            setEntryMode(accompanyDisplayShift, incrementCursor);
        }

        void shiftDisplayOrCursor(std::string direction, bool display)
        {
            if (direction != "left" && direction != "right")
                return;

            writeMode();
            gpio_put(RSPIN, 0);

            uint8_t data = direction == "right" ? 0b10100 : 0b10000;
            if (display)
                data |= 0b11000;

            writeData(data);
        }

        void setFunctionMode(uint8_t numDisplayLines = 2)
        {
            if (isFunctionSet)
                return;
            isFunctionSet = true;

            writeMode();
            gpio_put(RSPIN, 0);

            if (numDisplayLines == 2)
            {
                writeData(0b00110000 | 0b00001000);
            }
            else
            {
                writeData(0b00110000 | 0b00000100);
            }
        }

        void setEntryMode(bool accompanyDisplayShift, bool incrementCursor)
        {
            writeMode();
            gpio_put(RSPIN, 0);

            uint8_t data = 0b100;
            if (accompanyDisplayShift)
                data |= 0b1;
            if (incrementCursor)
                data |= 0b10;

            writeData(data);
        }

        void displayControl(bool blinkingCursor = true, bool cursorOn = true, bool displayOn = true)
        {
            writeMode();
            gpio_put(RSPIN, 0);

            uint8_t data = 0b1000;
            if (blinkingCursor)
                data |= 0b1;
            if (cursorOn)
                data |= 0b10;
            if (displayOn)
                data |= 0b100;

            writeData(data);
        }

        // converts uint8 to its binary representation and returns it as string
        std::string intToBinString(uint8_t value)
        {
            std::string binary = std::bitset<8>(value).to_string();
            std::reverse(binary.begin(), binary.end());
            return binary;
        }

        void setCGRAM(uint8_t addr)
        {
            writeMode();
            gpio_put(RSPIN, 0);

            writeData(0b01000000 | addr);
        }

        void setDDRAM(uint8_t addr)
        {
            writeMode();
            gpio_put(RSPIN, 0);

            writeData(0b10000000 | addr);
        }

        void writeData(uint8_t data)
        {
            auto binData = intToBinString(data);
            for (uint8_t pin = 0; pin < DATABITSIZE; pin++)
            {
                gpio_put(DATAPINS[pin], binData[pin] - '0');
            }
            clockEnable();
        }

        void readMode()
        {
            for (uint8_t pin : DATAPINS)
            {
                gpio_init(pin);
                gpio_set_dir(pin, GPIO_IN);
            }
            gpio_put(RWPIN, 1);
        }

        void writeMode()
        {
            for (uint8_t pin : DATAPINS)
            {
                gpio_init(pin);
                gpio_set_dir(pin, GPIO_OUT);
            }
            gpio_put(RWPIN, 0);
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
            readMode();
            gpio_put(RSPIN, 0);

            setEnable(1);
            bool bf = gpio_get(DATAPINS[7]);
            setEnable(0);
            return bf;
        }

        // checks whether the LCD is busy processing instructions
        bool isBusy(uint8_t &addrCounter)
        {
            readMode();
            gpio_put(RSPIN, 0);

            setEnable(1);
            bool bf = gpio_get(DATAPINS[7]);

            std::string ac;
            for (uint8_t pin = 6; pin > 0; pin--)
            {
                ac.append(gpio_get(DATAPINS[pin]) ? "1" : "0");
            }
            addrCounter = std::bitset<8>(ac).to_ulong();

            setEnable(0);
            return bf;
        }
    };
}
