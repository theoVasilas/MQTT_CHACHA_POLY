#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "wifi_config.h"

void connectWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

/* ---------- Clients ---------- */
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setupMQTT() {
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
}

void connectMQTT() {
    while (!mqttClient.connected()) {

        Serial.print("Connecting to MQTT...");

        if (mqttClient.connect(MQTT_CLIENT_ID)) {

            Serial.println("connected");

        } else {

            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" retrying...");
            
            delay(2000);
        }
    }
}