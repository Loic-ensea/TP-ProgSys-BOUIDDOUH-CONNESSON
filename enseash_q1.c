#include <unistd.h>
#include <string.h>

#define BUFSIZE 128
#define MAX_STR_LEN 1024

char *welcome[] =
    "Bienvenue dans le Shell ENSEA.\n"
    "Pour quitter, tapez 'exit'.\n";
char *prompt[] = "enseash % \n";

int main(void) {
    write(STDOUT_FILENO, welcome, strlen(welcome,MAX_STR_LEN));
    write(STDOUT_FILENO, prompt,  strlen(prompt,MAX_STR_LEN));
    return 0;
}
