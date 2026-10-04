#include "OledMenu.h"

namespace oledmenu {

Menu::Menu()
  : items_(nullptr),
    count_(0),
    selected_(0),
    scroll_(0),
    cursor_(0),
    targetScroll_(0),
    targetCursor_(0),
    lastTick_(0),
    animationReady_(false)
{
}

Menu::Menu(const Item *items, size_t count)
  : Menu()
{
  setItems(items, count);
}

void Menu::setItems(const Item *items, size_t count, size_t selected)
{
  items_ = items;
  count_ = count;
  selected_ = count == 0 ? 0 : (selected < count ? selected : count - 1);
  animationReady_ = false;
}

bool Menu::next()
{
  if (count_ < 2) return false;
  selected_ = (selected_ + 1) % count_;
  return true;
}

bool Menu::previous()
{
  if (count_ < 2) return false;
  selected_ = selected_ == 0 ? count_ - 1 : selected_ - 1;
  return true;
}

bool Menu::select(size_t index)
{
  if (index >= count_ || index == selected_) return false;
  selected_ = index;
  return true;
}

void Menu::activate()
{
  const Item *item = current();
  if (item && item->action) item->action(*this);
}

const Item *Menu::current() const
{
  return count_ == 0 || !items_ ? nullptr : &items_[selected_];
}

bool Menu::animating() const
{
  return animationReady_ &&
         (scroll_ != targetScroll_ || cursor_ != targetCursor_);
}

int16_t Menu::approach(int16_t current, int16_t target, uint8_t amount)
{
  if (current == target || amount == 0) return target;

  const int16_t delta = target - current;
  if (delta >= -1 && delta <= 1) return target;

  int16_t step = delta / amount;
  if (step == 0) step = delta > 0 ? 1 : -1;
  return current + step;
}

void Menu::updateAnimation(int16_t scroll, int16_t cursor)
{
  targetScroll_ = scroll;
  targetCursor_ = cursor;

  if (!animationReady_) {
    scroll_ = scroll;
    cursor_ = cursor;
    lastTick_ = millis();
    animationReady_ = true;
    return;
  }

  if (style_.animation == 0) {
    scroll_ = scroll;
    cursor_ = cursor;
    lastTick_ = millis();
    return;
  }

  const uint32_t now = millis();
  const uint32_t elapsed = now - lastTick_;
  uint8_t steps = elapsed / 16;

  if (steps == 0) return;
  if (steps > 8) steps = 8;

  for (uint8_t i = 0; i < steps; ++i) {
    scroll_ = approach(scroll_, targetScroll_, style_.animation);
    cursor_ = approach(cursor_, targetCursor_, style_.animation);
  }

  lastTick_ = now;
}

void Menu::draw(Adafruit_GFX &display)
{
  if (!items_ || count_ == 0) return;

  const int16_t width = display.width();
  const int16_t height = display.height();
  const int16_t itemHeight = style_.itemHeight ? style_.itemHeight : 1;

  const int32_t contentHeight = static_cast<int32_t>(count_) * itemHeight;
  const int16_t maxScroll =
    contentHeight > height ? static_cast<int16_t>(contentHeight - height) : 0;

  int32_t centered =
    static_cast<int32_t>(selected_) * itemHeight - (height - itemHeight) / 2;

  if (centered < 0) centered = 0;
  if (centered > maxScroll) centered = maxScroll;

  const int16_t targetScroll = static_cast<int16_t>(centered);
  const int16_t targetCursor =
    static_cast<int16_t>(selected_ * itemHeight - targetScroll);

  updateAnimation(targetScroll, targetCursor);

  const bool showScrollbar = style_.scrollbar && maxScroll > 0;
  const int16_t contentWidth = width - (showScrollbar ? 4 : 0);

  display.setTextWrap(false);
  display.setTextColor(1);

  for (size_t i = 0; i < count_; ++i) {
    const int32_t itemY32 = static_cast<int32_t>(i) * itemHeight - scroll_;
    if (itemY32 >= height || itemY32 + itemHeight <= 0) continue;

    const int16_t itemY = static_cast<int16_t>(itemY32);
    const Item &item = items_[i];

    int16_t x = style_.padding;

    if (item.icon && item.icon->data && item.icon->width && item.icon->height) {
      const int16_t iconY = itemY + (itemHeight - item.icon->height) / 2;
      display.drawBitmap(
        x,
        iconY,
        item.icon->data,
        item.icon->width,
        item.icon->height,
        1
      );
      x += item.icon->width + style_.iconGap;
    }

    if (item.label) {
      int16_t bx = 0;
      int16_t by = 0;
      uint16_t bw = 0;
      uint16_t bh = 0;
      display.getTextBounds(item.label, x, 0, &bx, &by, &bw, &bh);
      const int16_t textY =
        itemY + (itemHeight - static_cast<int16_t>(bh)) / 2 - by;
      display.setCursor(x, textY);
      display.print(item.label);
    }
  }

  display.drawRoundRect(
    0,
    cursor_,
    contentWidth,
    itemHeight > 2 ? itemHeight - 2 : itemHeight,
    style_.radius,
    1
  );

  if (showScrollbar) {
    const int16_t x = width - 2;
    display.drawFastVLine(x, 0, height, 1);

    int16_t thumbHeight =
      static_cast<int16_t>((static_cast<int32_t>(height) * height) / contentHeight);
    if (thumbHeight < 6) thumbHeight = 6;
    if (thumbHeight > height) thumbHeight = height;

    const int16_t travel = height - thumbHeight;
    const int16_t thumbY =
      maxScroll == 0 ? 0 :
      static_cast<int16_t>((static_cast<int32_t>(scroll_) * travel) / maxScroll);

    display.fillRect(x - 1, thumbY, 3, thumbHeight, 1);
  }
}

} // namespace oledmenu
