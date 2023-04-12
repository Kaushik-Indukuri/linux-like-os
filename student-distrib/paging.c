#include "paging.h"
//#include "pagingasm.S"
#define page_enteries 1024 //total enteries
#define KB4 4096 //4kb in bits
#define tableI 0xb8 // location of only table initalized

void page_init()
{
    int i;
    //For each entry, set bit based on descriptors.dvi
    for (i = 0; i < page_enteries; i++) {
        if (i == tableI) {
            page_table[i].p = 1;
            page_table[i].addr = tableI;
        }
        else {
            page_table[i].p = 0;
            page_table[i].addr = 0;
        }
        page_table[i].pcd = 0;
        page_table[i].rw = 1;
        page_table[i].us = 0;
        page_table[i].pwt = 0;
        page_table[i].a = 0;
        page_table[i].d = 0;
        page_table[i].pat = 0;
        page_table[i].g = 1;
        page_table[i].avl = 0;
    }
    //For each entry, set bit based on descriptors.dvi
    for (i = 0; i < page_enteries; i++) {
        // points to page table for vid mem
        if (i == 0) {
            page_directory[i].p = 1;
            page_directory[i].rw = 1;
            page_directory[i].us = 0;
            page_directory[i].pwt = 0;
            page_directory[i].pcd = 0;
            page_directory[i].a = 0;
            page_directory[i].d = 0;
            page_directory[i].g = 0;
            page_directory[i].res = 1;
            page_directory[i].ps = 0;
            // Set directory to point to table 
            page_directory[i].addrlong = (uint32_t)page_table >> 12; //Offset by 12 bc you only want 10 MSB 
        }
        // kernel 4mb page
        else if (i == 1) {
            page_directory[i].p = 1;
            page_directory[i].rw = 1;
            page_directory[i].us = 0;
            page_directory[i].pwt = 0;
            page_directory[i].pcd = 0;
            page_directory[i].a = 0;
            page_directory[i].d = 0;
            page_directory[i].g = 0;
            page_directory[i].res = 1;
            page_directory[i].ps = 1;
            // Set directory to point to table 
            page_directory[i].addrlong = 1 << 10; //Offset by 10 bc you only want 10 MSB
        }
        // user 4mb page
        else if (i == 32) {
            page_directory[i].p = 1;
            page_directory[i].rw = 1;
            page_directory[i].us = 1;
            page_directory[i].pwt = 0;
            page_directory[i].pcd = 0;
            page_directory[i].a = 0;
            page_directory[i].d = 0;
            page_directory[i].g = 0;
            page_directory[i].res = 1;
            page_directory[i].ps = 1;
            // Set directory to point to table 
            //page_directory[i].addrshort = 1 << 1; //Offset by 10 bc you only want 10 MSB
        }
        else if (i == 34) {
            page_directory[i].p = 0;
            page_directory[i].rw = 1;
            page_directory[i].us = 1;
            page_directory[i].pwt = 0;
            page_directory[i].pcd = 0;
            page_directory[i].a = 0;
            page_directory[i].d = 0;
            page_directory[i].g = 0;
            page_directory[i].res = 1;
            page_directory[i].ps = 1;
            // Set directory to point to vid mem 
            //page_directory[i].addrshort = 1 << 1; //Offset by 10 bc you only want 10 MSB
        }
        else {
            page_directory[i].p = 0;
            page_directory[i].rw = 1;
            page_directory[i].us = 0;
            page_directory[i].pwt = 0;
            page_directory[i].pcd = 0;
            page_directory[i].a = 0;
            page_directory[i].d = 0;
            page_directory[i].g = 0;
            page_directory[i].res = 0;
            page_directory[i].ps = 0;
            page_directory[i].addrlong = 0; // else addr is 0
        }

    }
    // Call assembly functions to set computer registers
    loadPageDirectory((uint32_t*)page_directory);
    enablePaging();
}
