#include <linux/syscalls.h>
#include <linux/vfs.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include "fat.h"

int open_journal(struct msdos_sb_info *sbi) { 
	sbi->journal_fd = sys_open("/journal.txt", O_CREAT|O_APPEND|O_RDWR, S_IRUSR|S_IWUSR);
	if (sbi->journal_fd >= 0) {
		printk("STUDENT MESSAGE: Opened the journal file succesfully");
		return 0;
	} 
	printk(KERN_INFO "STUDENT MESSAGE: Failed to open the journal file");
	return -1;
}

void write_journal(int journal_fd, char* data) {
	loff_t pos = 0;
	if (sys_write(journal_fd, data, strlen(data)) > 0) {
		struct file *file = fget(journal_fd);
		if (file) {
			vfs_write(file, data, strlen(data), &pos);
			fput(file);
		}
		return;
	} 
	printk("STUDENT MESSAGE: Failed to write to journal");
	return;
} 

void print_journal(int journal_fd) {
	printk("\nSTUDENT MESSAGE: Printing the Journal");
	char buf[100];
	while(sys_read(journal_fd, buf, sizeof(buf) - 1) > 0) {
		printk("%s\n", buf);
	} 
	printk("\nJournal End");
	return;
}
