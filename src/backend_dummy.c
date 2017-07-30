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

#include <capfs_internal.h>

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



static const char *debug_rootdir[6] = {
        ".",
        "..",
        "file-01.dat",
        "file-02.dat",
        "folder",
        NULL
};

static const char *debug_folderdir[4] = {
        ".",
        "..",
        "file-03.dat",
        NULL
};


/*
 * ============================================================================
 * Initialization and destroy functions
 * ============================================================================
 */


/**
 * @brief initializes the backend
 *
 * @param conn  FUSE connection information
 * @param cfg   FUSE configuration
 *
 * @return
 */
void *capfs_backend_init(struct fuse_conn_info * conn,
                         struct fuse_config * cfg)
{
    LOGA("Initializing backend is a no-op.\n");

    (void)conn;
    (void)cfg;

    return NULL;
}

/**
 * @brief destroys the backend
 * @return
 */
int capfs_backend_destroy(void *st)
{
    LOGA("Destroying backend is a no-op\n");

    (void)st;

    return 0;
}

/*
 * ============================================================================
 *
 * ============================================================================
 */


cap_fs_capref_t capfs_backend_get_rootcap(void)
{
    return (cap_fs_capref_t){.capaddr = 0xcafebabe};
}

const char *capfs_backend_get_direntry(cap_fs_capref_t cap,
                                       off_t offset)
{
    (void)cap;
    if (offset == 0) {
        return "..";
    }

    if (offset == 1) {
        return ".";
    }

    return 0;
}


int capfs_backend_resolve_path(cap_fs_capref_t root,
                               const char *path,
                               cap_fs_capref_t *retcap)
{
    (void)root;
    (void)path;
    (void)retcap;

    NYI();
}

int capfs_backend_get_caphandle(const char *path, cap_fs_capref_t *retcap)
{
    (void)path;
    (void)retcap;

    NYI();
}

cap_fs_filetype_t capfs_backend_get_filetype_cap(cap_fs_capref_t cap)
{
    (void)cap;
    NYI();
}

int capfs_backend_get_capsize(cap_fs_capref_t cap, size_t *retsize)
{
    (void)cap;
    (void)retsize;
    NYI();
}

int capfs_backend_get_perms(cap_fs_capref_t cap)
{

    (void)cap;
    NYI();
}


/*
 * ============================================================================
 * Load / Store capabilities
 * ============================================================================
 */


/**
 * @brief loads a capability at offset into another capability
 *
 * @param cap       the capability root
 * @param offset    offset into the capability root
 * @param retcap    returned capability, if valid
 *
 * @return
 */
int capfs_backend_get_cap(cap_fs_capref_t cap,
                          off_t offset, cap_fs_capref_t *retcap)
{
    LOG("cap=" PRIxCAP ", offset=%li\n", PRI_CAP(cap), offset);

    (void)cap;
    (void)retcap;
    return 0;
}

/**
 * @brief stores a capability at offset into another capability
 *
 * @param cap       the root capability
 * @param offset    offset into the root capability
 * @param newcap    the new capability to be stored
 *
 * @return
 */
int capfs_backend_put_cap(cap_fs_capref_t cap,
                          off_t offset, cap_fs_capref_t newcap)
{
    LOG("cap=" PRIxCAP ", offset=%li, newcap=" PRIxCAP "\n", PRI_CAP(cap),
        offset, PRI_CAP(newcap));

    (void)cap;
    (void)newcap;

    return 0;
}


/*
 * ============================================================================
 * Read / Write Data
 * ============================================================================
 */


int capfs_backend_read(cap_fs_capref_t cap, off_t offset,
                       char *rbuf, size_t bytes)
{
    LOG("cap=" PRIxCAP ", offset=%li, rbuf=%p, size=%zu\n", PRI_CAP(cap), offset,
        rbuf, bytes);

    (void)cap;

    return 0;
}

int capfs_backend_write(cap_fs_capref_t cap, off_t offset,
                        const char *wbuf, size_t bytes)
{
    LOG("cap=" PRIxCAP ", offset=%li, wbuf=%p, size=%zu\n", PRI_CAP(cap), offset,
        wbuf, bytes);

    (void)cap;

    return 0;
}




const char ** cap_fs_debug_get_dirents(const char *path) {
    if (strcmp(path, "/") == 0) {
        return debug_rootdir;
    } else if (strcmp(path+1, "folder/") == 0) {
        return debug_folderdir;
    } else {
        return NULL;
    }
}

cap_fs_filetype_t cap_fs_debug_get_file_type(const char *path)
{
    if (strcmp(path, "/") == 0) {
        return CAP_FS_FILETYPE_ROOT;
    } else if (strcmp(path+1, "file-01.dat") == 0) {
        return CAP_FS_FILETYPE_FILE;
    } else if (strcmp(path+1, "file-02.dat") == 0) {
        return CAP_FS_FILETYPE_FILE;
    } else if (strcmp(path+1, "folder") == 0) {
        return CAP_FS_FILETYPE_DIRECTORY;
    } else if (strcmp(path+1, "folder/file-03.dat") == 0) {
        return CAP_FS_FILETYPE_FILE;
    } else {
        return CAP_FS_FILETYPE_NONE;
    }
}
