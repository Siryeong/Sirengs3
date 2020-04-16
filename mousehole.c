#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cred.h>
#include <asm/unistd.h>

MODULE_LICENSE("GPL");

void ** sctable;
unsigned int cur_uid;
char filepath[12][128] = {{0x0,}};
unsigned int userid[12] = {0,};
unsigned int donkil[12] = {0,};
int uididx = 0;
int kilidx = 0;


asmlinkage int (*orig_sys_open)(const char __user * filename, int flags, umode_t mode);
asmlinkage long (*orig_sys_kill)(pid_t pid, int sig);

asmlinkage long mousehole_sys_kill(pid_t pid, int sig)
{
	struct task_struct * t;
	unsigned int a;
	for_each_process(t){
		a = t->cred->uid.val;
		if(donkil[0] != 0 && a == donkil[0]) return -1;
	}
	return orig_sys_kill(pid, sig);
}

asmlinkage int mousehole_sys_open(const char __user * filename, int flags, umode_t mode)
{
	char fname[256];
	char * find;

	cur_uid = current_uid().val;
	copy_from_user(fname, filename, 256);

	find = strstr(filename, filepath[0]);
	
	if(userid[0] != 0 &&
			find != 0x0 &&
			filepath[0][0] != 0x0 && 
			cur_uid == userid[0]) return -1;
	return orig_sys_open(filename, flags, mode);
}

static
int mousehole_open(struct inode *inode, struct file *file)
{
	return 0;
}

static
int mousehole_release(struct inode *inode, struct file *file)
{
	return 0;
}

static
ssize_t mousehole_read(struct file * file, char __user *ubuf, size_t size, loff_t *offset)
{
	char buf[128];
	ssize_t toread;
	sprintf(buf, "blocking user : %u, file : %s.\nprotect user : %u.\n", userid[0], filepath[0], donkil[0]);

	toread = strlen(buf) >= *offset + size ? size : strlen(buf) - *offset;

	if(copy_to_user(ubuf, buf + *offset, toread))
		return -EFAULT;

	*offset = *offset + toread;

	return toread;
}

static
ssize_t mousehole_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset)
{
	char buf[256];
	char filebuf[128];
	unsigned int namebuf;
	int md = 0;
	
	if (*offset != 0 || size > 128)
		return -EFAULT ;
	if (copy_from_user(buf, ubuf, size))
		return -EFAULT ;
	
	sscanf(buf, " %d %u %s", &md, &namebuf, filebuf);
	if(md == 0){userid[0] = namebuf; strcpy(filepath[0], filebuf);}
	if(md == 1){userid[0] = 0; strcpy(filepath[0], "");}
	if(md == 2){donkil[0] = namebuf;}
	if(md == 3){donkil[0] = 0;}

	*offset = strlen(buf);
	
	return *offset;				
}

static const struct file_operations mousehole_fops = {
	.owner = THIS_MODULE,
	.open = mousehole_open,
	.read = mousehole_read,
	.write = mousehole_write,
	.llseek = seq_lseek,
	.release = mousehole_release,
} ;

static
int __init mousehole_init(void){
	unsigned int level;
	pte_t * pte;

	proc_create("mousehole", S_IRUGO | S_IWUGO, NULL, &mousehole_fops);

	sctable = (void *) kallsyms_lookup_name("sys_call_table");
	
	orig_sys_open = sctable[__NR_open];
	orig_sys_kill = sctable[__NR_kill];
	pte = lookup_address((unsigned long) sctable, &level) ;
  if (pte->pte &~ _PAGE_RW)
	  pte->pte |= _PAGE_RW ;
	sctable[__NR_open] = mousehole_sys_open ;
	sctable[__NR_kill] = mousehole_sys_kill ;

	return 0;
}

static
void __exit mousehole_exit(void){
	unsigned int level;
	pte_t * pte;
	remove_proc_entry("mousehole", NULL);

	sctable[__NR_open] = orig_sys_open;
	sctable[__NR_kill] = orig_sys_kill;
	pte = lookup_address((unsigned long) sctable, &level);
	pte->pte = pte->pte &~ _PAGE_RW;
}

module_init(mousehole_init);
module_exit(mousehole_exit);
