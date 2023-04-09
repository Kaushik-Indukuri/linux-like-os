#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "lib.h"
#include "idt.h"
#include "file_system.h"
#include "paging.h"

#define MB_8 (1<<23) // 8 MB
#define KB_8 (1<<12) // 8 KB

#define user_program_start  0x48000
#define user_page_start     0x08000000


/* Segment selector values */
#define KERNEL_CS   0x0010
#define KERNEL_DS   0x0018
#define USER_CS     0x0023
#define USER_DS     0x002B
#define KERNEL_TSS  0x0030
#define KERNEL_LDT  0x0038


void syscall_init() {
    
}

int32_t execute (const uint8_t* command)
{
    /*Parse Arguements*/
    int i = 0;
    int cmdLen = 0;
    while(command[i]!=' ')
    {
        cmdLen++;
        i++;
    }
    char file_exec[cmdLen];
    for(i = 0;i<cmdLen;i++)
    {
        file_exec[i] = command[i];
    }

    /*Check executable*/
    dentry_t dentry;
    if(read_dentry_by_name (file_exec, &dentry)==-1)
    {
        return -1;
    }
    char buf[4];
    if(read_data(dentry.inode_num, 0, &buf, 4)==-1)
    {
        return -1;
    }
    char elf[4] = {0x7F, 'E', 'L', 'F'}; //ELF FORMAT MAGIC NUMBERS 
    if (!strncmp(buf, elf, 4))
    {
        return -1;
    }

    char entry_pt[4];
    if (read_data(dentry.inode_num,24, &entry_pt, 4) == -1) {
        return -1;
    }

    /*Setup program paging*/
    pcb_ptr = MB_8 - KB_8;
    uint8_t * program_ptr = user_page_start + user_program_start;
    if (read_data(dentry.inode_num, 0, program_ptr, (inode_t *)(boot_block_ptr + 1 + dentry.inode_num).length * KB4) == -1) {
        return -1;
    }
    flushtlb();
    pcb_ptr->pid = 0;
    pcb_ptr->parent_pid = 0;
    for (int i = 2; i < 8; i++) {
        pcb->file_array[i].inode = 0;
        pcb->file_array[i].file_position = 0;
        pcb->file_array[i].file_operations_ptr = NULL;
        pcb->file_array[i].flags = 0;
    }
    

    stdin.open = terminal_open;
    stdin.close = terminal_close;
    stdin.read = terminal_read;
    stdin.write = terminal_write;

    pcb->file_array[0].inode = 0;
    pcb->file_array[0].file_position = 0;
    pcb->file_array[0].file_operations_ptr = &stdin;
    pcb->file_array[0].flags = 1;

    stdout.open = terminal_open;
    stdout.close = terminal_close;
    stdout.read = terminal_read;
    stdout.write = terminal_write;

    pcb->file_array[1].inode = 0;
    pcb->file_array[1].file_position = 0;
    pcb->file_array[1].file_operations_ptr = &stdout;
    pcb->file_array[1].flags = 1;

    pcb->program_eip = entry_pt[0] | entry_pt[1] << 8 | entry_pt[2] << 16 | entry_pt[3] << 24;
    pcb->program_esp = KB_8;

    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");

    asm volatile("\
        xorl %eax, %eax






        iret    ;\
        "
        :
        :"a"(),"b"()
        :
    )



    return 0;
}

int32_t open(const uint8_t* filename)
{
    return 0;
}
int32_t close(int32_t fd)
{
    return 0;
}
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
    return 0;
}
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
    return 0;
}



// int32_t video_mapping(uint32_t ** pageDirAddr)
// {
//     if(pageDirAddr==NULL || uint32_t(pageDirAddr) < user_st_addr || uint32_t(pageDirAddr) > user_end_addr)
//     {
//         return -1;
//     }

//     return 0;
// }