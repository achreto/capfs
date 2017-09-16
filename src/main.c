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


int verbosity = 0;




/**
 * @brief the FUSE operations for the CAP-FS
 */

static struct fuse_operations capfs_ops = {
        .init       = capfs_op_init,
        .getattr    = capfs_op_getattr,
        .access     = capfs_op_access,
        .opendir    = capfs_op_opendir,
        .readdir    = capfs_op_readdir,
        .releasedir = capfs_op_releasedir,
        .readlink   = capfs_op_readlink,
        .mknod      = capfs_op_mknod,
        .mkdir      = capfs_op_mkdir,
        .symlink    = capfs_op_symlink,
        .unlink     = capfs_op_unlink,
        .rmdir      = capfs_op_rmdir,
        .rename     = capfs_op_rename,
        .link       = capfs_op_link,
        .chmod      = capfs_op_chmod,
        .chown      = capfs_op_chown,
        .truncate   = capfs_op_truncate,
        .utimens    = capfs_op_utimens,
        .open       = capfs_op_open,
        .flush      = capfs_op_flush,
        .fsync      = capfs_op_fsync,
        .release    = capfs_op_release,
        .read       = capfs_op_read,
        .write      = capfs_op_write,
        .statfs     = capfs_op_statfs,
        .create     = capfs_op_create,
        .ioctl      = capfs_op_ioctl,
        .destroy    = capfs_op_destroy,
};

/**
 * @brief the cap-fs state
 */
struct cap_fs capfs_st;

/**
 * @brief cp-fs main function
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char * argv[]) {
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    LOG("%s", "-----------------------------------------------\n");
    LOG("CAP-FS version %s\n", PACKAGE_VERSION);
    LOG("FUSE library version %s\n", fuse_pkgversion());
    fuse_lowlevel_version();
    LOG("%s", "-----------------------------------------------\n");


    /* TODO: initialize the connection to the capability  */

    capfs_st.initialized = true;

    LOG("%s\n", "calling fuse_main\n");

    return fuse_main(args.argc, args.argv, &capfs_ops, NULL);
}
