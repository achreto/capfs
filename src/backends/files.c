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


#include <capfs_internal.h>



/**
 * the root capability
 */
capfs_capref_t capfs_root_capability;


#define CAPABILITY_ROOT_CAP = \
    (struct capability){0, BACKEND_FILES_SIZE, BACKEND_FILES_SIZE_BITS, \
                           CAPFS_CAPABILITY_PERM_ALL}


/**
 * @brief the file name used to store the data TODO: make as an argument
 */
#define BACKEND_FILES_PATH "/tmp/foobar.bin"

/**
 * @brief the file size in bits
 */
#define BACKEND_FILES_SIZE_BITS (24)

/**
 * @brief the file size in bytes
 */
#define BACKEND_FILES_SIZE (1UL << BACKEND_FILES_SIZE_BITS)

/**
 * @brief the number of pointers in the file
 */
#define BACKEND_FILES_NUM_POINTERS (BACKEND_FILES_SIZE / sizeof(uintptr_t))

/**
 * @brief the size of the meta data area / offset where the data region starts
 */
#define BACKEND_FILES_DATA_OFFSET (BACKEND_FILES_NUM_POINTERS / 8)

/**
 * @brief the total size of the file in bytes
 */
#define BACKEND_FILES_TOTAL_SIZE (BACKEND_FILES_SIZE + BACKEND_FILES_DATA_OFFSET)


struct backend_state
{
    FILE *file;
    size_t data_size;
};

static struct backend_state g_st;


/*
 * ============================================================================
 * Capability
 * ============================================================================
 */


struct capability {
    uint64_t base;
    uint64_t size;
    uint8_t size_bits;
    capfs_capperms_t perms;
};


#define dump_capability(c) do { \
    LOG("{%lx, %lx %u, %u}\n", (c)->base, (c)->size, (c)->size_bits, (c)->perms); \
        } while(0);

static void capability_decompress(uint64_t comp, struct capability *cap)
{
    cap->base = (comp & 0xffffffffffff);
    cap->size_bits = (uint8_t)((comp >> 48) & 0xff);
    cap->size  = (1UL << cap->size_bits);
    cap->perms = (uint8_t)(comp >> 56);
}

static uint64_t capability_compres(struct capability *cap)
{
    return ((uint64_t)cap->size_bits << 48) | (cap->base & 0xffffffffffff) |
           ((uint64_t)cap->perms << 56);
}



/*
 * ===========================================================================
 * Capability to Capref Conversion
 * ===========================================================================
 *
 * XXX: We implement a dummy capability to capref conversion here. This is
 *      not considered to be save
 */


#define CAPFS_CAPREF_SALT (0xAAAAAAAAAAAAAAAAUL)

int capref_to_capability(capfs_capref_t cap, struct capability *ret_cap)
{
    capability_decompress(cap.capaddr ^ CAPFS_CAPREF_SALT, ret_cap);

    return 0;
}

int capability_to_capref(struct capability *cap, capfs_capref_t *ret_cap)
{
    ret_cap->capaddr = capability_compres(cap) ^ CAPFS_CAPREF_SALT;

    return 0;
}


/*
 * ============================================================================
 * Meta data
 * ============================================================================
 */

#define PTR2OFFSET(ptr) (((ptr) / (sizeof(uint32_t) * 8)) * sizeof(uint32_t))

static int metadata_rawread(uint64_t ptr, uint32_t *md)
{
    assert(PTR2OFFSET(ptr) < BACKEND_FILES_DATA_OFFSET);

    if (g_st.file == NULL) {
        return -1;
    }

    if (fseek(g_st.file, PTR2OFFSET(ptr), SEEK_SET)) {
        return -1;
    }

    if (fread(&md, sizeof(md), 1, g_st.file) != 1) {
        return -1;
    }

    return 0;
}

static int metadata_rawwrite(uint64_t ptr, uint32_t md)
{
    assert(PTR2OFFSET(ptr) < BACKEND_FILES_DATA_OFFSET);

    if (g_st.file == NULL) {
        return -1;
    }

    if (fseek(g_st.file, PTR2OFFSET(ptr), SEEK_SET)) {
        return -1;
    }

    if (fwrite(&md, sizeof(md), 1, g_st.file) != 1) {
        return -1;
    }

    return 0;
}

static int metadata_is_capability(uint64_t offset)
{
    uint64_t ptr = offset / sizeof(uintptr_t);

    /* must be pointer aligned */
    if (offset & 0x3) {
        return 0;
    }


    uint32_t md = 0;
    metadata_rawread(ptr, &md);

    return (md & (1 << ptr % 32));
}

static int metadata_valid_bits_generic(uint64_t from, uint64_t to, bool set)
{
    assert(from <= to);

    if (g_st.file == NULL) {
        return -1;
    }

    uint64_t ptr_from = from / sizeof(uintptr_t);
    uint64_t ptr_to = (to + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);


    uint32_t md;

    uint64_t bit = ptr_from % 32;
    if (bit) {
        metadata_rawread(ptr_from, &md);

        while((ptr_from <= ptr_to) && (bit < 32)) {
            md = (set ? md | (1<< bit) : md & ~(1<< bit));
            bit++;
            ptr_from++;
        }

        metadata_rawwrite(ptr_from, md);
    }

    assert((ptr_from % 32) == 0);

    md = (set ? 0xffffffff : 0);

    while(ptr_from + 32 <= ptr_to) {
        metadata_rawwrite(ptr_from, md);
        ptr_from += 32;
    }

    if (ptr_from < ptr_to) {

        metadata_rawread(ptr_from, &md);
        bit = 0;
        while((ptr_from <= ptr_to)) {
            assert(bit < 32);
            md = (set ? md | (1<< bit) : md & ~(1<< bit));
            bit++;
            ptr_from++;
        }

        metadata_rawwrite(ptr_from, md);
    }

    return 0;
}


static int metadata_clear_valid_bits(uint64_t from, uint64_t to)
{
    return metadata_valid_bits_generic(from, to, false);
}

static int metadata_set_valid_bits(uint64_t from, uint64_t to)
{
    return metadata_valid_bits_generic(from, to, true);
}



static inline uint64_t capstore_addr2offset(uintptr_t addr)
{
    return addr + BACKEND_FILES_DATA_OFFSET;
}


static int capstore_rawread(uint64_t offset, void *rbuf, size_t bytes)
{
    if (g_st.file == NULL) {
        LOGA("no file set\n");
        return -1;
    }

    if (offset + bytes >= g_st.data_size) {
        LOGA("outside of data range\n");
        return -1;
    }

    if (fseek(g_st.file, capstore_addr2offset(offset), SEEK_SET)) {
        LOGA("seek failed\n");
        return -1;
    }

    if (fread(rbuf, 1, bytes, g_st.file) != 1) {
        return ferror(g_st.file);
    }

    return 0;
}

static int capstore_rawwrite(uint64_t offset, const void *wbuf, size_t bytes)
{
    if (g_st.file == NULL) {
        return -1;
    }

    if (offset + bytes >= g_st.data_size) {
        return -1;
    }

    if (fseek(g_st.file, capstore_addr2offset(offset), SEEK_SET)) {
        return -1;
    }

    if (fwrite(wbuf, 1, bytes, g_st.file) != 1) {
        return ferror(g_st.file);
    }

    return 0;
}






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
                         struct fuse_config * cfg)
{
    int err;

    LOG("Initializing backend conn=%p, cfg=%p\n", conn, cfg);

    (void)conn;
    (void)cfg;


    LOG("Attempt to open file '%s'\n", BACKEND_FILES_PATH);
    g_st.file = fopen ( BACKEND_FILES_PATH , "r+b" );
    if (g_st.file == NULL) {
        LOGA("The file does not exist.. creating...\n");
        g_st.file = fopen ( BACKEND_FILES_PATH , "w+b");
        if (g_st.file == NULL) {
            PANIC(errno, "%s\n", "ERROR while opening file");
        }

        LOG("Truncate file to %" PRIu64" bytes\n", BACKEND_FILES_TOTAL_SIZE);
        if((err = truncate(BACKEND_FILES_PATH, BACKEND_FILES_TOTAL_SIZE))) {
            PANIC(err, "%s\n", "ERROR while truncating file");
        }
    }

    fseek(g_st.file, 0L, SEEK_END);
    if(ftell(g_st.file) != BACKEND_FILES_TOTAL_SIZE) {
        PANIC(EINVAL, "bad file size: %" PRIu64 " expected %" PRIu64 "\n",
              ftell(g_st.file), BACKEND_FILES_TOTAL_SIZE);
    };
    rewind(g_st.file);


    g_st.data_size = BACKEND_FILES_SIZE;

    /* create the root capability */

    struct capability rootcap = {0, BACKEND_FILES_SIZE, BACKEND_FILES_SIZE_BITS,
                                 CAPFS_CAPABILITY_PERM_READ |
                                 CAPFS_CAPABILITY_PERM_WRITE |
                                 CAPFS_CAPABILITY_PERM_EXEC};

    LOGA("setting root capability\n");
    capability_to_capref(&rootcap, &capfs_root_capability);


    return NULL;
}


/**
 * @brief destroys the backend
 *
 * @param st    pointer to the returned backend data structure of init()
 *
 * @return error number TODO: possible error values
 */
int capfs_backend_destroy(void *st)
{
    (void)st;

    return 0;
}







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
 * @return capability permissicapfs_backend_cap_get_sizeons
 */
capfs_capperms_t  capfs_backend_cap_get_perms(capfs_capref_t cap)
{
    struct capability c;
    capref_to_capability(cap, &c);

    return c.perms;
}

/**
 * @brief obtains the size of the capability
 *
 * @param cap   the capablity to obtain the size from
 *
 * @return size of the capabilty
 */
uint64_t capfs_backend_cap_get_size(capfs_capref_t cap)
{
    struct capability c;
    capref_to_capability(cap, &c);

    return c.size;
}



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
                          off_t offset, capfs_capref_t *retcap)
{
    int err;

    struct capability c;
    if (capref_to_capability(cap, &c)) {
        return -1;
    }

    if (!(c.perms & CAPFS_CAPABILITY_PERM_READ)) {
        return -EACCES;
    }


    if ((offset + sizeof(capfs_capref_t) >= c.size)) {
        return -1;
    }

    if (!metadata_is_capability(offset)) {
        return -EACCES;
    }

    uint64_t data = 0;
    err = capstore_rawread(c.base + offset, (void *)&data, sizeof(uint64_t));
    if (err != 0) {
        return err;
    }

    struct capability nc;
    capability_decompress(data, &nc);

    return capability_to_capref(&nc, retcap);
};

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
                          off_t offset, capfs_capref_t newcap)
{
    int err;


    struct capability c;
    if (capref_to_capability(cap, &c)) {
        return -1;
    }

    if (!(c.perms & CAPFS_CAPABILITY_PERM_WRITE)) {
        return -EACCES;
    }


    if ((offset + sizeof(uint64_t) >= c.size)) {
        return -1;
    }

    struct capability nc;
    if (capref_to_capability(newcap, &nc)) {
        return -1;
    }


    uint64_t data = capability_compres(&nc);

    err = capstore_rawwrite(c.base + offset, (void *)&data, sizeof(uint64_t));
    if (err) {
        return err;
    }

    return metadata_set_valid_bits(c.base + offset, c.base + offset);
}


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
                       char *rbuf, size_t bytes)
{
    struct capability c;
    if (capref_to_capability(cap, &c)) {
        LOGA("capability conversion failed\n");
        return -1;
    }

    dump_capability(&c);

    if (!(c.perms & CAPFS_CAPABILITY_PERM_READ)) {
        return -EACCES;
    }

    LOG("offset=%li, bytes=%zu, rbuf=%p\n", offset, bytes, rbuf);

    if (offset < 0) {
        return -1;
    }

    if (offset + bytes >= c.size) {
        LOG("cap size: %lx, requested range %lx..%lx",
            c.size, offset, offset+bytes);
        return -1;
    }



    assert(offset + bytes < c.size);
    if(capstore_rawread(c.base + offset, rbuf, bytes)) {
        return -1;
    }

    return bytes;
}

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
                        const char *wbuf, size_t bytes)
{
    struct capability c;
    if (capref_to_capability(cap, &c)) {
        return -1;
    }

    if (!(c.perms & CAPFS_CAPABILITY_PERM_WRITE)) {
        return -EACCES;
    }

    LOG("offset=%li, bytes=%zu, rbuf=%p\n", offset, bytes, wbuf);

    if (offset < 0) {
        return -1;
    }

    if (offset + bytes >= c.size) {
        return -1;
    }

    metadata_clear_valid_bits(c.base + offset, c.base + offset + bytes);

    assert(offset + bytes < c.size);

    if(capstore_rawwrite(c.base + offset, wbuf, bytes)) {
        return -1;
    }

    return bytes;
}


static const char zero[256] = {0};

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
int capfs_backend_zero(capfs_capref_t cap)
{
    struct capability c;
    if (capref_to_capability(cap, &c)) {
        return -1;
    }

    if (!(c.perms & CAPFS_CAPABILITY_PERM_WRITE)) {
        return -EACCES;
    }

    for (size_t offset = 0; offset < c.size; offset += 256) {
        if (c.size > offset + 256) {
            capfs_backend_write(cap, offset, zero, 256);
        } else {
            capfs_backend_write(cap, offset, zero, c.size - offset);
        }
    }

    return 0;
}