#ifndef AHCI_H
#define AHCI_H

#include <stdint.h>
#include <pci/pci.h>

typedef enum {
    NONE = 0,
    SATA = 1,
    SEMB = 2,
    PM = 3,
    SATAPI = 4
} port_type;

typedef struct __hba_port_t {
    uint32_t cmd_list_base;
    uint32_t cmd_list_base_upper;
    uint32_t fis_base_address;
    uint32_t fis_base_address_upper;
    uint32_t interrupt_status;
    uint32_t interrupt_enable;
    uint32_t cmd_status;
    uint32_t rsv0;
    uint32_t task_file_data;
    uint32_t signature;
    uint32_t sata_status;
    uint32_t stata_control;
    uint32_t sata_error;
    uint32_t sata_active;
    uint32_t cmd_issue;
    uint32_t sata_notification;
    uint32_t fis_switch_control;
    uint32_t rsv1[11];
    uint32_t vendor[4];
} hba_port_t;

typedef struct __hba_memory_t {
    uint32_t host_capability;
    uint32_t global_host_control;
    uint32_t interrupt_status;
    uint32_t ports_implemented;
    uint32_t version;
    uint32_t ccc_control;
    uint32_t ccc_ports;
    uint32_t enclosurte_management_location;
    uint32_t enclosurte_management_control;
    uint32_t enclosurte_management_extended;
    uint32_t bios_handoff_ctrl_status;
    uint8_t rsv0[0x74];
    uint8_t vendor[0x60];
    hba_port_t ports[1];
} hba_memory_t;

typedef struct __hba_cmd_header_t {
    uint8_t cmd_fis_length : 5;
    uint8_t atapi : 1;
    uint8_t write : 1;
    uint8_t prefetchable : 1;

    uint8_t reset : 1;
    uint8_t bist : 1;
    uint8_t clear_busy : 1;
    uint8_t rsv0 : 1;
    uint8_t port_multiplier : 4;

    uint16_t prdt_len;
    uint32_t prdt_count;
    uint32_t cmd_table_base_addr;
    uint32_t cmd_table_base_addr_upper;
    uint32_t rsv1[4];
} hba_cmd_header_t;

typedef struct __ahci_port_t {
    hba_port_t *hba_port;
    port_type __port_type;
    uint8_t *buffer;
    uint8_t port_number;
} ahci_port_t;

void ahci_port_configure(ahci_port_t *this);
void ahci_port_start_cmd(ahci_port_t *this);
void ahci_port_stop_cmd(ahci_port_t *this);

typedef struct __ahci_driver_t {
    pci_device_header_t *pci_base_addr;
    hba_memory_t *abar;
    ahci_port_t *ports[21];
    uint8_t port_count;
} ahci_driver_t;

void ahci_driver_init(ahci_driver_t *this, pci_device_header_t *pci_base_addr);
void ahci_driver_probe_ports(ahci_driver_t *this);

#endif