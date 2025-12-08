#include <unistd.h>
#include <string.h>

#define BUFSIZE 128


static const char welcome[] =
    "Bienvenue dans le Shell ENSEA.\n"
    "Pour quitter, tapez 'exit'.\n";
static const char prompt[] = "enseash % \n";

int main(void) {
    write(STDOUT_FILENO, welcome, strlen(welcome));
    write(STDOUT_FILENO, prompt,  strlen(prompt));
    return 0;
}
