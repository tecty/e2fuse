#include <stdio.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>


#define FUSE_USE_VERSION 31

#include <fuse3/fuse_lowlevel.h>
#include <ext2fs/ext2fs.h>


#if !defined(FUSELL_EXT4)
#define FUSELL_EXT4
// maybe we won't need assert 

// dir structure 
struct dirbuf{
    char *p;
    size_t size;
};


// some handy macro functions 
#define min(x, y) ((x) < (y) ? (x) : (y))


/*
 * Context of user data 
**/
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

static inline ext2_filsys current_ext2fs(fuse_req_t req);

// path max ? 
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif


#endif // FUSELL_EXT4
