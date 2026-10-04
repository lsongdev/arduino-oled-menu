#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OledMenu.h>

constexpr uint8_t ButtonUp = 2;
constexpr uint8_t ButtonSelect = 3;
constexpr uint8_t ButtonDown = 4;

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void showStatus(oledmenu::Menu &) { Serial.println(F("Status")); }
void openWifi(oledmenu::Menu &) { Serial.println(F("Wi-Fi")); }
void openSettings(oledmenu::Menu &) { Serial.println(F("Settings")); }
void showAbout(oledmenu::Menu &) { Serial.println(F("About")); }

oledmenu::Item items[] = {
  {"Status", showStatus},
  {"Wi-Fi", openWifi},
  {"Settings", openSettings},
  {"About", showAbout},
};

oledmenu::Menu menu(items, sizeof(items) / sizeof(items[0]));

struct Button {
  uint8_t pin;
  bool previous;

  explicit Button(uint8_t pin) : pin(pin), previous(HIGH) {}

  bool pressed()
  {
    const bool current = digitalRead(pin);
    const bool edge = previous == HIGH && current == LOW;
    previous = current;
    return edge;
  }
};

Button up{ButtonUp};
Button selectButton{ButtonSelect};
Button down{ButtonDown};

void setup()
{
  Serial.begin(115200);

  pinMode(ButtonUp, INPUT_PULLUP);
  pinMode(ButtonSelect, INPUT_PULLUP);
  pinMode(ButtonDown, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop()
{
  if (up.pressed()) menu.previous();
  if (down.pressed()) menu.next();
  if (selectButton.pressed()) menu.activate();

  display.clearDisplay();
  menu.draw(display);
  display.display();

  delay(5);
}
