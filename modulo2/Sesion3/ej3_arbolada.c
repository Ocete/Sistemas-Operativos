/*
  for (i=1; i < nprocs; i++) {
    if ((childpid= fork()) == -1) {
      fprintf(stderr, "Could not create child %d: %s\n",i,strerror(errno));
      exit(-1);
    }
    if (!childpid)
      break;
  }
*/
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  pid_t childpid, daddypid, mypid;
  const int nprocs = 20;
  int i;

  for (i=1; i < nprocs; i++) {
    if ((childpid = fork()) == -1) {
      fprintf(stderr, "Could not create child %d: %s\n",i,strerror(errno));
      exit(-1);
    }

    if (!childpid)
      break;
  }


  if (childpid != 0){
    for(int contador = 1; contador < nprocs; contador++)  {
      if (wait(NULL) == -1) {
        printf("Error en wait. Contador: %d\n", contador);
        exit(-1);
      }
    }
  }


  mypid =  getpid();
  daddypid = getppid();
  printf ("PID --> %d PPID --> %d\n", mypid, daddypid);

  return 0;
}
