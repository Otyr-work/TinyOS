#define NO_ERROR 0

#include <stdio.h> 
#include <unistd.h> 

struct block { 
    
    size_t memo_allocated_size;
    int free;
    struct block* next_ptr;
    
};

struct block* heap_start = NULL;

void* dem_malloc(size_t memo_size){
    
    struct block *block_ptr = sbrk(sizeof(struct block) + memo_size);
    
    if (block_ptr == (void *)-1) return NULL;
    
    block_ptr->memo_allocated_size = memo_size;
    block_ptr->free = 0;
    block_ptr->next_ptr = NULL;
    
    if(heap_start == NULL) { heap_start = block_ptr;} 
    
    else {
        
        struct block * current_blk = heap_start;
        
        while (current_blk->next_ptr != NULL) current_blk=current_blk->next_ptr;
        
        current_blk->next_ptr = block_ptr;
        
    }
    
    return (void *)(block_ptr + 1);
    
} 

void dem_free(void* ptr) { 
    
    struct block *block_ptr = (struct block*)ptr - 1;
    
    block_ptr->free = 1;
    
}

int main() {

    int *numbers = dem_malloc(5 * sizeof(int));
    int *more_numbers = dem_malloc(3 * sizeof(int));
    int *even_more_numbers = dem_malloc(8 * sizeof(int));

    if (numbers == NULL || more_numbers == NULL || even_more_numbers == NULL)
        return 1;

    dem_free(more_numbers);

    struct block *current_blk = heap_start;

    while (current_blk != NULL) {

        printf(
            "block: %p | size: %zu | free: %d | next: %p\n",
            (void *)current_blk,
            current_blk->memo_allocated_size,
            current_blk->free,
            (void *)current_blk->next_ptr
        );

        current_blk = current_blk->next_ptr;
    }

    return NO_ERROR;
}
