/*
  Programa maestro. Se le pasan los extremos de un intervalo como parámetros.
  Crea dos procesos, cambia su salida estándar por un pipe (distinto para
  cada proceso) y ejecutan el programa esclavo, el primero con la primera
  mitad del intervalo introducido y el segundo, con la segunda mitad.
  Por último, recoge los datos de los procesos hijos a través de sendos
  pipes y los imprime por pantalla.
*/

#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<math.h>
#include<sys/wait.h>
#define RUTA "./esclavo"

int main (int argc, char *argv[]) {
  int inicio, final;

  if (argc != 3) {
    printf("Modo de empleo: maestro <inicio-del-intervalo> <final-del-intervalo>\n");
    exit(0);
  }

  char lectura;
  int fd1[2], fd2[2], bytes_leidos;
  pid_t PID;
  inicio = atoi(argv[1]);
  final  = atoi(argv[2]);
  int p_medio = (final - inicio) / 2;

  //Pasamos el punto medio a char para poder pasarlo a la función exec().
  char char_p_medio[10];
  if (sprintf(char_p_medio, "%d", p_medio) < 0) {
    printf("Error en sprintf.\n");
    exit(1);
  }

  if (inicio >= final) {
    printf("Error en la definición del intervalo.\n");
    exit(0);
  }

  //Creamos el primer pipe
  if (pipe(fd1) == -1) {
    perror("Pipe 1");
    exit(1);
  }

  if ((PID= fork())<0) {
    perror("Fork 1");
    exit(1);
  }

  if(PID == 0) { // Hijo 1
    //Cerramos el descriptor de lectura del primer pipe en el proceso hijo 1
    close(fd1[0]);

    //Duplicar el descriptor de escritura del primer pipe en el descriptor
    //correspondiente a la salida estándar (stdout)
    dup2(fd1[1],STDOUT_FILENO);

    //Ejecutamos el programa 'esclavo' con el hijo 1
    if (execl(RUTA, "esclavo", argv[1], char_p_medio, NULL) == -1 ) {
      perror("Exec");
      exit(1);
    }
  } else {

    //Cerramos el descriptor de escritura del primer pipe en los procesos
    //hijo 2 y padre.
    close(fd1[1]);

    //Creamos el segundo pipe
    if (pipe(fd2) == 1) {
      perror("Pipe 2");
      exit(1);
    }

    if ( (PID= fork())<0) {
      perror("Fork 2");
      exit(1);
    }

    if (PID == 0) { //Hijo 2
      //Cerramos los decriptores de lectura del primer y segundo pipe.
      close(fd1[0]);
      close(fd2[0]);

      //Duplicar el descriptor de escritura del segundo pipe en el descriptor
      //correspondiente a la salida estándar (stdout).
      dup2(fd2[1],STDOUT_FILENO);

      //Evitamos que el ambos procesos comprueben el valor medio del intervalo
      p_medio++;
      sprintf(char_p_medio, "%d", p_medio);

      //Ejecutamos el programa 'esclavo' con el hijo 2
      if (execl(RUTA, "esclavo", char_p_medio, argv[2], NULL) == -1 ) {
        perror("Exec");
        exit(1);
      }

    } else { //Padre
      //Cerrar el descriptor de escritura del segundo pipe
      close(fd2[1]);

      printf("Primos entre %d y %d:\n", inicio, final);

      //Imprimimos los resultados del primer proceso
      while (bytes_leidos = read(fd1[0], &lectura, sizeof(lectura)) > 0) {
        if (bytes_leidos == -1) {
          perror("Read");
          exit(1);
        }
        printf("%c", lectura);
      }

      //Imprimimos los resultados del segundo proceso
      while (bytes_leidos = read(fd2[0], &lectura, sizeof(lectura)) > 0) {
        if (bytes_leidos == -1) {
          perror("Read");
          exit(1);
        }
        printf("%c", lectura);
      }
    }
  }

  //El padre espera a los dos hijos para que no queden en estado ZOMBIE
  wait(NULL);
  wait(NULL);
  return(0);
}
