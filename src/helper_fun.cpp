#include <declarations.h>
// Helper to print hex buffers
void print_hex(const char *label, uint8_t *buf, size_t len) {
    Serial.print(label);
    for (size_t i = 0; i < len; i++) {
        Serial.printf("%02x", buf[i]);
    }
    Serial.println();
}