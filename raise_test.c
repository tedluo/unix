//-----------------------------------------------------------
//raise a signal to it self
//The  raise()  function sends a signal to the calling process or
//thread.In a single-threaded program it is equivalent to
//			kill(getpid(), sig);
//In a multithreaded program it is equivalent to
//           pthread_kill(pthread_self(), sig);
//-----------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char **argv)
{
  int i;

  for (i = 0; i < 1000; i++)
  {
    fprintf(stdout, "%d", i);
    fflush(stdout);

    if (i > 3)
    {
      //int raise(int sig);
      raise(SIGKILL);
    }

    sleep(1);
  }

  return 0;
}
