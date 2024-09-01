#ifndef HEADER_FLASH_H_
#define HEADER_FLASH_H_

#include  <msp430g2553.h>          // MSP430x2xx

extern void write_Seg(void);

#define MAX_FILES 3
#define MAX_FILENAME_LENGTH 11

typedef struct Files{
    short num_of_files;
    char file_name[MAX_FILES][MAX_FILENAME_LENGTH];  // Array of filenames
    int* file_start_pointers[MAX_FILES];
    int file_sizes[MAX_FILES];
} FileSystem;

extern FileSystem file;

#endif 
