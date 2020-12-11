#include "insider.h"

int main(int argc, char** argv)
{
   unsigned long virt_addr;
   uint64_t phy_addr, pfn;

   if (argc < 2)
   {
      fprintf(stderr, "Usage: %s <process name>\n", argv[0]);
      return 1;
   }

   virt_addr=strtol(argv[2], NULL, 16);

   int result=get_pid(argv[1]);

   fprintf(stdout, "A %s process, with PID %d, has been detected.\n", argv[1], result);

   read_pagemap(result, virt_addr, &phy_addr, &pfn);

   printf("Result: 0x%llx\n", (unsigned long long) phy_addr);

   printf("PFN: 0x%llx\n", (unsigned long long) pfn);



   // just let the OS free this process' memory!
   //free(taskName);

   return 0;
}