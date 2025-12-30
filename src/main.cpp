#include <Arduino.h>

#include "crypto_engine.h"
#include "helper_fun.h"
#include "wifi_config.h"

static uint8_t plaintext_block[CHACHA_BLOCK_SIZE];
static uint8_t auth_tag[CHACHA_TAG_SIZE];
static uint8_t nonce[CHACHA_NONCE_SIZE];

Message msg;

void setup() {
    Serial.begin(9600);
    
    connectWiFi();
    setupMQTT();
    connectMQTT();

    mqttClient.setCallback(mqttCallback);
    
    memset(plaintext_block, 0x00, CHACHA_BLOCK_SIZE);

    int counter = 1; //debaging purpose

    for (int i = 0; i < 1; i++) {

        sprintf((char*)plaintext_block, "HELLO ESP32 CHACHA %d", counter);
        print_ASCII("Plaintext: ", plaintext_block, CHACHA_BLOCK_SIZE);
        counter++;
        
        generate_nonce(nonce);
        print_hex("Nonce: ", nonce, CHACHA_NONCE_SIZE);

        Cha_encryption(plaintext_block, msg.ciphertext, auth_tag, nonce);
        print_hex("Auth Tag: ", auth_tag, CHACHA_TAG_SIZE);
        //print_hex("Ciphertext: ", ciphertext_block, CHACHA_BLOCK_SIZE);

        //compose the rest of the message
        memcpy(msg.nonce, nonce, CHACHA_NONCE_SIZE);
        memcpy(msg.tag, auth_tag, CHACHA_TAG_SIZE);

        if (!mqttClient.publish(MQTT_TOPIC, (uint8_t*)&msg, sizeof(Message), false)) {
            Serial.println("MQTT publish failed");
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
    if (length == sizeof(Message)) { 
        Serial.println("saving...");
    }else{
        Serial.println("Invalid payload size");
        return;
    }

    Message* msg = (Message*)payload;

    // Access the fields:
    Serial.print("Nonce: ");
    for (int i = 0; i < CHACHA_NONCE_SIZE; i++) {
        Serial.print(msg->nonce[i], HEX); Serial.print(" ");
    }
    Serial.println();

    Serial.print("Tag: ");
    for (int i = 0; i < CHACHA_TAG_SIZE; i++) {
        Serial.print(msg->tag[i], HEX); Serial.print(" ");
    }
    Serial.println();

    Serial.print("Ciphertext: ");
    for (int i = 0; i < CHACHA_BLOCK_SIZE; i++) {
        Serial.print(msg->ciphertext[i], HEX); Serial.print(" ");
    }
    Serial.println();

    Cha_decryption(msg->ciphertext, plaintext_block, msg->tag, msg->nonce);
    Serial.print("Decrypting...");
    print_ASCII("Plaintext: ", plaintext_block, CHACHA_BLOCK_SIZE);

}

