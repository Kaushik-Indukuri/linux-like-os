#include "lib.h"
#define page_enteries 1024
#define KB4 4096 //4kb in bits

typedef struct ge_directory_entry {
    union {
        struct { // 4mb struct from desceiptors.dvi
            uint32_t p : 1;
            uint32_t rw : 1;
            uint32_t us : 1;
            uint32_t pwt : 1;
            uint32_t pcd : 1;
            uint32_t a : 1;
            uint32_t d : 1;
            uint32_t ps : 1;
            uint32_t g : 1;
            uint32_t avl : 3;
            uint32_t pat : 1;
            uint32_t res : 9;
            uint32_t addrshort : 10;
        } __attribute__ ((packed));
        struct {    // 4kb  struct from desceiptors.dvi
            uint32_t p : 1;
            uint32_t rw : 1;
            uint32_t us : 1;
            uint32_t pwt : 1;
            uint32_t pcd : 1;
            uint32_t a : 1;
            uint32_t d : 1;
            uint32_t ps : 1;
            uint32_t g : 1;
            uint32_t avl : 3;
            uint32_t addrlong : 20;
        } __attribute__ ((packed));
    };
} page_directory_entry_t;

//struct from desceiptors.dvi
typedef struct __attribute__ ((packed)) page_table_entry{
    uint32_t p : 1;
    uint32_t rw : 1;
    uint32_t us : 1;
    uint32_t pwt : 1;
    uint32_t pcd : 1;
    uint32_t a : 1;
    uint32_t d : 1;
    uint32_t pat : 1;
    uint32_t g : 1;
    uint32_t avl : 3;
    uint32_t addr : 20; 
} page_table_entry_t;

page_directory_entry_t page_directory[page_enteries] __attribute__((aligned(KB4)));

page_table_entry_t page_table[page_enteries] __attribute__((aligned(KB4)));

// Initalize page table vectors, directory vectors
void page_init();
// Put address of page directory into CR3 register
// C cannot access the comp reg, so use assembly to access CR3
extern void loadPageDirectory();
//Load address onto the CR3 register, where the MMU will find it
// Set 32th bit in the CR0 register, the paging bit to set protections
extern void enablePaging();
