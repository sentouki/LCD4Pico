#pragma once
#include "pico/stdlib.h"
#include <string>
#include "LCD4PicoBase/LCD4PicoBase.hpp"

namespace lcd4pico
{
    template <const Bit_Mode bit_mode>
    class LCD4Pico : private LCD4PicoBase<bit_mode>
    {
    public:
        using LCD4PicoBase<bit_mode>::LCD4PicoBase;
        using LCD4PicoBase<bit_mode>::setup;
        using LCD4PicoBase<bit_mode>::setEntryMode;
        using LCD4PicoBase<bit_mode>::displayControl;

        /**
         * @brief Clears entire display and moves the cursor to the head of the first line.
         *
         */
        void clearDisplay();

        /**
         * @brief Moves the cursor to the head of the first line and resets the display shift.
         *
         */
        void returnHome();

        /**
         * @brief Shifts the display, together with content and cursor, Left or Right.
         *
         */
        void shiftDisplay(Direction direction);

        /**
         * @brief Moves the cursor Left or Right.
         *
         */
        void moveCursor(Direction direction);

        /**
         * @brief Moves the cursor to a specific position on the display.
         *
         * @param displayPosition Position on the display, e.g. 0-15 for the first line, 64-79 for the second line.
         */
        void moveCursorTo(uint8_t displayPosition);

        /**
         * @brief Moves the cursor to the head of the first line.
         *
         */
        void toFirstLine();

        /**
         * @brief Moves the cursor to the head of the second line.
         *
         */
        void toSecondLine();

        /**
         * @brief Writes a string to the display.
         *
         * @param str ASCII string
         */
        void write(std::string str);

        /**
         * @brief Moves the cursor to the head of the first line and writes the `firstLine` to the display,
         *        then moves the cursor to the head of the second line and writes `secondLine` to the display.
         *
         * @param firstLine String to be displayed on the first line of the display.
         * @param secondLine String to be displayed on the second line of the display.
         */
        void writeLines(std::string firstLine, std::string secondLine);

        /**
         * @brief Create a Custom Character.
         *
         * @param index At which index should the character be saved. Indices from 0 to 7 are available.
         * @param character An array of 8 of 5 bits each that represents a character pattern (5x8).
         */
        void createCustomCharacter(uint8_t index, const uint8_t (&character)[8]);

        /**
         * @brief Writes a Custom Character to the display.
         *
         * @param index Index used to save the character with the `createCustomCharacter` method.
         */
        void writeCustomCharacter(uint8_t index);
    };
}

#include "LCD4Pico.cpp"
