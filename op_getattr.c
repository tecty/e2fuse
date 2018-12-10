#include "ops.h"

void op_ll_getattr(
    fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi
){
	// store the file stat
	struct stat stat_buf;

	(void) fi;

    /* We use user data to replace the fuse_context */

    
    ext2_filsys e2fs = current_ext2fs(req);

	memset(&stat_buf ,0, sizeof(struct stat));
	// if(op_stat(ino, &stat_buf)){
	// 	// file not found 
	// 	fuse_reply_err(req, ENOENT);
	// }
	// else{
	// 	fuse_reply_attr(req, &stat_buf, 1.0);
	// }
}
