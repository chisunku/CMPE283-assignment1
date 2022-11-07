/*  
 *  cmpe283-1.c - Kernel module for CMPE283 assignment 1
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <asm/msr.h>

#define MAX_MSG 80

/*
 * Model specific registers (MSRs) by the module.
 * See SDM volume 4, section 2.1
 */
#define IA32_VMX_PINBASED_CTLS	0x481
#define IA32_VMX_PROCBASED_CTLS 0x482
#define IA32_VMX_PROCBASED_CTLS2 	0x48B
#define IA32_VMX_EXIT_CTLS 	0x483
#define IA32_VMX_ENTRY_CTLS  0x484
#define IA32_VMX_PROCBASED_CTLS3 0x492

/*
 * struct caapability_info
 *
 * Represents a single capability (bit number and description).
 * Used by report_capability to output VMX capabilities.
 */
struct capability_info {
	uint8_t bit;
	const char *name;
};


/*
 * Pinbased capabilities
 * See SDM volume 3, section 24.6.1
 */
struct capability_info pinbased[5] =
{
	{ 0, "External Interrupt Exiting" },
	{ 3, "NMI Exiting" },
	{ 5, "Virtual NMIs" },
	{ 6, "Activate VMX Preemption Timer" },
	{ 7, "Process Posted Interrupts" }
};

struct capability_info procbased[22] =
{
	{ 2, "Interrupt-window exiting" },
	{ 3, "Use TSC offsetting" },
	{ 7, "HLT exiting" },
	{ 9, "INVLPG exiting" },
	{ 10, "MWAIT exiting" },
	{ 11, "RDPMC exiting" },
	{ 12, "RDTSC exiting" },
	{ 15, "CR3-load exiting" },
	{ 16, "CR3-store exiting" },
	{ 17, "Activate tertiary controls" },
	{ 19, "CR8-load exiting" },
	{ 20, "CR8-store exiting" },
	{ 21, "Use TPR shadow" },
	{ 22, "NMI-window exiting" },
	{ 23, "MOV-DR exiting" },
	{ 24, "Unconditional I/O exiting" },
	{ 25, "Use I/O bitmaps" },
	{ 27, "Monitor trap flag" },
	{ 28, "Use MSR bitmaps" },
	{ 29, "MONITOR exiting" },
	{ 30, "PAUSE exiting" },
	{ 31, "Activate secondary controls" }
};

struct capability_info procbased_ctls2[28] =
{
	{ 0, "Interrupt-window exiting" },
	{ 1, "Use TSC offsetting" },
	{ 2, "HLT exiting" },
	{ 3, "INVLPG exiting" },
	{ 4, "MWAIT exiting" },
	{ 5, "RDPMC exiting" },
	{ 6, "RDTSC exiting" },
	{ 7, "CR3-load exiting" },
	{ 8, "CR3-store exiting" },
	{ 9, "Activate tertiary controls" },
	{ 10, "CR8-load exiting" },
	{ 11, "CR8-store exiting" },
	{ 12, "Use TPR shadow" },
	{ 13, "NMI-window exiting" },
	{ 14, "MOV-DR exiting" },
	{ 15, "Unconditional I/O exiting" },
	{ 16, "Use I/O bitmaps" },
	{ 17, "Monitor trap flag" },
	{ 18, "Use MSR bitmaps" },
	{ 19, "MONITOR exiting" },
	{ 20, "PAUSE exiting" },
	{ 22, "Activate secondary controls" },
	{ 23, "Unconditional I/O exiting" },
	{ 24, "Use I/O bitmaps" },
	{ 25, "Monitor trap flag" },
	{ 26, "Use MSR bitmaps" },
	{ 27, "MONITOR exiting" },
	{ 28, "PAUSE exiting" }
};

struct capability_info vmx_exit[17] =
{
	{ 2, "Save debug controls" },
	{ 9, "Host address-space size" },
	{ 12, "Load IA32_PERF_GLOBAL_CTRL" },
	{ 15, "Acknowledge interrupt on exit" },
	{ 18, "Save IA32_PAT" },
	{ 19, "Load IA32_PAT" },
	{ 20, "Save IA32_EFER" },
	{ 21, "Load IA32_EFER" },
	{ 22, "Save VMX-preemption timer value" },
	{ 23, "Clear IA32_BNDCFGS" },
	{ 24, "Conceal VMX from PT" },
	{ 25, "Clear IA32_RTIT_CTL" },
	{ 26, "Clear IA32_LBR_CTL" },
	{ 28, "Load CET state" },
	{ 29, "Load PKRS" },
	{ 30, "Save IA32_PERF_GLOBAL_CTL" },
	{ 31, "Activate secondary controls" }
};

struct capability_info vmx_entry[13] =
{
	{ 2, "Load debug controls" },
	{ 9, "IA-32e mode guest" },
	{ 10, "Entry to SMM" },
	{ 11, "Deactivate dualmonitor treatment" },
	{ 13, "Load IA32_PERF_GLOBAL_CTRL" },
	{ 14, "Load IA32_PAT" },
	{ 15, "Load IA32_EFER" },
	{ 16, "Load IA32_BNDCFGS" },
	{ 17, "Conceal VMX from PT" },
	{ 18, "Load IA32_RTIT_CTL" },
	{ 20, "Load CET state" },
	{ 21, "Load guest IA32_LBR_CTL" },
	{ 22, "Load PKRS" }
};

struct capability_info procbased_ctls3[4] =
{
	{ 0, "LOADIWKEY exiting" },
        { 1, "Enable HLAT" },
        { 2, "EPT paging-write control" },
        { 3, "Guest-paging verification" }
};

/*
 * report_capability
 *
 * Reports capabilities present in 'cap' using the corresponding MSR values
 * provided in 'lo' and 'hi'.
 *
 * Parameters:
 *  cap: capability_info structure for this feature
 *  len: number of entries in 'cap'
 *  lo: low 32 bits of capability MSR value describing this feature
 *  hi: high 32 bits of capability MSR value describing this feature
 */
void report_capability(struct capability_info *cap, uint8_t len, uint32_t lo, uint32_t hi)
{
	uint8_t i;
	struct capability_info *c;
	char msg[MAX_MSG];

	memset(msg, 0, sizeof(msg));

	for (i = 0; i < len; i++) {
		c = &cap[i];
		snprintf(msg, 79, "  %s: Can set=%s, Can clear=%s\n",
		    c->name,
		    (hi & (1 << c->bit)) ? "Yes" : "No",
		    !(lo & (1 << c->bit)) ? "Yes" : "No");
		printk(msg);
	}
}

/*
 * detect_vmx_features
 *
 * Detects and prints VMX capabilities of this host's CPU.
 */
void
detect_vmx_features(void)
{
	uint32_t lo, hi;

	/* Pinbased controls */
	rdmsr(IA32_VMX_PINBASED_CTLS, lo, hi);
	pr_info("Pinbased Controls MSR: 0x%llx\n",
		(uint64_t)(lo | (uint64_t)hi << 32));
	report_capability(pinbased, 5, lo, hi);

	/* Procbased controls */
	rdmsr(IA32_VMX_PROCBASED_CTLS, lo, hi);
	pr_info("Procbased Controls MSR: 0x%llx\n",
		(uint64_t)(lo | (uint64_t)hi << 32));
	report_capability(procbased, 22, lo, hi);

	/* Procbased controls secondry */
	if((hi >> 31) && 1){
		rdmsr(IA32_VMX_PROCBASED_CTLS2, lo, hi);
		pr_info("Procbased secondary Controls MSR: 0x%llx\n",
			(uint64_t)(lo | (uint64_t)hi << 32));
		report_capability(procbased_ctls2, 28, lo, hi);
	}
	/* Procbased controls tertiary*/
	if((hi >> 17) && 1){
                rdmsr(IA32_VMX_PROCBASED_CTLS3, lo, hi);
                pr_info("Procbased tertiary Controls MSR: 0x%llx\n",
                        (uint64_t)(lo | (uint64_t)hi << 32));
                report_capability(procbased_ctls3, 4, lo, hi);
	}

	/* VMX EXIT controls */
	rdmsr(IA32_VMX_EXIT_CTLS, lo, hi);
	pr_info("VMX exit MSR: 0x%llx\n",
		(uint64_t)(lo | (uint64_t)hi << 32));
	report_capability(vmx_exit, 17, lo, hi);

	/* VMX EXIT controls */
	rdmsr(IA32_VMX_ENTRY_CTLS, lo, hi);
	pr_info("VMX entry MSR: 0x%llx\n",
		(uint64_t)(lo | (uint64_t)hi << 32));
	report_capability(vmx_entry, 13, lo, hi);
}

/*
 * init_module
 *
 * Module entry point
 *
 * Return Values:
 *  Always 0
 */
int
init_module(void)
{
	printk(KERN_INFO "CMPE 283 Assignment 1 Module Start\n");

	detect_vmx_features();

	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

/*
 * cleanup_module
 *
 * Function called on module unload
 */
void
cleanup_module(void)
{
	printk(KERN_INFO "CMPE 283 Assignment 1 Module Exits\n");
}

MODULE_LICENSE("GPL");
