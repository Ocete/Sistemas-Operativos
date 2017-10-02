/*
  Ejercicio 3. Construir un programa que verifique que, efectivamente, el
  kernel comprueba que puede darse una situación de interbloqueo en el
   bloqueo de archivos.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include<sys/wait.h>

int main (int argc, char * argv[]) {
  int fd;
  pid_t PID;
  struct flock cerrojo1, cerrojo2;
  char* archivo = "temporal";

  //Inicializamos los cerrojos
  cerrojo1.l_type = F_WRLCK;
  cerrojo1.l_whence = SEEK_SET;
  cerrojo1.l_start = 100;
  cerrojo1.l_len = 100;

  cerrojo2 = cerrojo1;
  cerrojo2.l_start = 300;

  //Abrimos (o creamos en su defecto) un archivo temporal
  if ((fd = open(archivo, O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR)) < 0) {
    perror("\nError en open");
    exit(EXIT_FAILURE);
  }

  //Realizamos un fork
  if ((PID = fork()) < 0) {
    perror("Error en fork");
    exit(EXIT_FAILURE);
  }

  if(PID == 0) { // Hijo
    printf("\nEl proceso hijo bloquea el archivo entre 100-200 bytes.\n");

    //Creamos un primer cerrojo entre 100-200 bytes
    if (fcntl(fd, F_SETLKW, &cerrojo1) == -1) {
      perror("Error en fcntl");
      exit(EXIT_FAILURE);
    }

    sleep(1);

    //Intentamos crear un segundo cerrojo entre 300-400 bytes
    //Este cerrojo deberia pisarse con el primer cerrojo creado por el padre
    if (fcntl(fd, F_SETLKW, &cerrojo2) == -1) {
      if (errno == EDEADLK) {
        printf("\nSe ha producido interbloqueo y el proceso hijo ha sido"
          " desbloqueado por el kernel.\n");
      } else {
        perror("Error en fcntl");
        exit(EXIT_FAILURE);
      }
    }

    sleep(1);
    printf("\nMuerte del proceso hijo.\n");

  } else { //Padre
    printf("\nEl proceso padre bloquea el archivo entre 300-400 bytes.\n");

    //Creamos un primer cerrojo entre 300-400 bytes
    if (fcntl(fd, F_SETLKW, &cerrojo2) == -1) {
      perror("Error en fcntl");
      exit(EXIT_FAILURE);
    }

    sleep(1);

    //Intentamos crear un segundo cerrojo entre 100-200 bytes
    //Este cerrojo deberia pisarse con el primer cerrojo creado por el hijo
    if (fcntl(fd, F_SETLKW, &cerrojo1) == -1) {
      if (errno == EDEADLK) {
        printf("\nSe ha producido interbloqueo y el proceso padre ha sido"
          " desbloqueado por el kernel.\n");
      } else {
        perror("Error en fcntl");
        exit(EXIT_FAILURE);
      }
    }

    sleep(1);
    wait(NULL);
    printf("\nMuerte del proceso padre.\n");
  }

  return(0);
}
