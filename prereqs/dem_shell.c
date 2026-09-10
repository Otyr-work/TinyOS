#define NO_ERROR 0

// - - - - - - - - - - 

#define IN 0
#define OUT -1

// - - - - - - - - - -

#define MAX_INPUT 256
#define MAX_ARGS 16


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

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
            token = strtok(NULL, " ");
            
        }
        
        command_string[arg_count] = NULL;
        
        pid_t pid = fork();
        
        if ( pid < 0 ) 
        {
            perror("fork");
            continue;
        }
        
        if (pid == 0) {
            
            execvp(command_string[0], command_string);
            
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        
        waitpid(pid, NULL, 0);
            
    }
    
    return NO_ERROR;
    
}
