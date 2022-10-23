#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <AccelStepper.h>

#define IN1 D5
#define IN2 D6
#define IN3 D7
#define IN4 D8

AccelStepper motor(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

String message = "";

const char *ssid = "";
const char *password = "";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String direction = "STOP";
String steps;

bool motor_stop = false;

void notifyClients(String state)
{
  ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    message = (char *)data;
    steps = message.substring(0, message.indexOf("/"));
    direction = message.substring(message.indexOf("/") + 1, message.length());
    Serial.print("steps");
    Serial.println(steps);
    Serial.print("direction");
    Serial.println(direction);
    notifyClients(direction);
    motor_stop = true;
    if (direction == "Clockwise")
    {
      Serial.print("Clockwise: ");
      motor.move(steps.toInt());
    }
    else
    {
      Serial.print("Anti-clockwise: ");
      motor.move(-steps.toInt());
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    // Notify client of motor current state when it first connects
    notifyClients(direction);
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  initWebSocket();

  if (!LittleFS.begin())
  {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else
  {
    Serial.println("LittleFS mounted successfully");
  }
  motor.setMaxSpeed(500);
  motor.setAcceleration(100);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  server.serveStatic("/", LittleFS, "/");

  server.begin();
}

void loop()
{
  if (motor.distanceToGo() == 0 && motor_stop == true)
  {
    direction = "stop";
    notifyClients(direction);
    motor_stop = false;
  }
  ws.cleanupClients();
  motor.run();
}
