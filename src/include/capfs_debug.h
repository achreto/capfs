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

#ifndef CAP_FS_DEBUG_H
#define CAP_FS_DEBUG_H 1

#include <stdio.h>
#include <inttypes.h>

#define PRIxCAP "%" PRIx64
#define PRI_CAP(x) ((x).capaddr)

/* debugging utility */
#define LOG(format, args...)                        \
    do {fprintf(stderr, "## cap-fs # %s:%u # " format, \
        __FUNCTION__, __LINE__, args); } while(0)


/* debugging utility */
#define LOGA(format)                        \
    do {fprintf(stderr, "## cap-fs # %s:%u # " format, \
        __FUNCTION__, __LINE__); } while(0)


/* debugging utility */
#define PANIC(err, format, args...)                        \
    do {fprintf(stderr, "## cap-fs # %s:%u # %u " format, \
        __FUNCTION__, __LINE__, err, args);                 \
        exit(err); } while(0)



/* macro for not yet implemented functions */
#define NYI() LOG("%s\n", "FILESYSTEM OPERATION NOT YET IMPLEMENTED"); \
              return -ENOTSUP;


#endif //CAP_FS_DEBUG_H_H
