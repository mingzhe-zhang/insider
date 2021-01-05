#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>

//#define DEBUG_MODE

#define PAGEMAP_ENTRY 8
#define GET_BIT(X,Y) (X & ((uint64_t)1<<Y)) >> Y
#define GET_PFN(X) X & 0x7FFFFFFFFFFFFF



// get the pid by the name of process
// when not found, return -1
int get_pid(char* pname);

// translate the virtual address to the physical address
// return -1 when the address is illegal
int read_pagemap(int pid, unsigned long virt_addr, uint64_t* phy_addr, uint64_t* page_frame_num);