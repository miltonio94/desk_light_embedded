#include "Adafruit_NeoPixel.h"
#include "Arduino.h"
#include "credentials.h"
#include "pixie.h"

#define BAUD_RATE 38400

Adafruit_NeoPixel pixels(500, D3, NEO_GRB + NEO_KHZ800);

void setup() {
  setupPixels(&pixels);

  Serial.begin(BAUD_RATE);

  setupWifi();
  setupSocket();
  setup_mDNS();
}

void loop() {
  pixelUpdate(&pixels);
  socketUpdate();
  MDNS.update();
  togglePixels(&pixels);

  pixels.show();
}
