#include "jobs.h"
#include "star.h"
#include <signal.h>
#include <sys/wait.h>

/*
 *  RTEMS Coding Style:
 *  - 2 spaces indentation
 *  - No tabs
 *  - Function names: lower_case_with_underscores
 */

static job_t jobs[MAX_JOBS];
static int next_job_id = 1;

void init_jobs( void )
{
  memset( jobs, 0, sizeof( jobs ) );
}

int add_job( pid_t pid, int is_background, const char *cmdline )
{
  int i;

  for ( i = 0; i < MAX_JOBS; i++ ) {
    if ( jobs[i].pid == 0 ) {
      jobs[i].pid = pid;
      jobs[i].job_id = next_job_id++;
      jobs[i].is_background = is_background;
      jobs[i].is_stopped = 0;
      strncpy( jobs[i].cmdline, cmdline, sizeof( jobs[i].cmdline ) - 1 );
      jobs[i].cmdline[sizeof( jobs[i].cmdline ) - 1] = '\0';
      return jobs[i].job_id;
    }
  }
  return -1;
}

int remove_job( pid_t pid )
{
  int i;

  for ( i = 0; i < MAX_JOBS; i++ ) {
    if ( jobs[i].pid == pid ) {
      jobs[i].pid = 0;
      return 0;
    }
  }
  return -1;
}

void list_jobs( void )
{
  int i;

  for ( i = 0; i < MAX_JOBS; i++ ) {
    if ( jobs[i].pid != 0 ) {
      const char *status_str = jobs[i].is_stopped ? "Stopped" : "Running";
      printf( "[%d] %s\t%s\n", jobs[i].job_id, status_str, jobs[i].cmdline );
    }
  }
}

void bring_job_to_foreground( int job_id )
{
  int i;
  int stat;

  for ( i = 0; i < MAX_JOBS; i++ ) {
    if ( jobs[i].job_id == job_id ) {
      if ( jobs[i].is_stopped ) {
        kill( jobs[i].pid, SIGCONT );
        jobs[i].is_stopped = 0;
      }
      tcsetpgrp( STDIN_FILENO, jobs[i].pid );
      waitpid( jobs[i].pid, &stat, WUNTRACED );
      tcsetpgrp( STDIN_FILENO, getpid() );

      if ( WIFSTOPPED( stat ) ) {
        jobs[i].is_stopped = 1;
        printf( "\n[%d] Stopped\t%s\n", jobs[i].job_id, jobs[i].cmdline );
      } else {
        remove_job( jobs[i].pid );
      }
      return;
    }
  }
  fprintf( stderr, "forefront: no such job [%d]\n", job_id );
}

void send_job_to_background( int job_id )
{
  int i;

  for ( i = 0; i < MAX_JOBS; i++ ) {
    if ( jobs[i].job_id == job_id ) {
      if ( jobs[i].is_stopped ) {
        kill( jobs[i].pid, SIGCONT );
        jobs[i].is_stopped = 0;
        jobs[i].is_background = 1;
        printf( "[%d] %s &\n", jobs[i].job_id, jobs[i].cmdline );
      } else {
        fprintf( stderr, "wakeup: job [%d] already running\n", job_id );
      }
      return;
    }
  }
  fprintf( stderr, "wakeup: no such job [%d]\n", job_id );
}

int get_next_job_id( void )
{
  return next_job_id;
}

job_t *find_job_by_id( int job_id )
{
  int i;

  for ( i = 0; i < MAX_JOBS; i++ ) {
    if ( jobs[i].job_id == job_id && jobs[i].pid != 0 ) {
      return &jobs[i];
    }
  }
  return NULL;
}

void update_job_status( void )
{
  int i;
  int stat;
  pid_t pid;

  for ( i = 0; i < MAX_JOBS; i++ ) {
    if ( jobs[i].pid != 0 ) {
      pid = waitpid( jobs[i].pid, &stat, WNOHANG | WUNTRACED );
      if ( pid > 0 ) {
        if ( WIFEXITED( stat ) || WIFSIGNALED( stat ) ) {
          printf( "[%d] Done\t%s\n", jobs[i].job_id, jobs[i].cmdline );
          remove_job( jobs[i].pid );
        } else if ( WIFSTOPPED( stat ) ) {
          jobs[i].is_stopped = 1;
        }
      }
    }
  }
}
