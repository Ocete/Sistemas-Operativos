
#include <stdio.h>
#include <signal.h>

int main() {
  sigset_t new_mask;

  //Inicializar la nueva mascara con todas las señales
  sigfillset(&new_mask);

  //Le quitamos la señal SIGUSR1 a la máscara
  sigdelset(&new_mask, SIGUSR1);

  //Esperamos a la señal SIGUSR1
  sigsuspend(&new_mask);
}
