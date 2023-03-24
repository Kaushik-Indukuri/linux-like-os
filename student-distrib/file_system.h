#include "kernel.c"
#include "lib.h"

#define FILENAME_LEN 32

typedef struct dentry {
    int8_t filename[FILENAME_LEN];
    int32_t filetype;
    int32_t inode_num;
    int8_t reserve[24];
} dentry_t;

typedef struct boot_block {
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t reserved[52];
    dentry_t direntries[63];
} boot_block_t;

typedef struct inode {
    int32_t length;
    int32_t data_block_num[1023];
} inode_t;

typedef struct file_descriptor {
    int32_t file_operations_table_ptr;
    int32_t inode;
    int32_t file_position;
    int32_t flags;
} file_descriptor_t;


extern uint32_t * block_ptr;

int32_t directory_open();
int32_t directory_close();
int32_t directory_read();
int32_t directory_write();

int32_t file_open();
int32_t file_close();
int32_t file_read();
int32_t file_write();

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
