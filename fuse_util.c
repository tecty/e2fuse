#include "fusell_ext4.h"
/* TODO: background thread */
/* max timeout to flush bitmaps, to reduce inconsistencies */
#define FLUSH_BITMAPS_TIMEOUT 10

struct extfs_data {
	unsigned char debug;
	unsigned char silent;
	unsigned char force;
	unsigned char readonly;
	time_t last_flush;
	char *mnt_point;
	char *options;
	char *device;
	char *volname;
	ext2_filsys e2fs;
};

static inline ext2_filsys current_ext2fs(void)
{
	struct fuse_context *mycontext=fuse_get_context();
	struct extfs_data *e2data=mycontext->private_data;
	time_t now=time(NULL);
	if ((now - e2data->last_flush) > FLUSH_BITMAPS_TIMEOUT) {
		ext2fs_write_bitmaps(e2data->e2fs);
		e2data->last_flush=now;
	}
	return (ext2_filsys) e2data->e2fs;
}