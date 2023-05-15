#include "Adafruit_NeoPixel.h"
#include "Arduino.h"
#include "credentials.h"
#include "pixie.h"

#define BAUD_RATE 38400

Adafruit_NeoPixel pixels(500, D3, NEO_GRB + NEO_KHZ800);

void setup() {
  setupPixels(&pixels);

  Serial.begin(BAUD_RATE);

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  setupWifi();
  setupSocket();
  setup_mDNS();
}

void loop() {
  socketUpdate();
  MDNS.update();
  togglePixels(&pixels);

  pixels.show();
}
