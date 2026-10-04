# API

All public types live in the `oledmenu` namespace.

## Item

```cpp
oledmenu::Item(
  const char *label,
  oledmenu::Action action = nullptr,
  const oledmenu::Icon *icon = nullptr
);
```

An item does not own the label or icon. Keep those objects alive while the item is in use; static/global arrays are the normal Arduino pattern.

`action` is an optional `void (*)(Menu &)` callback invoked by `Menu::activate()`. The current menu is passed to the callback, so submenu actions can call `setItems()` without relying on a global menu variable.

## Icon

```cpp
oledmenu::Icon icon(bitmap, width, height);
```

The bitmap uses the normal one-bit Adafruit GFX bitmap format.

## Menu

### Construction

```cpp
oledmenu::Menu menu(items, count);
```

A default constructor is also available:

```cpp
oledmenu::Menu menu;
menu.setItems(items, count);
```

### Navigation

```cpp
menu.next();
menu.previous();
menu.select(index);
menu.activate();
```

`next()` and `previous()` wrap at the ends of the list.

The navigation functions return `true` when the selected index changed.

### Current item

```cpp
size_t index = menu.selected();
const oledmenu::Item *item = menu.current();
```

`current()` returns `nullptr` for an empty menu.

### Replacing the menu

```cpp
menu.setItems(items, count, selected);
```

This is enough to implement submenus without a tree model. Changing the item set resets animation state so the first frame does not slide in from an unrelated menu.

### Drawing

```cpp
display.clearDisplay();
menu.draw(display);
display.display();
```

`draw()` only draws into the Adafruit GFX framebuffer. It deliberately does not clear or flush the display.

That means applications remain free to draw a header, status bar, footer, overlay, battery icon, or anything else around the menu.

Call `draw()` continuously while the UI is visible so animations can advance.

### Animation

```cpp
menu.animating();
menu.style().animation = 4;
```

Animation is integer/pixel based and advances on approximately 16 ms ticks. It does not allocate memory and does not require floating point.

Set `animation` to `0` for immediate movement. Larger divisors produce slower easing.

## Style

```cpp
struct Style {
  uint8_t itemHeight;
  uint8_t padding;
  uint8_t iconGap;
  uint8_t radius;
  uint8_t animation;
  bool scrollbar;
};
```

Defaults:

| Field | Default |
| --- | ---: |
| `itemHeight` | 20 |
| `padding` | 4 |
| `iconGap` | 5 |
| `radius` | 3 |
| `animation` | 4 |
| `scrollbar` | true |

The renderer uses the font currently configured on the supplied `Adafruit_GFX` object and computes text bounds at draw time.
