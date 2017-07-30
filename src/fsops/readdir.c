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
 * @brief Return one or more directory entries (struct dirent) to the caller. 
 *
 * @param path      the path
 * @param dbuf      directory buffer
 * @param filler    filler function
 * @param offset    offset into the directory
 * @param fi        fuse file info
 * @param flags     readdir flags
 *
 * @return
 *
 * This is one of the most complex FUSE functions. It is related to, but not
 * identical to, the readdir(2) and getdents(2) system calls, and the 
 * readdir(3) library function. Because of its complexity, it is described 
 * separately below. Required for essentially any filesystem, since it's what 
 * makes ls and a whole bunch of other things work.
 */
int capfs_op_readdir(const char * path, void * dbuf, fuse_fill_dir_t filler,
                     off_t offset, struct fuse_file_info * fi,
                     enum fuse_readdir_flags flags)
{
    assert(path);
    assert(dbuf);
    assert(filler);

    (void)flags;

    LOG("path='%s'\n", path);


    cap_fs_capref_t cap;
    cap_fs_filetype_t ft;
    if (fi && fi->fh) {
        cap = ((struct capfs_handle *)fi->fh)->cap;
        ft = ((struct capfs_handle *)fi->fh)->type;
    } else {

        if (capfs_backend_resolve_path(CAPFS_ROOTCAP, path, &cap)) {
            return -ENOENT;
        }

        ft = capfs_backend_get_filetype_cap(cap);
    }
    switch(ft) {
        case CAP_FS_FILETYPE_DIRECTORY :
        case CAP_FS_FILETYPE_ROOT :
            /* no-op */
            break;
        default:
            return -ENOENT;
    }

    const char *dirent = NULL;
    while((dirent = capfs_backend_get_direntry(cap, offset)) != NULL) {
        LOG("path=%s\n", dirent);
        filler(dbuf, dirent, NULL, 0, 0);
        offset++;
    }

    return 0;
}
