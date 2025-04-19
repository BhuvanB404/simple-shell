#include "star.h"
//#include<stdio.h>
void Getcwd(char *buf[1024], size_t size)
    {
      if(NULL == getcwd(buf, size))
        perror("getcwd failed");
      }

char *read_line(void)
    {
          char *buf;
          ssize_t bufsize;
          char *cwd[1024];

          buf = NULL;

          Getcwd(cwd, sizeof(cwd));
          printf(BLUE" %s "RST WHITE "*>" RST ,cwd);


          if(getline(&buf, &bufsize, stdin) == -1)
          {
            buf = NULL;
            if(feof(stdin))
              printf(RED"[EOF]"RST);
            else
              printf(RED"Getline failed"RST);

            printf("\n");

          }

          return buf;
  }

int main(int argc, char** argv)
{

  char *line;
   //TASK: PRINT A BANNER
    //DONT FORGET



    while(line = read_line())
    {
          line = read_line();
          printf("%s", line);

       // getting tokens from
//          pause();


      }

    return EXIT_SUCCESS;
}