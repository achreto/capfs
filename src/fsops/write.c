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
 * @brief Write size bytes to the given file into the buffer buf, beginning
 *        offset bytes into the file.
 *
 * @param path      the file path
 * @param wbuf      buffer to return the data
 * @param size      bytes to read
 * @param offset    offset to read from
 * @param fi        FUSE file info
 *
 * @return Returns the number of bytes transferred
 */
int capfs_op_write(const char * path, const char * wbuf, size_t size,
                   off_t offset, struct fuse_file_info * fi)
{
    LOG("path='%s'\n", path);

    /* check whether the path is in fact a file */
    if (cap_fs_debug_get_file_type(path) != CAP_FS_FILETYPE_FILE) {
        return -EACCES;
    }

    capref_t cap;
    /* obtain the cap handle form the filehandle */
    if (fi && fi->fh) {
        struct cap_fs_handle * fh = (struct cap_fs_handle *)fi->fh;
        cap = fh->cap;
    } else {
        cap = cap_fs_debug_get_caphandle(path);
    }


    LOG("invoke store to cap (%lx, %lu, %p, %lu)\n", cap.capaddr, offset, 
        wbuf, size);

    if (size < INT_MAX) {
        return (int)size;
    } else {
        return INT_MAX;
    }
}