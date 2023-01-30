#pragma once
#include "pico/stdlib.h"

namespace lcd4pico
{
    namespace symbols
    {
        const uint8_t bell[] =
            {
                0b00100,
                0b01110,
                0b01110,
                0b01110,
                0b11111,
                0b00000,
                0b00100,
                0b00000};

        const uint8_t note[] =
            {
                0b00000,
                0b00011,
                0b00010,
                0b00010,
                0b01110,
                0b11110,
                0b01100,
                0b00000};

        const uint8_t note2[] =
            {
                0b00001,
                0b00011,
                0b00101,
                0b01001,
                0b01001,
                0b01011,
                0b11011,
                0b11000};

        const uint8_t checkMark[] =
            {
                0b00000,
                0b00001,
                0b00011,
                0b10110,
                0b11100,
                0b01000,
                0b00000,
                0b00000};

        const uint8_t heart[] =
            {
                0b00000,
                0b01010,
                0b11111,
                0b11111,
                0b01110,
                0b00100,
                0b00000,
                0b00000};

        const uint8_t clock[] =
            {
                0b00000,
                0b01110,
                0b10101,
                0b10111,
                0b10001,
                0b01110,
                0b00000,
                0b00000};

        const uint8_t smile[] =
            {
                0b00000,
                0b01010,
                0b00000,
                0b10001,
                0b01110,
                0b00000,
                0b00000,
                0b00000};

        const uint8_t neutral[] =
            {
                0b00000,
                0b01010,
                0b00000,
                0b00000,
                0b11111,
                0b00000,
                0b00000,
                0b00000};

        const uint8_t sad[] =
            {
                0b00000,
                0b01010,
                0b00000,
                0b00000,
                0b01110,
                0b10001,
                0b00000,
                0b00000};

        const uint8_t checked[] =
            {
                0b01010,
                0b10101,
                0b01010,
                0b10101,
                0b01010,
                0b10101,
                0b01010,
                0b10101};

        const uint8_t lock[] =
            {
                0b01110,
                0b10001,
                0b10001,
                0b11111,
                0b11011,
                0b11011,
                0b11111,
                0b00000};

        const uint8_t lock2[] =
            {
                0b01110,
                0b10000,
                0b10000,
                0b11111,
                0b11011,
                0b11011,
                0b11111,
                0b00000};

        const uint8_t speaker[] =
            {
                0b00001,
                0b00011,
                0b01111,
                0b01111,
                0b01111,
                0b00011,
                0b00001,
                0b00000};
    }
}
