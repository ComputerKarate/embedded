/**
 * Connecting ILI9341 to a Sparkfun ESP32 Thing Plus USB-C
 * 
 * The ESP32 was purchased from Sparkfun:
 * https://www.sparkfun.com/sparkfun-thing-plus-esp32-wroom-usb-c.html
 * 
 * The ILI9341 is the 3.2" model purchased from pjrc.com
 * These displays are feature rich for the price
 * NOTE: I have a personal bias towards pjrc.com as my first embedded contract used a teensy for POC
 */

// I had issues getting the touch screen to work properly until I came across:
// https://registry.platformio.org/libraries/tedtoal/XPT2046_Touchscreen_TT/examples/TouchTest/TouchTest.ino

// Handy list of ESP32 pin names and default values
// ~/Arduino/hardware/espressif/esp32/variants/esp32thing_plus_c/pins_arduino.h

#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen_TT.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// Display pin layout
#define DISPLAY_CS_PIN    15 // GPIO15, silk screen says 08
#define DISPLAY_RST_PIN   33 // GPIO33, silk screen says 10
#define DISPLAY_DC_PIN    27 // GPIO27, silk screen says 11
#define DISPLAY_MOSI_PIN  23 // GPIO23, silk screen says PICO
#define DISPLAY_CLK_PIN   18 // GPIO18, silk screen says SCK
#define DISPLAY_LED_PIN   14 // GPIO14, silk screen says 04
#define DISPLAY_MISO_PIN  19 // GPIO19, silk screen says POCI

// Touchscreen pin layout
#define TOUCH_CLK_PIN   18 // GPIO18, silk screen says SCK
#define TOUCH_CS_PIN    21 // GPIO21, silk screen says SDA
#define TOUCH_DIN_PIN   23 // GPIO23, silk screen says PICO
#define TOUCH_DOUT_PIN  19 // GPIO19, silk screen says POCI
// #define TOUCH_IRQ (Not connected)

Adafruit_ILI9341 display = Adafruit_ILI9341(DISPLAY_CS_PIN, 
                                            DISPLAY_DC_PIN, 
                                            DISPLAY_MOSI_PIN, 
                                            DISPLAY_CLK_PIN, 
                                            DISPLAY_RST_PIN, 
                                            DISPLAY_MISO_PIN);

XPT2046_Touchscreen touchscreen = XPT2046_Touchscreen(TOUCH_CS_PIN);

bool displayOn = true;
bool verboseOutput = true;

/**
 * Rotation values:
 *    0 = Portrait mode, pins on bottom
 *    1 = Landscape mode, pins on right side
 *    2 = Portrait mode, pins on top
 *    3 = Landscape mode, pins on left side
 */
bool initDisplay()
{
  bool returnCode = false;
  uint8_t x = 0;

  if(true == verboseOutput)
  {
    Serial.println("Initializing display");
    Serial.println("Powering backlight on");
  }

  digitalWrite(DISPLAY_LED_PIN, HIGH);

  display.begin();

  display.fillScreen(ILI9341_BLACK);

  display.setTextSize(1);
  
  display.setRotation(3);

  x = display.readcommand8(ILI9341_RDDST);
  if(true == verboseOutput)
  {
    Serial.println("Display status: 0x" + String(x, HEX));
  }

  // It seems I have to tickle the device first to get this to return a value hence the above status call
  // NOTE: it is probably dangerous to assume that the only valid diag code is 0x80
  x = display.readcommand8(ILI9341_RDSELFDIAG);
  if(0x80 == x)
  {
    returnCode = true;
    Serial.println("Self Diagnostic: 0x" + String(x, HEX)); 
  }
  else
  {
    Serial.println("ERROR: Self Diagnostic value expected to be 80. Actual value: 0x" + 
                    String(x, HEX)); 
  }

  return returnCode;
}


bool initTouchScreen()
{
  bool returnCode = touchscreen.begin();
  if(returnCode)
  {
    if(true == verboseOutput)
    {
      Serial.println("touchscreen initialized successfully");
    }
  }
  else
  {
      Serial.println("ERROR: touchscreen failed to initialize");
  }
  return returnCode;
}

void drawBox()
{
  // Yes it's boring but it works for display purposes
  display.drawRoundRect(0, 0, display.width(), display.height(), 29.5, display.color565(234, 0, 0));

  display.setTextSize(1);

  // Crude centering for display porpoises
  display.setCursor(display.width() / 5, display.height() / 2);

  display.println("Touch me to toggle off and on!");
}

/**
 * NOTE: What should happen if a device fails to initialize?
 *        This ESP32 has a built-in RGB (GPIO02) we could set
 */
void setup() {
  Serial.begin(115200);
  while (!Serial && (millis() <= 1000));

  // Connect the ILI9341 LED pin to something we can bring up/down at will
  pinMode(DISPLAY_LED_PIN, OUTPUT);

  initDisplay();      // Intentionally ignoring return value for now
  drawBox();
  initTouchScreen();  // Intentionally ignoring return value for now

}


void loop() {
  if (touchscreen.touched()) 
  {
    // Toggle display value
    displayOn = !displayOn;

    if (displayOn)
    {
      if(true == verboseOutput)
      {
        Serial.println("Turning backlight on");
      }
      digitalWrite(DISPLAY_LED_PIN, HIGH);
    }
    else
    {
      if(true == verboseOutput)
      {
        Serial.println("Turning backlight off");
      }
      digitalWrite(DISPLAY_LED_PIN, LOW);
    }

    // delay() keeps taps from becoming double taps
    delay(50);
  }
}
