#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

#define MAX_CMD_LEN    128
#define MAX_PROMPT_LEN 64

void print_str(const char *s)
{
    size_t len = strlen(s);
    write(STDOUT_FILENO, s, len);
}

int main(void)
{
    const char *welcome =
        "Bienvenue dans le Shell ENSEA.\n"
        "Pour quitter, tapez 'exit' ou faites Ctrl+D.\n";
    const char *bye = "Bye bye...\n";

    char cmd_line[MAX_CMD_LEN];
    char prompt[MAX_PROMPT_LEN];

    int last_status = 0;   
    int has_status  = 0;   

    print_str(welcome);

    while (1)
    {
        ssize_t n;
        pid_t pid;

        
        if (!has_status) 
        {
            sprintf(prompt, "enseash %% ");
        } else 
        {
            if (WIFEXITED(last_status)) 
            {
                int code = WEXITSTATUS(last_status);
                sprintf(prompt, "enseash [exit:%d] %% ", code);
            } else if (WIFSIGNALED(last_status)) 
            {
                int sig = WTERMSIG(last_status);
                sprintf(prompt, "enseash [sign:%d] %% ", sig);
            }
        }

        print_str(prompt);

        // Read commands
        n = read(STDIN_FILENO, cmd_line, MAX_CMD_LEN);

        //Ctrl+D (EOF) or read error management
        if (n <= 0) {
            print_str(bye);
            break;
        }

       
        cmd_line[n] = '\0';

        //Remove final '\n' final if it exists
        if (n > 0 && cmd_line[n - 1] == '\n') {
            cmd_line[n - 1] = '\0';
        }

        
        if (cmd_line[0] == '\0') {
            continue;
        }

        // Intern command "exit"
        if (strcmp(cmd_line, "exit") == 0) {
            print_str(bye);
            break;
        }

        // Creating the child process to execute the command
        pid = fork();

        if (pid == 0) {
            // Child process
            char *argv[2];
            argv[0] = cmd_line;  
            argv[1] = NULL;

            execvp(argv[0], argv);

            // execvp failed if we reach here
            _exit(1);
        }
        else if (pid > 0) {
            // Father process
            
            waitpid(pid, &last_status, 0);
            has_status = 1;
        }
        else {
            
            print_str("Erreur : fork a échoué.\n");
        }
    }

    return 0;
}


