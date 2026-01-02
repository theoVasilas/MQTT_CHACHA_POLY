#include <Arduino.h>

#include "crypto_engine.h"
#include "helper_fun.h"
#include "wifi_config.h"



static uint8_t plaintext_block[CHACHA_MESSAGE_SIZE];
static uint8_t auth_tag[CHACHA_TAG_SIZE];
static uint8_t nonce[CHACHA_NONCE_SIZE];

//--------- Time tracking variables -----------
static uint32_t timing_AES[REPETITIONS];
uint16_t timing_count = 0;
int elapsed = 0;
uint64_t t_end = 0;
uint64_t t_start = 0;
uint64_t start_communication = 0;
uint64_t end_communication = 0;
//---------------------------------------------

Message msg;

void setup() {
    Serial.begin(115200);

    monitorMemory();

    connectWiFi();
    setupMQTT();
    connectMQTT();

    #ifdef DEVICE_ROLE_SUBSCRIBER
        mqttClient.setCallback(mqttCallback);
        mqttClient.subscribe(MQTT_TOPIC);
        ESP_LOGI("MAIN", "Started as SUBSCRIBER");
    #endif

    #ifdef DEVICE_ROLE_PUBLISHER

        start_communication = esp_timer_get_time();

        ESP_LOGI("MAIN", "Started as PUBLISHER");
        memset(plaintext_block, 0xFF, CHACHA_MESSAGE_SIZE);

        int counter = 1; //debaging purpose

        for (int i = 0; i < REPETITIONS; i++) {

            sprintf((char*)plaintext_block, "HELLO ESP32 CHACHA %d", counter);
            //print_ASCII("Plaintext: ", plaintext_block, CHACHA_MESSAGE_SIZE);
            counter++;

            //-- start timing---
            elapsed = 0;
            t_start = esp_timer_get_time();
            //------------------

            Cha_encryption(plaintext_block, msg.ciphertext, auth_tag, nonce);

            //-- end timing---
            t_end = esp_timer_get_time();
            elapsed = t_end - t_start;
            //------------------

            //print_hex("Auth Tag: ", auth_tag, CHACHA_TAG_SIZE);
            //print_hex("Ciphertext: ", ciphertext_block, CHACHA_MESSAGE_SIZE);

            //compose the rest of the message
            memcpy(msg.nonce, nonce, CHACHA_NONCE_SIZE);
            memcpy(msg.tag, auth_tag, CHACHA_TAG_SIZE);

            if (!mqttClient.publish(MQTT_TOPIC, (uint8_t*)&msg, sizeof(Message), false)) {
                Serial.println("MQTT publish failed");
            } else {
                Serial.println("Published encrypted message to MQTT");
            }

            //float bytes_per_sec = (AES_BLOCK_SIZE * 1e6) / (t_end - t_start);
            timing_AES[timing_count++] = elapsed;

        }

        end_communication = esp_timer_get_time();
        Serial.printf("Total communication time: %lu us\n", end_communication - start_communication);
        analyze_timing(timing_AES, timing_count);

        
    #endif
    
}

bool flage = 0;
void loop() {

    #ifdef DEVICE_ROLE_SUBSCRIBER

        if(flage == 0){
            start_communication = esp_timer_get_time();
            flage = 1;
        }

        mqttClient.loop(); // Maintain MQTT connection

        if (timing_count == REPETITIONS) {
            end_communication = esp_timer_get_time();
            Serial.printf("Total communication time: %lu us\n", end_communication - start_communication);
            analyze_timing(timing_AES, timing_count);
            timing_count++; //to prevent re-entering
        } 

    #endif
    
}



void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // Serial.print("Message arrived on topic: ");
    // Serial.println(topic);

    // Serial.print("Payload length: ");
    // Serial.println(length);

    // // Example: copy binary payload
    // if (length == sizeof(Message)) { 
    //     Serial.println("saving...");
    // }else{
    //     Serial.println("Invalid payload size");
    //     return;
    // }

    Message* msg = (Message*)payload;

    // // Access the fields:
    // Serial.print("Nonce: ");
    // for (int i = 0; i < CHACHA_NONCE_SIZE; i++) {
    //     Serial.print(msg->nonce[i], HEX); Serial.print(" ");
    // }
    // Serial.println();

    // Serial.print("Tag: ");
    // for (int i = 0; i < CHACHA_TAG_SIZE; i++) {
    //     Serial.print(msg->tag[i], HEX); Serial.print(" ");
    // }
    // Serial.println();

    // Serial.print("Ciphertext: ");
    // for (int i = 0; i < CHACHA_MESSAGE_SIZE; i++) {
    //     Serial.print(msg->ciphertext[i], HEX); Serial.print(" ");
    // }
    // Serial.println();

    bool done = false;

    //-- start timing---
    elapsed = 0;
    t_start = esp_timer_get_time();
    //------------------

    Cha_decryption(msg->ciphertext, plaintext_block, msg->tag, msg->nonce);

    //-- end timing---
    t_end = esp_timer_get_time();
    elapsed = t_end - t_start;
    timing_AES[timing_count++] = elapsed;
    //------------------

    //Serial.print("Decrypting...");
    //print_ASCII("Plaintext: ", plaintext_block, CHACHA_MESSAGE_SIZE);

}

