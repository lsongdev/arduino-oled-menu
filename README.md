# Arduino OLED Menu

A small animated menu for monochrome Arduino displays.

This fork started from [upiir/arduino_oled_menu](https://github.com/upiir/arduino_oled_menu). The upstream project is a useful visual demo; this repository turns the core idea into a reusable Arduino library with a deliberately small API.

The library uses **Adafruit GFX** as the drawing boundary. It does not own your SSD1306/SH110X driver, buttons, rotary encoder, application screens, or display refresh.

## Why

A menu library for a 128×64 OLED does not need a widget framework.

```text
input
  ↓
Menu state
  ↓
animation
  ↓
draw(Adafruit_GFX)
```

No heap allocation, no event bus, no screen manager and no hidden input handling.

## Quick start

```cpp
#include <Adafruit_SSD1306.h>
#include <OledMenu.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void openStatus(oledmenu::Menu &) {
  Serial.println("Status selected");
}

oledmenu::Item items[] = {
  {"Status", openStatus},
  {"Wi-Fi"},
  {"Settings"},
  {"About"},
};

oledmenu::Menu menu(items, sizeof(items) / sizeof(items[0]));

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
}

void loop() {
  // Your own buttons / encoder:
  // menu.previous();
  // menu.next();
  // menu.activate();

  display.clearDisplay();
  menu.draw(display);
  display.display();
}
```

Navigation is explicit:

```cpp
menu.next();
menu.previous();
menu.select(3);
menu.activate();
```

The selected row and list scroll smoothly toward their new positions when `draw()` is called repeatedly.

## Items

An item contains only what a menu item actually needs:

```cpp
struct Item {
  const char *label;
  Action action; // void (*)(Menu &)
  const Icon *icon;
};
```

Text-only:

```cpp
oledmenu::Item item{"Settings", openSettings};
```

With a bitmap icon:

```cpp
const uint8_t wifiBitmap[] PROGMEM = { /* ... */ };
oledmenu::Icon wifiIcon(wifiBitmap, 16, 16);

oledmenu::Item item{"Wi-Fi", openWifi, &wifiIcon};
```

The bitmap format is the normal 1-bit format accepted by `Adafruit_GFX::drawBitmap()`.

## Submenus

There is intentionally no menu-tree abstraction. Replace the current item array:

```cpp
menu.setItems(settingsItems, settingsCount);
```

A Back item can switch to the parent array. See [examples/submenu](examples/submenu).

This keeps navigation visible in application code instead of hiding it inside a framework.

## Styling

The defaults target a 128×64 display:

```cpp
menu.style().itemHeight = 20;
menu.style().padding = 4;
menu.style().iconGap = 5;
menu.style().radius = 3;
menu.style().animation = 4;
menu.style().scrollbar = true;
```

`animation = 0` disables animation. Larger values make the easing slower.

The menu respects the current Adafruit GFX font. Set your font before `menu.draw(display)` if needed.

## Project layout

```text
.
├── src/
│   ├── OledMenu.h
│   └── OledMenu.cpp
├── examples/
│   ├── basic/
│   ├── icons/
│   └── submenu/
├── docs/
│   ├── api.md
│   ├── design.md
│   └── matrixbit.md
├── library.properties
└── README.md
```

The original demo files and generated image assets remain available in Git history and in the upstream repository; they are not part of the reusable library.

## Examples

- **basic** — SSD1306 + three buttons + actions.
- **icons** — menu items with 16×16 bitmaps.
- **submenu** — nested navigation using only `setItems()`.

## Matrix:bit

The library fits the Matrix:bit project directly because `matrixbit::display()` returns an `Adafruit_SSD1306&`.

See [docs/matrixbit.md](docs/matrixbit.md).

## Design

The important design decisions, including what was intentionally left out, are documented in [docs/design.md](docs/design.md).

## License

MIT. The original project copyright is preserved in [LICENSE](LICENSE).
