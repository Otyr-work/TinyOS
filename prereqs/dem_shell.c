#define NO_ERROR 0

// - - - - - - - - - - 

#define IN 0
#define OUT -1

// - - - - - - - - - -

#define MAX_INPUT 256
#define MAX_ARGS 16


#include <stdio.h>
#include <string.h>




int main() {
    
    printf("shellom$ \n");
    
    char user_input[MAX_INPUT];
    int flag = IN;
    
    while(flag != OUT) {
        
        // checking if user wants to exit or no input was given
        
        fflush(stdout);
        if (fgets(user_input, sizeof(user_input), stdin) == NULL) flag = OUT;
        
        user_input[strcspn(user_input, "\n")] = '\0';
        
        if (strcmp(user_input, "exit") == 0 ) flag = OUT;
        else if (user_input[0] == '\0') continue;
        
        // splitting command into parts(tokens)
        
        char *token = strtok(user_input, " ");
        char *command_string[MAX_ARGS];
        int arg_count = 0;
        
        
        while (token != NULL && arg_count < MAX_ARGS - 1) {
            
            command_string[arg_count++] = token;
            token = strtok(NULL, "");
            
        }
            
    }
    
    printf("Done");
            
    // create child process running the command
    
    // wait for it to finish
    
    return NO_ERROR;
    
}
