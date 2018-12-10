#include "fusell_ext4.h"
/* TODO: background thread */


ext2_filsys current_ext2fs(fuse_req_t req)
{
	struct extfs_data *e2data=fuse_req_userdata(req);
	// time_t now=time(NULL);
// deprecate this, since we will have another thread to 
// flush the data
	// if ((now - e2data->last_flush) > FLUSH_BITMAPS_TIMEOUT) {
	// 	ext2fs_write_bitmaps(e2data->e2fs);
	
	// 	e2data->last_flush=now;
	// }
	return (ext2_filsys) e2data->e2fs;
}
