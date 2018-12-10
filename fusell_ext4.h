#include <stdio.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

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


#endif // FUSELL_EXT4
