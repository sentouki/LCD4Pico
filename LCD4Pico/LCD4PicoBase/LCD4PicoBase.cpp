#include "pico/stdlib.h"
#include "../Enums.hpp"
#include "LCD4PicoBase.hpp"

namespace lcd4pico
{
    template <const Bit_Mode bit_mode>
    LCD4PicoBase<bit_mode>::LCD4PicoBase(uint8_t Enable_Pin,
                                         uint8_t RS_Pin,
                                         uint8_t RW_Pin,
                                         const uint8_t (&Data_Pins)[bit_mode]) :

                                                                                 ENABLEPIN(Enable_Pin),
                                                                                 RSPIN(RS_Pin),
                                                                                 RWPIN(RW_Pin),
                                                                                 DATAPINS(Data_Pins)
    {
    }

    template <const Bit_Mode bit_mode>
    LCD4PicoBase<bit_mode>::LCD4PicoBase(uint8_t Enable_Pin,
                                         uint8_t RS_Pin,
                                         const uint8_t (&Data_Pins)[bit_mode]) :

                                                                                 ENABLEPIN(Enable_Pin),
                                                                                 RSPIN(RS_Pin),
                                                                                 RWPIN(WRITE_ONLY),
                                                                                 DATAPINS(Data_Pins)
    {
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::setup(uint8_t numOfdisplayLines,
                                       bool largeFont,
                                       bool blinkingCursor,
                                       bool cursorOn,
                                       bool displayOn,
                                       bool accompanyDisplayShift,
                                       bool incrementCursor)
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
        setEnable(0);
        setRegister(INSTRUCTION_REGISTER);

        setFunctionMode(numOfdisplayLines, largeFont);
        displayControl(blinkingCursor, cursorOn, displayOn);
        setEntryMode(accompanyDisplayShift, incrementCursor);
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::setFunctionMode(uint8_t numDisplayLines, bool largeFont)
    {
        if (isFunctionSet)
            return;

        writeMode();
        setRegister(INSTRUCTION_REGISTER);

        uint8_t data = FUNCTION_SET;
        if (bit_mode == _8BIT)
            data |= _8BIT_MODE;
        else
        {
            writeUpperNibble(data); // set operation mode to 4bit
        }

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

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::shiftDisplayOrCursor(Direction direction, bool display)
    {
        if (direction != Direction::Left && direction != Direction::Right)
            return;

        writeMode();
        setRegister(INSTRUCTION_REGISTER);

        uint8_t data = direction == Direction::Right ? RIGHT_SHIFT : LEFT_SHIFT;
        if (display)
            data |= DISPLAY_SHIFT;

        writeData(data);
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::setEntryMode(bool accompanyDisplayShift, bool incrementCursor)
    {
        writeMode();
        setRegister(INSTRUCTION_REGISTER);

        uint8_t data = ENTRY_MODE_SET;
        if (accompanyDisplayShift)
            data |= ACCOMPANY_DISPLAY_SHIFT;
        if (incrementCursor)
            data |= INCREMENT_CURSOR;

        writeData(data);
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::displayControl(bool blinkingCursor, bool cursorOn, bool displayOn)
    {
        writeMode();
        setRegister(INSTRUCTION_REGISTER);

        uint8_t data = DISPLAY_CONTROL;
        if (blinkingCursor)
            data |= BLINKING_CURSOR;
        if (cursorOn)
            data |= CURSOR_ON;
        if (displayOn)
            data |= DISPLAY_ON;

        writeData(data);
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::setCGRAM(uint8_t addr)
    {
        writeMode();
        setRegister(INSTRUCTION_REGISTER);

        writeData(SET_CGRAM | addr);
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::setDDRAM(uint8_t addr)
    {
        writeMode();
        setRegister(INSTRUCTION_REGISTER);

        writeData(SET_DDRAM | addr);
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::readMode()
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

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::writeMode()
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

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::pulseEnable(uint64_t pulseWidth_us)
    {
        setEnable(1);
        sleep_us(pulseWidth_us);
        setEnable(0);
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::pulseEnable()
    {
        setEnable(1);
        sleep_us(1);
        setEnable(0);
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::setEnable(bool value)
    {
        gpio_put(ENABLEPIN, value);
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::setRegister(bool reg)
    {
        gpio_put(RSPIN, reg);
    }

    template <const Bit_Mode bit_mode>
    bool LCD4PicoBase<bit_mode>::isBusy()
    {
        if (writeOnlyMode)
            return false;
        setRegister(INSTRUCTION_REGISTER);
        readMode();

        uint8_t data = readData();
        bool bf = data & BUSY_FLAG; // extract the busy-flag

        return bf;
    }

    template <const Bit_Mode bit_mode>
    bool LCD4PicoBase<bit_mode>::isBusy(uint8_t &addrCounter)
    {
        if (writeOnlyMode)
            return false;
        setRegister(INSTRUCTION_REGISTER);

        uint8_t data = readData();
        bool bf = data & BUSY_FLAG;           // extract the busy-flag
        addrCounter = data & ADDRESS_COUNTER; // extract address counter

        return bf;
    }

    template <const Bit_Mode bit_mode>
    uint8_t LCD4PicoBase<bit_mode>::readData()
    {
        if (writeOnlyMode)
            return 0;
        readMode();

        setEnable(1);
        sleep_us(1);

        uint8_t data = 0;

        for (uint8_t pin = 0; pin < bit_mode; pin++)
        {
            uint8_t bit = gpio_get(DATAPINS[pin]) ? 1 : 0;
            data |= bit << pin;
        }

        setEnable(0);
        if (bit_mode == _4BIT)
        {
            sleep_us(1);
            setEnable(1);
            sleep_us(1);

            data <<= 4;
            for (uint8_t pin = 0; pin < bit_mode; pin++)
            {
                uint8_t bit = gpio_get(DATAPINS[pin]) ? 1 : 0;
                data |= bit << pin;
            }

            setEnable(0);
        }
        return data;
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::writeData(uint8_t data)
    {
        if (!writeOnlyMode)
            waitWhileBusy(); // use busy flag checking if it's available as it's more safer
        else
            sleep_us(INSTRUCTION_WAITING_TIME);

        writeMode();

        for (uint8_t bit = bit_mode == _8BIT ? 1 : 16, pin = 0; pin < bit_mode; bit <<= 1, pin++)
        {
            gpio_put(DATAPINS[pin], data & bit);
        }

        pulseEnable();

        if (bit_mode == _4BIT)
        {
            for (uint8_t bit = 1, pin = 0; pin < bit_mode; bit <<= 1, pin++)
            {
                gpio_put(DATAPINS[pin], data & bit);
            }
            pulseEnable();
        }
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::waitWhileBusy()
    {
        if (isFunctionSet)
        {
            bool state = gpio_get(RSPIN); // save the current state of the RS pin
            while (isBusy())
            {
                sleep_us(1);
            }
            gpio_put(RSPIN, state); // reset the state
        }
        else
            sleep_us(INSTRUCTION_WAITING_TIME);
    }

    template <const Bit_Mode bit_mode>
    void LCD4PicoBase<bit_mode>::writeUpperNibble(uint8_t data)
    {
        if (!writeOnlyMode)
            waitWhileBusy();
        else
            sleep_us(INSTRUCTION_WAITING_TIME);

        writeMode();

        for (uint8_t bit = 16, pin = 0; pin < 4; bit <<= 1, pin++)
        {
            gpio_put(DATAPINS[pin], data & bit);
        }

        pulseEnable();
    }
}
