#include <acpi/acpi.h>
#include <boot.h>
#include <uart.h>
#include <string.h>
#include <pci/pci.h>

void prepare_acpi() {
    sdt_header_t *xsdt = (sdt_header_t *)(kernel_info->rsdp->xsdt_address);

    mcfg_header_t *mcfg = (mcfg_header_t *)find_table(xsdt, (char *)"MCFG");
    uart_puts(to_hstring64((uint64_t)mcfg));
    uart_puts("\n");

    enumerate_pci(mcfg);
}

void *find_table(sdt_header_t *sdt_header, char *signature) {
    int entries = (sdt_header->length - sizeof(sdt_header_t)) / 8;

    for (int i = 0; i < entries; i++) {
        sdt_header_t *new_sdt_header = (sdt_header_t *)*(uint64_t *)((uint64_t)sdt_header + sizeof(sdt_header_t) + (i * 8));

        for (int j = 0; j < 4; j++) {
            if (new_sdt_header->signature[i] != signature[i])
                break;

            if (j == 3)
                return new_sdt_header;
        }
    }

    return 0;
}