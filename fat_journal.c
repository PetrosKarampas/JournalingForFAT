#include <linux/syscalls.h>
#include <linux/vfs.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>

#include "fat.h"

int open_journal(struct msdos_sb_info *sbi) { 
	sbi->journal_fd = sys_open("/journal.txt", O_CREAT|O_APPEND|O_RDWR, 0);
	if (sbi->journal_fd >= 0) {
		printk("STUDENT MESSAGE: Opened the journal file succesfully");
		return 0;
	} 
	printk(KERN_INFO "STUDENT MESSAGE: Failed to open the journal file");
	return -1;
}

void write_journal(int journal_fd, char* data) {
	struct file *file = fget(journal_fd);
	loff_t offset;
	if (!file) {
		printk("STUDENT MESSAGE: Failed to open journal file");
		return;
	}
	
	// Set offset at the end of the file
	offset = vfs_llseek(file, 0, SEEK_END);
	if (offset < 0) {
		printk("STUDENT MESSAGE: Failed to update the offset of the journal");
		fput(file);
		return;
	}

	// Write to file
	if (vfs_write(file, data, strlen(data), &offset) < 0) {
		printk("STUDENT MESSAGE: Failed to write to file");
		fput(file);
		return;
	}
	fput(file);
	return;
} 

void print_journal(int journal_fd) {
	struct file *file ;
	char buf[100];

	file = fget(journal_fd);
	if (!file) {
		printk("Failed to open journal file");
		return;
	} 

	vfs_llseek(file, 0, SEEK_SET); // Start reading the journal from the beginning
	printk("STUDENT MESSAGE: Printing the Journal"); 
	while(sys_read(journal_fd, buf, strlen(buf) - 1) > 0) {
		printk("Journal Entry: %s", buf);
	} 
	printk("%s\n", buf);
	printk("Journal End");

	fput(file);
	return;
}
