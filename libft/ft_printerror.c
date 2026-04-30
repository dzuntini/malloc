#include "libft.h"

void ft_errorPut(char *str) {

    int i = ft_strlen(str);
    write(2, str, i);
}

void ft_errorPutExit(char *str) {

    ft_errorPut(str);
    exit(1);
}

void ft_Perror(char *str, char *pref) {
    char prefix[280] = "";
    ft_strlcat(prefix, pref, sizeof(prefix));
    ft_strlcat(prefix, ": '", sizeof(prefix));
    ft_strlcat(prefix, str, sizeof(prefix));
    ft_strlcat(prefix, "'",sizeof(prefix));
    perror(prefix);
}

void ft_PerrorFd(char *str, int fd, char *pref) {
    close(fd);
    ft_Perror(str, pref);
}
