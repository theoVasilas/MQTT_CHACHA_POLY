#ifndef HELPERS_H
#define HELPERS_H

#include <stdint.h>
#include <stddef.h>

void print_hex(const char *label, uint8_t *buf, size_t len) ;

void print_ASCII(const char *label, uint8_t *buf, size_t len) ;

void monitorMemory() ;

void analyze_timing(uint32_t* timing_array, uint16_t timing_count) ;

#endif
