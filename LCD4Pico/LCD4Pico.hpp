#pragma once
#include "pico/stdlib.h"
#include <string>
#include "LCD4PicoBase.hpp"

namespace lcd4pico
{
    template <const Bit_Mode bit_mode>
    class LCD4Pico : private LCD4PicoBase<bit_mode>
    {
    public:
        using LCD4PicoBase<bit_mode>::LCD4PicoBase;
        using LCD4PicoBase<bit_mode>::setup;

        void clearDisplay()
        {
            this->writeMode();
            gpio_put(this->RSPIN, 0);

            this->writeData(0x1);

            if (this->writeOnlyMode)
                sleep_ms(2);
        }

        void returnHome()
        {
            this->writeMode();
            gpio_put(this->RSPIN, 0);

            this->writeData(0x2);

            if (this->writeOnlyMode)
                sleep_ms(2);
        }

        // shifts the display Left or Right
        void shiftDisplay(Direction direction)
        {
            this->writeMode();
            gpio_put(this->RSPIN, 0);

            this->shiftDisplayOrCursor(direction, true);
        }
        // moves the cursor Left or Right
        void moveCursor(Direction direction)
        {
            this->writeMode();
            gpio_put(this->RSPIN, 0);

            this->shiftDisplayOrCursor(direction, false);
        }

        // moves the cursor to the head of the first line
        void toFirstLine()
        {
            this->setDDRAM(0);
        }
        // moves the cursor to the head of the second line
        void toSecondLine()
        {
            this->setDDRAM(0x40);
        }

        void write(std::string str)
        {
            this->writeMode();
            gpio_put(this->RSPIN, 1);

            for (auto s : str)
            {
                this->writeData(s);
            }
        }

        void writeLines(std::string firstLine, std::string secondLine)
        {
            this->writeMode();
            gpio_put(this->RSPIN, 1);

            write(firstLine);
            toSecondLine();
            write(secondLine);
        }
    };
}
