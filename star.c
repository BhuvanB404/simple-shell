#include "star.h"

/*
 *  RTEMS Coding Style:
 *  - 2 spaces indentation
 *  - No tabs
 *  - Function names: lower_case_with_underscores
 *  - control statements: space after keyword (if, while, for)
 */

static void sigint_handler( int sig )
{
  (void) sig;
  printf( "\n" );
}

static void sigtstp_handler( int sig )
{
  (void) sig;
  /* Ignore Ctrl+Z in shell itself */
}

void setup_signal_handlers( void )
{
  signal( SIGINT, sigint_handler );
  signal( SIGTSTP, sigtstp_handler );
  signal( SIGCHLD, SIG_IGN );
}

static void get_cwd( char *buf, size_t size )
{
  if ( NULL == getcwd( buf, size ) ) {
    perror( "getcwd failed" );
  }
}

char *read_line( void )
{
  char *buf = NULL;
  size_t bufsize = 0;
  char cwd[PATH_MAX];

  get_cwd( cwd, sizeof( cwd ) );
  printf( BLUE " %s " RST WHITE "*> " RST, cwd );

  if ( getline( &buf, &bufsize, stdin ) == -1 ) {
    free( buf );
    if ( feof( stdin ) ) {
      printf( RED "[EOF]\n" RST );
      exit( EXIT_SUCCESS );
    }
    perror( RED "Getline failed" RST );
    exit( EXIT_FAILURE );
  }
  return buf;
}

/*
 *  Custom tokenizer to handle quotes and whitespace
 */
char **tokenize_input( char *line )
{
  int bufsize = BUFSIZ;
  int position = 0;
  char **tokens = Malloc( bufsize * sizeof( char* ) );
  char *token;
  char *cursor = line;

  if ( !tokens ) {
    fprintf( stderr, "start: allocation error\n" );
    exit( EXIT_FAILURE );
  }

  while ( *cursor != '\0' ) {
    /* Skip whitespace */
    while ( *cursor == ' ' || *cursor == '\t' || *cursor == '\n' || *cursor == '\r' ) {
      *cursor = '\0';
      cursor++;
    }

    if ( *cursor == '\0' ) {
      break;
    }

    token = cursor;
    if ( *cursor == '"' || *cursor == '\'' ) {
      char quote = *cursor;
      cursor++;
      token = cursor; /* Start of actual token content */
      while ( *cursor != '\0' && *cursor != quote ) {
        cursor++;
      }
      if ( *cursor == '\0' ) {
        fprintf( stderr, "Error: Unclosed quote\n" );
        free( tokens );
        return NULL;
      }
      *cursor = '\0'; /* Terminate token at closing quote */
      cursor++;
    } else if ( *cursor == '|' || *cursor == '&' ) {
      /* Special single-character tokens */
      cursor++;
      if ( *cursor != '\0' && *cursor != ' ' && *cursor != '\t' && *cursor != '\n' && *cursor != '\r' ) {
        *cursor = '\0';
        cursor++;
      }
    } else {
      while ( *cursor != '\0' && *cursor != ' ' && *cursor != '\t' && *cursor != '\n' && *cursor != '\r' && *cursor != '|' && *cursor != '&' ) {
        cursor++;
      }
    }

    tokens[position] = token;
    position++;

    if ( position >= bufsize ) {
      bufsize += BUFSIZ;
      tokens = Realloc( tokens, bufsize * sizeof( char* ) );
      if ( !tokens ) {
        fprintf( stderr, "star: allocation error\n" );
        exit( EXIT_FAILURE );
      }
    }
  }

  tokens[position] = NULL;
  return tokens;
}

int main( void )
{
  char *line;
  char **args;

  init_jobs();
  setup_signal_handlers();

  while ( ( line = read_line() ) ) {
    /* Update job status before prompt */
    update_job_status();

    /* Skip empty lines check is implicitly handled by tokenizer returning empty list or loops */
    if ( line[0] == '\n' && line[1] == '\0' ) {
      free( line );
      continue;
    }

    args = tokenize_input( line );

    if ( args != NULL ) {
      if ( args[0] != NULL ) {
        cmd_exec( args );
      }
      free( args );
    }
    free( line );
  }
  return EXIT_SUCCESS;
}
