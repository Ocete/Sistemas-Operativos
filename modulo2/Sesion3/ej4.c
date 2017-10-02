/*
  Implementa un programa que lance cinco procesos hijo. Cada uno de ellos se
  identificará en la salida estándar, mostrando un mensaje del tipo Soy el hijo PID. El proceso
  Guía Práctica de Sistemas Operativos-97padre simplemente tendrá que esperar la finalización de todos sus hijos y cada vez que detecte
  la finalización de uno de sus hijos escribirá en la salida estándar un mensaje del tipo:
  Acaba de finalizar mi hijo con <PID>
  Sólo me quedan <NUM_HIJOS> hijos vivos
*/
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

int main() {
  const int nprocs_creados = 5;
  int i, nprocs_actuales;
  pid_t childpid;

  for (nprocs_actuales=0; nprocs_actuales<nprocs_creados; nprocs_actuales++) {
    if ((childpid = fork())== -1) {
      fprintf(stderr, "Could not create child %d: %s\n",i,strerror(errno));
      exit(-1);
    }

    if (childpid == 0) {
      printf("Soy el hijo con PID %d\n\n", getpid());
      break;
    }
  }

  if (childpid != 0) {
    while (nprocs_actuales > 0) {
      if ((childpid = wait(NULL)) == -1) {
        printf("Error en wait. Procesos restantes: %d\n", nprocs_actuales);
        exit(-1);
      }

      nprocs_actuales--;

      printf("Acaba de finalizar mi hijo con PID %d\n"
        "Sólo me quedan %d hijos vivos\n\n",
        childpid, nprocs_actuales);
    }

  }

  return(0);
}
