// enseash_q4.c
// Affichage du code de retour (ou du signal) de la commande précédente dans le prompt

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

    int last_status = 0;   // status de la dernière commande
    int has_status  = 0;   // indique si une commande a déjà été exécutée

    print_str(welcome);

    while (1)
    {
        ssize_t n;
        pid_t pid;

        // Construction du prompt en fonction du dernier status
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

        // Ligne vide : on réaffiche juste le prompt au prochain tour
        if (cmd_line[0] == '\0') {
            continue;
        }

        // Commande interne "exit"
        if (strcmp(cmd_line, "exit") == 0) {
            print_str(bye);
            break;
        }

        // Création du processus fils pour exécuter la commande
        pid = fork();

        if (pid == 0) {
            // Processus fils
            char *argv[2];
            argv[0] = cmd_line;  
            argv[1] = NULL;

            execvp(argv[0], argv);

            // Si on arrive ici, execvp a échoué
            _exit(1);
        }
        else if (pid > 0) {
            // Processus père
            // On attend la fin du fils et on met à jour last_status
            waitpid(pid, &last_status, 0);
            has_status = 1;
        }
        else {
            // fork a échoué
            print_str("Erreur : fork a échoué.\n");
        }
    }

    return 0;
}


