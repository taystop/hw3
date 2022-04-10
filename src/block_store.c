#include <stdio.h>
#include <stdint.h>
#include "bitmap.h"
#include "block_store.h"
// include more if you need

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

struct block_store {
    uint8_t store_arr[BLOCK_STORE_NUM_BLOCKS];
};

block_store_t* block_store_create()
{

    block_store_t* store = (block_store_t*)malloc(sizeof(block_store_t));
    return store;

}

void block_store_destroy(block_store_t* const bs)
{
    if (bs != NULL)
    {
        free(bs);
    }

}
size_t block_store_allocate(block_store_t* const bs)
{
    if (bs == NULL)
    {
        return 0;
    }
    return 0;


}

bool block_store_request(block_store_t* const bs, const size_t block_id)
{
    if (bs == NULL)
    {
        return false;
    }if (block_id)
    {
        return false;
    }

    return true;
}

void block_store_release(block_store_t* const bs, const size_t block_id)
{
    UNUSED(bs);
    UNUSED(block_id);
}

size_t block_store_get_used_blocks(const block_store_t* const bs)
{
    UNUSED(bs);
    return 0;
}

size_t block_store_get_free_blocks(const block_store_t* const bs)
{
    UNUSED(bs);
    return 0;
}

size_t block_store_get_total_blocks()
{
    return 0;
}

size_t block_store_read(const block_store_t* const bs, const size_t block_id, void* buffer)
{
    if (bs == NULL)
    {
        return 0;
    }
    if (block_id)
    {
        return 0;
    }
    if (buffer == NULL)
    {
        return 0;
    }
    return 0;
}

size_t block_store_write(block_store_t* const bs, const size_t block_id, const void* buffer)
{
    if (bs == NULL)
    {
        return 0;
    }
    if (block_id)
    {
        return 0;
    }
    if (buffer == NULL)
    {
        return 0;
    }
    return 0;
}

block_store_t* block_store_deserialize(const char* const filename)
{
    if (filename == NULL)
    {
        return NULL;
    }

    return NULL;
}

size_t block_store_serialize(const block_store_t* const bs, const char* const filename)
{
    if (bs == NULL)
    {
        return 0;
    }
    if (filename)
    {
        return 0;
    }
    return 0;
}