/*
  Ejercicio 2. Escribe un programa en C llamado contador, tal que cada vez que reciba una
  señal que se pueda manejar, muestre por pantalla la señal y el número de veces que se ha
  recibido ese tipo de señal, y un mensaje inicial indicando las señales que no puede manejar. En el
  cuadro siguiente se muestra un ejemplo de ejecución del programa.
*/

#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

int cont[30];

static void handler(int sigNum) {
  printf("La señal %d se ha recibido %d veces.\n", sigNum, ++cont[sigNum - 1]);
}

int main(int argc, char *argv[]) {
  struct sigaction sig_user_action;

  printf ("No puedo manejar las señales %d y %d.\n", SIGKILL, SIGSTOP);
  printf ("Introduzca %d para finalizar el proceso.\n", SIGKILL);

  //Inicializamos los contadores a 0.
  for (int i=0; i<30; i++)
    cont[i] = 0;

  //Establecemos el manejador de señales.
  sig_user_action.sa_handler = handler;
  //Establecemos la máscara de bloqueo cómo vacía.
  sigemptyset (&sig_user_action.sa_mask);
  // No establecemos ninguna opción mediante flags.
  sig_user_action.sa_flags = 0;

  //Establecemos las señales para las que actuará nuestro manejador (todas
  // excepto SIGKILL y SIGSTOP).
  for (int sig = 1; sig<32; sig++) {
    if (sig != SIGKILL && sig != SIGSTOP)
      if (sigaction(sig, &sig_user_action, NULL) <0) {
        printf("%d\n", sig);
        perror("\nError al intentar establecer el manejador de se�al para SIGUSR1");
        exit(-1);
      }
  }

  while(1);
}
