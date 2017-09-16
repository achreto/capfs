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
 *
 */

#define CAPFS_ROOT_MAGIC 0x0cabf5cabf5cabf5cab

/**
 * @brief the capfs root data structure
 */
struct capfs_root
{
    uint64_t magic;         ///< magic value to
    size_t   capacity;      ///< the capacity of the region in bytes
    size_t   freeX;          ///< number of not allocated bytes

    capfs_capref_t free;    ///<

    uint32_t checksum;      ///< checksum for the header
};




struct capfs_region {
    uint64_t magic;
    uint32_t checksum;
};

struct capfs_directory
{
    const char name[32];
    int perms;
    capfs_capref_t entries; ///< capability to all the directory entries
};


struct capfs_file
{
    const char  name[32];
    int perms;                  ///< permissions for this file
    size_t bytes;               ///< number of used bytes
    capfs_capref_t content;     ///< capability to the file content
};

struct capfs_free
{

};