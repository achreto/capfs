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

#ifndef CAP_FS_HANDLE_H
#define CAP_FS_HANDLE_H 1

/* capfs public include */
#include <capfs.h>
#include <stdlib.h>

/**
 * @brief this stores/caches addition information for file handles
 *
 * this structure is allcated and populated on open/opendir/create
 */
struct capfs_handle {
    cap_fs_filetype_t type;
    size_t            size;
    cap_fs_capref_t   cap;
    int               perms;
};

/**
 * @brief allocates a new cap_fs_handle struct
 */
static inline struct capfs_handle *cap_fs_handle_alloc() {
    return calloc(1, sizeof(struct capfs_handle));
}

/**
 * @brief frees a allocated cap_fs_handle struct
 */
static inline void  cap_fs_handle_free(struct capfs_handle *h) {
    if (h) {
        free(h);
    }
}

#endif //CAP_FS_HANDLE_H_H
