#pragma once
#include "pico/stdlib.h"

namespace lcd4pico
{

    enum Bit_Mode : const uint8_t
    {
        _4BIT = 4,
        _8BIT = 8
    };

    enum Direction : const uint8_t
    {
        Left,
        Right
    };
}