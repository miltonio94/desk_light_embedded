#ifndef PIXIE_H_
#define PIXIE_H_

#include "Adafruit_NeoPixel.h"
#include "ESP8266WiFi.h"
#include "credentials.h"
#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>
#include <stdio.h>
#include <stdlib.h>

#define PIXEL_PIN D2
#define NUMBER_OF_PIXELS 50
#define WEBSOCKET_PORT 81

void setupPixels(Adafruit_NeoPixel *pixels);
void togglePixels(Adafruit_NeoPixel *pixels);
void pixelUpdate(Adafruit_NeoPixel *pixels);
void setupWifi();
void setup_mDNS();
void setupSocket();
void socketUpdate();

#endif // PIXIE_H_
