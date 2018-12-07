#include "fusell_ext4.h"

static const char * hello_str = "hello world!\n";
static const char * hello_name = "hello";


static int hello_stat(fuse_ino_t ino, struct stat *stbuf)
{
	stbuf->st_ino = ino;
	switch (ino) {
	case 1:
		stbuf->st_mode = LINUX_S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		break;

	case 2:
		stbuf->st_mode = LINUX_S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
		break;

	default:
		return -1;
	}
	return 0;
}

static void hello_ll_getattr(
    fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi
){
	// store the file stat
	struct stat stat_buf;

	(void) fi;

	memset(&stat_buf ,0, sizeof(struct stat));
	if(hello_stat(ino, &stat_buf)){
		// file not found 
		fuse_reply_err(req, ENOENT);
	}
	else{
		fuse_reply_attr(req, &stat_buf, 1.0);
	}
}

static void hello_ll_lookup(
	fuse_req_t req, fuse_ino_t parent, 
	const char * name 	
){
	struct fuse_entry_param e;
	
	if (parent != 1 || strcmp(name, hello_name)!= 0) {
		fuse_reply_err(req, ENONET);
	}
	else {
		memset(&e, 0, sizeof(e));
		e.ino = 2; 
		e.attr_timeout = 1.0;
		e.entry_timeout = 1.0;
		hello_stat(e.ino, & e.attr);
		// reply the message 
		fuse_reply_entry(req, &e);
	}
}

static void dirbuf_add(
	fuse_req_t req, struct dirbuf *b,
	const char * name, fuse_ino_t ino
){
	struct stat stbuf; 
	size_t oldsize = b-> size;
	b->size += fuse_add_direntry(req, NULL, 0, name, NULL, 0);
	b->p    = (char *) realloc(b->p, b->size);
	memset(&stbuf, 0, sizeof(stbuf));

	stbuf.st_ino = ino; 
	fuse_add_direntry(
		req, b->p + oldsize, b->size - oldsize,
		name, &stbuf, b->size
	);
}

static int reply_buf_limited(
	fuse_req_t req, const char *buf, size_t bufsize,
	off_t off, size_t maxsize
){
	if (off < bufsize){
		return fuse_reply_buf(
			req, buf+ off,
			min(bufsize - off, maxsize)
		);
	}
	// ELSE:
	// reply an empty buffer, size the bufer is not enough
	return fuse_reply_buf(req, NULL, 0);
}


static void hello_ll_readdir(
	fuse_req_t req, fuse_ino_t ino, 
	size_t size, off_t off, 
	struct fuse_file_info *fi
){

}




static struct fuse_lowlevel_ops hello_ll_oper = {
	.lookup		= hello_ll_lookup,
	// ls function 
	.getattr	= hello_ll_getattr,
	.readdir	= hello_ll_readdir,
	// .open		= hello_ll_open,
	// .read		= hello_ll_read,
};


int main(int argc, char *argv[])
{
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	struct fuse_session *se; 
	struct fuse_cmdline_opts opts;
	int ret = -1;
	
	if (fuse_parse_cmdline(&args, &opts)!= 0) {
		return 1;
	}

	
	if (opts.show_help) {
		printf("usage: %s [options] <mountpoint>\n\n", argv[0]);
		fuse_cmdline_help();
		fuse_lowlevel_help();
		ret = 0;
		goto error_out1;
	}
	else if(opts.show_version) {
		printf("FUSE library version %s",fuse_pkgversion());
		fuse_lowlevel_version();
		ret = 0 ;
		goto error_out1;
	}
	se = fuse_session_new(&args, &hello_ll_oper,
			      sizeof(hello_ll_oper), NULL);

	if (se ==NULL){
		goto error_out1;
	}

	if (fuse_set_signal_handlers(se)!= 0){
		goto error_out2;
	}

	
	if (fuse_session_mount(se, opts.mountpoint)!= 0) {
		goto error_out3;
	}
	

	return 0;

error_out3:
	fuse_remove_signal_handlers(se);
error_out2:
	fuse_session_destroy(se);
error_out1:
	free(opts.mountpoint);
	fuse_opt_free_args(&args);
	return ret ? 1: 0;
}
