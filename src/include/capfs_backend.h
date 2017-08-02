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

#ifndef CAP_FS_BACKEND_H
#define CAP_FS_BACKEND_H 1

#include <capfs_internal.h>


/*
 * ============================================================================
 * Backend initialization
 * ============================================================================
 */

/**
 * @brief initializes the backend
 *
 * @param conn      the FUSE connection information
 * @param cfg       the FUSE configuration
 *
 * @return pointer to allocated backend data structure
 */
void *capfs_backend_init(struct fuse_conn_info * conn,
                         struct fuse_config * cfg);


/**
 * @brief destroys the backend
 *
 * @param st    pointer to the returned backend data structure of init()
 *
 * @return error number TODO: possible error values
 */
int capfs_backend_destroy(void *st);


/*
 * ============================================================================
 * Obtaining the root capability
 * ============================================================================
 */


/**
 * @brief obtains the root capability of the file system
 *
 * @return root capability or null cap
 */
capfs_capref_t capfs_backend_get_rootcap(void);

#define CAPFS_ROOTCAP capfs_backend_get_rootcap()

/*
 * ============================================================================
 * Resolving
 * ============================================================================
 */

/**
 * @brief resolves a path and obtains a capability to the resource
 *
 * @param root      the root cap to start with
 * @param path      the path to resolve relative to the root cap
 * @param retcap    the returned capability of the resolved path
 *
 * @return error number TODO: possible error values
 */
int capfs_backend_resolve_path(capfs_capref_t root,
                               const char *path,
                               capfs_capref_t *retcap);


/*
 * ============================================================================
 * Accessing a directory
 * ============================================================================
 */


/**
 * @brief reads an entry of the directory
 *
 * @param cap       the directory capability
 * @param offset    offset into the directory capability
 *
 * @return string of the entry name or NULL if there is none
 */
const char *capfs_backend_get_direntry(capfs_capref_t cap,
                                       off_t offset);


/*
 * ===========================================================================
 * Functions to obtain the meta data
 * ===========================================================================
 */


/**
 * @brief obtais the file type of a capability
 *
 * @param cap   the capability
 *
 * @return capfs file type
 */
capfs_filetype_t capfs_backend_get_filetype_cap(capfs_capref_t cap);

/**
 * @brief returns the size of the capability
 *
 * @param cap       the capability
 * @param retsize   returns the size of the capability
 *
 * @return error number TODO: possible error values
 */
int capfs_backend_get_capsize(capfs_capref_t cap, size_t *retsize);

/**
 * @brief obtains the permissions to access the capability
 *
 * @param cap   the capability
 *
 * @return the capability permissions
 */
int capfs_backend_get_perms(capfs_capref_t cap);


/*
 * ===========================================================================
 * Load and store capabilities
 * ===========================================================================
 */

/**
 * @brief loads a capability inside another capability
 *
 * @param cap       the capability
 * @param offset    offset into the capability in byes
 * @param retcap    returns the capability if any
 *
 * @return error number TODO: possible error values
 */
int capfs_backend_get_cap(capfs_capref_t cap,
                          off_t offset, capfs_capref_t *retcap);

/**
 * @brief stores a capability inside another capability
 *
 * @param cap       the capability
 * @param offset    offset into the capability in bytes
 * @param newcap    the capability to be stored
 *
 * @return error number TODO: possible error values
 */
int capfs_backend_put_cap(capfs_capref_t cap,
                          off_t offset, capfs_capref_t newcap);


/*
 * ===========================================================================
 * Read and write capability data
 * ===========================================================================
 */

/**
 * @brief reads data from a capability
 *
 * @param cap       the capability
 * @param offset    offset into the capability
 * @param rbuf      buffer to store the read data
 * @param bytes     size of the read buffer in bytes
 *
 * @return read bytes or error number TODO: possible error values
 */
int capfs_backend_read(capfs_capref_t cap, off_t offset,
                                char *rbuf, size_t bytes);

/**
 * @brief writes data into a capability
 *
 * @param cap       the capability
 * @param offset    offset into the capability
 * @param wbuf      buffer containing data to be written
 * @param bytes     size of the buffer in bytes
 *
 * @return written bytes or error number TODO: possible error values
 */
int capfs_backend_write(capfs_capref_t cap, off_t offset,
                               const char *wbuf, size_t bytes);


#endif //CAP_FS_BACKEND_H_H
