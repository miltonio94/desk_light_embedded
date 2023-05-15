#include "./pixie.h"

const char *ssid = SSID_NAME;
const char *password = SSID_PASSWORD;
WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);
unsigned int messageInterval = 10000;
unsigned long lastUpdate = millis();
bool connected = false;

bool buttonState = false;
bool buttonWebSocketState = false;

void setupPixels(Adafruit_NeoPixel *pixels) {
  pixels->begin();
  pixels->clear();
  pixels->show();
}

void togglePixels(Adafruit_NeoPixel *pixels) {
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

void setupWifi() {
  uint8_t wifiStatus = WiFi.status();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.printf("status: %d\n", WiFi.status());
  }
  Serial.println("Conected!");
}

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

void setupSocket() {
  Serial.println("WebSocket complete uri is: ");
  Serial.print("ws://");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.print(WEBSOCKET_PORT);
  Serial.println("/");

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
  if (lastUpdate + messageInterval < millis()) {
    Serial.println("[WSc] SENT: Simple broadcast client message!!");
    webSocket.broadcastTXT("Simple broadcast client message!!");
    lastUpdate = millis();
  }
}
