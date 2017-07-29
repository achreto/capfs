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

#ifndef CAP_FS_CAP_FS_H
#define CAP_FS_CAP_FS_H 1

#include <stdbool.h>


/* debugging utility */
#define LOG(format, args...)                        \
    do {fprintf(stderr, "## cap-fs # %s:%u # " format, \
        __FUNCTION__, __LINE__, args); } while(0)

/* macro for not yet implemented functions */
#define NYI() LOG("%s\n", "FILESYSTEM OPERATIONNOT YET IMPLEMENTED"); \
              return -ENOTSUP;



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



/**
 * @brief global options for the CAP-FS module.
 */
extern struct cap_fs_options cap_fs_global_options;


/**
 * @brief options for the IOCTL interface
 */
enum cap_fs_ioctl {
    CAP_FS_IOCTL_GET_CAP = 0,
    CAP_FS_IOCTL_SET_CAP = 0,
} ;

/**
 * @brief enumeration of different file types
 */
enum cap_fs_filetpe {
    CAP_FS_FILETYPE_NONE,
    CAP_FS_FILETYPE_ROOT,
    CAP_FS_FILETYPE_DIRECTORY,
    CAP_FS_FILETYPE_FILE,
    CAP_FS_FILETYPE_SYMLINK,
    CAP_FS_FILETYPE_HARDLINK,
} ;

/**
 * @brief stub representation for capabilities
 */
typedef struct capref {
    uint64_t capaddr;
} capref_t;



/**
 * @brief this stores/caches addition information for filehandles
 *
 * this structure is allcated and populated on open/opendir/create
 */
struct cap_fs_handle {
    enum cap_fs_filetpe type;
    size_t size;
    capref_t cap;
};

/**
 * @brief allocates a new cap_fs_handle struct
 */
static inline struct cap_fs_handle *cap_fs_handle_alloc() {
    return calloc(1, sizeof(struct cap_fs_handle));
}

/**
 * @brief frees a allocated cap_fs_handle struct
 */
static inline void  cap_fs_handle_free(struct cap_fs_handle *h) {
    free(h);
}



/*
 * DEBUG Functions for testing
 */

const char **cap_fs_debug_get_dirents(const char *path);
enum cap_fs_filetpe cap_fs_debug_get_file_type(const char *path);
capref_t cap_fs_debug_get_caphandle(const char *path);
size_t cap_fs_debug_get_filesize(const char *path);

#endif //CAP_FS_CAP_FS_H_H
