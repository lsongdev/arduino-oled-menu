#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OledMenu.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
oledmenu::Menu menu;

void openSettings(oledmenu::Menu &menu);
void goBack(oledmenu::Menu &menu);
void toggleSound(oledmenu::Menu &menu);
void toggleAnimation(oledmenu::Menu &menu);

oledmenu::Item mainItems[] = {
  {"Status"},
  {"Settings", openSettings},
  {"About"},
};

oledmenu::Item settingsItems[] = {
  {"Sound", toggleSound},
  {"Animation", toggleAnimation},
  {"Back", goBack},
};

void openSettings(oledmenu::Menu &menu)
{
  menu.setItems(settingsItems, sizeof(settingsItems) / sizeof(settingsItems[0]));
}

void goBack(oledmenu::Menu &menu)
{
  menu.setItems(mainItems, sizeof(mainItems) / sizeof(mainItems[0]), 1);
}

void toggleSound(oledmenu::Menu &)
{
  Serial.println(F("toggle sound"));
}

void toggleAnimation(oledmenu::Menu &menu)
{
  menu.style().animation = menu.style().animation ? 0 : 4;
}

void setup()
{
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  menu.setItems(mainItems, sizeof(mainItems) / sizeof(mainItems[0]));
}

void loop()
{
  // Auto-advance so the navigation model is visible without assuming
  // any particular board or button wiring.
  static uint32_t changedAt = 0;
  static uint8_t step = 0;

  if (millis() - changedAt > 1200) {
    changedAt = millis();

    if (step == 0) menu.next();       // Settings
    if (step == 1) menu.activate();   // enter Settings
    if (step == 2) menu.next();       // Animation
    if (step == 3) menu.next();       // Back
    if (step == 4) menu.activate();   // return
    step = (step + 1) % 5;
  }

  display.clearDisplay();
  menu.draw(display);
  display.display();
}
