#include "./pixie.h"

const char *ssid = SSID_NAME;
const char *password = SSID_PASSWORD;
WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);
unsigned int messageInterval = 10000;
unsigned long lastUpdate = millis();
bool websocketConnected = false;

bool buttonState = false;
bool buttonWebSocketState = false;

struct colour {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

colour rgba = {0, 0, 0, 255};

void setupPixels(Adafruit_NeoPixel *pixels) {
  pixels->begin();
  pixels->clear();
  pixels->show();
}

void pixelUpdate(Adafruit_NeoPixel *pixels) {
  pixels->clear();
  if (buttonWebSocketState)
    for (uint8_t i = 0; i < NUMBER_OF_PIXELS; i++) {
      pixels->setBrightness(rgba.a);
      pixels->setPixelColor(i, pixels->Color(rgba.g, rgba.r, rgba.b));
    }
  pixels->show();
}

void togglePixels(Adafruit_NeoPixel *pixels) {
  if (buttonState != buttonWebSocketState) {
    buttonState = buttonWebSocketState;
    if (buttonState) {
      pixels->setBrightness(rgba.a);
      for (uint8_t i = 0; i < NUMBER_OF_PIXELS; i++) {
        pixels->setPixelColor(i, pixels->Color(rgba.g, rgba.r, rgba.b));
      }
    } else {
      pixels->setBrightness(0);
      pixels->clear();
    }
  }
}

void setupWifi() {
  uint8_t wifiStatus = WiFi.status();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(5);
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                    size_t length) {
  switch (type) {
  case WStype_DISCONNECTED:
    websocketConnected = false;
    break;

  case WStype_CONNECTED: {
    websocketConnected = true;
    lastUpdate = millis();

    uint8_t colourVal[5];

    webSocket.sendTXT(num, "CONNECTED");
    sprintf((char *)colourVal, "R_%i", rgba.r);
    webSocket.sendTXT(num, colourVal);
    sprintf((char *)colourVal, "G_%i", rgba.g);
    webSocket.sendTXT(num, colourVal);
    sprintf((char *)colourVal, "B_%i", rgba.b);
    webSocket.sendTXT(num, colourVal);
    sprintf((char *)colourVal, "A_%i", rgba.a);
    webSocket.sendTXT(num, colourVal);

    if (buttonWebSocketState)
      webSocket.sendTXT(num, "STATE_ON");
    else
      webSocket.sendTXT(num, "STATE_OFF");
  } break;

  case WStype_TEXT: {
    lastUpdate = millis();

    if (length >= 7 && payload[6] == 'O' && payload[7] == 'N') {
      buttonWebSocketState = true;
      webSocket.sendTXT(num, "LAMP ON!");
      return;
    }
    if (length >= 8 && payload[6] == 'O' && payload[7] == 'F' &&
        payload[8] == 'F') {
      webSocket.sendTXT(num, "LAMP OFF!");
      buttonWebSocketState = false;
      return;
    }

    if (payload[0] == 'R') {
      char *ptr;
      rgba.r = (uint8_t)strtol((char *)payload + 2, &ptr, 10);
      return;
    }

    if (payload[0] == 'G') {
      char *ptr;
      rgba.g = (uint8_t)strtol((char *)payload + 2, &ptr, 10);
      return;
    }

    if (payload[0] == 'B') {
      char *ptr;
      rgba.b = (uint8_t)strtol((char *)payload + 2, &ptr, 10);
      return;
    }

    if (payload[0] == 'A') {
      char *ptr;
      rgba.a = (uint8_t)strtol((char *)payload + 2, &ptr, 10);
      return;
    }
  } break;
  }
}

void setupSocket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void setup_mDNS() {
  if (MDNS.begin("pixie", WiFi.localIP()))
    Serial.println("mDns started");
  MDNS.addService("ws", "tcp", WEBSOCKET_PORT);
}

void socketUpdate() {
  webSocket.loop();
  if (lastUpdate + messageInterval < millis() && websocketConnected) {
    webSocket.broadcastTXT("Simple broadcast client message!!");
    lastUpdate = millis();
  }
}
