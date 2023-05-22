#include <linux/syscalls.h>

#include "fat.h"

int open_journal(struct msdos_sb_info *sbi) { 
	// File opens in Kernel space, thus it is not visible by the user
	sbi->journal_fd = sys_open("/journal", O_CREAT|O_APPEND|O_RDWR, 0); 
	if (sbi->journal_fd >= 0) {
		printk("STUDENT MESSAGE: Opened the journal file succesfully");
		// Make first write 
		write_journal(sbi->journal_fd, "Start of Journal\n"); 
		
		return 0;
	} 
	printk(KERN_INFO "STUDENT WARNING: Failed to open the journal file");
	return -1;
}

void write_journal(int journal_fd, char* data, ...) { 
	char buf[128]; 

	// Format string (just like a printk)
	va_list args;
	va_start(args, data);
	vsprintf(buf, data, args);
	va_end(args); 

	// Write to file
	if (sys_write(journal_fd, buf, strlen(buf)) < 0) {
		printk("STUDENT WARNING: Failed to write to file");
	}
	
	return;
} 

void print_journal(int journal_fd) {
	char buf[1024]; // We assume that the size is sufficient 
	
	// Start reading the journal from the beginning 
	sys_lseek(journal_fd, 0, SEEK_SET); 

	printk("STUDENT MESSAGE: Printing the Journal"); 
	while(sys_read(journal_fd, buf, sizeof(buf)) > 0) {
		printk("%s", buf);
	} 

	printk("STUDENT MESSAGE: End of Journal"); 
	return;
}

void close_journal(int journal_fd) { 
	print_journal(journal_fd);
	printk("STUDENT MESSAGE: Closing the journaling fd");
	sys_close(journal_fd); 
	return;
}
