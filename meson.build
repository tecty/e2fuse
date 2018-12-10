project('fusell-ext4', 'c', version: '0.0.1', license:'MIT')


fuse3_ll_dep = dependency('fuse3')


op_lib = shared_library(
    'op_lib',['op_getattr.c', 'fuse_util.c']
)

executable(
    'hello', 'main.c', 
    dependencies: fuse3_ll_dep,
    link_with: op_lib
)