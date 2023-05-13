#include "./pixie.h"

void setupPixels(Adafruit_NeoPixel *pixels) {
  pixels->begin();
  pixels->clear();
  for (uint8_t i = 0; i < NUMBER_OF_PIXELS; i++) {
    pixels->setBrightness(25);
    pixels->setPixelColor(i, pixels->Color(000, 111, 00));
  }
  pixels->show();
}

void togglePixels(bool buttonState, bool buttonWebSocketState,
                  Adafruit_NeoPixel *pixels) {
  if (buttonState != buttonWebSocketState) {
    buttonState = buttonWebSocketState;
    if (buttonState) {
      pixels->setBrightness(115);
      for (uint8_t i = 0; i < NUMBER_OF_PIXELS; i++) {
        pixels->setPixelColor(i, pixels->Color(0, 0, 255));
      }
    } else {
      pixels->setBrightness(0);
      pixels->clear();
    }
  }
}

const char *ssid = SSID_NAME;
const char *password = SSID_PASSWORD;
void setupWifi() {
  uint8_t wifiStatus = WiFi.status();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.printf("status: %d\n", WiFi.status());
  }
  Serial.println("Conected!");
}

void setupSocket(WebSocketsServer *webSocket) {
  Serial.println("WebSocket complete uri is: ");
  Serial.print("ws://");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.print(WEBSOCKET_PORT);
  Serial.println("/");

  webSocket->begin();
  // webSocket->onEvent(webSocketEvent);
}
