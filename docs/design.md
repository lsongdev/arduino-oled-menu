# Design

## Starting point

The upstream project is an excellent visual/tutorial demo, but it is not structured as a reusable library.

The original sketch combines several unrelated responsibilities in one very large source file:

- generated bitmap assets
- button polling
- demo-mode timing
- menu selection state
- screen switching
- rendering
- application-specific screenshots and QR codes

Menu labels, icons, screenshots and QR codes are also stored in parallel arrays that must remain in exactly the same order.

That design is useful for a tutorial because everything is visible in one file. It becomes difficult to reuse because changing the menu also means understanding the demo application.

## Goals

This fork keeps the useful part: a compact, animated OLED menu.

The core should be:

- small enough to understand in one sitting
- deterministic and allocation-free
- independent from a specific OLED controller
- independent from a specific input device
- usable from two buttons, three buttons or a rotary encoder
- easy to integrate into an existing application
- visually pleasant without becoming a widget toolkit

## Boundaries

### Adafruit GFX is the drawing boundary

The library accepts an `Adafruit_GFX&` when drawing.

This is an existing abstraction and avoids inventing a second canvas interface. SSD1306, SH110X and many other Arduino display drivers already derive from it.

The library does not call `display.display()` because framebuffer flushing is driver-specific and belongs to the application.

### Input stays outside

There is no `Button`, `Encoder`, GPIO pin configuration or debounce code in the library.

Applications translate input into:

```cpp
menu.previous();
menu.next();
menu.activate();
```

This makes the same core usable across boards.

### No menu tree

A tree sounds convenient, but it quickly introduces ownership, parent pointers, navigation policy and dynamic configuration.

The small primitive:

```cpp
menu.setItems(items, count);
```

is enough to build nested menus explicitly.

If a future real application demonstrates that a tree abstraction removes more code than it adds, it can be reconsidered then.

### No dynamic allocation

`Menu` stores a pointer to caller-owned item arrays. Items and icons are normally static/global data.

There is no `new`, `std::vector`, `std::function`, hidden `String` allocation or runtime registration. Item actions are plain function pointers and receive the current `Menu&` as context.

### Animation is state, not a framework

The renderer keeps only current and target pixel positions for list scroll and selection.

A small integer easing step runs on roughly 16 ms ticks. This gives the visual smoothness we want without an animation object hierarchy.

## What is intentionally missing

The first version does not include:

- text editing
- sliders
- toggles
- dialogs
- page transitions
- focus trees
- input repeat
- key debounce
- persistence
- layout containers
- a scene/screen manager

The next useful additions are likely value editors (bool/int) and optional page transitions, but they should remain orthogonal to the menu core.
