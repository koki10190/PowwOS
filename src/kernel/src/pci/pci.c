#include <pci/pci.h>
#include <uart.h>
#include <string.h>

void enumerate_function(uint64_t device_addr, uint64_t function) {
    uint64_t offset = function << 12;
    uint64_t function_addr = device_addr + offset;

    pci_device_header_t *pci_device_header = (pci_device_header_t *)function_addr;
    if (pci_device_header->device_id == 0)
        return;
    if (pci_device_header->device_id == 0xFFFF)
        return;

    uart_puts(to_hstring16(pci_device_header->vendor_id));
    uart_puts(" ");
    uart_puts(to_hstring16(pci_device_header->device_id));
    uart_puts("\n");
}

void enumerate_device(uint64_t bus_addr, uint64_t device) {
    uint64_t offset = device << 15;
    uint64_t device_addr = bus_addr + offset;

    pci_device_header_t *pci_device_header = (pci_device_header_t *)device_addr;
    if (pci_device_header->device_id == 0)
        return;
    if (pci_device_header->device_id == 0xFFFF)
        return;

    for (uint64_t function = 0; function < 8; function++) {
        enumerate_function(device_addr, function);
    }
}

void enumerate_bus(uint64_t base_addr, uint64_t bus) {
    uint64_t offset = bus << 20;
    uint64_t bus_addr = base_addr + offset;

    pci_device_header_t *pci_device_header = (pci_device_header_t *)bus_addr;
    if (pci_device_header->device_id == 0)
        return;
    if (pci_device_header->device_id == 0xFFFF)
        return;

    for (uint64_t device = 0; device < 32; device++) {
        enumerate_device(bus_addr, device);
    }
}

void enumerate_pci(mcfg_header_t *mcfg) {
    int entries = ((mcfg->header.length) - sizeof(mcfg_header_t)) / sizeof(device_config_t);

    for (int i = 0; i < entries; i++) {
        device_config_t *new_device_config = (device_config_t *)((uint64_t)mcfg + sizeof(mcfg_header_t) + (sizeof(device_config_t) * i));

        for (uint64_t bus = new_device_config->start_bus; bus < new_device_config->end_bus; bus++) {
            enumerate_bus(new_device_config->base_addr, bus);
        }
    }
}