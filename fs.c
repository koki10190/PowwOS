#include <fs.h>
#include <string.h>
#include <memory.h>
#include <port_io.h>

// Global variables
unsigned int fat_type;
unsigned int first_fat_sector;
unsigned int first_data_sector;
unsigned int total_clusters;
fat_BS_t bootsect;

extern unsigned long part_start_lba;
extern unsigned long part_length;
extern unsigned char bios_disk_number;
extern Real_Mode_SW_Int real_mode_linear_sw_int;
extern Disk_Address_Packet disk_address_packet;

int int13h_read_o(unsigned long sector_offset, unsigned char num_blocks, unsigned long readLocationOffset) {

    // Bounds check; only read sectors on the booted partition
    if (sector_offset + num_blocks > part_length)
        return -1;

    // Size check; only read blocks if the resulting size read will still be between 0x4000 and 0x8000
    if (readLocationOffset + num_blocks * 512 > (0x8000 - 0x4000))
        return -1;

    // Sanity check; BIOS int13h can only read up to 0x7F blocks
    if (num_blocks > 0x7F)
        return -1;

    sector_offset = sector_offset + part_start_lba;

    // Implementing LBA28

    // Send out LBA and read instructions to disk
    outb(0x1F1, NULL);
    outb(0x1F2, num_blocks);
    outb(0x1F3, (unsigned char)sector_offset);
    outb(0x1F4, (unsigned char)(sector_offset >> 8));
    outb(0x1F5, (unsigned char)(sector_offset >> 16));
    outb(0x1F6, 0xE0 | (0 << 4) | ((sector_offset >> 24) & 0x0F));
    outb(0x1F7, 0x20);

    unsigned char iterator = 0;
    unsigned short *buffer = DISK_READ_LOCATION + readLocationOffset;
    for (iterator = 0; iterator < num_blocks; iterator++) {
        while (!(inb(0x1F7) & 0x08)) {
        }

        // for read:

        unsigned short idx = 0;
        // read in bytes from disk
        for (idx = 0; idx < 256; idx++) {
            buffer[idx] = inw(0x1F0);
        }
        buffer += idx;
    }
    return 0;
}

// Write sector(s) to booted partition from 0x4000:0000
// Returns 0 on success and non-zero on failure
int int13h_write_o(unsigned long sector_offset, unsigned char num_blocks, unsigned long writeLocationOffset) {

    // Bounds check; only write sectors on the booted partition
    if (sector_offset + num_blocks > part_length)
        return -1;

    // Size check; only write blocks if the resulting size written will still be between 0x4000 and 0x8000
    if (writeLocationOffset + num_blocks * 512 > (0x8000 - 0x4000))
        return -1;

    // Sanity check; BIOS int13h can only write up to 0x7F blocks
    if (num_blocks > 0x7F)
        return -1;

    sector_offset = sector_offset + part_start_lba;

    // Implementing LBA28

    // Send out LBA and write instructions to disk
    outb(0x1F1, NULL);
    outb(0x1F2, num_blocks);
    outb(0x1F3, (unsigned char)sector_offset);
    outb(0x1F4, (unsigned char)(sector_offset >> 8));
    outb(0x1F5, (unsigned char)(sector_offset >> 16));
    outb(0x1F6, 0xE0 | (0 << 4) | ((sector_offset >> 24) & 0x0F));
    outb(0x1F7, 0x30);

    unsigned char iterator = 0;
    unsigned short *buffer = DISK_WRITE_LOCATION + writeLocationOffset;
    for (iterator = 0; iterator < num_blocks; iterator++) {
        // check for errors before writing
        while (1) {
            if ((inb(0x1F7) & 0x01) == 0x01)
                printss("error1\n");
            else if ((inb(0x1F7) & 0x40) != 0x40)
                printss("Not ready!\n");
            else if ((inb(0x1F7) & 0x80) == 0x80)
                printss("Busy!\n");
            else if ((inb(0x1F7) & 0x08) != 0x08)
                printss("Have no data/not ready to recieve PIO!\n");
            else
                break;
        }

        unsigned short idx = 0;
        // send out data to disk
        for (idx = 0; idx < 256; idx++) {
            outw(0x1F0, buffer[idx]);

            // short delay
            inb(0x323);
            inb(0x323);
            inb(0x323);
        }
        buffer += idx;
    }
    outb(0x1F7, 0xE7);        // send cache clear
    while (inb(0x1F7) & 0x80) // wait for drive to finish clearing cache
    {
    }
    return 0;
}

// Read sector(s) from booted partition to 0x4000:0000
// Returns 0 on success and non-zero on failure
int int13h_read(unsigned long sector_offset, unsigned char num_blocks) {
    return int13h_read_o(sector_offset, num_blocks, 0);
}

// Write sector(s) to booted partition from 0x4000:0000
// Returns 0 on success and non-zero on failure
int int13h_write(unsigned long sector_offset, unsigned char num_blocks) {
    return int13h_write_o(sector_offset, num_blocks, 0);
}

// Initializes struct "bootsect" to store critical data from the boot sector of the volume
int FATInitialize() {
    // reads the first sector of the FAT
    if (int13h_read(0, 1) != 0) {
        return -1;
    }

    fat_BS_t *bootstruct = (fat_BS_t *)DISK_READ_LOCATION;

    total_clusters = bootstruct->total_sectors_16 / bootstruct->sectors_per_cluster;

    if (total_clusters == 0) // there's more than 65535 sectors, find the real number
    {
        total_clusters = bootstruct->total_sectors_32 / bootstruct->sectors_per_cluster;
    }

    if (total_clusters < 4085) {
        fat_type = 12;
        first_data_sector = bootstruct->reserved_sector_count + bootstruct->table_count * bootstruct->table_size_16 + (bootstruct->root_entry_count * 32 + bootstruct->bytes_per_sector - 1) / bootstruct->bytes_per_sector; // Explanation: the first data sector is after the reserved sectors, the FAT table sectors, and the root directory sectors. The size of the root directory is found by multiplying the amount of root entries by the size of each entry (32 bytes), then adding bytes per sector - 1 so that when divided by bytes per sector, the calculation starts at 1 sector, not zero, while also maintaining 1 sector when there are exactly 512 bytes worth of entries.
    } else {
        if (total_clusters < 65525) {
            fat_type = 16;
            first_data_sector = bootstruct->reserved_sector_count + bootstruct->table_count * bootstruct->table_size_16 + (bootstruct->root_entry_count * 32 + bootstruct->bytes_per_sector - 1) / bootstruct->bytes_per_sector; // Explanation: the first data sector is after the reserved sectors, the FAT table sectors, and the root directory sectors. The size of the root directory is found by multiplying the amount of root entries by the size of each entry (32 bytes), then adding bytes per sector - 1 so that when divided by bytes per sector, the calculation starts at 1 sector, not zero, while also maintaining 1 sector when there are exactly 512 bytes worth of entries.
        } else {
            fat_type = 32;
            first_data_sector = bootstruct->reserved_sector_count + bootstruct->table_count * ((fat_extBS_32_t *)(bootstruct->extended_section))->table_size_32;
        }
    }

    __memcpy((void *)&bootsect, (void *)bootstruct, 512);

    first_fat_sector = bootstruct->reserved_sector_count;

    return 0;
}

// read FAT table
// This function deals in absolute data clusters
int FATRead(unsigned int clusterNum) {
    if (clusterNum < 2 || clusterNum >= total_clusters) {
        return -1;
    }

    if (fat_type == 32) {
        unsigned int cluster_size = bootsect.bytes_per_sector * bootsect.sectors_per_cluster;
        unsigned char FAT_table[32 * 1024] = {'\0'}; // Takes into consideration the largest standard cluster size (32kB) since arrays can't be dynamically allocated without "new" :/
        unsigned int fat_offset = clusterNum * 4;
        unsigned int fat_sector = first_fat_sector + (fat_offset / cluster_size);
        unsigned int ent_offset = fat_offset % cluster_size;

        // at this point you need to read from sector "fat_sector" on the disk into "FAT_table".
        if (int13h_read(fat_sector, 1) != 0) {
            return -1;
        }
        __memcpy(&FAT_table, (char *)DISK_READ_LOCATION, bootsect.bytes_per_sector);

        // remember to ignore the high 4 bits.
        unsigned int table_value = *(unsigned int *)&FAT_table[ent_offset] & 0x0FFFFFFF;

        // the variable "table_value" now has the information you need about the next cluster in the chain.
        return table_value;
    } else if (fat_type == 16) {
        unsigned int cluster_size = bootsect.bytes_per_sector * bootsect.sectors_per_cluster;
        unsigned char FAT_table[32 * 1024]; // Takes into consideration the largest standard cluster size (32kB) since arrays can't be dynamically allocated without "new" :/
        unsigned int fat_offset = clusterNum * 2;
        unsigned int fat_sector = first_fat_sector + (fat_offset / cluster_size);
        unsigned int ent_offset = fat_offset % cluster_size;

        // at this point you need to read from sector "fat_sector" on the disk into "FAT_table".
        if (int13h_read(fat_sector, 1) != 0) {
            return -1;
        }
        __memcpy(&FAT_table, (char *)DISK_READ_LOCATION, bootsect.bytes_per_sector);

        unsigned short table_value = *(unsigned short *)&FAT_table[ent_offset];

        // the variable "table_value" now has the information you need about the next cluster in the chain.
        return table_value;
    }
    /*else if (fat_type == 12)
    {
    unsigned int cluster_size = bootsect.bytes_per_sector * bootsect.sectors_per_cluster;
    unsigned char FAT_table[32 * 1024]; //Takes into consideration the largest standard cluster size (32kB) since arrays can't be dynamically allocated without "new" :/
    unsigned int fat_offset = clusterNum + (clusterNum / 2);// multiply by 1.5
    unsigned int fat_sector = first_fat_sector + (fat_offset / cluster_size);
    unsigned int ent_offset = fat_offset % cluster_size;

    //at this point you need to read from sector "fat_sector" on the disk into "FAT_table".

    unsigned short table_value = *(unsigned short*)&FAT_table[ent_offset];

    if (current_cluster & 0x0001)
    table_value = table_value >> 4;
    else
    table_value = table_value & 0x0FFF;
    }*/
    else {
        return -1;
    }
}

int FATWrite(unsigned int clusterNum, unsigned int clusterVal) {
    // clusterVal does not need to be checked, since all values from 0 - 0xFFFFFFFF are valid.

    if (clusterNum < 2 || clusterNum >= total_clusters) {
        return -1;
    }

    if (fat_type == 32) {
        unsigned int cluster_size = bootsect.bytes_per_sector * bootsect.sectors_per_cluster;
        unsigned char FAT_table[32 * 1024] = {'\0'}; // Takes into consideration the largest standard cluster size (32kB) since arrays can't be dynamically allocated without "new" :/
        unsigned int fat_offset = clusterNum * 4;
        unsigned int fat_sector = first_fat_sector + (fat_offset / cluster_size);
        unsigned int ent_offset = fat_offset % cluster_size;

        // at this point you need to read from sector "fat_sector" on the disk into "FAT_table".
        if (int13h_read(fat_sector, 1) != 0) {
            return -1;
        }
        __memcpy(&FAT_table, (char *)DISK_READ_LOCATION, bootsect.bytes_per_sector);

        // copy clusterVal into FAT_table
        *(unsigned int *)&FAT_table[ent_offset] = clusterVal;

        // send modified FAT_table back to disk
        __memcpy((char *)DISK_WRITE_LOCATION, &FAT_table, bootsect.bytes_per_sector);

        if (int13h_write(fat_sector, 1) != 0) {
            return -1;
        }

        return 0;
    } else if (fat_type == 16) {
        unsigned int cluster_size = bootsect.bytes_per_sector * bootsect.sectors_per_cluster;
        unsigned char FAT_table[32 * 1024]; // Takes into consideration the largest standard cluster size (32kB) since arrays can't be dynamically allocated without "new" :/
        unsigned int fat_offset = clusterNum * 2;
        unsigned int fat_sector = first_fat_sector + (fat_offset / cluster_size);
        unsigned int ent_offset = fat_offset % cluster_size;

        // at this point you need to read from sector "fat_sector" on the disk into "FAT_table".
        if (int13h_read(fat_sector, 1) != 0) {
            return -1;
        }
        __memcpy(&FAT_table, (char *)DISK_READ_LOCATION, bootsect.bytes_per_sector);

        // copy clusterVal into FAT_table
        *(unsigned short *)&FAT_table[ent_offset] = (unsigned short)clusterVal;

        // send modified FAT_table back to disk
        __memcpy((char *)DISK_WRITE_LOCATION, &FAT_table, bootsect.bytes_per_sector);

        if (int13h_write(fat_sector, 1) != 0) {
            return -1;
        }

        return 0;
    }
    /*else if (fat_type == 12)
    {
    NULL; //Not Implemented!
    }*/
    else {
        return -1;
    }
}

unsigned int allocateFreeFAT() {
    // use generic variables so that the function can work with either FAT32, FAT16, or FAT12 without any code modifications.
    unsigned int free_cluster = BAD_CLUSTER_12;
    unsigned int bad_cluster = BAD_CLUSTER_12;
    unsigned int end_cluster = BAD_CLUSTER_12;

    // Associate the generic variables with the appropriate values depending on FAT type.
    if (fat_type == 32) {
        free_cluster = FREE_CLUSTER_32;
        bad_cluster = BAD_CLUSTER_32;
        end_cluster = END_CLUSTER_32;
    } else if (fat_type == 16) {
        free_cluster = FREE_CLUSTER_16;
        bad_cluster = BAD_CLUSTER_16;
        end_cluster = END_CLUSTER_16;
    } else if (fat_type == 12) {
        free_cluster = FREE_CLUSTER_12;
        bad_cluster = BAD_CLUSTER_12;
        end_cluster = END_CLUSTER_12;
    } else {
        return BAD_CLUSTER_12;
    }

    unsigned int cluster = 2;
    unsigned int clusterStatus = free_cluster;

    // iterate through the clusters, looking for a free cluster
    while (cluster < total_clusters) {
        clusterStatus = FATRead(cluster);

        if (clusterStatus == free_cluster) {
            // cluster found, allocate it.
            if (FATWrite(cluster, end_cluster) == 0)
                return cluster;
            else {
                return bad_cluster;
            }
        } else if (clusterStatus < 0) {
            return bad_cluster;
        }

        cluster++; // cluster is taken, check the next one
    }

    return bad_cluster; // no free clusters were found, return bad_cluster as a signal
}

// Reads one cluster and dumps it to DISK_READ_LOCATION, offset "cluster_size" number of bytes from DISK_READ_LOCATION
// This function deals in absolute data clusters
int clusterRead(unsigned int clusterNum, unsigned int clusterOffset) {
    if (clusterNum < 2 || clusterNum >= total_clusters) {
        return -1;
    }

    // not sure how to error-check clusterOffset, leave it to int13h_read_o to error check for me.

    unsigned int start_sect = (clusterNum - 2) * (unsigned short)bootsect.sectors_per_cluster + first_data_sector; // Explanation: Since the root cluster is cluster 2, but data starts at first_data_sector, subtract 2 to get the proper cluster offset from zero.

    if (int13h_read_o(start_sect, (unsigned short)bootsect.sectors_per_cluster, clusterOffset * (unsigned short)bootsect.sectors_per_cluster * (unsigned short)bootsect.bytes_per_sector) != 0) {
        return -1;
    } else
        return 0;
}

// Deals in absolute clusters
// contentsToWrite: contains a pointer to the data to be written to disk
// contentSize: contains how big contentsToWrite's data is (in bytes)
// contentBuffOffset: sets how far offset from DISK_WRITE_LOCATION to place the data from contentsToWrite in preparation for writing to disk (in clusters)
// clusterNum: Specifies the on-disk cluster to write the data to
int clusterWrite(void *contentsToWrite, unsigned int contentSize, unsigned int contentBuffOffset, unsigned int clusterNum) {
    if (clusterNum < 2 || clusterNum >= total_clusters) {
        return -1;
    }

    // not sure how to error-check contnetBuffOffset, leave it to int13h_write_o to error check for me.

    unsigned int byteOffset = contentBuffOffset * (unsigned short)bootsect.sectors_per_cluster * (unsigned short)bootsect.bytes_per_sector; // converts cluster memory offset into bytes

    // copy contents to be written to disk to the memory write location
    __memcpy((char *)DISK_WRITE_LOCATION + byteOffset, contentsToWrite, contentSize);

    unsigned int start_sect = (clusterNum - 2) * (unsigned short)bootsect.sectors_per_cluster + first_data_sector; // Explanation: Since the root cluster is cluster 2, but data starts at first_data_sector, subtract 2 to get the proper cluster offset from zero.

    if (int13h_write_o(start_sect, (unsigned short)bootsect.sectors_per_cluster, byteOffset) != 0) {
        return -1;
    } else
        return 0;
}

//. and .. entries not supported yet!

// receives the cluster to list, and will list all regular entries and directories, plus whatever attributes are passed in
// returns: -1 is a general error
int directoryList(const unsigned int cluster, unsigned char attributesToAdd, BOOL exclusive) {
    if (cluster < 2 || cluster >= total_clusters) {
        return -1;
    }

    const unsigned char default_hidden_attributes = (FILE_HIDDEN | FILE_SYSTEM); // FILE_LONG_NAME is ALWAYS hidden.
    // unsigned char attributes_to_display = 0;

    unsigned char attributes_to_hide = default_hidden_attributes;

    if (exclusive == FALSE) // for when trying to see the normally-hidden entries
        attributes_to_hide &= (~attributesToAdd);
    else if (exclusive == TRUE) // when trying to filter to a set attribute (e.g. archive attribute)
        attributes_to_hide = (~attributesToAdd);

    // read cluster of the directory/subdirectory
    if (clusterRead(cluster, 0) != 0) {
        return -1;
    }
    directory_entry_t *file_metadata = (directory_entry_t *)DISK_READ_LOCATION;
    unsigned int meta_pointer_iterator_count = 0; // holds how many directories have been looked at

    // iterate through clusters, checking for a file name match
    while (1) {
        if (file_metadata->file_name[0] == ENTRY_END) // end of directory entries; searching can stop now
        {
            break;
        } else if (strncmp(file_metadata->file_name, "..", 2) == 0 || strncmp(file_metadata->file_name, ".", 1) == 0) {
            if (file_metadata->file_name[1] == '.')
                ;
            else
                file_metadata++;

            meta_pointer_iterator_count++;
        } else if (((file_metadata->file_name)[0] == ENTRY_FREE) || ((file_metadata->attributes & FILE_LONG_NAME) == FILE_LONG_NAME) || ((file_metadata->attributes & attributes_to_hide) != 0)) // if the entry is a free entry, a long name, or it contains an attribute not wanted
        {
            if (meta_pointer_iterator_count < bootsect.bytes_per_sector * bootsect.sectors_per_cluster / sizeof(directory_entry_t) - 1) // if the pointer hasn't iterated outside of what that cluster can hold (the 1 is to prevent the comparisons from the line above from reading past the cluster boundary)
            {
                file_metadata++;
                meta_pointer_iterator_count++;
            } else // search next cluster in directory
            {
                unsigned int next_cluster = FATRead(cluster);

                if ((next_cluster >= END_CLUSTER_32 && fat_type == 32) || (next_cluster >= END_CLUSTER_16 && fat_type == 16) || (next_cluster >= END_CLUSTER_12 && fat_type == 12))
                    break;
                else if (next_cluster < 0) {
                    return -1;
                } else
                    return directoryList(next_cluster, attributesToAdd, exclusive); // search next cluster
            }
        } else {
            char conversion[13];
            convertFromFATFormat((char *)file_metadata->file_name, conversion);
            if ((file_metadata->attributes & FILE_DIRECTORY) != FILE_DIRECTORY) {
            } else if ((file_metadata->attributes & FILE_DIRECTORY) == FILE_DIRECTORY) {
            }

            file_metadata++;
            meta_pointer_iterator_count++;
        }
    }

    return 0; // done searching
}

// receives the cluster to read for a directory and the requested file, and will iterate through the directory's clusters - returning the entry for the searched file/subfolder, or no file/subfolder
// return value holds success or failure code, file holds directory entry if file is found
// entryOffset points to where the directory entry was found in sizeof(directory_entry_t) starting from zero (can be NULL)
// returns: -1 is a general error, -2 is a "not found" error
int directorySearch(const char *filepart, const unsigned int cluster, directory_entry_t *file, unsigned int *entryOffset) {
    if (cluster < 2 || cluster >= total_clusters) {
        return -1;
    }

    char searchName[13] = {'\0'};
    strcpy(searchName, filepart);

    // the file path piece sent in is not in FAT format; convert.
    if (testIfFATFormat(searchName) != 0)
        convertToFATFormat(searchName);

    // read cluster of the directory/subdirectory
    if (clusterRead(cluster, 0) != 0) {
        return -1;
    }
    directory_entry_t *file_metadata = (directory_entry_t *)DISK_READ_LOCATION;
    unsigned int meta_pointer_iterator_count = 0; // holds how many directories have been looked at

    // iterate through clusters, checking for a file name match
    while (1) {
        if (file_metadata->file_name[0] == ENTRY_END) // end of directory entries; searching can stop now
            break;
        else if (strncmp((char *)file_metadata->file_name, searchName, 11) != 0) // if the file doesn't match
        {
            if (meta_pointer_iterator_count < bootsect.bytes_per_sector * bootsect.sectors_per_cluster / sizeof(directory_entry_t) - 1) // if the pointer hasn't iterated outside of what that cluster can hold (the 1 is to prevent strncmp from the line above from reading past the cluster boundary)
            {
                file_metadata++;
                meta_pointer_iterator_count++;
            } else {
                int next_cluster = FATRead(cluster);

                if ((next_cluster >= END_CLUSTER_32 && fat_type == 32) || (next_cluster >= END_CLUSTER_16 && fat_type == 16) || (next_cluster >= END_CLUSTER_12 && fat_type == 12))
                    break; // no more clusters to search
                else if (next_cluster < 0) {
                    return -1;
                } else
                    return directorySearch(filepart, next_cluster, file, entryOffset); // search next cluster
            }
        } else // found a file match!
        {
            if (file != NULL) {
                __memcpy(file, file_metadata, sizeof(directory_entry_t)); // copy found data to file
            }

            if (entryOffset != NULL)
                *entryOffset = meta_pointer_iterator_count;

            return 0;
        }
    }

    return -2; // nothing found, return error.
}

//. and .. entries not supported yet!

// pass in the cluster to write the directory to and the directory struct to write.
// struct should only have a file name, attributes, and size. the rest will be filled in automatically
int directoryAdd(const unsigned int cluster, directory_entry_t *file_to_add) {
    if (testIfFATFormat(file_to_add->file_name) != 0) {
        return -1;
    }

    // read cluster of the directory/subdirectory
    if (clusterRead(cluster, 0) != 0) {
        return -1;
    }
    directory_entry_t *file_metadata = (directory_entry_t *)DISK_READ_LOCATION;
    unsigned int meta_pointer_iterator_count = 0; // holds how many directories have been looked at

    while (1) {
        if (file_metadata->file_name[0] != ENTRY_FREE && file_metadata->file_name[0] != ENTRY_END) // if the file directory slot isn't free
        {
            if (meta_pointer_iterator_count < bootsect.bytes_per_sector * bootsect.sectors_per_cluster / sizeof(directory_entry_t) - 1) // if the pointer hasn't iterated outside of what that cluster can hold (the 1 is to prevent strncmp from the line above from reading past the cluster boundary)
            {
                file_metadata++;
                meta_pointer_iterator_count++;
            } else {
                unsigned int next_cluster = FATRead(cluster);

                if ((next_cluster >= END_CLUSTER_32 && fat_type == 32) || (next_cluster >= END_CLUSTER_16 && fat_type == 16) || (next_cluster >= END_CLUSTER_12 && fat_type == 12)) // no free spaces left in the directory cluster, and no more clusters to search. Allocate a new one.
                {
                    next_cluster = allocateFreeFAT();

                    if ((next_cluster == BAD_CLUSTER_32 && fat_type == 32) || (next_cluster == BAD_CLUSTER_16 && fat_type == 16) || (next_cluster == BAD_CLUSTER_12 && fat_type == 12)) // allocation unsuccessful
                    {
                        return -1;
                    }

                    // write the new cluster number to the previous cluster's FAT
                    if (FATWrite(cluster, next_cluster) != 0) {
                        return -1;
                    }
                }

                return directoryAdd(next_cluster, file_to_add); // search next cluster
            }
        } else {
            // convertToFATFormat((char*)file_to_add->file_name); //convert name to FAT format before saving
            // DO NOT CONVERT FILE NAME TO FAT FORMAT - IT SHOULD ALREADY BE IN SAID FORMAT!
            unsigned short dot_checker = 0;
            for (dot_checker = 0; dot_checker < 11; dot_checker++) {
                if (file_to_add->file_name[dot_checker] == '.') {
                    return -1;
                }
            }
            // uppercase_str((char *)file_to_add->file_name);
            file_to_add->creation_date = CurrentDate();
            file_to_add->creation_time = CurrentTime();
            file_to_add->creation_time_tenths = CurrentTimeTenths();
            file_to_add->last_accessed = file_to_add->creation_date;
            file_to_add->last_modification_date = file_to_add->creation_date;
            file_to_add->last_modification_time = file_to_add->creation_time;

            // allocate new cluster for new file
            unsigned int new_cluster = allocateFreeFAT();

            if ((new_cluster == BAD_CLUSTER_32 && fat_type == 32) || (new_cluster == BAD_CLUSTER_16 && fat_type == 16) || (new_cluster == BAD_CLUSTER_12 && fat_type == 12)) // allocation unsuccessful
            {
                return -1;
            }

            file_to_add->low_bits = GET_ENTRY_LOW_BITS(new_cluster);
            file_to_add->high_bits = GET_ENTRY_HIGH_BITS(new_cluster);

            // copy data to empty location
            __memcpy(file_metadata, file_to_add, sizeof(directory_entry_t));

            if (clusterWrite((void *)DISK_WRITE_LOCATION, bootsect.bytes_per_sector * bootsect.sectors_per_cluster, 0, cluster) != 0) {
                return -1;
            }
            return 0;
        }
    }

    return -1; // return error.
}

// retrieves a specified file from the File System (readInOffset is in clusters)
// Returns: -1 is general error, -2 is directory not found, -3 is path specified is a directory instead of a file
int getFile(const char *filePath, char **fileContents, directory_entry_t *fileMeta, unsigned int readInOffset) {
    if (fileContents == NULL || fileMeta == NULL) {
        return -1;
    }

    char fileNamePart[256]; // holds the part of the path to be searched

    unsigned short start = 3; // starting at 3 to skip the "C:\" bit
    unsigned int active_cluster;
    if (fat_type == 32)
        active_cluster = ((fat_extBS_32_t *)bootsect.extended_section)->root_cluster; // holds the cluster to be searched for directory entries related to the path
    else {
        return -1;
    }

    directory_entry_t file_info; // holds found directory info

    // starting at 3 to skip the "C:\" bit
    unsigned int iterator = 3;
    for (iterator = 3; filePath[iterator - 1] != '\0'; iterator++) {
        if (filePath[iterator] == '\\' || filePath[iterator] == '\0') {
            // clean out fileNamePart before copy
            __memset(fileNamePart, '\0', 256);

            // hacked-together strcpy derivative...
            __memcpy(fileNamePart, filePath + start, iterator - start);

            int retVal = directorySearch(fileNamePart, active_cluster, &file_info, NULL); // go looking for a directory in the specified cluster with the specified name

            if (retVal == -2) // no directory matching found
                return -2;
            else if (retVal == -1) // error occured
            {
                return retVal;
            }

            start = iterator + 1;
            active_cluster = GET_CLUSTER_FROM_ENTRY(file_info); // shift the high bits into their appropriate spots, and OR with low_bits (could also add, I think) in prep for next search
        }
    }

    *fileMeta = file_info; // copy fileinfo over

    if ((file_info.attributes & FILE_DIRECTORY) != FILE_DIRECTORY) // if final directory listing found isn't a directory
    {
        if (readInOffset < 1 || (readInOffset * (unsigned short)bootsect.bytes_per_sector * (unsigned short)bootsect.sectors_per_cluster) + file_info.file_size > 262144) // prevent offsets that extend into FATRead's working range or outside the allocated BIOS int13h space
            return -3;                                                                                                                                                    // you cannot have an offset below 1, nor can you read in more than 256kB

        int cluster = GET_CLUSTER_FROM_ENTRY(file_info); // initialize file read-in with first cluster of file
        unsigned int clusterReadCount = 0;
        while (cluster < END_CLUSTER_32) {
            clusterRead(cluster, clusterReadCount + readInOffset); // Always offset by at least one, so any file operations happening exactly at DISK_READ_LOCATION (e.g. FAT Table lookups) don't overwrite the data (this is essentially backwards compatibility with previously written code)
            clusterReadCount++;
            cluster = FATRead(cluster);
            if (cluster == BAD_CLUSTER_32) {
                return -1;
            } else if (cluster == -1) {
                return -1;
            }
        }

        *fileContents = (char *)(DISK_READ_LOCATION + (unsigned short)bootsect.sectors_per_cluster * (unsigned short)bootsect.bytes_per_sector * readInOffset); // return a pointer in the BIOS read-in space where the file is.

        return 0; // file successfully found
    } else
        return -3; // the path specified was a directory
}

// writes a new file to the file system
// filepath: specifies the path to where the file will be written
// filecontents: contains the char array to the data that will be written
// fileMeta: contains the metadata that will be written. struct should only have a file name, attributes, and size. the rest will be filled in automatically
// returns: -1 is general error, -2 indicates a bad path/file name, -3 indicates file with same name already exists, -4 indicates file size error
int putFile(const char *filePath, char **fileContents, directory_entry_t *fileMeta) {
    if (testIfFATFormat(fileMeta->file_name) != 0) {
        return -2;
    }

    char fileNamePart[256]; // holds the part of the path to be searched

    unsigned short start = 3;    // starting at 3 to skip the "C:\" bit
    unsigned int active_cluster; // holds the cluster to be searched for directory entries related to the path
    if (fat_type == 32)
        active_cluster = ((fat_extBS_32_t *)bootsect.extended_section)->root_cluster;
    else {
        return -1;
    }

    directory_entry_t file_info; // holds found directory info

    // starting at 3 to skip the "C:\" bit
    unsigned int iterator = 3;
    if (strcmp(filePath, "C:\\") == 0) {
        if (fat_type == 32) {
            active_cluster = ((fat_extBS_32_t *)bootsect.extended_section)->root_cluster;
            file_info.attributes = FILE_DIRECTORY | FILE_VOLUME_ID;
            file_info.file_size = 0;
            file_info.high_bits = GET_ENTRY_HIGH_BITS(active_cluster);
            file_info.low_bits = GET_ENTRY_LOW_BITS(active_cluster);
        } else {
            return -1;
        }
    } else {
        for (iterator = 3; filePath[iterator - 1] != '\0'; iterator++) {
            if (filePath[iterator] == '\\' || filePath[iterator] == '\0') {
                // clean out fileNamePart before copy
                __memset(fileNamePart, '\0', 256);

                // hacked-together strcpy derivative...
                __memcpy(fileNamePart, filePath + start, iterator - start);

                int retVal = directorySearch(fileNamePart, active_cluster, &file_info, NULL); // go looking for a directory in the specified cluster with the specified name

                if (retVal == -2) // no directory matching found
                {
                    return -2;
                } else if (retVal == -1) // error occured
                {
                    return retVal;
                }

                start = iterator + 1;
                active_cluster = GET_CLUSTER_FROM_ENTRY(file_info); // prep for next search
            }
        }
    }

    // directory to receive the file is now found, and its cluster is stored in active_cluster. Search the directory to ensure the specified file name is not already in use
    char output[13];
    convertFromFATFormat((char *)fileMeta->file_name, output);
    int retVal = directorySearch(output, active_cluster, NULL, NULL);
    if (retVal == -1) {
        return -1;
    } else if (retVal != -2) {
        return -3;
    }

    if ((file_info.attributes & FILE_DIRECTORY) == FILE_DIRECTORY) // if final directory listing found is a directory
    {
        if (directoryAdd(active_cluster, fileMeta) != 0) {
            return -1;
        }

        // now filling file_info with the information of the file directory entry
        char output[13];
        convertFromFATFormat((char *)fileMeta->file_name, output);
        retVal = directorySearch(output, active_cluster, &file_info, NULL);
        if (retVal == -2) {
            return -2;
        } else if (retVal != 0) {
            return -1;
        }

        active_cluster = GET_CLUSTER_FROM_ENTRY(file_info);

        unsigned int dataLeftToWrite = fileMeta->file_size;

        // start writing information to disk
        while (dataLeftToWrite > 0) {
            unsigned int dataWrite = 0;
            if (dataLeftToWrite >= bootsect.bytes_per_sector * bootsect.sectors_per_cluster)
                dataWrite = bootsect.bytes_per_sector * bootsect.sectors_per_cluster + 1;
            else
                dataWrite = dataLeftToWrite;

            // Always offset by at least one, so any file operations happening exactly at DISK_READ_LOCATION (e.g. FAT Table lookups) don't overwrite the data (this is essentially backwards compatibility with previously written code)
            if (clusterWrite(*fileContents + (fileMeta->file_size - dataLeftToWrite), dataWrite, 1, active_cluster) != 0) {
                return -1;
            }

            dataLeftToWrite -= dataWrite; // subtract the bytes that were just written

            // if there's no data left to write, exit
            if (dataLeftToWrite == 0)
                break;
            else if (dataLeftToWrite < 0) {
                return -1;
            }

            // there's more data to write, so allocate new cluster, change fat of current cluster to point to new cluster, and change active cluster to new cluster

            unsigned int new_cluster = allocateFreeFAT();

            if ((new_cluster == BAD_CLUSTER_32 && fat_type == 32) || (new_cluster == BAD_CLUSTER_16 && fat_type == 16) || (new_cluster == BAD_CLUSTER_12 && fat_type == 12)) // allocation error
            {
                return -1;
            }
            if (FATWrite(active_cluster, new_cluster) != 0) {
                return -1;
            }
            active_cluster = new_cluster;
        }

        return 0; // file successfully written
    } else {
        return -2; // invalid path!
    }
}

// clock hasn't been implemented yet
unsigned short CurrentTime() {
    return 0;
}

// clock nor date has been implemented yet
unsigned short CurrentDate() {
    return 0x11;
}

// clock hasn't been implemented yet
unsigned char CurrentTimeTenths() {
    return 0x0;
}

/*-----------------------------------------------------------------------------
ChkSum()
Returns an unsigned byte checksum computed on an unsigned byte
array. The array must be 11 bytes long and is assumed to contain
a name stored in the format of a MS-DOS directory entry.
Passed: pFcbName Pointer to an unsigned byte array assumed to be
11 bytes long.
Returns: Sum An 8-bit unsigned checksum of the array pointed
to by pFcbName.
------------------------------------------------------------------------------*/
unsigned char ChkSum(unsigned char *pFcbName) {
    short FcbNameLen;
    unsigned char Sum;
    Sum = 0;
    for (FcbNameLen = 11; FcbNameLen != 0; FcbNameLen--) {
        // NOTE: The operation is an unsigned char rotate right
        Sum = ((Sum & 1) ? 0x80 : 0) + (Sum >> 1) + *pFcbName++;
    }
    return (Sum);
}
/* ChkSum function courtesy of http://staff.washington.edu/dittrich/misc/fatgen103.pdf */

// Notes: Does not consider making short forms of long names, nor support multiple periods in a name.

char uppercase(char c) {

    if ((c >= 'a') && (c <= 'z'))
        c -= 'a' - 'A';

    return c;
}

char *uppercase_str(char *input) {
    unsigned int counter = 0;
    unsigned int inputLength = strlen(input);

    while (counter < inputLength) // iterate through input, converting the characters to uppercase
    {
        if ((short)input[counter] >= 97 && (short)input[counter] <= 122)
            input[counter] -= 32;
        counter++;
    }
    return input;
}

char *convertToFATFormat(char *input) {
    unsigned int counter = 0;

    uppercase_str(input);

    char searchName[13] = {'\0'};
    unsigned short dotPos = 0;

    counter = 0;
    while (counter <= 8) // copy all the characters from filepart into searchname until a dot or null character is encountered
    {
        if (input[counter] == '.' || input[counter] == '\0') {
            dotPos = counter;
            counter++; // iterate off dot
            break;
        } else {
            searchName[counter] = input[counter];
            counter++;
        }
    }

    if (counter > 9) // a sanity check in case there was a dot-less 11 character filename
    {
        counter = 8;
        dotPos = 8;
    }

    unsigned short extCount = 8;
    while (extCount < 11) // add the extension to the end, putting spaces where necessary
    {
        if (input[counter] != '\0')
            searchName[extCount] = input[counter];
        else
            searchName[extCount] = ' ';

        counter++;
        extCount++;
    }

    counter = dotPos; // reset counter to position of the dot

    while (counter < 8) // if the dot is within the 8 character limit of the name, iterate through searchName putting in spaces up until the extension
    {
        searchName[counter] = ' ';
        counter++;
    }

    strcpy(input, searchName); // copy results back to input

    return input;
}

// Return Codes:
//  -1: bad characters
//  -2: Lowercase
BOOL testIfFATFormat(char *input) {
    short retVal = 0;

    // Invalid Values:
    /*Values less than 0x20 except for the special case of 0x05 in DIR_Name[0] described above.
            � 0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D,
            and 0x7C.*/
    unsigned short iterator;
    for (iterator = 0; iterator < 11; iterator++) {
        if (input[iterator] < 0x20 && input[iterator] != 0x05) {
            retVal = retVal | BAD_CHARACTER;
        }

        switch (input[iterator]) {
        case 0x2E: {
            if ((retVal & NOT_CONVERTED_YET) == NOT_CONVERTED_YET) // a previous dot has already triggered this case
                retVal |= TOO_MANY_DOTS;

            retVal ^= NOT_CONVERTED_YET; // remove NOT_CONVERTED_YET flag if already set

            break;
        }
        case 0x22:
        case 0x2A:
        case 0x2B:
        case 0x2C:
        case 0x2F:
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x7C:
            retVal = retVal | BAD_CHARACTER;
        }

        if (input[iterator] >= 'a' && input[iterator] <= 'z') {
            retVal = retVal | LOWERCASE_ISSUE;
        }
    }

    // if (input[11] != '\0')
    // retVal = retVal | BAD_TERMINATION;

    return retVal;
}

// Converts the file name stored in a FAT directory entry into a more human-sensible format
void convertFromFATFormat(char *input, char *output) {

    // If the entry passed in is one of the dot special entries, just return them unchanged.
    if (input[0] == '.') {
        if (input[1] == '.') {
            strcpy(output, "..");
            return;
        }

        strcpy(output, ".");
        return;
    }

    unsigned short counter = 0;

    // iterate through the 8 letter file name, adding a dot when the end is reached
    for (counter = 0; counter < 8; counter++) {
        if (input[counter] == 0x20) {
            output[counter] = '.';
            break;
        }

        output[counter] = input[counter];
    }

    // if the entire 8 letters of the file name were used, tack a dot onto the end
    if (counter == 8) {
        output[counter] = '.';
    }

    unsigned short counter2 = 8;

    // iterate through the three-letter extension, adding it on. (Note: if the input is a directory (which has no extension) it erases the dot put in previously)
    for (counter2 = 8; counter2 < 11; counter2++) {
        ++counter;
        if (input[counter2] == 0x20 || input[counter2] == 0x20) {
            if (counter2 == 8) // there is no extension, the dot added earlier must be removed
                counter -= 2;  // it's minus two because the for loop above iterates the loop as well
            break;
        }
        output[counter] = input[counter2];
    }

    ++counter;
    while (counter < 12) {
        output[counter] = ' ';
        ++counter;
    }

    output[12] = '\0'; // ensures proper termination regardless of program operation previously
    return;
}