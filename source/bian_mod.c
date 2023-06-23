#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/list.h>
#include <linux/kprobes.h>
#include <linux/kernel.h>

/////////////////////////

#define LOG_FILE "aa.txt"

static struct file *logfile = NULL;

static int write_log(const char *fmt, ...)
{
    va_list args;
    char buf[256];
    int len;

    if (!logfile)
        return -EINVAL;

    va_start(args, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    return vfs_write(logfile, buf, len, &logfile->f_pos);
}





//////////////////////////////////////


#define FILENAME "tt.txt"  // 要写入的文件名

static int  write(int a)
{
    struct file *filp;
	
    loff_t pos;  // 文件当前偏移量
    ssize_t ret;  // 写入的字节数或者错误代码
    //char data[20] = {0}; // 要写入的数据
   // strcpy(data,name);
    //strcat(data,"\n");
	char data[]="bb\n";
    int len = sizeof(data) - 1;  // 数据长度，去掉末尾的 \n

    // 获取文件对象，如果文件不存在就创建它
    filp = filp_open(FILENAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (IS_ERR(filp)) {
        printk(KERN_ERR "Failed to open file %s for writing: %ld\n", FILENAME, PTR_ERR(filp));
        return PTR_ERR(filp);
    }

    // 设置起始位置为文件当前大小
    pos = vfs_llseek(filp, 0, SEEK_END);

    // 写入数据到文件
    ret = kernel_write(filp, data, len, &pos);
    if (ret < 0) {
        printk(KERN_ERR "Failed to write data to file %s: %zd\n", FILENAME, ret);
        filp_close(filp, NULL);
        return ret;
    }

    // 关闭文件
    filp_close(filp, NULL);
	
    return 0;
}





///////////////////////////////////////////////////////////

struct module *mod=NULL;
static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{	
	
	mod=(struct module *)(regs->di);
	
	printk("load mod name: %s\n",mod->name);
	//printk("do_init_module is run\n");
	return 0;
}


static void handler_post(struct kprobe *p, struct pt_regs *regs,unsigned long flags)
{
		
}


static void handler_post_1(struct kprobe *p, struct pt_regs *regs,unsigned long flags)
{
	if(mod!=NULL)
		{
		write_log(mod->name);
		write_log("\n");
		}
	 
}


static int handler_pre_1(struct kprobe *p, struct pt_regs *regs)
{
	
	printk("ftrace_release_mod\n");
	return 0;
}



char *name=NULL;
static void handler_post_3(struct kprobe *p, struct pt_regs *regs,unsigned long flags)
{
	//long ret=(long)(regs->ax);
	//if(name==NULL)
		//printk("name==NULL\n");
	//if(ret==0)
		//printk("ret==%ld\n",ret);
	//if(name!=NULL)
	//	printk("%s is unload\n ",name);
	
	//else	printk("is not not unload\n ");

	
}

static int handler_pre_3(struct kprobe *p, struct pt_regs *regs)
{
	//name=(char*)(regs->di);
	struct module *mod1=(struct module *)(regs->di);
	if(mod1!=NULL)
		printk("unload name==%s\n",mod1->name);
	return 0;
}
static struct kprobe kp1 = {
	.symbol_name	= "do_init_module" ,          //"do_free_init",
	.post_handler   =handler_post,
	.pre_handler    =handler_pre,
};


static struct kprobe kp2 = {
	.symbol_name	= "do_free_init",
	.post_handler   =handler_post_1,
	.pre_handler    =handler_pre_1,
};
//delete_modules

static struct kprobe kp3 = {
	.symbol_name	= "ftrace_release_mod",    // sys_delete_module
	.post_handler   =handler_post_3,
	.pre_handler    =handler_pre_3,
};


static int __init hello_init(void)
{
	///////////////////


	logfile = filp_open(LOG_FILE, O_RDWR | O_APPEND | O_CREAT, 0644);
    	if (IS_ERR(logfile)) {
        	printk(KERN_ALERT "cannot open log file %s\n", LOG_FILE);
       		return PTR_ERR(logfile);
   	 }
	///////////////////
    
	register_kprobe(&kp1);
	register_kprobe(&kp2);
	register_kprobe(&kp3);
    

    /* 遍历已加载的模块链表 */
   // list_for_each_entry(mod, THIS_MODULE->list.prev, list) {
      //  printk(KERN_INFO "Loaded module: %s\n", mod->name);
   // }

    return 0;
}

static void __exit hello_exit(void)
{
    	//filp_close(logfile, NULL);
	unregister_kprobe(&kp1);
	unregister_kprobe(&kp2);
	unregister_kprobe(&kp3);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your name");
MODULE_DESCRIPTION("A simple module to list loaded modules in the kernel.");
