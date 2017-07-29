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
 * @brief   Support the ioctl(2) system call. 
 *
 * @param path  the path to do ioctl on
 * @param cmd   command to execute
 * @param arg   argument for the command
 * @param fi    fuse file info
 * @param flags flags to pass
 * @param data  data buffer to be returned
 *
 * @return
 *
 * As such, almost everything is up to the filesystem. On a 64-bit machine, 
 * FUSE_IOCTL_COMPAT will be set for 32-bit ioctls. The size and direction of 
 * data is determined by _IOC_*() decoding of cmd. For _IOC_NONE, data will be 
 * NULL; for _IOC_WRITE data is being written by the user; for _IOC_READ it is 
 * being read, and if both are set the data is bidirectional. In all non-NULL 
 * cases, the area is _IOC_SIZE(cmd) bytes in size.
 */
int capfs_op_ioctl(const char *path, int cmd, void *arg,
                   struct fuse_file_info *fi, unsigned int flags, void *data)
{
    LOG("path='%s', cmd=%i\n", path, cmd);

    (void) arg;
    (void) fi;

    if (flags & FUSE_IOCTL_COMPAT)
        return -ENOSYS;

    switch(cap_fs_debug_get_file_type(path)) {
        case CAP_FS_FILETYPE_NONE:
            return -EINVAL;
        case CAP_FS_FILETYPE_ROOT:
            break;
        case CAP_FS_FILETYPE_DIRECTORY:
            break;
        case CAP_FS_FILETYPE_FILE:
            break;
        case CAP_FS_FILETYPE_SYMLINK:
            /* currently no support for links */
            return -EINVAL;
        case CAP_FS_FILETYPE_HARDLINK:
            /* currently no support for links */
            return -EINVAL;
        default:
            return -EINVAL;
    }

    switch (cmd) {
        case CAP_FS_IOCTL_GET_CAP:
            *(capref_t *)data = cap_fs_debug_get_caphandle(path);
            return 0;
    }

    return -EINVAL;
}
