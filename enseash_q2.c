#include <unistd.h>     
#include <string.h>     
#include <sys/types.h>  
#include <sys/wait.h>   

#define MAX_CMD_LEN 128   



void print_str(const char *s) 
{
    size_t len = strlen(s);
    write(STDOUT_FILENO, s, len);
}

int main(void) 
{
    const char *welcome =
        "Bienvenue dans le Shell ENSEA.\n"
        "Pour quitter, tapez 'exit'.\n";
    const char *prompt = "enseash % ";
    
    char cmd_line[MAX_CMD_LEN];
    
    print_str(welcome);
    
    while (1) 
    {
        ssize_t n;
        pid_t pid;
        int status;

        print_str(prompt);

        n = read(STDIN_FILENO, cmd_line, MAX_CMD_LEN);

        cmd_line[n] = '\0';

        if (n > 0 && cmd_line[n - 1] == '\n') 
        {
            cmd_line[n - 1] = '\0';
        }

        if (cmd_line[0] == '\0') 
        {
            continue;
        }

        pid = fork();

        if (pid == 0) 
        {
            char *argv[2];
            argv[0] = cmd_line;  
            argv[1] = NULL;

            execvp(argv[0], argv);

            _exit(1);
        } 
        else if (pid > 0) 
        {
            waitpid(pid, &status, 0);
        } 
        else 
        {
            print_str("Erreur : fork a échoué.\n");
        }
    }
    return 0;
}




