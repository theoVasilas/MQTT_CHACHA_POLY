#ifndef CRYPTO_ENGINE_H
#define CRYPTO_ENGINE_H

#include <ChaChaPoly.h>
#include <stdint.h>

void Cha_encryption(
    const uint8_t * plaintext,
    uint8_t * ciphertext,
    uint8_t * tag,
    const uint8_t * nonce
);

bool Cha_decryption(
    const uint8_t * ciphertext,
    uint8_t * decrypted,
    const uint8_t * tag,
    const uint8_t *nonce
);

#endif
