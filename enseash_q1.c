#include <unistd.h>
#include <string.h>

static const char welcome[] =
    "Bienvenue dans le Shell ENSEA.\n"
    "Pour quitter, tapez 'exit'.\n";
static const char prompt[] = "enseash % ";

int main(void) {
    write(STDOUT_FILENO, welcome, sizeof(welcome));
    write(STDOUT_FILENO, prompt,  sizeof(prompt));
    return 0;
}
