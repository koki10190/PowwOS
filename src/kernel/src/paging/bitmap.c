#include <paging/bitmap.h>
#include <uart.h>

bool bitmap_get(bitmap_t *this, uint64_t index) {
    if (index > this->size * 8)
        return false;
    uint64_t byte_index = index / 8;
    uint8_t bit_index = index % 8;
    uint8_t bit_indexer = 0b10000000 >> bit_index;
    // uart_puts("Got past indexer\n");

    if ((this->buffer[byte_index] & bit_indexer) > 0) {
        return true;
    }

    return false;
}

void bitmap_set(bitmap_t *this, uint64_t index, bool value) {
    if (index > this->size * 8)
        return;

    uint64_t byte_index = index / 8;
    uint8_t bit_index = index % 8;
    uint8_t bit_indexer = 0b10000000 >> bit_index;

    this->buffer[byte_index] &= ~bit_indexer;
    if (value) {
        this->buffer[byte_index] |= bit_indexer;
    }
}