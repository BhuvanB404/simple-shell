#ifndef STAR_H
#define STAR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <linux/limits.h>

#define DEL " \t\r\n"
// #define BUFSIZ 1024

// Colors
#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define WHITE "\033[1;37m"
#define RST "\033[0m"

typedef struct s_builtin
{
    const char *b_name;
    int(*foo)(char **);
} t_builtin;

int cmd_echo(char **args);
int cmd_env(char **args);
int cmd_exit(char **args);
void cmd_exec(char **args);
int cmd_launch(char **args);



void *Malloc(size_t size);
void *Realloc(void *ptr, size_t size);


extern t_builtin g_builtin[];
extern int status;

#endif