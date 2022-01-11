#pragma once
#include "pico/stdlib.h"
#include <string>
#include <bitset>

namespace lcd4pico
{
    template <uint8_t data_length>
    class LCD4PicoBase
    {
    public:
        const uint8_t ENABLEPIN;
        const uint8_t RSPIN;
        const uint8_t RWPIN;
        const uint8_t (&DATAPINS)[data_length];
        bool isFunctionSet = false;

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

        void setup(uint8_t displayLines = 2,
                   bool largeFont = false,
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
            setFunctionMode(displayLines, largeFont);
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

        void setFunctionMode(uint8_t numDisplayLines = 2, bool largeFont = false)
        {
            if (isFunctionSet)
                return;

            writeMode();
            gpio_put(RSPIN, 0);

            uint8_t data = 0b100000;
            if (data_length == 8)
                data |= 0b10000;
            else
                writeData(data); // set operation mode to 4bit

            isFunctionSet = true;
            if (numDisplayLines == 2)
            {
                data |= 0b1000;
            }
            if (largeFont && numDisplayLines == 1)
            {
                data |= 0b100;
            }
            writeData(data);
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
            return binary;
        }

        void setCGRAM(uint8_t addr)
        {
            writeMode();
            gpio_put(RSPIN, 0);

            writeData(0b1000000 | addr);
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
            for (uint8_t pin = 0; pin < data_length; pin++)
            {
                gpio_put(DATAPINS[pin], binData[pin] - '0');
            }
            clockEnable();
            if (data_length != 8 && isFunctionSet)
            {
                for (uint8_t pin = 0, i = data_length; pin < data_length; pin++, i++)
                {
                    gpio_put(DATAPINS[pin], binData[i] - '0');
                }
                clockEnable();
            }
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
