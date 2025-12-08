#include <unistd.h>
#include <string.h>

#define BUFSIZE 128
#define MAX_STR_LEN 1024

static const char welcome[] =
    "Bienvenue dans le Shell ENSEA.\n"
    "Pour quitter, tapez 'exit'.\n";
static const char prompt[] = "enseash % \n";

int main(void) {
    write(STDOUT_FILENO, welcome, strnlen(welcome,MAX_STR_LEN));
    write(STDOUT_FILENO, prompt,  strnlen(prompt,MAX_STR_LEN));
    return 0;
}
