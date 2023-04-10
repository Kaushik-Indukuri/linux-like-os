//syscall.h
#ifndef SYSCALL_H
#define SYSCALL_H

void syscall_init();

/*Function for executing user programs with system calls*/
int32_t halt (const uint8_t* command);
int32_t execute (const uint8_t* command);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);

typedef struct file_operations {
    int32_t (*open) (const uint8_t* filename);
    int32_t (*close) (int32_t fd);
    int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
} file_operations_t;

typedef struct file_descriptor {
    file_operations_t * file_operations_ptr;
    uint32_t inode;
    uint32_t file_position;
    uint32_t flags;
} file_descriptor_t;

typedef struct pcb{
    uint32_t pid;
    uint32_t parent_pid;  
    //uint8_t active;
    uint32_t prev_ebp;
    uint32_t prev_esp;
    uint32_t prev_eip;
    uint32_t cur_ebp;
    file_descriptor_t file_array[8];
} pcb_t;



extern void flushtlb();

#endif
