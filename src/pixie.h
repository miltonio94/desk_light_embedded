#ifndef PIXIE_H_
#define PIXIE_H_

#define NUMBER_OF_PIXELS 50
#include "Adafruit_NeoPixel.h"
#include "ESP8266WiFi.h"
#include "credentials.h"
#include <Arduino.h>
#include <WebSocketsServer.h>

#define PIXEL_PIN D2
#define WEBSOCKET_PORT 81

void setupPixels(Adafruit_NeoPixel *pixels);
void togglePixels(bool buttonState, bool buttonWebSocketState,
                  Adafruit_NeoPixel *pixels);
void setupWifi();
#endif // PIXIE_H_
