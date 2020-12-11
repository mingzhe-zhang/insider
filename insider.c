#include "insider.h"

const int __endian_bit = 1;
#define is_bigendian() ( (*(char*)&__endian_bit) == 0 )

int get_pid(char* pname)
{
	const char* directory = "/proc";
	size_t taskNameSize = 1024;
	char* taskName = (char*)calloc(1, taskNameSize);

	DIR* dir = opendir(directory);

	if (dir)
	{
		struct dirent* de = 0;

		while ((de = readdir(dir)) != 0)
		{
			if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
			continue;

			int pid = -1;
			int res = sscanf(de->d_name, "%d", &pid);

			if (res == 1)
			{
            	// we have a valid pid

            	// open the cmdline file to determine what's the name of the process running
				char cmdline_file[1024] = {0};
				sprintf(cmdline_file, "%s/%d/cmdline", directory, pid);

				FILE* cmdline = fopen(cmdline_file, "r");

				if (getline(&taskName, &taskNameSize, cmdline) > 0)
				{
					// is it the process we care about?
					if (strstr(taskName, pname) != 0)
					{
#ifdef DEBUG_MODE
						fprintf(stdout, "A %s process, with PID %d, has been detected.\n", pname, pid);
#endif 
						return pid;
					}
				}

				fclose(cmdline);
			}
		}

		closedir(dir);
		return -1;
	}

	return -1;
}

int read_pagemap(int pid, unsigned long virt_addr, uint64_t* phy_addr, uint64_t* page_frame_num)
{
	FILE * f;
	char path_buf [0x100] = {};
	uint64_t read_val, file_offset;
	int status, i, c;

	if(pid!=-1)
	{
		sprintf(path_buf, "/proc/%u/pagemap", pid);
	}
	else
	{
		sprintf(path_buf, "/proc/self/pagemap");
	}

#ifdef DEBUG_MODE
	printf("Big endian? %d\n", is_bigendian());
#endif
	
	f = fopen(path_buf, "rb");
	if(!f)
	{
		printf("Error! Cannot open %s\n", path_buf);
		return -1;
	}
   
	//Shifting by virt-addr-offset number of bytes
	//and multiplying by the size of an address (the size of an entry in pagemap file)
	file_offset = virt_addr / getpagesize() * PAGEMAP_ENTRY;

#ifdef DEBUG_MODE
	printf("Vaddr: 0x%lx, Page_size: %d, Entry_size: %d\n", virt_addr, getpagesize(), PAGEMAP_ENTRY);
	printf("Reading %s at 0x%llx\n", path_buf, (unsigned long long) file_offset);
#endif

	status = fseek(f, file_offset, SEEK_SET);
	if(status)
	{
		perror("Failed to do fseek!");
		return -1;
	}
	errno = 0;
	read_val = 0;
	unsigned char c_buf[PAGEMAP_ENTRY];
	for(i=0; i < PAGEMAP_ENTRY; i++)
	{
		c = getc(f);
		if(c==EOF)
		{

#ifdef DEBUG_MODE
			printf("\nReached end of the file\n");
#endif

			return 0;
		}
		if(is_bigendian())
			c_buf[i] = c;
		else
			c_buf[PAGEMAP_ENTRY - i - 1] = c;

#ifdef DEBUG_MODE
		printf("[%d]0x%x ", i, c);
#endif

	}
	for(i=0; i < PAGEMAP_ENTRY; i++)
	{
		//printf("%d ",c_buf[i]);
		read_val = (read_val << 8) + c_buf[i];
	}

#ifdef DEBUG_MODE
	printf("\n");
	printf("PAGEMAP_ENTRY: 0x%llx\n", (unsigned long long) read_val);
#endif

	if(GET_BIT(read_val, 63))
	{
#ifdef DEBUG_MODE
		printf("PFN: 0x%llx\n",(unsigned long long) GET_PFN(read_val));
#endif
	}
	else
	{
#ifdef DEBUG_MODE
		printf("Page not present\n");
#endif
		return 1;
	}

	if(GET_BIT(read_val, 62))
	{
#ifdef DEBUG_MODE
		printf("Page swapped\n");
#endif
		return 2;
	}

	*page_frame_num = GET_PFN(read_val);
	*phy_addr = (*page_frame_num) * getpagesize() + virt_addr % getpagesize();

	fclose(f);
	return 0;

}


