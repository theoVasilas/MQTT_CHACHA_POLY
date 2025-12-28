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

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASS";
const char* mqtt_server = "192.168.1.100";  // Mosquitto IP

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    Serial.begin(9600);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    client.setServer(mqtt_server, 1883);

    memset(plaintext_block, 0x00, CHACHA_BLOCK_SIZE);
    strcpy((char*)plaintext_block, "HELLO ESP32 CHACHA");
    print_hex("Plaintext: ", plaintext_block, 64);

    for (int i = 0; i < 10; i++) {
        generate_nonce(nonce);

        Cha_encryption(plaintext_block, ciphertext_block, auth_tag, nonce);

        print_hex("Ciphertext: ", ciphertext_block, 64);
        print_hex("Tag: ", auth_tag, CHACHA_TAG_SIZE);
        
        if (Cha_decryption(ciphertext_block, plaintext_block, auth_tag, nonce)) {
            Serial.println("Decryption OK");
            print_hex("Decrypted: ", plaintext_block, 64);
        } else {
            Serial.println("Decryption failed!");
        }

    }

    // monitorMemory();
}

void loop() {}