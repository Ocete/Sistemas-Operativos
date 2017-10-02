/*
  Implementa un programa en C que tenga como argumento un número entero. Este
  programa debe crear un proceso hijo que se encargará de comprobar si dicho número es un
  número par o impar e informará al usuario con un mensaje que se enviará por la salida estándar.
  A su vez, el proceso padre comprobará si dicho número es divisible por 4, e informará si lo es o
  no usando igualmente la salida estándar.
*/

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  pid_t pid;
  int num = atoi(argv[1]);

  if (argc != 2) {
    printf("Uso del programa: ej1 <número>\n");
    exit(-1);
  }

  if ((pid = fork()) < 0) {
    perror("\nError en el fork");
    exit(-1);
  } else if(pid == 0) { //Proceso hijo
    if ((num % 2) == 0)
      printf("El número introducido es par.\n");
    else
      printf("El número introducido es impar.\n");
  } else { //Proceso padre
    if (num % 4 == 0)
      printf("El número introducido es divisible por 4.\n");
    else
      printf("El número introducido no es divisible por 4.\n");

      if (wait(NULL) == -1) {
        printf("Error en wait.");
        exit(-1);
      }
  }

  return 0;
}
