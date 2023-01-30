#include "pico/stdlib.h"
#include <string>
#include "LCD4Pico.hpp"

namespace lcd4pico
{
    template <const Bit_Mode bit_mode>
    void LCD4Pico<bit_mode>::clearDisplay()
    {
        this->writeMode();
        this->setRegister(INSTRUCTION_REGISTER);

        this->writeData(0x1);

        if (this->writeOnlyMode)
            sleep_ms(2);
    }

    template <const Bit_Mode bit_mode>
    void LCD4Pico<bit_mode>::returnHome()
    {
        this->writeMode();
        this->setRegister(INSTRUCTION_REGISTER);

        this->writeData(0x2);

        if (this->writeOnlyMode)
            sleep_ms(2);
    }

    template <const Bit_Mode bit_mode>
    void LCD4Pico<bit_mode>::shiftDisplay(Direction direction)
    {
        this->writeMode();
        this->setRegister(INSTRUCTION_REGISTER);

        this->shiftDisplayOrCursor(direction, true);
    }

    template <const Bit_Mode bit_mode>
    void LCD4Pico<bit_mode>::moveCursor(Direction direction)
    {
        this->writeMode();
        this->setRegister(INSTRUCTION_REGISTER);

        this->shiftDisplayOrCursor(direction, false);
    }

    template <const Bit_Mode bit_mode>
    void LCD4Pico<bit_mode>::moveCursorTo(uint8_t displayPosition)
    {
        this->setDDRAM(displayPosition);
    }

    template <const Bit_Mode bit_mode>
    void LCD4Pico<bit_mode>::toFirstLine()
    {
        this->setDDRAM(0);
    }

    template <const Bit_Mode bit_mode>
    void LCD4Pico<bit_mode>::toSecondLine()
    {
        this->setDDRAM(0x40);
    }

    template <const Bit_Mode bit_mode>
    void LCD4Pico<bit_mode>::write(std::string str)
    {
        this->writeMode();
        this->setRegister(DATA_REGISTER);

        for (auto s : str)
        {
            this->writeData(s);
        }
    }

    template <const Bit_Mode bit_mode>
    void LCD4Pico<bit_mode>::writeLines(std::string firstLine, std::string secondLine)
    {
        toFirstLine();
        write(firstLine);
        toSecondLine();
        write(secondLine);
    }

    template <const Bit_Mode bit_mode>
    void LCD4Pico<bit_mode>::createCustomCharacter(uint8_t index, const uint8_t (&character)[8])
    {
        if (index > 7)
            return;

        this->setCGRAM(0x40 + (index * 8));
        this->setRegister(DATA_REGISTER);

        for (auto c : character)
        {
            this->writeData(c);
        }

        this->setDDRAM(0); // restore
    }

    template <const Bit_Mode bit_mode>
    void LCD4Pico<bit_mode>::writeCustomCharacter(uint8_t index)
    {
        this->setRegister(DATA_REGISTER);
        this->writeData(index);
    }
}
