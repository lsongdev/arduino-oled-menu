# Matrix:bit integration

`esp32-matrixbit` already exposes its OLED as an `Adafruit_SSD1306&`, so no adapter is required.

With two buttons, a useful mapping is:

- A: next item
- B: activate item

```cpp
#include "matrixbit.h"
#include <OledMenu.h>

oledmenu::Item items[] = {
  {"Sensors"},
  {"Wi-Fi"},
  {"Settings"},
  {"About"},
};

oledmenu::Menu menu(items, sizeof(items) / sizeof(items[0]));

bool lastA = false;
bool lastB = false;

void setup()
{
  matrixbit::begin();
  matrixbit::beginDisplay();
}

void loop()
{
  const bool a = matrixbit::buttonA();
  const bool b = matrixbit::buttonB();

  if (a && !lastA) menu.next();
  if (b && !lastB) menu.activate();

  lastA = a;
  lastB = b;

  auto &display = matrixbit::display();
  display.clearDisplay();
  menu.draw(display);
  display.display();

  delay(5);
}
```

A long-press or chord can later provide Back/Previous if the application needs it. That policy belongs to the Matrix:bit application, not to OLED Menu.
