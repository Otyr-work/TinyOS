#define NO_ERROR 0

// - - - - - - - - - - 

#define IN 0
#define OUT -1

// - - - - - - - - - -

#define MAX_INPUT 256



#include <stdio.h>
#include <string.h>




int main() {
    
    printf("shellom$ \n");
    
    char user_input[MAX_INPUT];
    int flag = IN;
    
    while(flag != OUT) {
        
        fflush(stdout);
        if (fgets(user_input, sizeof(user_input), stdin) == NULL) flag = OUT;
        
        user_input[strcspn(user_input, "\n")] = '\0';
        
        if (strcmp(user_input, "pwd") == 0 || strcmp(user_input, "ls") == 0|| strcmp(user_input, "exit") == 0 ) flag = OUT;
            
    }
    
    printf("Done");
    
    // read the command from the user
    
    // split the command into arguments
    
    // create child process running the command
    
    // wait for it to finish
    
    return NO_ERROR;
    
}
