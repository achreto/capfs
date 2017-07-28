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

#define _GNU_SOURCE /* don't declare *pt* functions  */

#define FUSE_USE_VERSION 31

#include "config.h"

#include <fuse.h>
#include <fuse_opt.h>
#include <fuse_lowlevel.h>

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>

#include "../cap-fs.h"
#include "fsops.h"



/**
 * @brief Return file attributes.
 * @param path  the path name
 * @param stbuf buffer to return the stat
 * @param fi    fuse file information
 * @return
 *
 * The "stat" structure is described in detail in the stat(2) manual page. For
 * the given pathname, this should fill in the elements of the "stat" structure.
 * If a field is meaningless or semi-meaningless (e.g., st_ino) then it should
 * be set to 0 or given a "reasonable" value. This call is pretty much required
 * for a usable filesystem.
 */
int capfs_op_getattr(const char * path, struct stat * stbuf,
                     struct fuse_file_info * fi)
{
    LOG("path='%s', fh=%p\n", path, (fi ? (struct cap_fs_handle *)fi->fh : NULL));

    enum cap_fs_filetpe t = CAP_FS_FILETYPE_NONE;
    size_t sz = 0;
    if (fi && fi->fh) {
        struct cap_fs_handle *h = (struct cap_fs_handle *)fi->fh;
        t = h->type;
        sz = h->size;
    } else {
        LOG("%s\n", "cap_fs_debug_get_file_type\n");
        t = cap_fs_debug_get_file_type(path);

        LOG("%s\n", "cap_fs_debug_get_filesize\n");
        sz = cap_fs_debug_get_filesize(path);
    }

    switch (t) {
        case CAP_FS_FILETYPE_ROOT:
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            break;
        case CAP_FS_FILETYPE_DIRECTORY:
            stbuf->st_mode = S_IFDIR | 0755;
            break;
        case CAP_FS_FILETYPE_FILE:
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_size = sz;
            stbuf->st_nlink = 1;
            break;
        case CAP_FS_FILETYPE_SYMLINK:
        case CAP_FS_FILETYPE_HARDLINK:
            LOG("%s\n", "links are not supported\n");
            return -ENOENT;
        default:
            return -ENOENT;
    }

    return 0;
}
