#include "Adafruit_NeoPixel.h"
#include "Arduino.h"

#define NUMBER_OF_PIXELS 10
#define PIXEL_PIN D3
#define BUTTON_PIN D1

Adafruit_NeoPixel pixels(500, D3, NEO_GRB + NEO_KHZ800);

uint8_t buttonLastState = HIGH;
uint8_t buttonCurrentState = HIGH;
bool buttonToggleState = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pixels.begin();

  // doing this to fix a weird bug where when the microcontroller is reset
  // it fails to drive the pixels properly until it's reset
  pixels.clear();
  pixels.show();
  pixels.updateLength(NUMBER_OF_PIXELS);
}

void loop() {
  pixels.clear();

  // Toggling button state on and off! Debounce is not implemented
  // digitalRead reads high unless the button is pressed down
  // so I'm iverting that here
  if (!digitalRead(BUTTON_PIN)) {
    if (!buttonCurrentState) {
      buttonCurrentState = true;
    } else
      buttonCurrentState = false;
  }

  if (buttonCurrentState) {
    pixels.setBrightness(255);
    for (int8 i = 0; i < NUMBER_OF_PIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 255));
    }
  }

  pixels.show();
}
