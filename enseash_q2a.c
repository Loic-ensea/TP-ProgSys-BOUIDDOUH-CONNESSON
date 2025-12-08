#include <unistd.h>
#include <string.h>

#define BUFSIZE 128
#define MAX_STR_LEN 1024


int main(void) 
{
    static const char welcome[] =
    "Bienvenue dans le Shell ENSEA.\n"
    "Pour quitter, tapez 'exit'.\n";
    static const char prompt[] = "enseash % \n";
    
    ssize_t n;
    char buf[BUFSIZE]

    n = read(STDIN_FILENO,buf,BUFSIZE);
    if (n > 0 && buf[n - 1] == '\n') 
    {
        buf[n - 1] = '\0';
    }
    write(STDOUT_FILENO, prompt,  strlen(prompt,MAX_STR_LEN));
}