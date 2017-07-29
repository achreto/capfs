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
 * @brief Open a file.
 * @param path  path to the file
 * @param fi    fuse file info
 * @return
 *
 * If you aren't using file handles, this function should just check for 
 * existence and permissions and return either success or an error code. If 
 * you use file handles, you should also allocate any necessary structures and 
 * set fi->fh. In addition, fi has some other fields that an advanced 
 * filesystem might find useful; see the structure definition in fuse_common.h 
 * for very brief commentary.
 */
int capfs_op_open(const char * path, struct fuse_file_info * fi)
{
    LOG("path='%s'\n", path);

    enum cap_fs_filetpe ft = cap_fs_debug_get_file_type(path);
    if (ft != CAP_FS_FILETYPE_FILE) {
        return -EINVAL;
    }

    struct cap_fs_handle *h = cap_fs_handle_alloc();
    if (!h) {
        return -ENOMEM;
    }

    h->cap = cap_fs_debug_get_caphandle(path);
    h->type = ft;
    h->size = cap_fs_debug_get_file_type(path);

    fi->fh = (uint64_t)h;

    return 0;
}
