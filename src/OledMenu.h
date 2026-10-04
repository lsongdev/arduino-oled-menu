#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>

namespace oledmenu {

class Menu;
using Action = void (*)(Menu &);

struct Icon {
  const uint8_t *data;
  uint8_t width;
  uint8_t height;

  Icon(const uint8_t *data = nullptr, uint8_t width = 0, uint8_t height = 0)
    : data(data), width(width), height(height) {}
};

struct Item {
  const char *label;
  Action action;
  const Icon *icon;

  Item(const char *label = nullptr, Action action = nullptr, const Icon *icon = nullptr)
    : label(label), action(action), icon(icon) {}
};

struct Style {
  uint8_t itemHeight;
  uint8_t padding;
  uint8_t iconGap;
  uint8_t radius;
  uint8_t animation;
  bool scrollbar;

  Style()
    : itemHeight(20),
      padding(4),
      iconGap(5),
      radius(3),
      animation(4),
      scrollbar(true) {}
};

class Menu {
public:
  Menu();
  Menu(const Item *items, size_t count);

  void setItems(const Item *items, size_t count, size_t selected = 0);

  bool next();
  bool previous();
  bool select(size_t index);
  void activate();

  size_t selected() const { return selected_; }
  size_t count() const { return count_; }
  bool empty() const { return count_ == 0; }
  bool animating() const;

  const Item *current() const;

  Style &style() { return style_; }
  const Style &style() const { return style_; }

  void draw(Adafruit_GFX &display);

private:
  static int16_t approach(int16_t current, int16_t target, uint8_t amount);
  void updateAnimation(int16_t scroll, int16_t cursor);

  const Item *items_;
  size_t count_;
  size_t selected_;

  Style style_;

  int16_t scroll_;
  int16_t cursor_;
  int16_t targetScroll_;
  int16_t targetCursor_;

  uint32_t lastTick_;
  bool animationReady_;
};

} // namespace oledmenu
