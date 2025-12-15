#define _POSIX_C_SOURCE 199309L

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <time.h>

#define MAX_CMD_LEN    128
#define MAX_PROMPT_LEN 64
#define MAX_ARGS       16

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
    long duration_ms = 0;

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
                sprintf(prompt, "enseash [exit:%d|%ldms] %% ", code, duration_ms);
            } else if (WIFSIGNALED(last_status)) 
            {
                int sig = WTERMSIG(last_status);
                sprintf(prompt, "enseash [sign:%d|%ldms] %% ", sig, duration_ms);
            }
        }

        print_str(prompt);

        // Lecture de la commande
        n = read(STDIN_FILENO, cmd_line, MAX_CMD_LEN);

        // Gestion de Ctrl+D (EOF) ou erreur de lecture
        if (n <= 0) {
            print_str(bye);
            break;
        }

        cmd_line[n] = '\0';

        // Enlever le '\n' final s'il existe
        if (n > 0 && cmd_line[n - 1] == '\n') {
            cmd_line[n - 1] = '\0';
        }

        
        if (cmd_line[0] == '\0') {
            continue;
        }

        // Commande interne "exit"
        if (strcmp(cmd_line, "exit") == 0) {
            print_str(bye);
            break;
        }

        char *argv[MAX_ARGS];
        int   argc = 0;
        char *token = strtok(cmd_line, " ");

        while (token != NULL && argc < (MAX_ARGS - 1)) {
            argv[argc] = token;
            argc++;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        
        struct timespec t_start, t_end;
        clock_gettime(CLOCK_REALTIME, &t_start);


        // Création du processus fils pour exécuter la commande
        pid = fork();

        if (pid == 0) {
            // Processus fils

            execvp(argv[0], argv);

            // Si on arrive ici, execvp a échoué
            _exit(1);
        }
        else if (pid > 0) {
            // Processus père
            
            waitpid(pid, &last_status, 0);

            clock_gettime(CLOCK_REALTIME, &t_end);

            long sec_diff  = t_end.tv_sec  - t_start.tv_sec;
            long nsec_diff = t_end.tv_nsec - t_start.tv_nsec;
            duration_ms = sec_diff * 1000 + nsec_diff / 1000000;

            has_status = 1;
        }
        else {
            
            print_str("Erreur : fork a échoué.\n");
        }
    }

    return 0;
}


