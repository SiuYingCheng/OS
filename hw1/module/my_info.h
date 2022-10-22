#ifndef my_info
#define my_info
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/module.h>

#include <linux/utsname.h>

#include <generated/compile.h>
#include <linux/export.h>
#include <linux/uts.h>
#include <linux/version.h>
#include <linux/proc_ns.h>

#include <linux/cpufreq.h>
#include <linux/smp.h>
#include <linux/timex.h>
#include <linux/string.h>

#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/mman.h>
#include <linux/mmzone.h>
#include <linux/quicklist.h>
#include <linux/swap.h>
#include <linux/vmstat.h>
#include <linux/atomic.h>
#include <linux/vmalloc.h>
#ifdef CONFIG_CMA
#include <linux/cma.h>
#endif
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/processor.h>

#include <linux/sched.h>
#include <linux/time.h>
#include <linux/kernel_stat.h>
#include <linux/delay.h>
#include <linux/ktime.h>
#include <linux/math64.h>
#include <linux/percpu.h>
#include <linux/smp.h>

MODULE_LICENSE("GPL");
#endif

