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

/* max timeout to flush bitmaps, to reduce inconsistencies */
#define FLUSH_BITMAPS_TIMEOUT 10


ext2_filsys current_ext2fs(fuse_req_t req);


// path max ? 
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif



#if ENABLE_DEBUG

static inline void debug_printf (const char *function, char *file, int line, const char *fmt, ...)
{
	va_list args;
	struct fuse_context *mycontext=fuse_get_context();
	struct extfs_data *e2data=mycontext->private_data;
	if (e2data && (e2data->debug == 0 || e2data->silent == 1)) {
		return;
	}
	printf("%s: ", PACKAGE);
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf(" [%s (%s:%d)]\n", function, file, line);
}

#define debugf(a...) { \
	debug_printf(__FUNCTION__, __FILE__, __LINE__, a); \
}

static inline void debug_main_printf (const char *function, char *file, int line, const char *fmt, ...)
{
	va_list args;
	printf("%s: ", PACKAGE);
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf(" [%s (%s:%d)]\n", function, file, line);
}

#define debugf_main(a...) { \
	debug_main_printf(__FUNCTION__, __FILE__, __LINE__, a); \
}

#else /* ENABLE_DEBUG */

#define debugf(a...) do { } while(0)
#define debugf_main(a...) do { } while(0)

#endif /* ENABLE_DEBUG */


#endif // FUSELL_EXT4
