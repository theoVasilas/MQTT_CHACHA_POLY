#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>

#include "crypto_engine.h"
#include "helper_fun.h"
#include "crypto_config.h"

static uint8_t plaintext_block[CHACHA_BLOCK_SIZE];
static uint8_t ciphertext_block[CHACHA_BLOCK_SIZE];
static uint8_t auth_tag[CHACHA_TAG_SIZE];
static uint8_t nonce[CHACHA_NONCE_SIZE];

/* ---------- WiFi ---------- */
const char* WIFI_SSID = "WindWiFi_FE42";
const char* WIFI_PASS = "58769331";

/* ---------- MQTT ---------- */
const char* MQTT_BROKER = "192.168.1.5";  // Mosquitto IP
const int   MQTT_PORT   = 1883;
const char* MQTT_TOPIC  = "esp32-001/send";

/* ---------- Clients ---------- */
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void connectWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
}

void connectMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("Connecting to MQTT...");
        if (mqttClient.connect("ESP32_Client")) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" retrying...");
            delay(2000);
        }
    }
}

void setup() {
    Serial.begin(9600);

    connectWiFi();

    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    if (!mqttClient.connected()) {
        connectMQTT();
    }

    // memset(plaintext_block, 0x00, CHACHA_BLOCK_SIZE);
    // strcpy((char*)plaintext_block, "HELLO ESP32 CHACHA");
    // //print_hex("Plaintext: ", plaintext_block, 64);

    // for (int i = 0; i < 10; i++) {
    //     generate_nonce(nonce);

    //     Cha_encryption(plaintext_block, ciphertext_block, auth_tag, nonce);

    //     //print_hex("Ciphertext: ", ciphertext_block, 64);
    //     //print_hex("Tag: ", auth_tag, CHACHA_TAG_SIZE);
        
    //     // Publish ciphertext over MQTT
    //     mqttClient.publish(MQTT_TOPIC, ciphertext_block, CHACHA_BLOCK_SIZE);
        
    //     if (Cha_decryption(ciphertext_block, plaintext_block, auth_tag, nonce)) {
    //         //Serial.println("Decryption OK");
    //         //print_hex("Decrypted: ", plaintext_block, 64);
    //     } else {
    //         Serial.println("Decryption failed!");
    //     }

    // }

    // // monitorMemory();
}

void loop() {
    if (!mqttClient.connected()) {
        connectMQTT();
    }

    mqttClient.loop();

    static bool sent = false;
    if (!sent) {
        sent = true;

        memset(plaintext_block, 0x00, CHACHA_BLOCK_SIZE);
        strcpy((char*)plaintext_block, "HELLO ESP32 CHACHA");

        for (int i = 0; i < 10; i++) {
            generate_nonce(nonce);

            Cha_encryption(plaintext_block, ciphertext_block, auth_tag, nonce);

            // TEMP: send only 64 bytes (safe)
            mqttClient.publish(MQTT_TOPIC, ciphertext_block, 64);

            delay(500);  // VERY IMPORTANT
        }
    }
}
