#ifndef AHCI_H
#define AHCI_H

#include <stdint.h>
#include <stdbool.h>
#include <pci/pci.h>

#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define HBA_PxIS_TFES (1 << 30)

typedef enum {
    NONE = 0,
    SATA = 1,
    SEMB = 2,
    PM = 3,
    SATAPI = 4
} port_type;

typedef enum {
    FIS_TYPE_REG_H2D = 0x27,
    FIS_TYPE_REG_D2H = 0x34,
    FIS_TYPE_DMA_ACT = 0x39,
    FIS_TYPE_DMA_SETUP = 0x41,
    FIS_TYPE_DATA = 0x46,
    FIS_TYPE_BIST = 0x58,
    FIS_TYPE_PIO_SETUP = 0x5F,
    FIS_TYPE_DEV_BITS = 0xA1
} fis_type;

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

typedef struct __fis_reg_h2d_t {
    uint8_t fis_type;
    uint8_t port_multiplier : 4;
    uint8_t rsv0 : 3;
    uint8_t cmd_control : 1;
    uint8_t command;
    uint8_t feature_low;
    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device_register;
    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t feature_high;
    uint8_t count_low;
    uint8_t count_high;
    uint8_t iso_cmd_completion;
    uint8_t control;
    uint8_t rsv1[4];
} fis_reg_h2d_t;

typedef struct __ahci_port_t {
    hba_port_t *hba_port;
    port_type __port_type;
    uint8_t *buffer;
    uint8_t port_number;
} ahci_port_t;

typedef struct __hba_prdt_entry_t {
    uint32_t database_addr;
    uint32_t database_addr_upper;
    uint32_t rsv0;
    uint32_t byte_count : 22;
    uint32_t rsv1 : 9;
    uint32_t intr_on_completion : 1;

} hba_prdt_entry_t;

typedef struct __hba_cmd_table_t {
    uint8_t command_fis[64];
    uint8_t atapi_cmd[16];
    uint8_t rsv[48];
    hba_prdt_entry_t prdt_entry[];
} hba_cmd_table_t;

void ahci_port_configure(ahci_port_t *this);
void ahci_port_start_cmd(ahci_port_t *this);
void ahci_port_stop_cmd(ahci_port_t *this);
bool ahci_port_read(ahci_port_t *this, uint64_t sector, uint32_t sector_count, void *buffer);

typedef struct __ahci_driver_t {
    pci_device_header_t *pci_base_addr;
    hba_memory_t *abar;
    ahci_port_t *ports[21];
    uint8_t port_count;
} ahci_driver_t;

void ahci_driver_init(ahci_driver_t *this, pci_device_header_t *pci_base_addr);
void ahci_driver_probe_ports(ahci_driver_t *this);

#endif