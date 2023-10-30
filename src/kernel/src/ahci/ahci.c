#include <ahci/ahci.h>
#include <uart.h>
#include <memory.h>
#include <paging/page_frame_allocator.h>

#define HBA_PORT_DEV_PRESENT 0x3
#define HBA_PORT_IPM_ACTIVE 0x1
#define SATA_SIGNATURE_ATAPI 0xEB140101
#define SATA_SIGNATURE_ATA 0x00000101
#define SATA_SIGNATURE_SEMB 0xC33C0101
#define SATA_SIGNATURE_PM 0x96690101
#define HBA_PXCMD_CR 0x8000
#define HBA_PXCMD_FRE 0x0010
#define HBA_PXCMD_ST 0x0001
#define HBA_PXCMD_FR 0x4000

port_type check_port_type(hba_port_t *port) {
    uint32_t sata_status = port->sata_status;

    uint8_t interface_power_management = (sata_status >> 8) & 0b111;
    uint8_t device_detection = sata_status & 0b111;

    if (device_detection != HBA_PORT_DEV_PRESENT)
        return NONE;

    if (interface_power_management != HBA_PORT_IPM_ACTIVE)
        return NONE;

    switch (port->signature) {
    case SATA_SIGNATURE_ATAPI:
        return SATAPI;
    case SATA_SIGNATURE_ATA:
        return SATA;
    case SATA_SIGNATURE_PM:
        return PM;
    case SATA_SIGNATURE_SEMB:
        return SEMB;
    default:
        return NONE;
    }
}

void ahci_driver_init(ahci_driver_t *this, pci_device_header_t *pci_base_addr) {
    this->pci_base_addr = pci_base_addr;

    this->abar = (hba_memory_t *)((pci_header0_t *)pci_base_addr)->BAR5;
    ahci_driver_probe_ports(this);

    for (int i = 0; i < this->port_count; i++) {
        ahci_port_t *port = this->ports[i];
        ahci_port_configure(port);
    }

    uart_puts("[POWW-KERNEL] AHCI Driver initialized\n");
}

void ahci_driver_probe_ports(ahci_driver_t *this) {
    uint32_t ports_implemented = this->abar->ports_implemented;
    for (int i = 0; i < 32; i++) {
        if (ports_implemented & (1 << i)) {
            port_type __port_type = check_port_type(&this->abar->ports[i]);
            if (__port_type == SATA)
                uart_puts("fuck ");

            if (__port_type == SATA || __port_type == SATAPI) {
                this->ports[this->port_count] = (void *)__malloc(sizeof(ahci_port_t));
                this->ports[this->port_count]->__port_type = __port_type;
                this->ports[this->port_count]->port_number = this->port_count;
                this->ports[this->port_count]->hba_port = &this->abar->ports[i];
                this->port_count++;
            }
        }
    }
}

void ahci_port_configure(ahci_port_t *this) {
    ahci_port_stop_cmd(this);

    void *new_base = pg_alloc_request_page(&global_allocator);
    this->hba_port->cmd_list_base = (uint32_t)(uint64_t)new_base;
    this->hba_port->cmd_list_base_upper = (uint32_t)((uint64_t)new_base >> 32);
    m_memset((void *)(this->hba_port->cmd_list_base), 0, 1024);

    void *fis_base = pg_alloc_request_page(&global_allocator);
    this->hba_port->fis_base_address = (uint32_t)(uint64_t)fis_base;
    this->hba_port->fis_base_address_upper = (uint32_t)((uint64_t)fis_base >> 32);
    m_memset(fis_base, 0, 256);

    hba_cmd_header_t *cmd_header = (hba_cmd_header_t *)((uint64_t)this->hba_port->cmd_list_base + ((uint64_t)this->hba_port->cmd_list_base_upper << 32));
    for (int i = 0; i < 32; i++) {
        // cmd_header[i].prdt_len = 8;

        // void *cmd_table_addr = pg_alloc_request_page(&global_allocator);
        // uint64_t addr = (uint64_t)cmd_table_addr + (i << 8);
        // cmd_header[i].cmd_table_base_addr = (uint32_t)(uint64_t)addr;
        // cmd_header[i].cmd_table_base_addr_upper = (uint32_t)((uint64_t)addr >> 32);
        // m_memset(cmd_table_addr, 0, 256);
    }

    ahci_port_start_cmd(this);
}

void ahci_port_start_cmd(ahci_port_t *this) {
    while (this->hba_port->cmd_status & HBA_PXCMD_CR)
        ;

    this->hba_port->cmd_status |= HBA_PXCMD_FRE;
    this->hba_port->cmd_status |= HBA_PXCMD_ST;
}
void ahci_port_stop_cmd(ahci_port_t *this) {
    this->hba_port->cmd_status &= ~HBA_PXCMD_ST;
    this->hba_port->cmd_status &= ~HBA_PXCMD_FRE;

    while (true) {
        if (this->hba_port->cmd_status & HBA_PXCMD_FR)
            continue;
        if (this->hba_port->cmd_status & HBA_PXCMD_CR)
            continue;

        break;
    }
}