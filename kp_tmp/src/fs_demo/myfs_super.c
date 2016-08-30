#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/inet.h>
#include <linux/pagemap.h>
#include <linux/seq_file.h>
#include <linux/mount.h>


static const struct super_operations v9fs_super_ops;

/**
 * v9fs_get_sb - mount a superblock
 * @fs_type: file system type
 * @flags: mount flags
 * @dev_name: device name that was mounted
 * @data: mount options
 * @mnt: mountpoint record to be instantiated
 */

static int myfs_get_sb(struct file_system_type *fs_type, int flags,
			const char *dev_name, void *data, struct vfsmount *mnt)
{
	int ret = 0;
	struct super_block *sb = NULL;
	struct inode *inode = NULL;
	struct dentry *root = NULL;

	return -1;
}

/**
 * myfs_kill_super - Kill Superblock
 * @s: superblock
 */

static void myfs_kill_super(struct super_block *s)
{

}

static const struct super_operations v9fs_super_ops = {
	.statfs = simple_statfs,
	.show_options = generic_show_options,
#if 0
	.alloc_inode = v9fs_alloc_inode,
	.destroy_inode = v9fs_destroy_inode,
	.clear_inode = v9fs_clear_inode,
	.umount_begin = v9fs_umount_begin,
#endif
};
        
struct file_system_type my_fs_type = {
	.name = "myfs",
	.get_sb = myfs_get_sb,
	.kill_sb = myfs_kill_super,
	.owner = THIS_MODULE,
}; 
