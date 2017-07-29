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

#ifndef CAP_FS_H
#define CAP_FS_H 1

#include <stdbool.h>
#include <stdint.h>

/*
 * ============================================================================
 * CAP FS Type definitions
 * ============================================================================
 */

/**
 * @brief enumeration of different file / capability types
 */
typedef enum {
    CAP_FS_FILETYPE_NONE,
    CAP_FS_FILETYPE_ROOT,
    CAP_FS_FILETYPE_DIRECTORY,
    CAP_FS_FILETYPE_FILE,
    CAP_FS_FILETYPE_SYMLINK,
    CAP_FS_FILETYPE_HARDLINK,
} cap_fs_filetype_t;


/**
 * @brief stub representation for capabilities
 */
typedef struct capref {
    uint64_t capaddr;
} cap_fs_capref_t;

/*
 * ============================================================================
 * CAP FS IOCTL Functions
 * ============================================================================
 */

/**
 * @brief CAP-FS IOCTL operations
 */
typedef enum {
    CAP_FS_IOCTL_OP_GET_CAP =  0,
    CAP_FS_IOCTL_OP_SET_CAP =  1,
    CAP_FS_IOCTL_OP_IDENTIFY = 2,
} cap_fs_ioctl_op_t;


/**
 * @brief CAP-FS IOCTL arguments
 */
union cap_fs_ioctl_args {
    struct {
        const char     *path;
        off_t           offset;
    } get_cap;

    struct {
        const char     *path;
        off_t           offset;
        cap_fs_capref_t cap;
    } set_cap;

    struct {
        cap_fs_capref_t cap;
    } identify;
};

/**
 * @brief CAP-FS IOCTL results
 */
union cap_fs_ioctl_res {
    struct {
        int              status;
        cap_fs_capref_t  cap;
    } get_cap;

    struct {
        int              status;
    } set_cap;

    struct {
        int               status;
        cap_fs_filetype_t type;
    } identify;
};



int cap_fs_ioctl(int fd, cap_fs_ioctl_op_t op,
                 union cap_fs_ioctl_args *args, union cap_fs_ioctl_res *res);

int cap_fs_ioctl_path(const char *path, cap_fs_ioctl_op_t op,
                      union cap_fs_ioctl_args *args, union cap_fs_ioctl_res *res);


#endif //CAP_FS_H_H
