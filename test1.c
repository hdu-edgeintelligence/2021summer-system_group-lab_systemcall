#include<linux/module.h>
#include<linux/init.h>
#include<linux/mm.h>
#include<linux/sched.h>
#include<linux/types.h>

static int pid;
static int tpid;
module_param(pid,int,0644);


static unsigned long vaddr2paddr(unsigned long vaddr)
{
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	unsigned long paddr = 0;
	unsigned long page_addr = 0;
	unsigned long page_offset = 0;
	
	pgd = pgd_offset(current->mm,vaddr);
	printk("pgd_val=0x%lx\n",pgd_val(*pgd));
	printk("pgd_index=%lu\n",pgd_index(vaddr));
	if(pgd_none(*pgd))
	{
	printk("not mapped in pgd\n");
	return -1;
	}
	
	p4d = p4d_offset(pgd,vaddr);
	printk("p4d_val=0x%lx\n",p4d_val(*p4d));
	if(p4d_none(*p4d))
	{
		printk("not mapped in p4d\n");
		return -1;
	}
	
	pud = pud_offset(p4d,vaddr);
	printk("pud_val=0x%lx\n",pud_val(*pud));
	//pud = pud_offset(pgd,vaddr);
	if(pud_none(*pud))
	{
		printk("not mapped in pud\n");
		return -1;
	}
	
	pmd = pmd_offset(pud,vaddr);
	printk("pmd_val = 0x%lx\n",pmd_val(*pmd));
	if(pmd_none(*pmd))
	{
		printk("not mapped in pmd\n");
		return -1;
	}
	
	pte = pte_offset_kernel(pmd,vaddr);
	printk("pte_val = 0x%lx\n",pte_val(*pte));
	printk("pte_index = %lu\n",pte_index(vaddr));
	pte_index(vaddr);
	if(pte_none(*pte))
	{
		printk("not mapped in pte\n");
		return -1;
	}
	page_addr = pte_val(*pte)&PAGE_MASK;
	page_offset = vaddr &~PAGE_MASK;
	paddr = page_addr|page_offset;
	unsigned long physical_addr=pte_pfn(*pte)<<PAGE_SHIFT;
	printk("the physical address is:%d\n",physical_addr);
	printk("page_addr=%lx,page_offset=%lx\n",page_addr,page_offset);
	printk("vaddr=%lx,paddr=%lx\n",vaddr,paddr);
	return paddr;
}

static void printit(struct task_struct *tsk)
	struct mm_struct *mm;
	struct vm_area_struct *vma;
	int j=0;
	unsigned long start,end,length;
	mm = tsk->mm;
	pr_info("mm_struct addr = 0x%p\n",mm);
	vma = mm->mmap;
	//down_read(&mm->mmap_sem);
	while(vma)
	{
		vaddr2paddr(vma);
		start= vma->vm_start;
		end = vma->vm_end;
		length=end-start;
		pr_info("%d %d %d %d",vma,start,end,length);
		vma=vma->vm_next;
		
	}
	//up_read(&mm->mmap_sem);
}		
static int __init my_vma_init(void)
{
	struct task_struct *tsk;
	//struct task_struct *parent=current->parent;
	if(pid==0)
	{
		tsk = current;
		pid = current->pid;
		
	}
	else
	{
		tsk = pid_task(find_vpid(pid),PIDTYPE_PID);
	}
	printit(tsk);
	pr_info("this just a test\n");
	return 0;
}

static void __exit vma_exit(void)
{
	pr_info("module exit\n");
}

module_init(my_vma_init);
module_exit(vma_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YMH");
MODULE_DESCRIPTION("TEST");
