#define NO_ERROR 0
#define ALIGNMENT 8

#include <stdio.h> 
#include <unistd.h> 





struct block { 
    
    size_t memo_allocated_size;
    int free;
    struct block* next_ptr;
    
};





struct block* heap_start = NULL;



size_t align_size(size_t size)
{
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT -1);
}




void* dem_malloc(size_t memo_size){
    
    memo_size = align_size(memo_size);
    
    // re-use already freed blocks
    struct block * current_blk = heap_start;
    
    
    
    while( current_blk != NULL ) {
        
        if ( current_blk->free && current_blk->memo_allocated_size >= memo_size )
        {
            
            if (current_blk->memo_allocated_size >= memo_size + sizeof(struct block) + 1) {
                
                struct block *new_blk = (struct block *)((char *)(current_blk + 1) + memo_size);
                
                new_blk->memo_allocated_size = current_blk->memo_allocated_size - memo_size - sizeof(struct block);
                
                new_blk->free = 1;
                new_blk->next_ptr = current_blk->next_ptr;
                
                current_blk->memo_allocated_size = memo_size;
                current_blk->free = 0;
                current_blk->next_ptr = new_blk;
                
            }
            
            else current_blk->free = 0;
            
            return(void *)(current_blk + 1);
        }
        
        current_blk = current_blk->next_ptr;
        
    }
    
    
    
    // go over the heap to find a free space 
    struct block *block_ptr = sbrk(sizeof(struct block) + memo_size);
    
    
    if (block_ptr == (void *)-1) return NULL;
    
    
    
    block_ptr->memo_allocated_size = memo_size;
    block_ptr->free = 0;
    block_ptr->next_ptr = NULL;
    
    
    
    if(heap_start == NULL)  heap_start = block_ptr;
    
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
    
    struct block * current_blk = heap_start;
    
    while (current_blk != NULL) {
        
        if (current_blk->next_ptr == block_ptr) break;
        
        current_blk = current_blk->next_ptr;
        
    } 
    
    if (block_ptr->next_ptr != NULL && block_ptr->next_ptr->free) {
        
        struct block * next_blk = block_ptr->next_ptr;
        
        block_ptr->memo_allocated_size += sizeof(struct block) + next_blk->memo_allocated_size;
        
        block_ptr->next_ptr = next_blk->next_ptr;
        
    }
    
    if (current_blk != NULL && current_blk != block_ptr && current_blk->free) {
        
        current_blk->memo_allocated_size += sizeof(struct block) + block_ptr->memo_allocated_size;
        
        current_blk->next_ptr = block_ptr->next_ptr;
        
    }
    
}




void * dem_calloc(size_t count, size_t size) {
    
    size_t total_size = count * size;
    
    void * ptr = dem_malloc(total_size);
    
    if (ptr == NULL) return NULL;
    
    unsigned char * bytes = ptr;
    
    for (size_t i = 0; i < total_size; i++) bytes[i] = 0;
    
    return ptr;
    
} 



void * dem_realloc(void * ptr, size_t new_size) {
    
    if (ptr == NULL) return dem_malloc(new_size);
    
    if (new_size == 0) {
        
        dem_free(ptr);
        return NULL;
    }
    
    struct block * old_blk = (struct block *)ptr - 1;
    
    void * new_ptr = dem_malloc(new_size);
    
    if (new_ptr == NULL) return NULL;
    
    size_t copy_size = old_blk->memo_allocated_size;
    
    if(new_size < copy_size) copy_size = new_size;
    
    unsigned char *src = ptr;
    unsigned char *dst = new_ptr;
    
    for (size_t i = 0; i < copy_size; i++) dst[i] = src[i];
    
    dem_free(ptr);
    
    return new_ptr;
    
}





int main() {

    int *numbers = dem_malloc(5 * sizeof(int));
    int *more_numbers = dem_malloc(8 * sizeof(int));
    int *even_more_numbers = dem_malloc(8 * sizeof(int));
    
    

    if (numbers == NULL || more_numbers == NULL || even_more_numbers == NULL)
        return 1;

    

    dem_free(more_numbers);
    dem_free(even_more_numbers);
    
    printf("\nBookkeeping after freeing adjacent ");


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
    
    int *zeroed = dem_calloc(5, sizeof(int));

    if (zeroed == NULL)
        return 1;

    for (int i = 0; i < 5; i++)
        printf("zeroed[%d] = %d\n", i, zeroed[i]);
    
    
    int * numbers_r = dem_malloc(5 * sizeof(int));

    for (int i = 0; i < 5; i++)
        numbers_r[i] = i * 10;

    numbers_r = dem_realloc(numbers_r, 10 * sizeof(int));

    for (int i = 0; i < 5; i++)
        printf("%d\n", numbers_r[i]);

    return NO_ERROR;
}
