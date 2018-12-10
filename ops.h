#include "fusell_ext4.h"
void op_ll_init(
    void *userdata, struct fuse_conn_info *conn);
void op_ll_getattr(
    fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
void op_ll_read(
    fuse_req_t req, fuse_ino_t ino, size_t size,
    off_t off, struct fuse_file_info *fi);
void op_ll_open(
    fuse_req_t req, fuse_ino_t ino,
    struct fuse_file_info *fi);

void op_ll_readdir(
    fuse_req_t req, fuse_ino_t ino, size_t size,
    off_t off, struct fuse_file_info *fi);