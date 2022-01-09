#pragma once
#include "pico/stdlib.h"
#include <vector>
#include <string>
#include "LCD4PicoBase.hpp"

namespace lcd4pico
{
    class LCD4Pico : private LCD4PicoBase
    {
    public:
        using LCD4PicoBase::LCD4PicoBase;
        using LCD4PicoBase::setup;

        void clearDisplay()
        {
            writeMode();
            gpio_put(RSPIN, 0);

            writeData(0b1);

            sleep_ms(2);
        }

        void returnHome()
        {
            writeMode();
            gpio_put(RSPIN, 0);

            writeData(0b10);

            sleep_ms(2);
        }

        // shifts the display "left" or "right"
        void shiftDisplay(std::string direction)
        {
            writeMode();
            gpio_put(RSPIN, 0);

            if (direction != "left" && direction != "right")
                return;
            shiftDisplayOrCursor(direction, true);
        }
        // moves the cursor "left" or "right"
        void moveCursor(std::string direction)
        {
            writeMode();
            gpio_put(RSPIN, 0);

            if (direction != "left" && direction != "right")
                return;
            shiftDisplayOrCursor(direction, false);
        }

        // moves the cursor to the head of the first line
        void toFirstLine()
        {
            setDDRAM(0);
        }
        // moves the cursor to the head of the second line
        void toSecondLine()
        {
            setDDRAM(0x40);
        }

        void write(std::string str)
        {
            writeMode();
            gpio_put(RSPIN, 1);

            for (auto s : str)
            {
                writeData(s);
            }
        }

        void writeLines(std::string firstLine, std::string secondLine)
        {
            writeMode();
            gpio_put(RSPIN, 1);

            write(firstLine);
            toSecondLine();
            write(secondLine);
        }
    };
}
