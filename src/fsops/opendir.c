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

#include <capfs_internal.h>

#include <assert.h>
#include <errno.h>


/**
 * @brief Open a directory for reading.
 *
 * @param path  the path to be opened
 * @param fi    the fuse file info
 *
 * @return
 */
int capfs_op_opendir(const char * path, struct fuse_file_info * fi)
{
    LOG("path='%s'\n", path);

    assert(path);

    cap_fs_capref_t cap;
    if (capfs_backend_resolve_path(CAPFS_ROOTCAP, path, &cap)) {
        LOG("path '%s' not found\n", path);
        return -EINVAL;
    }


    cap_fs_filetype_t ft = capfs_backend_get_filetype_cap(cap);
    switch(ft) {
        case CAP_FS_FILETYPE_ROOT:
        case CAP_FS_FILETYPE_DIRECTORY:
            break;
        default:
            LOG("path '%s' is not a directory\n", path);
            return -EINVAL;
    }

    struct capfs_handle *h = cap_fs_handle_alloc();
    if (!h) {
        return -ENOMEM;
    }

    h->cap = cap;
    h->type = ft;
    capfs_backend_get_capsize(cap, &h->size);
    h->perms = capfs_backend_get_perms(cap);

    fi->fh = (uint64_t)h;

    return 0;
}
