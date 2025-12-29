#include <Arduino.h>

#include "crypto_engine.h"
#include "helper_fun.h"
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

    mqttClient.setCallback(mqttCallback);
    
    memset(plaintext_block, 0x00, CHACHA_BLOCK_SIZE);

    int counter = 1; //debaging purpose

    for (int i = 0; i < 10; i++) {

        sprintf((char*)plaintext_block, "HELLO ESP32 CHACHA %d", counter);
        memset(plaintext_block, 0x00, CHACHA_BLOCK_SIZE);
        print_ASCII("Plaintext: ", plaintext_block, CHACHA_BLOCK_SIZE);
        counter++;
        
        generate_nonce(nonce);
        Cha_encryption(plaintext_block, ciphertext_block, auth_tag, nonce);
        //print_hex("Ciphertext: ", ciphertext_block, CHACHA_BLOCK_SIZE);

        if (!mqttClient.publish(MQTT_TOPIC, ciphertext_block, CHACHA_BLOCK_SIZE, false)) {
            Serial.println("MQTT publish failed (packet too large?)");
        } else {
        Serial.println("Published encrypted message to MQTT");
        }
        
    }
    

    //monitorMemory();
}


void loop() {
    mqttClient.loop(); // Maintain MQTT connection
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.println(topic);

    Serial.print("Payload length: ");
    Serial.println(length);

    // Example: copy binary payload
    if (length == CHACHA_BLOCK_SIZE) { // CHACHA_BLOCK_SIZE is temporary
        Serial.print("saving...");
        memcpy(ciphertext_block, payload, CHACHA_BLOCK_SIZE);
    }

    Cha_decryption(ciphertext_block, plaintext_block, auth_tag, nonce);
    Serial.print("Decrypting...");
    print_ASCII("Plaintext: ", plaintext_block, CHACHA_BLOCK_SIZE);

}


