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

#ifndef CAP_FS_INTERNAL_H
#define CAP_FS_INTERNAL_H 1

#define _GNU_SOURCE /* don't declare *pt* functions  */


/* capfs generated configuration file config */
#include "config.h"

/* libfuse includes  */
#include <fuse.h>
#include <fuse_opt.h>
#include <fuse_lowlevel.h>

/* capfs public include */
#include <capfs.h>

/* capfs internal includes */
#include <capfs_debug.h>
#include <capfs_handle.h>
#include <capfs_backend.h>
#include <capfs_fsops.h>


#include <stdbool.h>

/**
 * @brief this struct stores the options for the cap-fs
 */
struct cap_fs {
    bool initialized;
};

/**
 * @brief global cap state
 */
extern struct cap_fs capfs_g_st;




#endif //CAP_FS_INTERNAL_H_H
