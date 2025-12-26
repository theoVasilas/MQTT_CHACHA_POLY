#include "crypto_engine.h"
#include "crypto_config.h"
#include "secrets.h"

#include <ChaChaPoly.h>

void Cha_encryption(
    const uint8_t * plaintext,
    uint8_t * ciphertext,
    uint8_t * tag,
    const uint8_t * nonce
){
    ChaChaPoly chacha;

    chacha.clear();
    chacha.setKey(CHACHA_KEY, CHACHA_KEY_SIZE);
    chacha.setIV(CHACHA_NONCE, CHACHA_NONCE_SIZE);
    
    // Add AAD (Optional)
    //chacha.addAuthData(aad, aad_len);

    chacha.encrypt(ciphertext, plaintext, CHACHA_BLOCK_SIZE);
    chacha.computeTag(tag, CHACHA_TAG_SIZE);
}


bool Cha_decryption(
    const uint8_t * ciphertext,
    uint8_t * decrypted,
    const uint8_t * tag,
    const uint8_t * nonce
){
    ChaChaPoly chacha;
    
    chacha.clear();
    chacha.setKey(CHACHA_KEY, CHACHA_KEY_SIZE);
    chacha.setIV(CHACHA_NONCE, CHACHA_NONCE_SIZE);

    // Add AAD (Optional)
    //chacha.addAuthData(aad, aad_len);

    chacha.decrypt(decrypted, ciphertext, CHACHA_BLOCK_SIZE);

    
    return chacha.checkTag(tag, CHACHA_TAG_SIZE);

}