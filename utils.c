#include "star.h"

void *Malloc(size_t size) {
    void *ptr = malloc(size);
    if(!ptr && size > 0) {
        perror(RED"MALLOC FAILED"RST);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void *Realloc(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if(!new_ptr && size != 0) {
        perror(RED"Realloc failed"RST);
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}