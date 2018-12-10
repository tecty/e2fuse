project(
    'fusell-ext4', 'c',
    version: '0.0.1', 
    license:'MIT',
    default_options : ['c_std=gnu11']
)


fuse3_ll_dep = dependency('fuse3')
ext2fs_dep = dependency('ext2fs')

op_lib = shared_library(
    'ops',['op_getattr.c', 'fuse_util.c','op_init.c'],
    dependencies: [fuse3_ll_dep,ext2fs_dep],
)

executable(
    'hello', 'main.c', 
    dependencies: fuse3_ll_dep,
    link_with: op_lib
)