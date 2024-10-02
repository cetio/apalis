#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/binfmts.h>
#include <linux/fs_struct.h>
#include <linux/string.h>
#include <linux/security.h>

MODULE_LICENSE("GPL");

static int _pre_alloc_bprm(struct kprobe* p, struct pt_regs* regs)
{
    struct linux_binprm* bin = (struct linux_binprm*)regs->di;
    
    if (bin == NULL)
        return 0;

    printk(KERN_INFO "EX-H: %s\n", current->comm);
    return 0;
}

static int _pre_getname_flags(struct kprobe* p, struct pt_regs* regs)
{
    const char __user* filename = (const char __user*)regs->di;
    char kname[PATH_MAX];
    int len = strncpy_from_user(kname, filename, PATH_MAX);

    // If there's no data or RDX is 0 then this isn't actually a path that we care about (yet.)
    if (len == 0 || regs->dx != 0)
        return 0;

    // Absolute paths only.
    if (strcmp(kname, "/home/cet/.zshrc") == 0)
        return -1;

    printk(KERN_INFO "getname_flags | DI: %s SI: %d DX: %d\n", kname, regs->si, regs->dx);
    return 0;
}

static int _pre_iterate_dir(struct kprobe* p, struct pt_regs* regs)
{
    struct file* file = (struct file*)regs->di;
    struct dir_context* ctx = (struct dir_context*)regs->si;

    if (file == NULL || file->f_path.dentry == NULL || security_file_permission(file, MAY_READ))
        return 0;

    if (ctx == NULL)
        return 0;
    
    struct path path = file->f_path;
    char buf[PATH_MAX];
    char* name = dentry_path(path.dentry, buf, sizeof(buf));

    printk(KERN_INFO "iterate_dir | file: %s", name);
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
    // kp.pre_handler = _pre_getname_flags;
    kp.symbol_name = "iterate_dir";
    kp.pre_handler = _pre_iterate_dir;
    int err = register_kprobe(&kp);

    if (err != 0)
        printk(KERN_ERR "Failed to register getname_flags handler with error %d!\n", err);

    printk(KERN_INFO "Finalized load sequence.\n");
    return 0;
}

static void __exit apalis_exit(void) 
{
    unregister_kprobe(&kp);
    printk(KERN_INFO "Finalized unloading sequence.\n");
}

module_init(apalis_init);
module_exit(apalis_exit);;