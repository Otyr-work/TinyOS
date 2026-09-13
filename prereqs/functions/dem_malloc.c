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
    
}





int main() {

    int *numbers = dem_malloc(5 * sizeof(int));
    int *more_numbers = dem_malloc(16 * sizeof(int));
    int *even_more_numbers = dem_malloc(8 * sizeof(int));
    
    

    if (numbers == NULL || more_numbers == NULL || even_more_numbers == NULL)
        return 1;

    

    dem_free(more_numbers);
    
    
    
    int *reused_numbers = dem_malloc(2*sizeof(int));
    
    
    
    if(reused_numbers == NULL) return 1;
    
    printf("Original freed address: %p\n", (void *)more_numbers);
    printf("reused addresses: %p\n", (void *)reused_numbers);
    
    
    
    printf("\nBookkeeping:\n");

    

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
