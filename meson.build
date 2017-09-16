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
    'capfs',
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
    include_directories('include'),
    include_directories('src/include')
]

# source files
capfs_sources = [
    'src/capfs.c',
    'src/main.c',
    'src/filesystem.c',
    'src/fsops/init.c',
    'src/fsops/destroy.c',
    'src/fsops/getattr.c',
    'src/fsops/access.c',
    'src/fsops/opendir.c',
    'src/fsops/readdir.c',
    'src/fsops/releasedir.c',
    'src/fsops/readlink.c',
    'src/fsops/mknod.c',
    'src/fsops/mkdir.c',
    'src/fsops/symlink.c',
    'src/fsops/unlink.c',
    'src/fsops/rmdir.c',
    'src/fsops/rename.c',
    'src/fsops/link.c',
    'src/fsops/chmod.c',
    'src/fsops/chown.c',
    'src/fsops/truncate.c',
    'src/fsops/utimens.c',
    'src/fsops/open.c',
    'src/fsops/flush.c',
    'src/fsops/fsync.c',
    'src/fsops/release.c',
    'src/fsops/read.c',
    'src/fsops/write.c',
    'src/fsops/statfs.c',
    'src/fsops/create.c',
    'src/fsops/ioctl.c',
    'src/fsops/destroy.c'
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
executable('capfs', capfs_sources  + ['src/backends/files.c'],
           include_directories: include_dirs,
           dependencies: capfs_deps,
           c_args: ['-DFUSE_USE_VERSION=31'],
           install: true,
           install_dir: get_option('bindir'))

# build
executable('capfs-dummy', capfs_sources + ['src/backends/dummy.c'],
           include_directories: include_dirs,
           dependencies: capfs_deps,
           c_args: ['-DFUSE_USE_VERSION=31'],
           install: true,
           install_dir: get_option('bindir'))
