#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/binfmts.h>
#include <linux/fs_struct.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");

#define NUM_IVFS 4
#define NUM_BINMD 3

struct ivfs_descriptor
{
    struct kprobe kp[NUM_IVFS];

    int (*open)(const struct path*, struct file*, const struct cred*);
    ssize_t (*write)(struct file*, const char *, size_t, loff_t*);
    ssize_t (*read)(struct file*, const char*, size_t, loff_t*);
    int (*readdir)(struct file*, struct dir_context*);
};

struct binmd_descriptor
{
    struct kprobe kp[NUM_BINMD];

    int (*execve)(struct linux_binprm*);
    int (*usb_probe)(struct device*);
    int (*bus_probe)(struct device*);
};

int ivfs_init(struct ivfs_descriptor* desc)
{
    for (int i = 0; i < NUM_IVFS; i++)
    {
        if ((desc->kp[i]).symbol_name == NULL)
            continue;
            
        int err = register_kprobe(&(desc->kp[i]));

        if (err != 0)
            return err;
    }
    
    return 0;
}

int binmd_init(struct binmd_descriptor* desc)
{
    for (int i = 0; i < NUM_BINMD; i++)
    {
        if ((desc->kp[i]).symbol_name == NULL)
            continue;
            
        int err = register_kprobe(&(desc->kp[i]));

        if (err != 0)
            return err;
    }
    
    return 0;
}

static int bprm_handler_pre(struct kprobe* p, struct pt_regs* regs)
{
    struct linux_binprm* bin = (struct linux_binprm*)regs->di;
    
    if (bin == NULL)
        return 0;

    printk(KERN_INFO "EX-H: %s\n", current->comm);
    return 0;
}

static int fs_handler_pre(struct kprobe* p, struct pt_regs* regs)
{
    const char __user* filename = (const char __user*)regs->di;
    char kname[PATH_MAX];
    int len = strncpy_from_user(kname, filename, PATH_MAX);

    if (len == 0 || regs->si == 1)
        return 0;

    printk(KERN_INFO "FS-H DI: %s SI: %d DX: %d\n", kname, regs->si, regs->dx);
    return 0;
}

static int vfs_handler_pre(struct kprobe* p, struct pt_regs* regs)
{
    // char name[256];
    // char* ret = d_path((struct path*)regs->di, name, sizeof(name));

    // printk(KERN_INFO "VFS-H: %s\n", ret);
    printk(KERN_INFO "gaah");
    return 0;
}

static struct kprobe kp;

static int __init apalis_init(void) 
{
    // kp.symbol_name = "bprm_execve";
    // kp.pre_handler = bprm_handler_pre;
    // int err = register_kprobe(&kp);

    // if (err < 0)
    //     printk(KERN_ERR "Failed to register execution handler with error %d!\n", err);

    // ext4 is the one true way.
    if (strcmp(current->fs->root.mnt->mnt_sb->s_type->name, "ext4") != 0)
    {
        printk(KERN_ERR "Unsupported file system %s!\n", current->fs->root.mnt->mnt_sb->s_type->name);
        return -1;
    }

    // kp.symbol_name = "getname_flags";
    // kp.pre_handler = fs_handler_pre;
    kp.symbol_name = "vfs_iter_read";
    kp.pre_handler = vfs_handler_pre;
    int err = register_kprobe(&kp);

    if (err < 0)
        printk(KERN_ERR "Failed to register file system handler with error %d!\n", err);

    printk(KERN_INFO "Finalized load sequence.\n");
    return 0;
}

static void __exit apalis_exit(void) 
{
    unregister_kprobe(&kp);
    printk(KERN_INFO "Finalized unloading sequence.\n");
}

module_init(apalis_init);
module_exit(apalis_exit);