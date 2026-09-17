#define NO_ERROR 0
#define ALIGNMENT 8
#define MMAP_THRESHOLD 4096

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>


struct block {

    size_t memo_allocated_size;
    int free;
    int mmaped;
    struct block *next_ptr;

};


struct block *heap_start = NULL;


size_t align_size(size_t size)
{
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}


void *dem_malloc(size_t memo_size)
{
    memo_size = align_size(memo_size);


    /*
     * Large allocations use mmap()
     */
    if (memo_size >= MMAP_THRESHOLD) {

        struct block *block_ptr = mmap(
            NULL,
            sizeof(struct block) + memo_size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1,
            0
        );

        if (block_ptr == MAP_FAILED)
            return NULL;

        block_ptr->memo_allocated_size = memo_size;
        block_ptr->free = 0;
        block_ptr->mmaped = 1;
        block_ptr->next_ptr = NULL;

        return (void *)(block_ptr + 1);
    }


    /*
     * Search existing sbrk() blocks for reusable memory
     */
    struct block *current_blk = heap_start;

    while (current_blk != NULL) {

        if (current_blk->free &&
            current_blk->memo_allocated_size >= memo_size) {

            /*
             * Split the block if enough space remains
             * for another header and some payload.
             */
            if (current_blk->memo_allocated_size >=
                memo_size + sizeof(struct block) + 1) {

                struct block *new_blk =
                    (struct block *)(
                        (char *)(current_blk + 1) + memo_size
                    );

                new_blk->memo_allocated_size =
                    current_blk->memo_allocated_size
                    - memo_size
                    - sizeof(struct block);

                new_blk->free = 1;
                new_blk->mmaped = 0;
                new_blk->next_ptr = current_blk->next_ptr;

                current_blk->memo_allocated_size = memo_size;
                current_blk->free = 0;
                current_blk->mmaped = 0;
                current_blk->next_ptr = new_blk;
            }

            else {
                current_blk->free = 0;
            }

            return (void *)(current_blk + 1);
        }

        current_blk = current_blk->next_ptr;
    }


    /*
     * No reusable block found.
     * Extend the process heap with sbrk().
     */
    struct block *block_ptr =
        sbrk(sizeof(struct block) + memo_size);

    if (block_ptr == (void *)-1)
        return NULL;

    block_ptr->memo_allocated_size = memo_size;
    block_ptr->free = 0;
    block_ptr->mmaped = 0;
    block_ptr->next_ptr = NULL;


    /*
     * Add the new block to our bookkeeping list.
     */
    if (heap_start == NULL) {

        heap_start = block_ptr;
    }

    else {

        struct block *current_blk = heap_start;

        while (current_blk->next_ptr != NULL)
            current_blk = current_blk->next_ptr;

        current_blk->next_ptr = block_ptr;
    }


    return (void *)(block_ptr + 1);
}


void dem_free(void *ptr)
{
    if (ptr == NULL)
        return;


    struct block *block_ptr =
        (struct block *)ptr - 1;


    /*
     * mmap() allocations are returned directly to the kernel.
     */
    if (block_ptr->mmaped) {

        munmap(
            block_ptr,
            sizeof(struct block) +
            block_ptr->memo_allocated_size
        );

        return;
    }


    /*
     * Normal sbrk() allocation.
     */
    block_ptr->free = 1;


    /*
     * Find the previous block.
     */
    struct block *current_blk = heap_start;

    while (current_blk != NULL) {

        if (current_blk->next_ptr == block_ptr)
            break;

        current_blk = current_blk->next_ptr;
    }


    /*
     * Merge with the next block.
     */
    if (block_ptr->next_ptr != NULL &&
        block_ptr->next_ptr->free) {

        struct block *next_blk =
            block_ptr->next_ptr;

        block_ptr->memo_allocated_size +=
            sizeof(struct block) +
            next_blk->memo_allocated_size;

        block_ptr->next_ptr =
            next_blk->next_ptr;
    }


    /*
     * Merge with the previous block.
     */
    if (current_blk != NULL &&
        current_blk != block_ptr &&
        current_blk->free) {

        current_blk->memo_allocated_size +=
            sizeof(struct block) +
            block_ptr->memo_allocated_size;

        current_blk->next_ptr =
            block_ptr->next_ptr;
    }
}


void *dem_calloc(size_t count, size_t size)
{
    size_t total_size = count * size;

    void *ptr = dem_malloc(total_size);

    if (ptr == NULL)
        return NULL;


    unsigned char *bytes = ptr;

    for (size_t i = 0; i < total_size; i++)
        bytes[i] = 0;


    return ptr;
}


void *dem_realloc(void *ptr, size_t new_size)
{
    if (ptr == NULL)
        return dem_malloc(new_size);


    if (new_size == 0) {

        dem_free(ptr);

        return NULL;
    }


    struct block *old_blk =
        (struct block *)ptr - 1;


    void *new_ptr =
        dem_malloc(new_size);

    if (new_ptr == NULL)
        return NULL;


    size_t copy_size =
        old_blk->memo_allocated_size;


    if (new_size < copy_size)
        copy_size = new_size;


    unsigned char *src = ptr;
    unsigned char *dst = new_ptr;


    for (size_t i = 0; i < copy_size; i++)
        dst[i] = src[i];


    dem_free(ptr);


    return new_ptr;
}


int main()
{
    /*
     * Normal allocations
     */
    int *numbers =
        dem_malloc(5 * sizeof(int));

    int *more_numbers =
        dem_malloc(8 * sizeof(int));

    int *even_more_numbers =
        dem_malloc(8 * sizeof(int));


    if (numbers == NULL ||
        more_numbers == NULL ||
        even_more_numbers == NULL)
        return 1;


    /*
     * Test coalescing
     */
    dem_free(more_numbers);
    dem_free(even_more_numbers);


    printf("\nBookkeeping after freeing adjacent blocks:\n");


    struct block *current_blk = heap_start;


    while (current_blk != NULL) {

        printf(
            "block: %p | size: %zu | free: %d | mmaped: %d | next: %p\n",
            (void *)current_blk,
            current_blk->memo_allocated_size,
            current_blk->free,
            current_blk->mmaped,
            (void *)current_blk->next_ptr
        );

        current_blk =
            current_blk->next_ptr;
    }


    /*
     * Test calloc()
     */
    int *zeroed =
        dem_calloc(5, sizeof(int));

    if (zeroed == NULL)
        return 1;


    printf("\ncalloc:\n");

    for (int i = 0; i < 5; i++)
        printf("zeroed[%d] = %d\n",
               i,
               zeroed[i]);


    /*
     * Test realloc()
     */
    int *numbers_r =
        dem_malloc(5 * sizeof(int));

    if (numbers_r == NULL)
        return 1;


    for (int i = 0; i < 5; i++)
        numbers_r[i] = i * 10;


    numbers_r =
        dem_realloc(
            numbers_r,
            10 * sizeof(int)
        );


    if (numbers_r == NULL)
        return 1;


    printf("\nrealloc:\n");

    for (int i = 0; i < 5; i++)
        printf("numbers_r[%d] = %d\n",
               i,
               numbers_r[i]);


    /*
     * Test mmap()
     */
    int *big_numbers =
        dem_malloc(5000);


    if (big_numbers == NULL)
        return 1;


    big_numbers[0] = 123;
    big_numbers[1] = 456;


    struct block *big_block =
        (struct block *)big_numbers - 1;


    printf("\nmmap allocation:\n");

    printf(
        "block: %p | size: %zu | free: %d | mmaped: %d\n",
        (void *)big_block,
        big_block->memo_allocated_size,
        big_block->free,
        big_block->mmaped
    );

    printf(
        "big_numbers[0] = %d\n",
        big_numbers[0]
    );

    printf(
        "big_numbers[1] = %d\n",
        big_numbers[1]
    );


    /*
     * This should call munmap().
     */
    dem_free(big_numbers);


    return NO_ERROR;
}
