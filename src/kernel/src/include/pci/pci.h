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

typedef struct __pci_header0_t {
    pci_device_header_t header;
    uint32_t BAR0;
    uint32_t BAR1;
    uint32_t BAR2;
    uint32_t BAR3;
    uint32_t BAR4;
    uint32_t BAR5;
    uint32_t card_bus_cis_ptr;
    uint16_t subsys_vendor_id;
    uint16_t subsys_id;
    uint32_t expansion_rom_base_addr;
    uint8_t capabilities_ptr;
    uint8_t rsv0;
    uint16_t rsv1;
    uint32_t rsv2;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
} pci_header0_t;

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