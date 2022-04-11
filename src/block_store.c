#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "bitmap.h"
#include "block_store.h"
#include <errno.h>


#define UNUSED(x) (void)(x)
//the block_store struct, contains a map of available blocks and a bitmap that keeps track of free blocks
// Declaring the struct but not implementing in the header allows us to prevent users
// from using the object directly and monkeying with the contents
// They can only create pointers to the struct, which must be given out by us
// This enforces a black box device, but it can be restricting
//
struct block_store {
    void* blockMap;
    bitmap_t* freeMap;

};

///
/// This creates a new BS device, ready to go
/// \return Pointer to a new block storage device, NULL on error
///
block_store_t* block_store_create() {
    //Free space for the block store struct
    block_store_t* bs = malloc(sizeof(block_store_t));

    //check for allocation errors
    if (bs != NULL)
    {
        //Initialize the blocks in the store (calloc initializes the bitmap as all zeros)
        bs->blockMap = calloc(BLOCK_STORE_NUM_BLOCKS, BLOCK_SIZE_BYTES);
    }
    else
    {
        return NULL;
    }



    //check for allocation errors
    if (bs->blockMap != NULL)
    {
        //Initialize the bitmap
        bs->freeMap = bitmap_overlay(BLOCK_STORE_AVAIL_BLOCKS, bs->blockMap);
        //return resulting block store
        return bs;
    }
    else
    {
        block_store_destroy(bs);
        return NULL;
    }
}

///
/// Destroys the provided block storage device
/// This is an idempotent operation, so there is no return value
/// \param bs BS device
///
void block_store_destroy(block_store_t* const bs) {
    //check that bs is valid
    if (bs != NULL) {
        //Free the entire store from the inside out
                //Free the inside blockmap
        if (bs->blockMap != NULL) {
            free(bs->blockMap);

        }
        //Free the inside bitmap
        if (bs->freeMap != NULL) {
            free(bs->freeMap);
        }
        //Free the rest of the struct
        free(bs);

    }
    else {
        return;
    }
}

///
/// Searches for a free block, marks it as in use, and returns the block's id
/// \param bs BS device
/// \return Allocated block's id, SIZE_MAX on error
///
size_t block_store_allocate(block_store_t* const bs)
{
    //Check for valid blockstore
    if (bs != NULL)
    {
        //Retrieve the first free block from the bitmap
        size_t freeBlock = bitmap_ffz(bs->freeMap);
        //Check for valid block
        if (freeBlock == SIZE_MAX) {
            return freeBlock;
        }
        //Otherwise set that block as being in use
        bitmap_set(bs->freeMap, freeBlock);
        //return id of block used.
        return freeBlock;
    }
    return SIZE_MAX;
}

///
/// Attempts to allocate the requested block id
/// \param bs the block store object
/// \block_id the requested block identifier
/// \return boolean indicating succes of operation
///
bool block_store_request(block_store_t* const bs, const size_t block_id) {
    //Check for valid blockstore and id
    if (block_id < block_store_get_total_blocks())
    {
        if (bs != NULL)
        {
            //Check if the block has been set in the bitmap
            //If the block is not set, set it and return success
            if (!bitmap_test(bs->freeMap, block_id))
            {
                bitmap_set(bs->freeMap, block_id);
                return true;
            }
        }
    }
    //if the param check's fail or the block is not ser, we return false
    return false;

}

///
/// Frees the specified block
/// \param bs BS device
/// \param block_id The block to free
///
void block_store_release(block_store_t* const bs, const size_t block_id) {
    //Check for valid blockstore and id
    if (block_id < block_store_get_total_blocks())
    {
        if (bs != NULL)
        {
            //Reset the given block id in the bitmap
            bitmap_reset(bs->freeMap, block_id);
        }
    }
    //If param check's fail, the method finishes
}

///
/// Counts the number of blocks marked as in use
/// \param bs BS device
/// \return Total blocks in use, SIZE_MAX on error
///
size_t block_store_get_used_blocks(const block_store_t* const bs) {
    //bool valid = false;
    //valid = bitmap_test();
    //if(valid){}
    //Check for valid blockstore
    if (bs != NULL)
    {
        //Find total number of set bits in the bitmap and return them
        return bitmap_total_set(bs->freeMap);
    }
    else {
        return SIZE_MAX;
    }
    //return SIZE_MAX;
}

///
/// Counts the number of blocks marked free for use
/// \param bs BS device
/// \return Total blocks free, SIZE_MAX on error
///
size_t block_store_get_free_blocks(const block_store_t* const bs) {
    //Check for valid blockstore
    if (bs != NULL)
    {
        //Find the total number free by subtracting those in use from the total bits
        return bitmap_get_bits(bs->freeMap) - bitmap_total_set(bs->freeMap);;
    }

    //Return SIZE_MAX if not cvalid blockstore
    return SIZE_MAX;
}

///
/// Returns the total number of user-addressable blocks
///  (since this is constant, you don't even need the bs object)
/// \return Total blocks
///
size_t block_store_get_total_blocks() {
    return BLOCK_STORE_AVAIL_BLOCKS;
}

///
/// Reads data from the specified block and writes it to the designated buffer
/// \param bs BS device
/// \param block_id Source block id
/// \param buffer Data buffer to write to
/// \return Number of bytes read, 0 on error
///
size_t block_store_read(const block_store_t* const bs, const size_t block_id, void* buffer) {

    //Check that all parameters are valid and if the block is currently being used
    if (bs != NULL) {
        if (buffer != NULL) {
            if (block_id < BLOCK_STORE_AVAIL_BLOCKS) {
                if (bitmap_test(bs->freeMap, block_id) != 0) {
                    //copy block contents into the given buffer and return the total number of bytes read
                    memcpy(buffer, (bs->blockMap + (block_id * BLOCK_SIZE_BYTES)), BLOCK_SIZE_BYTES);
                    return BLOCK_SIZE_BYTES;
                }
            }
        }
    }
    return 0;
}

///
/// Reads data from the specified buffer and writes it to the designated block
/// \param bs BS device
/// \param block_id Destination block id
/// \param buffer Data buffer to read from
/// \return Number of bytes written, 0 on error
///
size_t block_store_write(block_store_t* const bs, const size_t block_id, const void* buffer) {
    //Check that all parameters are valid and if the block is currently being used
    if (bs != NULL) {
        if (buffer != NULL) {
            if (block_id < BLOCK_STORE_AVAIL_BLOCKS) {
                if (bitmap_test(bs->freeMap, block_id) != 0) {
                    //copy block contents into the given buffer and return the total number of bytes read
                    memcpy((bs->blockMap + (block_id * BLOCK_SIZE_BYTES)), buffer, BLOCK_SIZE_BYTES);
                    return BLOCK_SIZE_BYTES;
                }
            }
        }
    }
    return 0;
}

///
/// Imports BS device from the given file - for grads/bonus
/// \param filename The file to load
/// \return Pointer to new BS device, NULL on error
///
block_store_t* block_store_deserialize(const char* const filename) {
    UNUSED(filename);
    return NULL;
}

///
/// Writes the entirety of the BS device to file, overwriting it if it exists - for grads/bonus
/// \param bs BS device
/// \param filename The file to write to
/// \return Number of bytes written, 0 on error
///
size_t block_store_serialize(const block_store_t* const bs, const char* const filename)
{
    UNUSED(bs);
    UNUSED(filename);
    return 0;
}
