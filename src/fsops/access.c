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

#include <errno.h>

/**
 * @brief This is the same as the access(2) system call.
 *
 * @param path  path to check
 * @param mask
 *
 * @return -ENOENT if path does not exist
 *         -EACCESS if the requested permission isn't available,
 *
 * Note that it can be called on files, directories, or any other object that
 * appears in the filesystem. This call is not required but is highly recommended.
 */
int capfs_op_access(const char * path, int mask)
{
    LOG("path='%s, mask=0x%x'\n", path, mask);

    capfs_capref_t cap;
    if (capfs_filesystem_resolve_path(CAPFS_ROOTCAP, path, &cap)) {
        return -ENOENT;
    }

    struct capfs_filesystem_meta_data md;
    if (capfs_filesystem_get_metadata(cap, &md)) {
        return -ENOENT;
    }

    switch(md.type) {
        case CAP_FS_FILETYPE_NONE :
            return -ENOENT;
        default:
            return (mask & md.type);
    }
}
