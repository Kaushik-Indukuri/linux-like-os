#include "file_system.h"

file_descriptor_t pcb[8];
int curr_pcb_index = MINARRLEN;

/*
 * filesystem_init
 *   DESCRIPTION: Takes in a ptr to the star of the file space and converts to a block ptr
 *   INPUTS: ptr - ptr to file space
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Stores a local version of the first block ptr
 */
void filesystem_init(uint32_t * ptr) {
    boot_block_ptr = (boot_block_t *)ptr;
}

/*
 * directory_open
 *   DESCRIPTION: does nothing 
 *   INPUTS: Does not use input
 *   OUTPUTS: 
 *   RETURN VALUE: alwares ret o
 *   SIDE EFFECTS: 
 */
int32_t directory_open(const uint8_t* filename) {
    return  0;
}

/*
 * directory_close
 *   DESCRIPTION: does nothing 
 *   INPUTS: Does not use input
 *   OUTPUTS: 
 *   RETURN VALUE: alwares ret o
 *   SIDE EFFECTS: 
 */
int32_t directory_close(int32_t fd) {
    return 0;
}

/*
 * directory_read
 *   DESCRIPTION: read file name at fd and incr file pos
 *   INPUTS: int32_t fd: file desc, index to file array, void* buf: copy from  file name into buffer into, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0 always 
 *   SIDE EFFECTS: 
 */
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes) {
    pcb[fd].file_position++;
    int len;
    if (strlen((uint8_t *)boot_block_ptr->direntries[fd].filename) >= FILENAME_LEN) {
        len = FILENAME_LEN;
    }
    else {
        len = strlen((uint8_t *)boot_block_ptr->direntries[fd].filename);
    }
    memcpy(buf, boot_block_ptr->direntries[fd].filename, len);
    return 0;
}

/*
 * directory_write
 *   DESCRIPTION: does nothing
 *   INPUTS: int32_t fd: file desc, index to file array, void* buf: copy from  file name into buffer into, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: -1 always 
 *   SIDE EFFECTS: 
 */

int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

/*
 * file_open
 *   DESCRIPTION: Adds file descriptior to file array for that file, thats it
 *   INPUTS: const uint8_t* filename: name of file
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if fail, file desc
 *   SIDE EFFECTS: 
 */
int32_t file_open(const uint8_t* filename) {
    if (curr_pcb_index == MAXARRLEN) {
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

/*
 * file_close
 *   DESCRIPTION: Remove file desc from file array
 *   INPUTS: int32_t fd: file decirptior
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if fail, 0 if succ
 *   SIDE EFFECTS: 
 */

int32_t file_close(int32_t fd) {
    if (curr_pcb_index == MINARRLEN) {
        return -1;
    }
    curr_pcb_index--;
    return 0;
}

/*
 * file_read
 *   DESCRIPTION: pareses through file
 *   INPUTS: int32_t fd: file desc, index to file array, void* buf: copy from  file name into buffer into, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if fail, ret is bytes read
 *   SIDE EFFECTS: 
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes) {
    if (buf == NULL) {
        return -1;
    }
    int inode = pcb[fd].inode;
    int ret = read_data(inode, 0, buf, nbytes);
    pcb[fd].file_position += ret;
    return ret;
}

/*
 * file_write
 *   DESCRIPTION: nothing
 *   INPUTS: int32_t fd: file desc, index to file array, void* buf: copy from  file name into buffer into, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: -1 always 
 *   SIDE EFFECTS: 
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

/*
 * read_dentry_by_name
 *   DESCRIPTION: Read dentry by file name
 *   INPUTS: const uint8_t* fname: file name, dentry_t* dentry: dentry
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if fail, 0 if scces
 *   SIDE EFFECTS: 
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
    if (strlen((char*)fname) > FILENAME_LEN) {
        return -1;
    }
    if (dentry == NULL) {
        return -1;
    }
    dentry_t * dentry_ptr = boot_block_ptr->direntries;
    int i;
    for (i = 0; i < NUMFILES; i++) {
        dentry_t copy_dentry = *(dentry_ptr + i);
        if (strncmp((int8_t*)fname, (int8_t*)(copy_dentry.filename), FILENAME_LEN) == 0) {
            strncpy(dentry->filename, copy_dentry.filename, FILENAME_LEN);
            dentry->filetype = copy_dentry.filetype;
            dentry->inode_num = copy_dentry.inode_num;
            return 0;
        }
    }
    return -1;
}

/*
 * read_dentry_by_index
 *   DESCRIPTION: Read dentry by file name
 *   INPUTS: uint32_t index: idx in the dentry array to copy dentry from, dentry_t* dentry: sdentry struct
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if fail, 0 if scces
 *   SIDE EFFECTS: 
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
    if (index >= NUMFILES) {
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

/*
 * init_idt
 *   DESCRIPTION: Reads data from data blocks associated from inode and copies to buf with length bythes
 *   INPUTS: uint32_t inode: inode to copy data from, uint32_t offset: offset to start copying bytes from, uint8_t* buf: buf to copy to, uint32_t length: num of bytes to copy
 *   OUTPUTS: none
 *   RETURN VALUE: num of bytes read
 *   SIDE EFFECTS: 
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    inode_t * inode_ptr = (inode_t *)(boot_block_ptr + 1 + inode);
    uint8_t * data_ptr = (uint8_t *)(boot_block_ptr + 1 + boot_block_ptr->inode_count);
    int len = inode_ptr->length;
    if (offset >= len) {
        return 0;
    }
    int i;
    int ret = 0;
    for (i = offset; i < length + offset; i++, buf++, ret++) {
        if (i >= len) {
            return ret;
        }
        int t = inode_ptr->data_block_num[i / DATABLK_LEN];
        memcpy(buf, (uint8_t*)(data_ptr + DATABLK_LEN * t + i % DATABLK_LEN), 1);
    }
    return ret;
}
