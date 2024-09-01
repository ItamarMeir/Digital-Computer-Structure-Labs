#include  "../header/flash.h"    // private library - flash layer
#include  "../header/halGPIO.h"    // private library - halGPIO layer
#include  "string.h"

//-----------------------------------------------------------------------------
//           FLASH driver
//-----------------------------------------------------------------------------

FileSystem file;


void ScriptData(void)
{
    file.file_sizes[file.num_of_files - 1] = strlen(file_content) - 1;

}

// void write_Seg(void)
// {
//     char *Flash_ptr_write ;                   // Flash pointer
//     unsigned int k;
//     Flash_ptr_write = file.file_start_pointers[file.num_of_files - 1];      // Initialize Flash pointer
//     FCTL1 = FWKEY + ERASE;                    // Set Erase bit
//     FCTL3 = FWKEY;                            // Clear Lock bit
//    *Flash_ptr_write = 0;                      // Dummy write to erase Flash segment
//     FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
//     for (k = 0; k < file.file_size[file.num_of_files - 1]; k++){
//         if (file_content[k] == 0x0A || file_content[k] == 0x0D ){
//             continue;
//         }
//         *Flash_ptr_write++ = file_content[k];    // Write value to flash
//     }
//     memset(RX_str,0,strlen(RX_str)); //clear file_content
//     FCTL1 = FWKEY;                               // Clear WRT bit
//     FCTL3 = FWKEY + LOCK;                        // Set LOCK bit
// }
void write_Seg(void) {
    char *Flash_ptr_write;  // Flash pointer
    unsigned int k;

    // Initialize Flash pointer with the starting address for the current file
    Flash_ptr_write = (char*)file.file_start_pointers[file.num_of_files - 1];

    // Erase the flash segment
    FCTL1 = FWKEY + ERASE;  // Set Erase bit
    FCTL3 = FWKEY;          // Clear Lock bit
    *Flash_ptr_write = 0;   // Dummy write to erase Flash segment

    // Write data to flash memory
    FCTL1 = FWKEY + WRT;  // Set WRT bit for write operation
    for (k = 0; k < file.file_sizes[file.num_of_files - 1]; k++) {
        // Skip newline and carriage return characters
        if (RX_str[k] == 0x0A || RX_str[k] == 0x0D) {
            continue;
        }
        *Flash_ptr_write++ = RX_str[k];  // Write each byte to flash
    }

    // Clear the write bit and lock the flash memory
    FCTL1 = FWKEY;         // Clear WRT bit
    FCTL3 = FWKEY + LOCK;  // Set LOCK bit
}
