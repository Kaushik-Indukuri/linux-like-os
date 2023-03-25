#include "file_system.h"

file_descriptor_t pcb[8];
int curr_pcb_index = 2;

void filesystem_init(uint32_t * ptr) {
    boot_block_ptr = (boot_block_t *)ptr;
}

int32_t directory_open(const uint8_t* filename) {
    return  0;
}
int32_t directory_close(int32_t fd) {
    return 0;
}
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes) {
    memcpy(buf, boot_block_ptr->direntries[pcb[fd].file_position++].filename, 32);
    return 0;
}
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

int32_t file_open(const uint8_t* filename) {
    if (curr_pcb_index == 7) {
        return -1;
    }
    if (filename == NULL) {
        return -1;
    }
    dentry_t dentry;
    if (read_dentry_by_name(filename, &dentry) != 0) {
        return -1;
    }
    file_descriptor_t fd;
    fd.inode = dentry.inode_num;
    fd.file_position = 0;
    pcb[curr_pcb_index] = fd;
    curr_pcb_index++;
    return curr_pcb_index - 1;
}
int32_t file_close(int32_t fd) {
    if (curr_pcb_index == 2) {
        return -1;
    }
    curr_pcb_index--;
    return 0;
}
int32_t file_read(int32_t fd, void* buf, int32_t nbytes) {
    if (buf == NULL) {
        return -1;
    }
    int inode = pcb[fd].inode;
    int ret = read_data(inode, 0, buf, nbytes);
    pcb[fd].file_position += ret;
    return ret;
}
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
    if (strlen((char*)fname) > 32) {
        return -1;
    }
    if (dentry == NULL) {
        return -1;
    }
    dentry_t * dentry_ptr = boot_block_ptr->direntries;
    int i;
    for (i = 0; i < 63; i++) {
        dentry_t copy_dentry = *(dentry_ptr + i);
        if (strncmp((int8_t*)fname, (int8_t*)(copy_dentry.filename), 32) == 0) {
            strncpy(dentry->filename, copy_dentry.filename, 32);
            dentry->filetype = copy_dentry.filetype;
            dentry->inode_num = copy_dentry.inode_num;
            return 0;
        }
    }
    return -1;
}
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
    if (index >= 63) {
        return -1;
    }
    if (dentry == NULL) {
        return -1;
    }
    dentry_t * dentry_ptr = boot_block_ptr->direntries;
    dentry_t copy_dentry = *(dentry_ptr + index);
    dentry->filetype = copy_dentry.filetype;
    dentry->inode_num = copy_dentry.inode_num;
    return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    inode_t * inode_ptr = (inode_t *)(boot_block_ptr + 1 + inode);
    uint8_t * data_ptr = (uint8_t *)(boot_block_ptr + 1 + boot_block_ptr->inode_count);
    if (offset >= 4096) {
        return -1;
    }
    int i;
    int ret = 0; 
    for (i = offset; i < length + offset; i++, buf++, ret++) {
        if (i >= 4096) {
            return ret;
        }
        int t = inode_ptr->data_block_num[i / 4096];
        memcpy(buf, (uint8_t*)(data_ptr + 4096 * t + i % 4096), 1);
    }
    return ret;
}
