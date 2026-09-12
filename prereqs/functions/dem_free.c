#define NO_ERROR 0

#include <stdio.h>
#include <unistd.h>

struct block {
    
    size_t memo_allocated_size;
    int free;
    
};

void* dem_malloc(size_t memo_size){
    
    struct block *block_ptr = sbrk(sizeof(struct block) + memo_size);
    
    if (block_ptr == (void *)-1) return NULL;
    
    block_ptr->memo_allocated_size = memo_size;
    block_ptr->free = 0;
    
    return (void *)(block_ptr + 1);
}

void dem_free(void* ptr) {
    
    struct block *block_ptr = (struct block*)ptr - 1;
    block_ptr->free = 1;
    
}

int main() {
    
    int * numbers = dem_malloc(5 * sizeof(int));
    
    if (numbers == NULL) return 1;
    
    printf("%p\n", numbers);
    
    for (int i = 0; i < 5; i++) numbers[i] = i * 10;
    
    for (int i = 0; i < 5; i++) printf("%d\n", numbers[i]);
  
    dem_free(numbers);
    
    return NO_ERROR;
    
}
