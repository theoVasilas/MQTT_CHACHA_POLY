#include <Arduino.h>

#include "crypto_engine.h"
#include "helper_fun.h"
#include "crypto_config.h"
#include "wifi_config.h"

static uint8_t plaintext_block[CHACHA_BLOCK_SIZE];
static uint8_t ciphertext_block[CHACHA_BLOCK_SIZE];
static uint8_t auth_tag[CHACHA_TAG_SIZE];
static uint8_t nonce[CHACHA_NONCE_SIZE];


void setup() {
    Serial.begin(9600);
    
    connectWiFi();
    setupMQTT();
    connectMQTT();

    monitorMemory();
}

void loop() {
    if (!mqttClient.connected()) {
        connectMQTT();
    }

    mqttClient.loop(); // Maintain MQTT connection

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
            Serial.println("Published encrypted message to MQTT");

            delay(500);  // VERY IMPORTANT
        }
    }
}
