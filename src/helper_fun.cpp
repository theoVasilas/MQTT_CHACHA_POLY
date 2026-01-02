#include "helper_fun.h"
#include "crypto_engine.h"

#include <Arduino.h>
#include <esp_system.h>

// Helper to print hex buffers
void print_hex(const char *label, uint8_t *buf, size_t len) {
    Serial.print(label);
    for (size_t i = 0; i < len; i++) {
        Serial.printf("%02x", buf[i]);
    }
    Serial.println();
}

void print_ASCII(const char *label, uint8_t *buf, size_t len) {
    Serial.print(label);
    for (size_t i = 0; i < len; i++) {
        Serial.printf("%c", buf[i]);
    }
    Serial.println();
}

void monitorMemory() {
    Serial.printf("MQTT_MAX_PACKET_SIZE = %d bytes\n", MQTT_MAX_PACKET_SIZE);
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Min Free Heap: %d bytes\n", ESP.getMinFreeHeap()); // Lowest point reached
    Serial.printf("Max Alloc Block: %d bytes\n", ESP.getMaxAllocHeap()); // Largest single chunk available
}

void analyze_timing(uint32_t* timing_array, uint16_t timing_count) {
    uint32_t min_t = UINT32_MAX;
    uint32_t max_t = 0;
    uint64_t sum = 0;

    for (uint16_t i = 0; i < timing_count; i++) {
        uint32_t t = timing_array[i];
        min_t = min(min_t, t);
        max_t = max(max_t, t);
        sum += t;
    }

    float avg = (float)sum / timing_count;

    Serial.printf("Timing Analysis over %d runs:\n", timing_count);
    Serial.printf("-------------------------\n");
    Serial.printf("Min Time: %u us\n", min_t);
    Serial.printf("Max Time: %u us\n", max_t);
    Serial.printf("Avg Time: %.2f us\n", avg);
    
    for(uint16_t i = 0; i < timing_count; i++)
        Serial.printf("%d,%lu\n", i, timing_array[i]);

}