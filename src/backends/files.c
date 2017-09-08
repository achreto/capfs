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

#define BACKEND_FILES_PATH "foobar.bin"
#define BACKEND_FILES_SIZE_BITS (24)
#define BACKEND_FILES_SIZE (1UL << BACKEND_FILES_SIZE_BITS)

struct backend_state
{
    FILE *file;
    size_t num_cells;
};



struct capability {
    uint64_t base;
    uint64_t size;
    uint8_t size_bits;
    capfs_capperms_t perms;
};


#define CAPABILITY_ROOT_CAP = \
    (struct capability){0, BACKEND_FILES_SIZE, BACKEND_FILES_SIZE_BITS,\
                           CAPFS_CAPABILITY_PERM_READ | CAPFS_CAPABILITY_PERM_WRITE}




static struct backend_state g_st;

/*
 * ============================================================================
 * RAW load / store
 * ============================================================================
 */



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
    (void)conn;
    (void)cfg;

    g_st.file = fopen ( BACKEND_FILES_PATH , "ab" );
    if (g_st.file == NULL) {
        /* TODO ERROR */
        exit(1);
    }

    if(truncate(BACKEND_FILES_PATH, BACKEND_FILES_SIZE)) {
        exit(1);
    }


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
 * ============================================================================
 * Cells
 * ============================================================================
 *
 * In this backend data is stored in cells. Each cell stores eight bytes of data
 * and a one byte of meta data i.e. flags
 */

/**
 * @brief the ID of the cell
 *
 * cells are organized in a logical array and the cell id is the element index
 * into this array.
 */
typedef uint64_t cellid_t;

/**
 * @brief the number of data blocks in a cell
 */
#define CAPSTORE_CELL_DATA_BLOCKS 63

#define CAPSTORE_CALL_DATA_BLOCK_SIZE sizeof(uint64_t)

/**
 * @brief the number of bytes stored in a data cell
 */
#define CAPSTORE_CELL_DATA_SIZE (CAPSTORE_CELL_DATA_BLOCKS * sizeof(uint64_t))

/**
 * @brief the size of the entire cell in bytes
 */
#define CAPSTORE_CELL_SIZE (64 * sizeof(uint64_t))

/**
 * @brief the layout of a data cell in the cap store
 *
 * the data cell consists of 63 data blocks and a 64bit word of flags
 */
struct capstore_cell
{
    uint64_t data[CAPSTORE_CELL_DATA_BLOCKS];
    uint64_t flags;
};

static_assert(sizeof(struct capstore_cell) == CAPSTORE_CELL_SIZE,
              "Size of the capstore cell is not a 64x64 bytes");



/**
 * @brief
 * @param addr
 * @return
 */
static cellid_t capstore_addr2cellid(size_t addr)
{
    return (addr / CAPSTORE_CELL_DATA_SIZE);
}

/**
 * @brief
 * @param addr
 * @return
 */
static uintptr_t capstore_addr2celloffset(uintptr_t addr)
{
    return (addr % CAPSTORE_CELL_DATA_SIZE);
}

static uint8_t capstore_addr2cellblock(uintptr_t addr)
{
    return (addr % CAPSTORE_CELL_DATA_SIZE) / sizeof(uint64_t);
}

#if 0
/**
 * @brief
 * @param cid
 * @return
 */
static size_t capstore_cellid2addr(cellid_t cid)
{
    return (cid * CAPSTORE_CELL_DATA_SIZE);
}
#endif

#define CAPSTORE_CELL_FLAG_MASK(from, to) \
    (((1UL << (to + 1)) - 1) & ~((1UL << from) - 1))

static void capstore_cell_set_flags(struct capstore_cell *cell,
                                    uint8_t from, uint8_t to)
{
    cell->flags |= CAPSTORE_CELL_FLAG_MASK(from, to);
}

static void capstore_cell_clear_flags(struct capstore_cell *cell,
                                    uint8_t from, uint8_t to)
{
    cell->flags &= ~CAPSTORE_CELL_FLAG_MASK(from, to);
}



static int capstore_cell_rawwr(cellid_t cellid, struct capstore_cell *cell)
{
    if (g_st.file == NULL) {
        return -1;
    }

    if (cellid >= g_st.num_cells) {
        return -1;
    }

    if (fseek(g_st.file, cellid * CAPSTORE_CELL_SIZE, SEEK_SET)) {
        return -1;
    }

    if (fwrite(cell, 1, CAPSTORE_CELL_SIZE,g_st.file) != 1) {
        return ferror(g_st.file);
    }

    return 0;
}




static int capstore_cell_rawread(cellid_t cellid, struct capstore_cell *cell)
{
   if (g_st.file == NULL) {
        return -1;
    }

    if (cellid >= g_st.num_cells) {
        return -1;
    }

    if (fseek(g_st.file, cellid * CAPSTORE_CELL_SIZE, SEEK_SET)) {
        return -1;
    }

    if (fread(cell, 1, CAPSTORE_CELL_SIZE, g_st.file) != 1) {
        return ferror(g_st.file);
    }

    return 0;
}




static long capstore_cell_read(uintptr_t addr, void *rbuf, size_t bytes)
{
    int err;

    cellid_t cid = capstore_addr2cellid(addr);

    /* if the first or the last cell id is bigger than */
    if (cid >= g_st.num_cells || capstore_addr2cellid(addr + bytes) >= g_st.num_cells) {
        return -1;
    }

    static struct capstore_cell tmpcell;

    uintptr_t offset = capstore_addr2celloffset(addr);
    if (offset) {
        err = capstore_cell_rawread(cid, &tmpcell);
        if (err != 0) {
            return err;
        }

        memcpy(rbuf, ((uint8_t *)(tmpcell.data)) + offset, CAPSTORE_CELL_DATA_SIZE - offset);
        cid++;
        bytes -= CAPSTORE_CELL_DATA_SIZE - offset;
        rbuf = ((uint8_t *)rbuf) + CAPSTORE_CELL_DATA_SIZE - offset;
    }

    while(bytes > 0) {
        err = capstore_cell_rawread(cid, &tmpcell);
        if (err != 0) {
            return err;
        }

        if (bytes > CAPSTORE_CELL_DATA_SIZE) {
            memcpy(rbuf, tmpcell.data, CAPSTORE_CELL_DATA_SIZE);
            bytes -= CAPSTORE_CELL_DATA_SIZE;
            rbuf = ((uint8_t *)rbuf) + CAPSTORE_CELL_DATA_SIZE;
        } else {
            memcpy(rbuf, tmpcell.data, bytes);
            bytes = 0;
        }
        cid++;
    }

    return 0;
}

static long capstore_cell_write(uintptr_t addr, const void *wbuf, uint64_t bytes)
{
    int err;

    cellid_t cid = capstore_addr2cellid(addr);

    int64_t written = 0;

    /* if the first or the last cell id is bigger than */
    if (cid >= g_st.num_cells || capstore_addr2cellid(addr + bytes)  >= g_st.num_cells) {
        return -1;
    }

    static struct capstore_cell tmpcell;

    uintptr_t offset = capstore_addr2celloffset(addr);
    if (offset) {
        err = capstore_cell_rawread(cid, &tmpcell);
        if (err != 0) {
            return err;
        }

        written += CAPSTORE_CELL_DATA_SIZE - offset;

        memcpy(((uint8_t *)(tmpcell.data)) + offset, wbuf, CAPSTORE_CELL_DATA_SIZE - offset);
        capstore_cell_clear_flags(&tmpcell, offset, CAPSTORE_CELL_DATA_BLOCKS);

        err = capstore_cell_rawwr(cid, &tmpcell);
        if (err != 0) {
            return err;
        }
        cid++;
        bytes -= CAPSTORE_CELL_DATA_SIZE - offset;
        wbuf = ((uint8_t *)wbuf) + CAPSTORE_CELL_DATA_SIZE - offset;
    }

    while(bytes > 0) {

        if (bytes > CAPSTORE_CELL_DATA_SIZE) {
            memcpy( tmpcell.data, wbuf, CAPSTORE_CELL_DATA_SIZE);
            tmpcell.flags = 0;
            err = capstore_cell_rawwr(cid, &tmpcell);
            if (err != 0) {
                return err;
            }
            written += CAPSTORE_CELL_DATA_SIZE;
            bytes -= CAPSTORE_CELL_DATA_SIZE;
            wbuf = ((uint8_t *)wbuf) + CAPSTORE_CELL_DATA_SIZE;
        } else {
            err = capstore_cell_rawread(cid, &tmpcell);
            if (err != 0) {
                return err;
            }

            memcpy(tmpcell.data, wbuf, bytes);

            capstore_cell_clear_flags(&tmpcell, 0, (bytes + sizeof(uint64_t) - 1) / sizeof(uint64_t));
            err = capstore_cell_rawwr(cid, &tmpcell);
            if (err != 0) {
                return err;
            }
            written += bytes;
            bytes = 0;

        }
        cid++;
    }

    return written;
}







/*
 * ============================================================================
 * Capability
 * ============================================================================
 */


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

    /* check the offset for storing the cap */
    if (offset & (sizeof(CAPSTORE_CALL_DATA_BLOCK_SIZE) - 1)) {
        return -1;
    }

    struct capability c;
    if (capref_to_capability(cap, &c)) {
        return -1;
    }

    if (!(c.perms & CAPFS_CAPABILITY_PERM_READ)) {
        return -EACCES;
    }


    if ((offset + sizeof(uint64_t) >= c.size)) {
        return -1;
    }


    cellid_t cid = capstore_addr2cellid(c.base + offset);

    static struct capstore_cell tmpcell;
    err = capstore_cell_rawread(cid, &tmpcell);
    if (err != 0) {
        return err;
    }

    uint8_t bid = capstore_addr2cellblock(c.base + offset);

    if (!(tmpcell.flags & (1UL << bid))) {
        return -1;
    }

    struct capability nc;
    capability_decompress(tmpcell.data[bid], &nc);

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

    /* check the offset for storing the cap */
    if (offset & (sizeof(CAPSTORE_CALL_DATA_BLOCK_SIZE) - 1)) {
        return -1;
    }

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


    cellid_t cid = capstore_addr2cellid(c.base + offset);

    static struct capstore_cell tmpcell;
    err = capstore_cell_rawread(cid, &tmpcell);
    if (err != 0) {
        return err;
    }

    uint8_t bid = capstore_addr2cellblock(c.base + offset);

    tmpcell.data[bid] = capability_compres(&nc);
    capstore_cell_set_flags(&tmpcell, bid, bid);

    return capstore_cell_rawwr(cid, &tmpcell);
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
        return -1;
    }

    if (!(c.perms & CAPFS_CAPABILITY_PERM_READ)) {
        return -EACCES;
    }


    if (offset + bytes >= (1UL << c.size)) {
        bytes = (1UL << c.size) - offset;
    }
    assert(offset + bytes < (1UL << c.size));

    return capstore_cell_read(c.base + offset, rbuf, bytes);
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

    if (offset < 0) {
        return 0;
    }

    if ((size_t)offset > (1UL << c.size)) {
        return -1;
    }

    if (offset + bytes >= (1UL << c.size)) {
        bytes = (1UL << c.size) - offset;
    }
    assert(offset + bytes < (1UL << c.size));

    return capstore_cell_write(c.base + offset, wbuf, bytes);
}
