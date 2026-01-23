#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

#define MAX_JOBS 64
#define MAX_CMDLINE 1024

typedef struct {
  pid_t pid;
  int job_id;
  int is_background;
  int is_stopped;
  char cmdline[MAX_CMDLINE];
} job_t;

void init_jobs( void );
int add_job( pid_t pid, int is_background, const char *cmdline );
int remove_job( pid_t pid );
void list_jobs( void );
void bring_job_to_foreground( int job_id );
void send_job_to_background( int job_id );
int get_next_job_id( void );
job_t *find_job_by_id( int job_id );
void update_job_status( void );

#endif
