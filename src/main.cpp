#include "Adafruit_NeoPixel.h"
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include <WebSocketsServer.h>

#define NUMBER_OF_PIXELS 10
#define PIXEL_PIN D3
#define BAUD_RATE 38400

const char *ssid = "<YOUR_SSID>";
const char *password = "<PASSWORD>";

Adafruit_NeoPixel pixels(500, D3, NEO_GRB + NEO_KHZ800);

uint8_t webSocketPort = 81;
WebSocketsServer webSocket = WebSocketsServer(webSocketPort);
unsigned int messageInterval = 10000;
unsigned long lastUpdate = millis();
bool connected = false;

bool buttonState = false;
bool buttonWebSocketState = false;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                    size_t length) {
  switch (type) {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconected!\n", num);
    break;

  case WStype_CONNECTED: {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%ds url: %s\n", num, ip[0],
                  ip[1], ip[2], ip[3], payload);
  } break;

  case WStype_TEXT:
    lastUpdate = millis();

    if (length <= 2 && payload[0] == 'O' && payload[1] == 'N') {
      buttonWebSocketState = true;
      Serial.println("Turning on lamp");
      webSocket.sendTXT(num, "LAMP ON!");
      return;
    }
    if (length <= 3 && payload[0] == 'O' && payload[1] == 'F' &&
        payload[2] == 'F') {
      Serial.println("Turning off lamp");
      webSocket.sendTXT(num, "LAMP OFF!");
      buttonWebSocketState = false;
      return;
    }
    break;

  case WStype_BIN:
    Serial.printf("[%u] get binary length: %u\n", num, length);
  }
}

void setup() {
  pixels.begin();
  // doing this to fix a weird bug where when the microcontroller is reset
  // it fails to drive the pixels properly until it's reset
  pixels.clear();
  pixels.show();
  pixels.updateLength(NUMBER_OF_PIXELS);
  pixels.clear();

  Serial.begin(BAUD_RATE);

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.printf("status: %d\n", WiFi.status());
  }

  Serial.println("WebSocket complete uri is: ");
  Serial.print("ws://");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.print(webSocketPort);
  Serial.println("/");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  if (lastUpdate + messageInterval < millis()) {
    Serial.println("[WSc] SENT: Simple broadcast client message!!");
    webSocket.broadcastTXT("Simple broadcast client message!!");
    lastUpdate = millis();
  }

  if (buttonState != buttonWebSocketState) {
    buttonState = buttonWebSocketState;
    if (buttonState) {
      pixels.setBrightness(255);
      for (int8 i = 0; i < NUMBER_OF_PIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 255));
      }
    } else {
      pixels.setBrightness(0);
      pixels.clear();
    }
  }

  pixels.show();
}
