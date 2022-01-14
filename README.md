<h1 align="center">LCD4Pico</h1>

<h1 align="center">
  <img style="margin:-30px 15px -15px 0px;" width="350"
    src="https://user-images.githubusercontent.com/50547116/148704880-7c481e3c-cb6b-4654-8afe-c93fe6fdacee.jpg"/>
</h1>

<p align="center">LCD4Pico is a small and easy to use library for using the HD44780 LCD (16x2) with Raspberry Pi Pico.</p>
<p align="center">The library was written using the Raspberry Pi Pico C/C++ SDK.</p>

### Overview

The LCD4Pico class contains the "high level" methods for writing text to the LCD, moving the cursor,shifting the display and some other methods.
LCD4PicoBase is the base class for LCD4Pico and contains the "low level" methods like for writing raw bits, switching between read and write mode and setting the enable pin.


### Example
First of all you need to declare the pins.
Declare the data pins in the order from D7 to D0.

LCD4Pico works in 4bit mode as well,  in 4bit mode you only need to set the pins D7 to D4.

If you want to use the LCD in write-only mode (without the RW pin),  
call the overloaded ctor without the ```rw_pin``` arg or set ```rw_pin``` to ``` WRITE_ONLY```.

Pass all the pins to the constructor.
Call then the ```setup()``` function.
Possible arguments for the setup are:
 - numOfdisplayLines : 1/2,
 - largeFont : true/false,
 - blinkingCursor : true/false, 
 - cursorOn : true/false, 
 - displayOn : true/false, 
 - accompanyDisplayShift : true/false, 
 - incrementCursor : true/false

```c++
#include "pico/stdlib.h"
#include "LCD4Pico/LCD4Pico.hpp"

#define BIT_MODE 4

int main()
{
    stdio_init_all();

    const uint8_t dpins[BIT_MODE] = {7, 6, 5, 4}; // 3, 2, 1, 0};
    const uint8_t enable_pin = 16;
    const uint8_t rs_pin = 18;
    const uint8_t rw_pin = 17;

    lcd4pico::LCD4Pico<BIT_MODE> lcd(enable_pin, rs_pin, rw_pin, dpins);
    // lcd4pico::LCD4Pico<BIT_MODE> lcd(enable_pin, rs_pin, dpins);  for write-only mode

    lcd.setup();

    lcd.write("Hello,");
    lcd.toSecondLine();
    lcd.write("World!");

    sleep_ms(1000);

    for (uint i = 0; i < 100; i++)
    {
        lcd.shiftDisplay("right");
        sleep_ms(120);
    }

    sleep_ms(3000);

    lcd.clearDisplay();
    lcd.writeLines("LCD4Pico", "(^-^)");

    return 0;
}
```

 ### Features in the future

 - predefined custom characters
 - possibility to define own custom characters
 - animations