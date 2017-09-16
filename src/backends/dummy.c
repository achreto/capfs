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


/**
 * the root capability
 */
capfs_capref_t capfs_root_capability;


/*
 * ============================================================================
 * some dummy capability store
 * ============================================================================
 */


struct capability {
    const char *name;
    int perms;
    const char *payload;
    capfs_filetype_t type;
};


static struct capability capstore[] = {
    {
            .name = "/",
            .perms = 0755,
            .payload = "sdfsdfsfsdasd",
            .type = CAP_FS_FILETYPE_ROOT
    },
    {
            .name = "/file-01.dat",
            .perms = 0644,
            .payload = "FILE 1 CONTENT!\n",
            .type = CAP_FS_FILETYPE_FILE
    },
    {
            .name = "/file-02.dat",
            .perms = 0644,
            .payload = "FILE 2 CONTENT!\n",
            .type = CAP_FS_FILETYPE_FILE
    },
    {
            .name = "/folder",
            .perms = 0755,
            .payload = NULL,
            .type = CAP_FS_FILETYPE_DIRECTORY},
    {
            .name = "/folder/file-03.dat",
            .perms = 0644,
            .payload = "FILE 3 CONTENT!\n",
            .type = CAP_FS_FILETYPE_FILE
    },
};

#define NUMCAPS (sizeof(capstore) / sizeof(struct capability))

static struct capability *rootdir[4] = {
    capstore+1,
    capstore+2,
    capstore+3,
    NULL
};

static struct capability *folderdir[2] = {
    capstore+4,
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

    capfs_root_capability.capaddr = 0;

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

#define MKCAP(caddr) (capfs_capref_t){.capaddr = (caddr)}

static capfs_capref_t root_cap = MKCAP(0);

capfs_capref_t capfs_backend_get_rootcap(void)
{
    return root_cap;
}

const char *capfs_backend_get_direntry(capfs_capref_t cap,
                                       off_t offset)
{
    LOG("cap=" PRIxCAP "offset=%li\n", PRI_CAP(cap), offset);

    if (!(cap.capaddr < NUMCAPS)) {
        return NULL;
    }

    if (offset < 0) {
        return NULL;
    }

    struct capability *c = &capstore[cap.capaddr];
    struct capability **dir = NULL;
    off_t maxoffset = 2;
    switch(c->type) {
        case CAP_FS_FILETYPE_ROOT :
            dir = rootdir;
            maxoffset += 3;
            break;
        case CAP_FS_FILETYPE_DIRECTORY :
            dir = folderdir;
            maxoffset += 1;
            break;
        default:
            return NULL;
    }

    if (offset == 0) {
        LOG("cap=" PRIxCAP "offset=%li => ..\n", PRI_CAP(cap), offset);
        return "..";
    }

    if (offset == 1) {
        LOG("cap=" PRIxCAP "offset=%li => ..\n", PRI_CAP(cap), offset);
        return ".";
    }

    if (offset < maxoffset) {
        LOG("cap=" PRIxCAP "offset=%li => %s\n", PRI_CAP(cap), offset,
            dir[offset - 2]->name + 1);
        return dir[offset - 2]->name + 1;
    }

    return NULL;
}


int capfs_backend_resolve_path(capfs_capref_t root,
                               const char *path,
                               capfs_capref_t *retcap)
{
    LOG("root=" PRIxCAP " path=%s\n", PRI_CAP(root), path);

    if (root.capaddr == root_cap.capaddr) {
        for (size_t i = 0; i < NUMCAPS; i++) {
            LOG("strcmp='%s' '%s'\n" , path, capstore[i].name);

            if (!strcmp(capstore[i].name, path)) {
                *retcap =  MKCAP(i);

                LOG("path '%s' found at cap " PRIxCAP "\n", path, PRI_CAP(*retcap));

                return 0;
            }
        }

        LOG("INVALID: root=" PRIxCAP " path=%s\n", PRI_CAP(root), path);

        return -ENOENT;
    } else {
        LOG("resolve with non-root cap not supported. path='%s'\n", path);
        return -ENOTSUP;
    }
}


capfs_filetype_t capfs_backend_get_filetype_cap(capfs_capref_t cap)
{
    LOG("cap=" PRIxCAP "\n", PRI_CAP(cap));

    if (cap.capaddr < NUMCAPS) {
        return capstore[cap.capaddr].type;
    }
    return CAP_FS_FILETYPE_NONE;
}

int capfs_backend_get_capsize(capfs_capref_t cap, size_t *retsize)
{
    LOG("cap=" PRIxCAP "\n", PRI_CAP(cap));

    if (cap.capaddr < NUMCAPS) {
        if (retsize && (capstore[cap.capaddr].payload)) {
            *retsize = strlen(capstore[cap.capaddr].payload);
        }
        return 0;
    }
    return -EACCES;
}

int capfs_backend_get_perms(capfs_capref_t cap)
{
    LOG("cap=" PRIxCAP "\n", PRI_CAP(cap));

    if (cap.capaddr < NUMCAPS) {
        return capstore[cap.capaddr].perms;
    }
    return 0;
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
int capfs_backend_get_cap(capfs_capref_t cap,
                          off_t offset, capfs_capref_t *retcap)
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
int capfs_backend_put_cap(capfs_capref_t cap,
                          off_t offset, capfs_capref_t newcap)
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


long capfs_backend_read(capfs_capref_t cap, off_t offset,
                       char *rbuf, size_t bytes)
{
    LOG("cap=" PRIxCAP ", offset=%li, rbuf=%p, size=%zu\n", PRI_CAP(cap), offset,
        rbuf, bytes);

    if (!(cap.capaddr < NUMCAPS)) {
        return 0;
    }

    if (offset < 0) {
        offset = 0;
    }




    struct capability *c = &capstore[cap.capaddr];
    if (c->type != CAP_FS_FILETYPE_FILE) {
        return -EACCES;
    }

    if ((size_t) offset > strlen(c->payload)) {
        return 0;
    }

    LOG("copying cap:" PRIxCAP " -> %p \n", PRI_CAP(cap), rbuf);

    const char *p = c->payload;

    size_t i = 0;
    while(*p) {
        if (i == bytes) {
            break;
        }

        rbuf[i] = *p;
        p++;
        i++;
    }

    return i;
}

long capfs_backend_write(capfs_capref_t cap, off_t offset,
                        const char *wbuf, size_t bytes)
{
    LOG("cap=" PRIxCAP ", offset=%li, wbuf=%p, size=%zu\n", PRI_CAP(cap), offset,
        wbuf, bytes);

    (void)cap;

    return 0;
}

int capfs_backend_zero(capfs_capref_t cap)
{
    (void)cap;

    return -1;
}


/*
 * ===========================================================================
 * Capability Meta Information
 * ===========================================================================
 */

/**
 * @brief obtains the permissions of the capability
 *
 * @param cap   capability to obtain the permissions for
 *
 * @return capability permissions
 */
capfs_capperms_t  capfs_backend_cap_get_perms(capfs_capref_t cap)
{
    (void)cap;

    return 0;
}

/**
 * @brief obtains the size of the capability
 *
 * @param cap   the capablity to obtain the size from
 *
 * @return size of the capabilty
 */
uint64_t capfs_backend_cap_get_size(capfs_capref_t cap)
{
    (void)cap;

    return 0;
}