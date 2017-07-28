/*
 * Copyright (c) 2017, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CAP_FS_FSOPS_H
#define CAP_FS_FSOPS_H 1

#define FUSE_USE_VERSION 31

#include "config.h"

#include <fuse.h>


void *capfs_op_init(struct fuse_conn_info * conn,
                     struct fuse_config * cfg);

void capfs_op_destroy(void* private_data);

int capfs_op_getattr(const char * path, struct stat * stbuf,
                     struct fuse_file_info * fi);

int capfs_op_access(const char * path, int mask);

int capfs_op_opendir(const char * path, struct fuse_file_info * fi);

int capfs_op_readdir(const char * path, void * dbuf, fuse_fill_dir_t filler,
                     off_t offset, struct fuse_file_info * fi,
                     enum fuse_readdir_flags flags);

int capfs_op_releasedir(const char * path, struct fuse_file_info * fi);

int capfs_op_mknod(const char * path, mode_t mode, dev_t rdev);

int capfs_op_mkdir(const char * path, mode_t mode);

int capfs_op_symlink(const char * from, const char * to);

int capfs_op_readlink(const char * path, char * linkbuf, size_t size);

int capfs_op_unlink(const char * path);

int capfs_op_rmdir(const char * path);

int capfs_op_rename(const char * from, const char * to, unsigned int flags);

int capfs_op_link(const char * from, const char * to);

int capfs_op_open(const char * path, struct fuse_file_info * fi);

int capfs_op_chmod(const char * path, mode_t mode,
                   struct fuse_file_info * fi);

int capfs_op_chown(const char * path, uid_t uid, gid_t gid,
                   struct fuse_file_info * fi);

int capfs_op_truncate(const char * path, off_t size,
                      struct fuse_file_info * fi);

int capfs_op_utimens(const char * path, const struct timespec tv[2],
                     struct fuse_file_info * fi);

int capfs_op_flush(const char * path, struct fuse_file_info * fi);

int capfs_op_fsync(const char * path, int isdatasync,
                   struct fuse_file_info * fi);

int capfs_op_release(const char * path, struct fuse_file_info * fi);

int capfs_op_read(const char * path, char * rbuf, size_t size, off_t offset,
                  struct fuse_file_info * fi);

int capfs_op_write(const char * path, const char * wbuf, size_t size,
                   off_t offset, struct fuse_file_info * fi);

int capfs_op_open(const char * path, struct fuse_file_info * fi);

int capfs_op_statfs(const char * path, struct statvfs * buf);

int capfs_op_create(const char * path, mode_t mode,
                    struct fuse_file_info * fi);

int capfs_op_ioctl(const char *path, int cmd, void *arg,
                   struct fuse_file_info *fi, unsigned int flags, void *data);

int capfs_op_poll(const char* path, struct fuse_file_info* fi, struct
                  fuse_pollhandle* ph, unsigned* reventsp);

#endif //CAP_FS_FSOPS_H
