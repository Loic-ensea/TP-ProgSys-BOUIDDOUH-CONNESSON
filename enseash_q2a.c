#include <unistd.h>
#include <string.h>

#define BUFSIZE 128

int main(void) 
{
    ssize_t n;
    char buf[BUFSIZE]

    n = read(STDIN_FILENO,buf,BUFSIZE)
    if (n > 0 && buf[n - 1] == '\n') 
    {
        buf[n - 1] = '\0';
    }
    write
}