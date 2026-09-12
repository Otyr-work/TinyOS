#define NO_ERROR 0

#include <unistd.h>

void * dem_malloc(size_t memo_size){
    
    void * memo_ptr = sbrk(memo_size);
    
    if( memo_ptr == (void *)-1 ) return NULL;
    
    return memo_ptr;
}

int main() {
    
    // testing the dem_malloc
  
    return NO_ERROR;
    
}
