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


/*
 * DEBUG Functions for testing
 */

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

cap_fs_capref_t cap_fs_debug_get_caphandle(const char *path)
{
    (void)path;
    return (cap_fs_capref_t){.capaddr = 0xcafebabe};
}

size_t cap_fs_debug_get_filesize(const char *path)
{
    (void)path;
    return 42;
}