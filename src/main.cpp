#include "Adafruit_NeoPixel.h"
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include <WebSocketsServer.h>

#define NUMBER_OF_PIXELS 10
#define PIXEL_PIN D3
#define BUTTON_PIN D1

const char *ssid = "esp8266_hotspot";
const char *password = "Jojo1234";

Adafruit_NeoPixel pixels(500, D3, NEO_GRB + NEO_KHZ800);

uint8_t wsPort = 81;
unsigned int messageInterval = 5000;
bool connected = false;
bool buttonState = false;

WebSocketsServer webSocket = WebSocketsServer(wsPort);

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
    Serial.printf("[%u] RECEIVE TXT: %s\n", num, payload);
    webSocket.sendTXT(num, payload);
    break;

  case WStype_BIN:
    Serial.printf("[%u] get binary length: %u\n", num, length);
    hexdump(payload, length);
  }
}

uint8_t buttonLastState = HIGH;
uint8_t buttonCurrentState = HIGH;
bool buttonToggleState = HIGH;

void setup() {
  delay(5000);
  Serial.begin(38400);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  pixels.begin();
  // doing this to fix a weird bug where when the microcontroller is reset
  // it fails to drive the pixels properly until it's reset
  pixels.clear();
  pixels.show();
  pixels.updateLength(NUMBER_OF_PIXELS);

  uint8_t numberOfNetworks = WiFi.scanNetworks();
  printf("there are %d ssids\n", numberOfNetworks);
  delay(5000);
  while (numberOfNetworks) {
    Serial.println(WiFi.SSID(numberOfNetworks));
    Serial.flush();
    numberOfNetworks--;
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
  Serial.print(wsPort);
  Serial.println("/");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

unsigned long lastUpdate = millis();

void loop() {
  webSocket.loop();
  pixels.clear();

  if (lastUpdate + messageInterval < millis()) {
    Serial.println("[WSc] SENT: Simple broadcast client message!!");
    webSocket.broadcastTXT("Simple broadcast client message!!");
    lastUpdate = millis();
  }

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
