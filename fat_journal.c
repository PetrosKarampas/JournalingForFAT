#include <linux/syscalls.h>

#include "fat.h"

#define MAX_LINE_LEN 51

const char *LINE = "==================================================\n";


int open_journal(struct msdos_sb_info *sbi) { 
	// File opens in Kernel space, thus it is not visible by the user
	sbi->journal_fd = sys_open("/journal", O_CREAT|O_APPEND|O_RDWR, 0); 
	if (sbi->journal_fd >= 0) {
		printk("STUDENT MESSAGE: Opened the journal file succesfully");
		// Make first write 
		write_journal(sbi->journal_fd, "\t\t\tSTART OF JOURNAL\n%s", LINE); 
		
		return 0;
	} 
	printk(KERN_INFO "STUDENT WARNING: Failed to open the journal file");
	return -1;
} 

// There was a dream once
static char* warp(char* buf) {
	// How many bytes is the overhead?
	// How many lines more do we need?
	// Break the string in buffers of size MAX_LINE_LEN (including the \n char)
	// If strlen(buf) % MAX_LINE_LEN > 0, fill the last buf with \0 and finish with \n
	// If string is too large to be written in one line, break it
	if(strlen(buf) > MAX_LINE_LEN) {
		printk("There will be wrapping errors");
	}
}

void write_journal(int journal_fd, char* data, ...) { 
	char buf[1024]; 

	// Format string (just like a printk)
	va_list args;
	va_start(args, data);
	vsprintf(buf, data, args);
	va_end(args); 

	warp(buf); 

	// Write to file
	if (sys_write(journal_fd, buf, strlen(buf)) < 0) 
		printk("STUDENT WARNING: Failed to write to file"); 

	// Sync contents
	if(sys_fsync(journal_fd) < 0)
		printk("STUDENT WARNING: Failed to sync file");
	
	return;
} 

void print_journal(int journal_fd) {
	char buf[512]; 
	
	// Start reading the journal from the beginning 
	sys_lseek(journal_fd, 0, SEEK_SET); 

	printk(KERN_INFO "STUDENT MESSAGE: Printing the Journal:"); 
	int bytes_read;
	int bytes_total = 0;
	while((bytes_read = sys_read(journal_fd, buf, sizeof(buf))) > 0) {
		printk("%s\n", buf);
		bytes_total += bytes_read;
		memset(buf, 0, sizeof(buf)); // Clean the buffer, otherwise there are overlaps. 
	} 
	printk("Read %d bytes in total\n", bytes_total);
	return;
}

// A small break-check for the journal
static void overload_journal(int journal_fd) {
	int i = 0;
	while (i < 150000) { 
		write_journal(journal_fd, "OVERLOADING %d!!!\n", i);	
		i++;
	}
	write_journal(journal_fd, "\n================\nEnd of Journal\n");
	print_journal(journal_fd);
	return;
} 

void close_journal(int journal_fd) { 
	write_journal(journal_fd, "%s\t\t\tEND OF JOURNAL\n", LINE);
	print_journal(journal_fd);
	printk("STUDENT MESSAGE: Closing the journaling fd");
	sys_close(journal_fd); 
	return;
}
