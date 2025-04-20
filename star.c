#include "star.h"

char **line_splitting(char *line) {
    char **tokens;
    unsigned int position = 0;
    size_t bufsize = BUFSIZ;
    
    tokens = Malloc(bufsize * sizeof(char*));
    for (char *token = strtok(line, DEL); token; token = strtok(NULL, DEL)) {
        tokens[position++] = token;
        if (position >= bufsize) {
            bufsize *= 2;
            tokens = Realloc(tokens, bufsize * sizeof(char*));
        }
    }
    tokens[position] = NULL;
    return tokens;
}

void Getcwd(char *buf, size_t size) {
    if (NULL == getcwd(buf, size))
        perror("getcwd failed");
}

char *read_line(void) {
    char *buf = NULL;
    size_t bufsize = 0; // getline will allocate as needed
    char cwd[PATH_MAX];
    
    Getcwd(cwd, sizeof(cwd));
    printf(BLUE" %s "RST WHITE"*> "RST, cwd);
    
    if (getline(&buf, &bufsize, stdin) == -1) {
        free(buf);
        if (feof(stdin)) {
            printf(RED"[EOF]\n"RST);
            exit(EXIT_SUCCESS);
        }
        perror(RED"Getline failed"RST);
        exit(EXIT_FAILURE);
    }
    return buf;
}

int main() {
    char *line;
    while ((line = read_line())) {
        char **args = line_splitting(line);
        
        // Uncomment for debugging
        /*
        for (int i = 0; args[i]; i++) {
            printf("[%d]: %s\n", i, args[i]);
        }
        */
        
        cmd_exec(args);
        free(line);
        free(args);
    }
    return EXIT_SUCCESS;
}
