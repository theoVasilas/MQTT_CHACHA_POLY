#include <declarations.h>
#include <secrets.h>


void setup() {
    Serial.begin(9600);

    const char* plaintext = "Hello ESP32!";
    Serial.println((char*)plaintext);
    size_t len = strlen(plaintext);
    
    uint8_t ciphertext[len];
    uint8_t decrypted[len + 1];
    uint8_t tag[16];

    ChaChaPoly chacha;

    // --- ENCRYPTION ---
    chacha.clear();
    chacha.setKey(key, 32);
    chacha.setIV(nonce, 12);
    
    // Add AAD (Optional)
    const char* aad = "header";
    chacha.addAuthData(aad, strlen(aad));

    chacha.encrypt(ciphertext, (const uint8_t*)plaintext, len);
    chacha.computeTag(tag, 16);

    Serial.println("--- Encrypted ---");
    print_hex("Cipher: ", ciphertext, len);
    print_hex("Tag:    ", tag, 16);

    // --- DECRYPTION ---
    chacha.clear();
    chacha.setKey(key, 32);
    chacha.setIV(nonce, 12);
    chacha.addAuthData(aad, strlen(aad));

    chacha.decrypt(decrypted, ciphertext, len);
    
    // checkTag returns true if the message is authentic
    if (chacha.checkTag(tag, 16)) {
        decrypted[len] = '\0';
        Serial.print("Decrypted: ");
        Serial.println((char*)decrypted);
    } else {
        Serial.println("AUTHENTICATION FAILED!");
    }
}

void loop() {}