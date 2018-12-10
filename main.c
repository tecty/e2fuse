#include "ops.h"
static const char * op_str = "hello world!\n";
static const char * op_name = "hello";


static int op_stat(fuse_ino_t ino, struct stat *stbuf)
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
		stbuf->st_size = strlen(op_str);
		break;

	default:
		return -1;
	}
	return 0;
}

static void op_ll_lookup(
	fuse_req_t req, fuse_ino_t parent, 
	const char * name 	
){
	struct fuse_entry_param e;
	
	if (parent != 1 || strcmp(name, op_name)!= 0) {
		fuse_reply_err(req, ENONET);
	}
	else {
		memset(&e, 0, sizeof(e));
		e.ino = 2; 
		e.attr_timeout = 1.0;
		e.entry_timeout = 1.0;
		op_stat(e.ino, & e.attr);
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


static void op_ll_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
			     off_t off, struct fuse_file_info *fi)
{
	(void) fi;

	if (ino != 1)
		fuse_reply_err(req, ENOTDIR);
	else {
		struct dirbuf b;

		memset(&b, 0, sizeof(b));
		dirbuf_add(req, &b, ".", 1);
		dirbuf_add(req, &b, "..", 1);
		dirbuf_add(req, &b, op_name, 2);
		reply_buf_limited(req, b.p, b.size, off, size);
		free(b.p);
	}
}

static void op_ll_open(fuse_req_t req, fuse_ino_t ino,
			  struct fuse_file_info *fi)
{
	if (ino != 2)
		fuse_reply_err(req, EISDIR);
	else if ((fi->flags & 3) != O_RDONLY)
		fuse_reply_err(req, EACCES);
	else
		fuse_reply_open(req, fi);
}

static void op_ll_read(fuse_req_t req, fuse_ino_t ino, size_t size,
			  off_t off, struct fuse_file_info *fi)
{
	(void) fi;
	
	if (ino == 2) {
		fuse_reply_err(req, EACCES);
	}
	
	reply_buf_limited(req, op_str, strlen(op_str), off, size);
}


static struct fuse_lowlevel_ops op_ll_oper = {
	.init       = op_ll_init,
	.lookup		= op_ll_lookup,
	// ls function 
	.getattr	= op_ll_getattr,
	.readdir	= op_ll_readdir,
	.open		= op_ll_open,
	.read		= op_ll_read,
};



static int parse_options (int argc, char *argv[], struct extfs_data *opts)
{
	int c;

	static const char *sopt = "o:hv";
	static const struct option lopt[] = {
		{ "options",	 required_argument,	NULL, 'o' },
		{ "help",	 no_argument,		NULL, 'h' },
		{ "verbose",	 no_argument,		NULL, 'v' },
		{ NULL,		 0,			NULL,  0  }
	};

#if 0
	printf("arguments;\n");
	for (c = 0; c < argc; c++) {
		printf("%d: %s\n", c, argv[c]);
	}
	printf("done\n");
#endif

	opterr = 0; /* We'll handle the errors, thank you. */

	while ((c = getopt_long(argc, argv, sopt, lopt, NULL)) != -1) {
		switch (c) {
			case 'o':
				if (opts->options)
					if (strappend(&opts->options, ","))
						return -1;
				if (strappend(&opts->options, optarg))
					return -1;
				break;
			case 'h':
				usage();
				exit(9);
			case 'v':
				/*
				 * We must handle the 'verbose' option even if
				 * we don't use it because mount(8) passes it.
				 */
				opts->debug = 1;
				break;
			default:
				debugf_main("Unknown option '%s'", argv[optind - 1]);
				return -1;
		}
	}

	if (optind < argc) {
		optarg=argv[optind++];
		if (optarg[0] != '/') {
			char fulldevice[PATH_MAX+1];
			if (!realpath(optarg, fulldevice)) {
				debugf_main("Cannot mount %s", optarg);
				free(opts->device);
				opts->device = NULL;
				return -1;
			} else
				opts->device = strdup(fulldevice);
		} else
			opts->device = strdup(optarg);
	}

	if (optind < argc) {
		opts->mnt_point = argv[optind++];
	}

	if (optind < argc) {
		debugf_main("You must specify exactly one device and exactly one mount point");
		return -1;
	}

	if (!opts->device) {
		debugf_main("No device is specified");
		return -1;
	}
	if (!opts->mnt_point) {
		debugf_main("No mountpoint is specified");
		return -1;
	}

	return 0;
}


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
	/*TODO: parse args here */
	/* TODO: create and inject user data here */
	se = fuse_session_new(&args, &op_ll_oper,
			      sizeof(op_ll_oper), NULL);

	if (se ==NULL){
		goto error_out1;
	}

	if (fuse_set_signal_handlers(se)!= 0){
		goto error_out2;
	}

	
	if (fuse_session_mount(se, opts.mountpoint)!= 0) {
		goto error_out3;
	}
	
	fuse_daemonize(opts.foreground);

	if (opts.singlethread){
		ret = fuse_session_loop(se);
	}
	else {
		ret = fuse_session_loop_mt(se, opts.clone_fd);
	}
	fuse_session_unmount(se);



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
