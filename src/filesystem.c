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

#define CAPFS_FILE_NAME_MAX 127

#define CAPFS_FS_FILE_ROOT_HEADER "CAP-FS "
#define CAPFS_FS_FILE_ROOT_VERSION1 0x0100

#define CAPFS_FS_FILE_MAGIC 0x00cafebabe00UL


struct capfs_file
{
    uint64_t         magic;
    char             name [CAPFS_FILE_NAME_MAX + 1];
    capfs_filetype_t type;
    capfs_capref_t   content;
    uint64_t         size;
    union {
        struct {
            uint16_t version;
            const char header[8];
            capfs_capref_t heap;
        } root;

        struct {
            int      permission;
        } file;
        struct {
            int      permission;
        } directory;
    };
};

static struct capfs_file g_fs_root;


/**
 * @brief formats the space pointed to by capability for use as a file system
 *
 * @param root  root capability of the file system
 *
 * @return ERR_OK on sucecss, error value on failure
 */
int capfs_filesystem_format(capfs_capref_t root)
{
    int err;

    LOGA("Formatting file system...\n");

    err = capfs_backend_zero(root);
    if (err) {
        LOG("Zeroing the capability failed with errno=%i...\n", err);
        return err;
    }

    struct capfs_file fs_root;

    LOGA("Initializing file system root block..\n");

    fs_root.size = capfs_backend_cap_get_size(root);
    fs_root.magic = CAPFS_FS_FILE_MAGIC;
    fs_root.name[0] = '/';
    fs_root.name[1] = 0;

    memcpy((void *)&fs_root.root.header, CAPFS_FS_FILE_ROOT_HEADER, 8);

    if (capfs_backend_write(root, 0, (void *)&fs_root, sizeof(fs_root)) !=
            sizeof(fs_root)) {
        return -1;
    }

    return 0;
}

/**
 * @brief initializes the file system
 *
 * @param root capability to the root of the file system
 *
 * @return ERR_OK on success, error value on failure
 */
int capfs_filesystem_init(capfs_capref_t root)
{

    if (capfs_filesystem_format(root)) {
        PANIC(0, "%s", "ssdfsdf\n");
    }

    LOGA("initializing filesystem\n");
    if (capfs_backend_read(root, 0, (void *)&g_fs_root, sizeof(g_fs_root)) !=
            sizeof(g_fs_root)) {
        LOGA("ERROR - Root capability was invalid.\n");
        return -EINVAL;
    }

    if (g_fs_root.magic != CAPFS_FS_FILE_MAGIC) {
        LOG("ERROR - Magic Number not found %" PRIx64 " expected %" PRIx64 "\n",
            g_fs_root.magic, CAPFS_FS_FILE_MAGIC );
        return -EINVAL;
    }

    if (strncmp(g_fs_root.root.header, CAPFS_FS_FILE_ROOT_HEADER, 8)) {
        LOG("Header not match: '%s' expected '%s'\n", g_fs_root.root.header,
            CAPFS_FS_FILE_ROOT_HEADER);
        return -1;
    }

    if (strcmp(g_fs_root.name, "/")) {
        LOG("root name was invalid. expected '/' was '%s'\n", g_fs_root.name);
        return -1;
    }

    switch(g_fs_root.root.version) {
        case CAPFS_FS_FILE_ROOT_VERSION1:
            break;
        default:
            LOG("Unsupported version: 0x%x\n", g_fs_root.root.version);
            break;
    }

    /* todo: initialize the heap */

    return 0;
}

#define CAPFS_FS_SEPARATOR '/'

static int capfs_filessystem_resolve_one(capfs_capref_t root,
                                         const char * path,
                                         capfs_capref_t * ret_cap)
{
    LOG("path='%s'\n", path);


    struct capfs_file f;

    if (capfs_backend_read(root, 0, (void *)&f, sizeof(f)) != sizeof(f)) {
        return -1;
    }

    if (f.magic != CAPFS_FS_FILE_MAGIC) {

        return -EINVAL;
    }

    if (f.type == CAP_FS_FILETYPE_ROOT && (path[0] == CAPFS_FS_SEPARATOR) && (path[1] == 0)) {
        *ret_cap = root;
        return 0;
    }

    switch(f.type) {
        case CAP_FS_FILETYPE_DIRECTORY :
        case CAP_FS_FILETYPE_ROOT :
            break;
        default:
            return -EINVAL;
    }

    capfs_capref_t contentcap;
    if (capfs_filesystem_get_content_cap(root, &contentcap)) {
        return -1;
    }

    uint64_t current = 0;
    while(current != f.size) {
        struct capfs_file entry;
        capfs_backend_read(contentcap, current, (void *)&entry, sizeof(entry));
        current++;
    }


    return 0;
}



/**
 * @brief resolves a path relative to a given root.
 *
 * @param root      the root capability to start resolving from
 * @param path      path to resolve
 * @param ret_cap   returns the cap to the file of the path
 *
 * @return ERR_OK on success or error value on failure
 */
int capfs_filesystem_resolve_path(capfs_capref_t root,
                                  const char * path,
                                  capfs_capref_t * ret_cap)
{
    if (path == NULL) {
        return -1;
    }

    if (path[0] == CAPFS_FS_SEPARATOR && path[1] == 0) {
        return capfs_filessystem_resolve_one(root, path, ret_cap);
    }


    char *mypath = strdup(path);
    if (!mypath) {
        return -ENOMEM;
    }

    size_t pathlen = strlen(mypath);
    char *p = mypath;
    char *current = mypath;
    LOG("mypath+pathlen=%c\n", *(mypath+pathlen));
    while(*p) {
        if (*p == '/') {
            if (p == current) {
                current++;
                p++;
            } else {
                *p = 0;
                capfs_filessystem_resolve_one(root, current, &root);
                p++;
                current = p;
            }
        } else {
            p++;
        }
    }

    if (current != p) {
        capfs_filessystem_resolve_one(root, current, &root);
    }


    if (mypath) {
        free(mypath);
    }

    if (ret_cap) {
        *ret_cap = root;
    }

    return 0;
}


char *capfs_filesystem_get_direntry(capfs_capref_t dircap, off_t offset)
{
    (void)dircap;
    (void)offset;
    return NULL;

}

/**
 * @brief obtains the meta data associated to the file
 *
 * @param file  the capability of the file
 * @param md    filled in meta data
 *
 * @return ERR_OK on success, error value on failure
 */
int capfs_filesystem_get_metadata(capfs_capref_t file,
                                  struct capfs_filesystem_meta_data *md)
{
    (void)file;
    (void)md;
    return -1;
}

/**
 * @brief
 * @param cap_file
 * @param cap_content
 * @return
 */
int capfs_filesystem_get_content_cap(capfs_capref_t cap_file,
                                     capfs_capref_t *cap_content)
{
    (void)cap_file;
    (void)cap_content;
    return -1;
}