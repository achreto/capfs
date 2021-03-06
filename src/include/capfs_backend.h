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
 * CAPFS Capability Permissions
 * ============================================================================
 */

typedef uint8_t capfs_capperms_t;
#define CAPFS_CAPABILITY_PERM_NONE   0
#define CAPFS_CAPABILITY_PERM_READ  (1 << 1)
#define CAPFS_CAPABILITY_PERM_WRITE (1 << 2)
#define CAPFS_CAPABILITY_PERM_EXEC  (1 << 3)
#define CAPFS_CAPABILITY_PERM_ALL (CAPFS_CAPABILITY_PERM_READ    \
                                   | CAPFS_CAPABILITY_PERM_WRITE \
                                   | CAPFS_CAPABILITY_PERM_EXEC)

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
 *
 * Note this also obtains the root capability for the backend
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
 * The root capability to the path
 * ============================================================================
 */


extern capfs_capref_t capfs_root_capability;

#define CAPFS_ROOTCAP capfs_root_capability


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
capfs_capperms_t  capfs_backend_cap_get_perms(capfs_capref_t cap);

/**
 * @brief obtains the size of the capability
 *
 * @param cap   the capablity to obtain the size from
 *
 * @return size of the capabilty
 */
uint64_t capfs_backend_cap_get_size(capfs_capref_t cap);

/*
 * ===========================================================================
 * Capability Operations
 * ===========================================================================
 */

/**
 * @brief creates a new capability based on the previous one
 *
 * @param cap       the capability to be minted
 * @param offset    offset into the capability
 * @param bytes     size of the new capbility in bytes
 * @param perms     permissions of the new capability
 * @param ret_cap   returned capability
 *
 * @return zero on SUCCESS or error number on failure
 */
int capfs_backend_cap_mint(capfs_capref_t cap, uintptr_t offset, size_t bytes,
                           capfs_capperms_t perms, capfs_capref_t *ret_cap);



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
long capfs_backend_read(capfs_capref_t cap, off_t offset,
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
long capfs_backend_write(capfs_capref_t cap, off_t offset,
                         const char *wbuf, size_t bytes);



/**
 * @brief zeroes the entire capability
 *
 * @param cap   the capability to be zeroed
 *
 * @return ERR_OK on success error value on failure
 *
 * Note this is equivalent to capfs_backend_write with a zeroed buffer of
 * size of the capability.
 */
int capfs_backend_zero(capfs_capref_t cap);

#endif //CAP_FS_BACKEND_H_H
