#pragma once
#include "pico/stdlib.h"
#include <string>
#include "LCD4PicoBase.hpp"

namespace lcd4pico
{
    template <uint8_t data_length>
    class LCD4Pico : private LCD4PicoBase<data_length>
    {
    public:
        using LCD4PicoBase<data_length>::LCD4PicoBase;
        using LCD4PicoBase<data_length>::setup;

        void clearDisplay()
        {
            this->writeMode();
            gpio_put(this->RSPIN, 0);

            this->writeData(0x1);

            sleep_ms(2);
        }

        void returnHome()
        {
            this->writeMode();
            gpio_put(this->RSPIN, 0);

            this->writeData(0x2);
            
            sleep_ms(2);
        }

        // shifts the display "left" or "right"
        void shiftDisplay(std::string direction)
        {
            this->writeMode();
            gpio_put(this->RSPIN, 0);

            if (direction != "left" && direction != "right")
                return;
            this->shiftDisplayOrCursor(direction, true);
        }
        // moves the cursor "left" or "right"
        void moveCursor(std::string direction)
        {
            this->writeMode();
            gpio_put(this->RSPIN, 0);

            if (direction != "left" && direction != "right")
                return;
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
