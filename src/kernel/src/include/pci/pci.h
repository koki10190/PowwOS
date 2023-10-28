#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <acpi/acpi.h>

typedef struct __pci_device_header_t {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
} pci_device_header_t;

void enumerate_pci(mcfg_header_t *mcfg);

#endif