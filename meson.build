# Copyright (c) 2017, ETH Zurich
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


# project
project(
    'cap-fs',
    'c', 'cpp',
    version: '1.0.0',
    meson_version: '>= 0.38',
    default_options: [
        'buildtype=plain',
        'c_std=c11',
        'cpp_std=c++11'
    ])

# global compiler flags
add_global_arguments(
    '-D_REENTRANT',
    '-DHAVE_CONFIG_H',
    '-O2',
    '-g',
    '-Wall',
    '-Wextra',
    '-Werror',
    language: 'c')

# include directories
include_dirs = [
    include_directories('.')
]

# source files
capfs_sources = [
    'cap-fs.c',
    'fsops/init.c',
    'fsops/destroy.c',
    'fsops/getattr.c',
    'fsops/access.c',
    'fsops/opendir.c',
    'fsops/readdir.c',
    'fsops/releasedir.c',
    'fsops/readlink.c',
    'fsops/mknod.c',
    'fsops/mkdir.c',
    'fsops/symlink.c',
    'fsops/unlink.c',
    'fsops/rmdir.c',
    'fsops/rename.c',
    'fsops/link.c',
    'fsops/chmod.c',
    'fsops/chown.c',
    'fsops/truncate.c',
    'fsops/utimens.c',
    'fsops/open.c',
    'fsops/flush.c',
    'fsops/fsync.c',
    'fsops/release.c',
    'fsops/read.c',
    'fsops/write.c',
    'fsops/statfs.c',
    'fsops/create.c',
    'fsops/ioctl.c',
    'fsops/destroy.c'
]

# dependencies
capfs_deps = [
    dependency('fuse3', version: '>= 3.1.0'),
    dependency('glib-2.0'),
    dependency('gthread-2.0'),
    dependency('protobuf')
]

# set configurations appear as -DXY=val
cfg = configuration_data()
cfg.set_quoted('PACKAGE_VERSION', meson.project_version())
cfg.set_quoted('IDMAP_DEFAULT', 'none')

configure_file(output: 'config.h',
               configuration : cfg)


# build
executable('cap-fs', capfs_sources,
           include_directories: include_dirs,
           dependencies: capfs_deps,
           c_args: ['-DFUSE_USE_VERSION=31'],
           install: true,
           install_dir: get_option('bindir'))

