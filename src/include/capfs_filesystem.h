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

#ifndef CAPFS_FILESYSTEM_H_
#define CAPFS_FILESYSTEM_H_ 1


#include <inttypes.h>
#include <assert.h>

#include <capfs.h>

struct capfs_filesystem_meta_data
{
    int perms;                  ///< permissions for this file
    size_t bytes;               ///< number of used bytes
    capfs_filetype_t type;      ///< type of the file
};


/**
 * @brief initializes the file system
 *
 * @param root capability to the root of the file system
 *
 * @return ERR_OK on success, error value on failure
 */
int capfs_filesystem_init(capfs_capref_t root);

/**
 * @brief formats the space pointed to by capability for use as a file system
 *
 * @param root  root capability of the file system
 *
 * @return ERR_OK on sucecss, error value on failure
 */
int capfs_filesystem_format(capfs_capref_t root);


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
                                  capfs_capref_t * ret_cap);


/**
 * @brief obtains the meta data associated to the file
 *
 * @param file  the capability of the file
 * @param md    filled in meta data
 *
 * @return ERR_OK on success, error value on failure
 */
int capfs_filesystem_get_metadata(capfs_capref_t file,
                                  struct capfs_filesystem_meta_data *md);


/**
 * @brief
 * @param cap_file
 * @param cap_content
 * @return
 */
int capfs_filesystem_get_content_cap(capfs_capref_t cap_file,
                                     capfs_capref_t *cap_content);

/**
 * @brief obtains a directory entry for a given offset in a directory cap
 *
 * @param dircap    directory capability
 * @param offset    offset for the directory entry
 *
 * @return string to the directory entry, NULL of there is none
 */
char *capfs_filesystem_get_direntry(capfs_capref_t dircap, off_t offset);

#endif //CAPFS_FILESYSTEM_H__H
