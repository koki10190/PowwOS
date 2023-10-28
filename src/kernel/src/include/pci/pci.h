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

extern const char *device_classes[];

void enumerate_pci(mcfg_header_t *mcfg);
const char *get_vendor_name(uint16_t vendor_id);
const char *get_device_name(uint16_t vendor_id, uint16_t device_id);
const char *mass_storage_controller_subclass_mame(uint8_t subclass_code);
const char *serial_bus_controller_subclass_name(uint8_t subclass_code);
const char *bridge_device_subclass_name(uint8_t subclass_code);
const char *get_subclass_name(uint8_t class_code, uint8_t subclass_code);
const char *get_prog_if_name(uint8_t class_code, uint8_t subclass_code, uint8_t progIF);

#endif