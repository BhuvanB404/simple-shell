#include "star.h"

int status = 0;

t_builtin g_builtin[] = {
    {"echo", cmd_echo},
    {"env", cmd_env},
    {"exit", cmd_exit},
    {NULL, NULL}
};

int cmd_echo(char **args) {
    int i = 1;
    while (args[i]) {
        printf("%s", args[i]);
        if (args[i + 1])
            printf(" ");
        i++;
    }
    printf("\n");
    return 1;
}

int cmd_env(char **args) {
    extern char **environ;
    int i = 0;
    
    (void)args;
    
    while (environ[i]) {
        printf("%s\n", environ[i]);
        i++;
    }
    return 1;
}

int cmd_exit(char **args) {
    int exit_status = 0;
    
    if (args[1]) {
        exit_status = atoi(args[1]);
    }
    
    exit(exit_status);
    return 0;
}

int cmd_launch(char **args) {
    pid_t pid, wpid;
    int stat;
    
    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror(RED"Command execution failed"RST);
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror(RED"Fork failed"RST);
    } else {
        do {
            wpid = waitpid(pid, &stat, WUNTRACED);
        } while (!WIFEXITED(stat) && !WIFSIGNALED(stat));
        
        if (WIFEXITED(stat)) {
            status = WEXITSTATUS(stat);
        }
    }
    
    return 1;
}

void cmd_exec(char **args) {
    int i;
    
    if (args[0] == NULL) {
        return;
    }
    
    for (i = 0; g_builtin[i].b_name != NULL; i++) {
        if (strcmp(args[0], g_builtin[i].b_name) == 0) {
            g_builtin[i].foo(args);
            return;
        }
    }
    
    cmd_launch(args);
}
