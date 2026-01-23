#include "star.h"

int status = 0;

t_builtin g_builtin[] = {
  { "echo", cmd_echo },
  { "env", cmd_env },
  { "exit", cmd_exit },
  { "tasklist", cmd_tasklist },
  { "forefront", cmd_forefront },
  { "wakeup", cmd_wakeup },
  { NULL, NULL }
};

int cmd_echo( char **args )
{
  int i = 1;
  while ( args[i] ) {
    printf( "%s", args[i] );
    if ( args[i + 1] ) {
      printf( " " );
    }
    i++;
  }
  printf( "\n" );
  return 1;
}

int cmd_env( char **args )
{
  extern char **environ;
  int i = 0;

  (void) args;

  while ( environ[i] ) {
    printf( "%s\n", environ[i] );
    i++;
  }
  return 1;
}

int cmd_exit( char **args )
{
  int exit_status = 0;

  if ( args[1] ) {
    exit_status = atoi( args[1] );
  }

  exit( exit_status );
  return 0;
}

int cmd_tasklist( char **args )
{
  (void) args;
  list_jobs();
  return 1;
}

int cmd_forefront( char **args )
{
  int job_id;

  if ( !args[1] ) {
    fprintf( stderr, "forefront: missing job id\n" );
    fprintf( stderr, "Usage: forefront <job_id>\n" );
    return 1;
  }

  job_id = atoi( args[1] );
  bring_job_to_foreground( job_id );
  return 1;
}

int cmd_wakeup( char **args )
{
  int job_id;

  if ( !args[1] ) {
    fprintf( stderr, "wakeup: missing job id\n" );
    fprintf( stderr, "Usage: wakeup <job_id>\n" );
    return 1;
  }

  job_id = atoi( args[1] );
  send_job_to_background( job_id );
  return 1;
}

int cmd_launch( char **args )
{
  pid_t pid, wpid;
  int stat;
  int is_background = 0;
  int i = 0;
  int j;
  char cmdline[MAX_CMDLINE] = "";

  /* Check for background execution */
  while ( args[i] ) {
    i++;
  }
  if ( i > 1 && strcmp( args[i - 1], "&" ) == 0 ) {
    is_background = 1;
    args[i - 1] = NULL;
    i--;
  }

  /* Build command line for job tracking */
  for ( j = 0; j < i && j < 10; j++ ) {
    if ( j > 0 ) {
      strcat( cmdline, " " );
    }
    strncat( cmdline, args[j], MAX_CMDLINE - strlen( cmdline ) - 1 );
  }

  pid = fork();
  if ( pid == 0 ) {
    if ( !is_background ) {
      signal( SIGINT, SIG_DFL );
      signal( SIGTSTP, SIG_DFL );
    }

    if ( execvp( args[0], args ) == -1 ) {
      fprintf( stderr, RED "Command '%s' not found\n" RST, args[0] );
    }
    exit( EXIT_FAILURE );
  } else if ( pid < 0 ) {
    perror( RED "Fork failed" RST );
  } else {
    if ( is_background ) {
      int job_id = add_job( pid, 1, cmdline );
      printf( "[%d] %d\n", job_id, pid );
    } else {
      tcsetpgrp( STDIN_FILENO, pid );
      do {
        wpid = waitpid( pid, &stat, WUNTRACED );
      } while ( !WIFEXITED( stat ) && !WIFSIGNALED( stat ) && !WIFSTOPPED( stat ) );

      tcsetpgrp( STDIN_FILENO, getpid() );

      if ( WIFSTOPPED( stat ) ) {
        int job_id = add_job( pid, 0, cmdline );
        printf( "\n[%d] Stopped\t%s\n", job_id, cmdline );
      } else if ( WIFEXITED( stat ) ) {
        status = WEXITSTATUS( stat );
      }
    }
  }

  return 1;
}

void cmd_exec( char **args )
{
  int i;
  int has_pipe = 0;

  if ( args[0] == NULL ) {
    return;
  }

  /* Check for pipe */
  for ( i = 0; args[i] != NULL; i++ ) {
    if ( strcmp( args[i], "|" ) == 0 ) {
      has_pipe = 1;
      break;
    }
  }

  if ( has_pipe ) {
    execute_pipeline( args );
    return;
  }

  for ( i = 0; g_builtin[i].b_name != NULL; i++ ) {
    if ( strcmp( args[0], g_builtin[i].b_name ) == 0 ) {
      g_builtin[i].foo( args );
      return;
    }
  }

  cmd_launch( args );
}

void execute_pipeline( char **args )
{
  char **cmds[MAX_PIPE_CMDS];
  int num_cmds = 0;
  int i;
  int pipefd[2];
  pid_t pid;
  int fd_in = 0;

  /* Split commands by pipe */
  cmds[num_cmds++] = &args[0];
  for ( i = 0; args[i] != NULL; i++ ) {
    if ( strcmp( args[i], "|" ) == 0 ) {
      args[i] = NULL;
      cmds[num_cmds++] = &args[i + 1];
      if ( num_cmds >= MAX_PIPE_CMDS ) {
        fprintf( stderr, "Too many pipes\n" );
        return;
      }
    }
  }

  /* Execute pipeline */
  for ( i = 0; i < num_cmds; i++ ) {
    if ( cmds[i][0] == NULL ) {
      fprintf( stderr, "Invalid pipe syntax\n" );
      return;
    }

    if ( i < num_cmds - 1 ) {
      if ( pipe( pipefd ) == -1 ) {
        perror( "pipe failed" );
        return;
      }
    }

    pid = fork();
    if ( pid == -1 ) {
      perror( "fork failed" );
      return;
    } else if ( pid == 0 ) {
      /* Child process */
      if ( fd_in != 0 ) {
        dup2( fd_in, STDIN_FILENO );
        close( fd_in );
      }

      if ( i < num_cmds - 1 ) {
        close( pipefd[0] );
        dup2( pipefd[1], STDOUT_FILENO );
        close( pipefd[1] );
      }

      execvp( cmds[i][0], cmds[i] );
      fprintf( stderr, RED "Command '%s' not found\n" RST, cmds[i][0] );
      exit( EXIT_FAILURE );
    } else {
      /* Parent process */
      if ( fd_in != 0 ) {
        close( fd_in );
      }

      if ( i < num_cmds - 1 ) {
        close( pipefd[1] );
        fd_in = pipefd[0];
      }
    }
  }

  /* Wait for all children */
  for ( i = 0; i < num_cmds; i++ ) {
    wait( NULL );
  }
}
