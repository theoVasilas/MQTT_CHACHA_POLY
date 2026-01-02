#ifndef CRYPTO_ENGINE_H
#define CRYPTO_ENGINE_H

#define  CHACHA_MESSAGE_SIZE  256
#define  REPETITIONS  100 // 100 * 1024 bytes = 100 KB
// #define  MQTT_MAX_PACKET_SIZE  1024 //(256 + 12 + 16 + 50) 

#define CHACHA_NONCE_SIZE   12
#define CHACHA_KEY_SIZE     32
#define CHACHA_TAG_SIZE     16

#include <ChaChaPoly.h>
#include <stdint.h>

struct Message {
    uint8_t nonce[CHACHA_NONCE_SIZE];
    uint8_t tag[CHACHA_TAG_SIZE];
    uint8_t ciphertext[CHACHA_MESSAGE_SIZE];
};

void generate_nonce(uint8_t nonce[CHACHA_NONCE_SIZE]);

void Cha_encryption(
    const uint8_t * plaintext,
    uint8_t * ciphertext,
    uint8_t * tag,
    uint8_t * nonce
);

void Cha_decryption(
    const uint8_t * ciphertext,
    uint8_t * decrypted,
    uint8_t * tag,
    uint8_t * nonce
);

#endif
