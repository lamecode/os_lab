# MemoryAllocator

## Description

A general purpose memory allocator must do at least three
tasks: allocating a block of memory of a given size, freeing the allocated
memory block and resizing the allocated memory block. These functions
make up a general purpose memory allocator interface:
#### void * mem_alloc (size_t size);
the function must allocate a block of memory of the specified size in size bytes. If
the memory block was allocated successfully, then return the address of the beginning of this block, in
otherwise, return NULL.
#### void * mem_realloc (void * addr, size_t size);
the function should change the size of the memory block with address addr to size bytes.
In this case, the contents (all or part) of the old memory block can be
moved to another block of memory. If you managed to resize the block
memory, then the function must return the address of the new memory block, otherwise return
NULL and do not destroy the old block of memory. If addr is NULL, then call
function is the same as calling mem_alloc (size).
#### void mem_free (void * addr);
the function must free the previously allocated block of memory.
A few notes on the semantics of using these functions.
Mem_realloc () function can be used for both decrementing and
increasing the block size. This function can allocate a new block of memory by
new address, while the contents of the old block are always copied to the new block
(all in the case of increasing or not changing the block size, or a part in the case of
reduction in size). If the new block is successfully allocated, then the memory block by
the old address is considered invalid. If a block of a new size is selected
failed, then NULL is returned, and the old block of memory should be
available for use.
The mem_free () function frees the allocated memory block after calling this
functions this block is invalid and cannot be used
application.
If memory is allocated with mem_alloc () or mem_realloc (), then it cannot be
move somewhere before calling mem_free () or mem_realloc (), if mem_realloc ()
returned a different address.
# EXAMPLES OF USAGE
