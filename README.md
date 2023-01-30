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
#include "LCD4Pico/Enums.hpp"

int main()
{
    stdio_init_all();

    const uint8_t dpins[] = {4, 5, 6, 7};  // D4,D5,D6,D7 
    const uint8_t enable_pin = 16;
    const uint8_t rs_pin = 18;
    const uint8_t rw_pin = 17;

    lcd4pico::LCD4Pico<lcd4pico::Bit_Mode::_4BIT> lcd(enable_pin, rs_pin, rw_pin, dpins);
    // lcd4pico::LCD4Pico<lcd4pico::Bit_Mode::_4BIT> lcd(enable_pin, rs_pin, dpins);  for write-only mode

    lcd.setup();

    lcd.write("Hello,");
    lcd.toSecondLine();
    lcd.write("World!");
    // or lcd.writeLines("Hello,", "World!");

    sleep_ms(1000);

    for (uint i = 0; i < 100; i++)
    {
        lcd.shiftDisplay(lcd4pico::Direction::Right);
        sleep_ms(120);
    }

    sleep_ms(3000);

    lcd.clearDisplay();
    lcd.writeLines("LCD4Pico", "(^-^)");

    return 0;
}
```

### Custom Characters
Use `createCustomCharacter(uint8_t index, const uint8_t (&character)[8])` to define your own custom character. Then use `writeCustomCharacter(uint8_t index)` to write the custom character to the display. I recommend [this tool](https://omerk.github.io/lcdchargen/) to create your own characters as it allows you to preview the result.  

You can also use predefined symbols from the `lcd4pico::symbols` namespace.
```c++
    lcd.createCustomCharacter(0, lcd4pico::symbols::bell);
    lcd.createCustomCharacter(1, lcd4pico::symbols::checkMark);
    lcd.createCustomCharacter(2, lcd4pico::symbols::smile);

    lcd.writeCustomCharacter(0);  // writes bell symbol to the display
    lcd.writeCustomCharacter(1);  // writes check mark symbol to the display
    lcd.writeCustomCharacter(2);  // writes a smiley to the display
```

### Troubleshooting
If you experience any issues, try setting the `INSTRUCTION_WAITING_TIME` macro to 100 or higher:  
```c++
#define INSTRUCTION_WAITING_TIME 100

#include "pico/stdlib.h"
#include "LCD4Pico/LCD4Pico.hpp"
#include "LCD4Pico/Enums.hpp"
```
