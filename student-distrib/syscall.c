#include "rtc.h"
#include "lib.h"
#include "i8259.h"
// #include "lib.h"
#include "idt.h"
#include "paging.h"
#include "syscall.h"
#include "terminal.h"
#include "file_system.h"

#define MB_8 (1<<23) // 8 MB
#define KB_8 (1<<13) // 8 KB
#define MB_4 (1<<22) // 4 MB

#define user_program_start  0x48000
#define user_page_start     0x08000000

#define USRSTARTADDR 0x08000000
#define USRENDADDR   0x80480000
#define tableI 0xb8 // location of only table initalized

pcb_t* pcb_ptr;
pcb_t pcb_array[6];
int pid;
file_operations_t stdin;
file_operations_t stdout;
file_operations_t rtc;
file_operations_t file;
file_operations_t directory;

char arg[32];

/* Segment selector values */
#define KERNEL_CS   0x0010
#define KERNEL_DS   0x0018
#define USER_CS     0x0023
#define USER_DS     0x002B
#define KERNEL_TSS  0x0030
#define KERNEL_LDT  0x0038


void flushtlb() {
    asm volatile(" \n\
        movl %%cr3, %%eax \n\
        movl %%eax, %%cr3 \n\
        "
        :
        :
        : "memory"
    );
}

void syscall_init() {
    rtc.open = rtc_open;
    rtc.close = rtc_close;
    rtc.read = rtc_read;
    rtc.write = rtc_write;
    file.open = file_open;
    file.close = file_close;
    file.read = file_read;
    file.write = file_write;
    directory.open = directory_open;
    directory.close = directory_close;
    directory.read = directory_read;
    directory.write = directory_write;
    pid = -1;
}


/*
 * halt
 *   DESCRIPTION: Halts current terminal, opens new one if pid is 0 or -1
 *   INPUTS: const uint8_t command: status/command to be used for execute
 *   OUTPUTS: none
 *   RETURN VALUE: -1, but should never get there as in line asm should 
 *               return to other function. eax has commmand 32 b extended
 *   SIDE EFFECTS: ebp esp and eax restored. 
 */
int32_t halt (const uint8_t command)
{
/*    uint32_t ret;
    ret = (uint32_t)command;
    int i;
    for (i = 0; i < 8; i++) {
        pcb_ptr->file_array[i].flags = 0;
    }
    if(pid <= 0)
    {
        pid = -1;
        //pcb_ptr = pcb_array + pid;
        execute("shell");
    }
    pid--;
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (MB_8-(KB_8*(pid)) - 4);
    pcb_ptr = pcb_array + pid + 1;
    uint32_t ebp = pcb_ptr->prev_ebp;
    uint32_t esp = pcb_ptr->prev_esp;
    page_directory[32].addrlong = (MB_8 + MB_4*(pid)) / KB4;
    flushtlb();
    asm volatile(" \n\
        movl %%eax, %%ebp\n\
        movl %%ebx, %%esp\n\
        movl %%ecx, %%eax\n\
        leave \n\
        ret \n\
        "
        :
        :"a"(ebp),"b"(esp),"c"(ret)
        : "memory"
    );
    return -1;
*/
    pcb_ptr = pcb_array + pid;
    uint32_t ret;
    ret = (uint32_t)command;
    int i;
    for (i = 0; i < 8; i++) {
        pcb_ptr->file_array[i].flags = 0;
    }
    if(pid <= 0)
    {
        pid = -1;
        //pcb_ptr = pcb_array + pid;
        // printf("Halt called from base shell.\n");
        execute((uint8_t*) "shell");
        return command;
    }
    uint32_t ebp = pcb_ptr->prev_ebp;
    uint32_t esp = pcb_ptr->prev_esp;
    pid--;
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (MB_8-(KB_8*(pid)) - 4); // skip first location
    pcb_ptr = pcb_array + pid;

    page_directory[32].addrlong = (MB_8 + MB_4*(pid)) / KB4;
    flushtlb();
    asm volatile(" \n\
        movl %%eax, %%ebp\n\
        movl %%ebx, %%esp\n\
        movl %%ecx, %%eax\n\
        leave\n\
        ret \n\
        "
        :
        :"a"(ebp),"b"(esp),"c"(ret)
        : "memory"
    );
    return -1;
}



/*
 * execute
 *   DESCRIPTION: Gets command input, Runs specified executable file
 *   INPUTS: const uint8_t command: status/command to be used for running
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if any errors. If completed, return 0 but should never
 *                 get there as in line asm should return to other function.
 *   SIDE EFFECTS: ebp esp and eax restored, to go to shell. 
 */

int32_t execute (const uint8_t* command)
{
    /*Parse Arguements*/
    int space = 0;
    int i = 0;
    int cmdLen = 0;
    int argStart=0;
    while(command[space]== ' ')
    {
        space++;
    }
    while(command[i + space]!=0x00 )
    {
        if(command[i + space]== ' ')
        {
            argStart=i+1;
            break;
        }
        cmdLen++;
        i++;
    }
    char file_exec[cmdLen+1];
    for(i = space; i < cmdLen + space;i++)
    { 
        file_exec[i-space] = command[i];
    }
    if(argStart>0)
    {
        for(i = space + cmdLen+1; command[i] != '\0';i++)
        {
            if(command[i]!=' ')
            {
                arg[argStart-cmdLen-1]=command[i];
                argStart++;
            }
            else{
                break;
            }
        }
    }

    file_exec[cmdLen] = '\0';
 

    /*Check executable*/
    dentry_t dentry;
    if(read_dentry_by_name((uint8_t*)file_exec, &dentry)==-1)
    {
        return -1;
    }
    

    char buf[5];
    if(read_data(dentry.inode_num, 0,(uint8_t*)buf, 4)==-1)
    {
        return -1;
    }
    buf[4] = '\0'; // 4 is for last char
    char elf[4] = {0x7F, 'E', 'L', 'F'}; //ELF FORMAT MAGIC NUMBERS 
    if (strncmp(buf, elf, 4) != 0) // 4 is size of ELF
    {
        return -1;
    }

    /*Setup program paging*/
    pid++;
    if (pid > 5) {
        pid--;
        printf("Max Processes Reached\n");
        return 0;
    }
    page_directory[32].addrlong = (MB_8 + MB_4*pid) / KB4;

    flushtlb();

    /*Move exectubale data into virtual address space*/
    pcb_ptr = pcb_array + pid;
    uint8_t * program_ptr = (uint8_t *)(user_page_start + user_program_start);
    if (read_data(dentry.inode_num, 0, program_ptr, ((inode_t *)(boot_block_ptr) + 1 + dentry.inode_num)->length) == -1) {
        pid--;
        return -1;
    }

    pcb_ptr->pid = pid;
    pcb_ptr->parent_pid = pid-1;
    for (i = 2; i < 8; i++) { // 8 locations in file array
        pcb_ptr->file_array[i].inode = 0;
        pcb_ptr->file_array[i].file_position = 0;
        pcb_ptr->file_array[i].file_operations_ptr = NULL;
        pcb_ptr->file_array[i].flags = 0;
    }

    stdin.open = terminal_open;
    stdin.close = terminal_close;
    stdin.read = terminal_read;
    stdin.write = null_write;

    pcb_ptr->file_array[0].inode = 0;
    pcb_ptr->file_array[0].file_position = 0;
    pcb_ptr->file_array[0].file_operations_ptr = &stdin;
    pcb_ptr->file_array[0].flags = 1;

    stdout.open = terminal_open;
    stdout.close = terminal_close;
    stdout.read = null_read;
    stdout.write = terminal_write;

    pcb_ptr->file_array[1].inode = 0;
    pcb_ptr->file_array[1].file_position = 0;
    pcb_ptr->file_array[1].file_operations_ptr = &stdout;
    pcb_ptr->file_array[1].flags = 1;

    uint8_t entry_pt[4]; // 4 is size of 4 bytes
    if (read_data(dentry.inode_num, 24, (uint8_t *)entry_pt, 4) == -1) {
        pid--;
        return -1;
    }

    // 8,16,24 is for shifting multiple bytes
    uint32_t new_eip = entry_pt[0] + (entry_pt[1] << 8) + (entry_pt[2] << 16) + (entry_pt[3] << 24);
    uint32_t new_esp = (MB_4 * 33) - 4;
    register uint32_t ebp asm("ebp");
    register uint32_t esp asm("esp");
    
    pcb_ptr->prev_ebp = ebp;
    pcb_ptr->prev_esp = esp;
    //pcb_ptr->prev_eip = new_eip;

    tss.ss0 = KERNEL_DS;
    // 4 is for skipping first location
    tss.esp0 = (MB_8-(KB_8*(pid)) - 4);

    sti();


     asm volatile(" \n\
        pushl %%eax \n\
        pushl %%ebx \n\
        pushfl \n\
        pushl %%ecx \n\
        pushl %%edx  \n\
        iret    \n\
        "
        :
        :"a"(USER_DS),"b"(new_esp),"c"(USER_CS),"d"(new_eip)
        : "memory"
    );
    return 0;
}


/*
 * open
 *   DESCRIPTION: Find file in FS.  assign an unused file descriptor. swt up fds
 *   INPUTS: const uint8_t filename: name of file needed for execute/other functions
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if any errors. return fd assigned if success
 *   SIDE EFFECTS: none
 */
int32_t open(const uint8_t* filename)
{
    int fd = 0;
    int i;
    // 8 is for file array length
    for (i = 0; i < 8; i++) {
        if (pcb_ptr->file_array[i].flags == 0) {
            fd = i;
            break;
        }
        // 7 is is last index
        if (i == 7) {
            return -1;
        }
    }
    dentry_t dentry;
    if(read_dentry_by_name (filename, &dentry)==-1)
    {
        return -1;
    }
    if (dentry.filetype == 0) {
        pcb_ptr->file_array[fd].file_operations_ptr = &rtc;
        pcb_ptr->file_array[fd].inode = dentry.inode_num;
        pcb_ptr->file_array[fd].file_position = 0;
        pcb_ptr->file_array[fd].flags = 1;
    }
    else if (dentry.filetype == 1) {
        pcb_ptr->file_array[fd].file_operations_ptr = &directory;
        pcb_ptr->file_array[fd].inode = dentry.inode_num;
        pcb_ptr->file_array[fd].file_position = 0;
        pcb_ptr->file_array[fd].flags = 1;
    }
    else if (dentry.filetype == 2) {
        pcb_ptr->file_array[fd].file_operations_ptr = &file;
        pcb_ptr->file_array[fd].inode = dentry.inode_num;
        pcb_ptr->file_array[fd].file_position = 0;
        pcb_ptr->file_array[fd].flags = 1;
    }
    else {
        return -1;
    }
    return fd;
}


/*
 * close
 *   DESCRIPTION: Close the file descriptor passed in
 *   INPUTS: int32_t fd. fd from open used to close file
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if any errors. return close success/fail 
 *   SIDE EFFECTS: none
 */
int32_t close(int32_t fd)
{
    // 7 is last one and 2 is after stdin and stdout
    if(fd > 7 || fd < 2)
    {
        return -1;
    }
    if(pcb_ptr->file_array[fd].flags == 0)
    {
        return -1;
    }
    pcb_ptr->file_array[fd].inode = 0;
    pcb_ptr->file_array[fd].file_position = 0; // Definetly wrong lol rahul is supposed to be here LOL
    pcb_ptr->file_array[fd].flags = 0;

    return pcb_ptr->file_array[fd].file_operations_ptr->close(fd);     
}

/*
 * read
 *   DESCRIPTION: Use file operations jump table to call the corresponding read fxn
 *   INPUTS: file desc, index to file array, void* buf: copy from  file name into buffer into, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if any errors. return bytes read if success
 *   SIDE EFFECTS: none
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
    // Error Checking:
    // 7 is for last index
    if(fd>7 || fd<0 || nbytes<0 || buf == NULL)
    {
        return -1;
    }
    if(pcb_ptr->file_array[fd].flags ==0)
    {
        return -1;
    }
    return pcb_ptr->file_array[fd].file_operations_ptr->read(fd,buf,nbytes);     

}

/*
 * write
 *   DESCRIPTION: Use file operations jump table to call the corresponding write fxn
 *   INPUTS: file desc, index to file array, void* buf: copy from  file name into buffer into, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if any errors. return bytes written if success
 *   SIDE EFFECTS: none
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
    // 7 is for last index
    if(fd>7 || fd<0 || nbytes<0 || buf==NULL)  //62 is max num files
    {
        return -1;
    }
    if( pcb_ptr->file_array[fd].flags ==0)
    {
        return -1;
    }
    return pcb_ptr->file_array[fd].file_operations_ptr->write(fd,buf,nbytes);
}

/*
 * read
 *   DESCRIPTION: Use file operations jump table to call the corresponding read fxn
 *   INPUTS: file desc, index to file array, void* buf: copy from  file name into buffer into, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if called
 *   SIDE EFFECTS: none
 */
int32_t null_read(int32_t fd, void* buf, int32_t nbytes)
{
    // Error Checking:
    return -1;   

}

/*
 * write
 *   DESCRIPTION: Use file operations jump table to call the corresponding write fxn
 *   INPUTS: file desc, index to file array, void* buf: copy from  file name into buffer into, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if called
 *   SIDE EFFECTS: none
 */
int32_t null_write(int32_t fd, const void* buf, int32_t nbytes)
{
    return -1;
}

/*
 * getargs
 *   DESCRIPTION: Arguments are parsed from the command buffer in execute
 *   INPUTS: void* buf: copy from  file name into buffer into, int32_t nbytes  num bytes modified
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if fail, 0 if success
 *   SIDE EFFECTS: none
 */
int32_t getargs (uint8_t* buf, int32_t nbytes)
{
    if(buf==NULL || nbytes<0)
    {
        return -1;
    }
    pcb_t* temp;
    temp= (pcb_t*)(MB_8-(KB_8*(pid)) - 4); //This addr is same we used to get execute addr, but no PID so?
    if(temp==NULL)
    {
        return -1;
    }
    strcpy((char*)buf, arg);
    return 0;
}

/*
 * vidmap
 *   DESCRIPTION: ?????????
 *   INPUTS: uint8_t** screen_start: double pointer to mem accessed
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if fail, 0 if success
 *   SIDE EFFECTS: none
 */
int32_t vidmap (uint8_t** screen_start)
{
    if(screen_start==NULL || (uint32_t)screen_start < USRSTARTADDR || (uint32_t)screen_start> USRENDADDR)
    {
        return -1;
    }

    page_directory[34].ps=0;
    page_directory[34].p=1;
    page_directory[34].us=1;        
    page_directory[34].rw=1;        
    page_directory[34].addrlong= (int)(video_mapping)/KB4;

    video_mapping[0].p=1;
    video_mapping[0].us=1;    
    video_mapping[0].addr=tableI;
    video_mapping[0].rw=1;    

    flushtlb();
    // How / Why modify screen start
    // uint32_t* ofset=((MB_8 + KB4*2)); // 2nd KB assigned, this is 2nd
    // *screen_start =(uint32_t*) ((MB_8 + KB4*2)); //How do I calculate offset for mem that screen start has
    *screen_start =(uint32_t*) (0x8800000); //How do I calculate offset for mem that screen start has
    return 0;
}

