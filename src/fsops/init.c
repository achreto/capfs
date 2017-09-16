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


#include <capfs_internal.h>


/**
 * @brief Initialize the filesystem.
 * @param conn
 * @param cfg
 * @return
 *
 * Initialize the filesystem. This function can often be left unimplemented,
 * but it can be a handy way to perform one-time setup such as allocating
 * variable-sized data structures or initializing a new filesystem. The
 * fuse_conn_info structure gives information about what features are supported
 * by FUSE, and can be used to request certain capabilities (see below for more
 * information). The return value of this function is available to all file
 * operations in the private_data field of fuse_context. It is also passed as a
 * parameter to the destroy() method.
 */
void *capfs_op_init(struct fuse_conn_info * conn,
                    struct fuse_config * cfg)
{
    int err;

    LOG("conn=%p, cfg=%p\n", conn, cfg);
    (void) conn;

    /* TODO: set the options accordningly */
    cfg->kernel_cache = 1;

    void *backend_state = capfs_backend_init(conn, cfg);

    if ((err = capfs_filesystem_init(capfs_root_capability))) {
        PANIC(err, "%s", "Filesystem initialization failed");
    }

    return backend_state;
}
