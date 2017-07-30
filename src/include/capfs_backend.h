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

#ifndef CAP_FS_BACKEND_H
#define CAP_FS_BACKEND_H 1

#include <capfs_internal.h>


void *capfs_backend_init(struct fuse_conn_info * conn,
                         struct fuse_config * cfg);
int capfs_backend_destroy(void *st);

cap_fs_capref_t capfs_backend_get_rootcap(void);

#define CAPFS_ROOTCAP capfs_backend_get_rootcap()

#define CAPFS_NULLCAP 0

int capfs_backend_resolve_path(cap_fs_capref_t root,
                               const char *path,
                               cap_fs_capref_t *retcap);

const char *capfs_backend_get_direntry(cap_fs_capref_t cap,
                                       off_t offset);

cap_fs_filetype_t capfs_backend_get_filetype_cap(cap_fs_capref_t cap);

int capfs_backend_get_capsize(cap_fs_capref_t cap, size_t *retsize);

int capfs_backend_get_perms(cap_fs_capref_t cap);

int capfs_backend_get_cap(cap_fs_capref_t cap,
                          off_t offset, cap_fs_capref_t *retcap);
int capfs_backend_put_cap(cap_fs_capref_t cap,
                          off_t offset, cap_fs_capref_t newcap);

int capfs_backend_read(cap_fs_capref_t cap, off_t offset,
                                char *rbuf, size_t bytes);
int capfs_backend_write(cap_fs_capref_t cap, off_t offset,
                               const char *wbuf, size_t bytes);


#endif //CAP_FS_BACKEND_H_H
