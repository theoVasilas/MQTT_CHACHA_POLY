#pragma once
#include <WiFi.h>
#include <PubSubClient.h>

/* ---------- WiFi ---------- */
#define WIFI_SSID "COSMOTE-031748" //"WindWiFi_FE42"
#define WIFI_PASS "ndxe7dxrra7r7g7p" //"58769331"

void connectWiFi();


/* ---------- MQTT ---------- */
#define MQTT_BROKER "10.4.3.23"  //"192.168.1.5"
#define MQTT_PORT   1883
#define MQTT_TOPIC  "esp32-001/send"
#define MQTT_CLIENT_ID "ESP32_Client"

extern PubSubClient mqttClient;

void setupMQTT();

void connectMQTT();