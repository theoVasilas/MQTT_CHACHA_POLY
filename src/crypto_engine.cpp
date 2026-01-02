#include "crypto_engine.h"
#include "helper_fun.h"
#include "secrets.h"

#include "esp_log.h"
static const char* TAG = "CRYPTO";

#include <ChaChaPoly.h>
#include <Arduino.h>

void generate_nonce(uint8_t nonce[CHACHA_NONCE_SIZE]) {
    uint32_t *p = (uint32_t *)nonce;
    p[0] = esp_random();
    p[1] = esp_random();
    p[2] = esp_random();
}


void Cha_encryption(
    const uint8_t * plaintext,
    uint8_t * ciphertext,
    uint8_t * tag,
    uint8_t * nonce
){
    ChaChaPoly chacha;

    chacha.clear();
    
    generate_nonce(nonce);
    chacha.setKey(CHACHA_KEY, CHACHA_KEY_SIZE); //is set  in header
    chacha.setIV(nonce, CHACHA_NONCE_SIZE);
    
    // Add AAD (Optional)
    //chacha.addAuthData(aad, aad_len);

    chacha.encrypt(ciphertext, plaintext, CHACHA_MESSAGE_SIZE);
    chacha.computeTag(tag, CHACHA_TAG_SIZE);
}


void Cha_decryption(
    const uint8_t * ciphertext,
    uint8_t * decrypted,
    uint8_t * tag,
    uint8_t * nonce
){
    ChaChaPoly chacha;
    
    //print_hex("Nonce: ", nonce, CHACHA_NONCE_SIZE);
    
    chacha.clear();
    chacha.setKey(CHACHA_KEY, CHACHA_KEY_SIZE); //is set  in header
    chacha.setIV(nonce, CHACHA_NONCE_SIZE);

    // Add AAD (Optional)
    //chacha.addAuthData(aad, aad_len);
    
    chacha.decrypt(decrypted, ciphertext, CHACHA_MESSAGE_SIZE);

    if (!chacha.checkTag(tag, CHACHA_TAG_SIZE)) {
        ESP_LOGE(TAG, "Authentication failed!");
    }
}