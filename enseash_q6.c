// enseash_q6.c
// Shell ENSEA – Q6 : commandes avec arguments + exit/sign + temps en ms

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

    int  last_status  = 0;   // status de la dernière commande
    int  has_status   = 0;   // = 0 tant qu’aucune commande n’a été exécutée
    long last_time_ms = 0;   // temps de la dernière commande en ms

    print_str(welcome);

    while (1)
    {
        ssize_t n;
        pid_t pid;

        // Construire le prompt
        if (!has_status) {
            sprintf(prompt, "enseash %% ");
        } else {
            if (WIFEXITED(last_status)) {
                int code = WEXITSTATUS(last_status);
                sprintf(prompt, "enseash [exit:%d|%ldms] %% ", code, last_time_ms);
            } else if (WIFSIGNALED(last_status)) {
                int sig = WTERMSIG(last_status);
                sprintf(prompt, "enseash [sign:%d|%ldms] %% ", sig, last_time_ms);
            } else {
                sprintf(prompt, "enseash %% ");
            }
        }

        print_str(prompt);

        // Lire la ligne
        n = read(STDIN_FILENO, cmd_line, MAX_CMD_LEN - 1);
        if (n <= 0) {               // Ctrl+D ou erreur
            print_str(bye);
            break;
        }

        if (n >= MAX_CMD_LEN) {     // sécurité
            n = MAX_CMD_LEN - 1;
        }
        cmd_line[n] = '\0';

        // Enlever le '\n' final
        if (n > 0 && cmd_line[n - 1] == '\n') {
            cmd_line[n - 1] = '\0';
        }

        // Ligne vide
        if (cmd_line[0] == '\0') {
            continue;
        }

        // Commande interne exit
        if (strcmp(cmd_line, "exit") == 0) {
            print_str(bye);
            break;
        }

        // Découpage en mots (commande + arguments)
        char *argv[MAX_ARGS];
        int   argc = 0;
        char *token = strtok(cmd_line, " ");

        while (token != NULL && argc < (MAX_ARGS - 1)) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        if (argc == 0) {
            continue;
        }

        // Mesure du temps avant exécution
        struct timespec t_start, t_end;
        clock_gettime(CLOCK_MONOTONIC, &t_start);

        pid = fork();

        if (pid == 0) {
            // --- Fils ---
            execvp(argv[0], argv);

            // Si on arrive ici, execvp a échoué : on affiche pourquoi
            perror("execvp");
            _exit(1);
        }
        else if (pid > 0) {
            // --- Père ---
            waitpid(pid, &last_status, 0);

            clock_gettime(CLOCK_MONOTONIC, &t_end);

            long sec  = t_end.tv_sec  - t_start.tv_sec;
            long nsec = t_end.tv_nsec - t_start.tv_nsec;
            last_time_ms = sec * 1000 + nsec / 1000000;
            if (last_time_ms < 0) last_time_ms = 0;

            has_status = 1;
        }
        else {
            print_str("Erreur : fork a échoué.\n");
        }
    }

    return 0;
}

