#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
  const int TAMANIO_BLOQUE = 1024, TAMANIO_NOMBRE = 50;
  int tam_lectura, fdbloqueo;
  char bloque[TAMANIO_BLOQUE], nombrefifo[TAMANIO_NOMBRE];
  pid_t mypid = getpid();
  FILE *tmp;
  struct flock cerrojo;

  if (argc != 2) {
    dprintf(STDERR_FILENO, "Error en proxy: número de argumentos incorrecto.\n");
    exit(EXIT_FAILURE);
  }
  fdbloqueo = atoi(argv[1]);

  //Inicializamos el cerrojo para que abarque el archivo completo
  cerrojo.l_whence = SEEK_SET;
  cerrojo.l_start = 0;
  cerrojo.l_len = 0;

  //Creamos el archivo temporal
  printf("Proxy %d: Crea archivo temporal.\n", mypid);
  if ((tmp = tmpfile()) == NULL) {
    perror("\nProxy: error al crear el archivo temporal");
    exit(EXIT_FAILURE);
  }

  //Leemos los datos del cliente a través del fifo
  printf("Proxy %d: Comienza lectura.\n", mypid);

  while ((tam_lectura = read(STDIN_FILENO, &bloque, TAMANIO_BLOQUE)) > 0) {
    if (fwrite(&bloque, sizeof(char), tam_lectura, tmp) != tam_lectura) {
      dprintf(2, "Proxy %d: error en la lectura de datos del fifo.\n", mypid);
      exit(EXIT_FAILURE);
    }
  }
  if (tam_lectura == -1) {
    perror("\nError de lectura en proxy");
    exit(EXIT_FAILURE);
  }

  //Colocamos el cerrojo
  cerrojo.l_type = F_WRLCK;
  if (fcntl(fdbloqueo, F_SETLKW, &cerrojo) == -1) {
    perror("\nProxy: error al colocar el cerrojo");
    exit(EXIT_FAILURE);
  }

  //Imprimimos los datos por pantalla
  printf("\nProxy %d: Comienza impresión.\n",mypid);

  rewind(tmp);
  while ((tam_lectura = fread(&bloque, sizeof(char), TAMANIO_BLOQUE, tmp)) > 0) {
    if (write(STDOUT_FILENO, &bloque, tam_lectura) == -1) {
      perror("\nError de escritura en proxy");
      exit(EXIT_FAILURE);
    }
  }

  //Liberamos el cerrojo
  cerrojo.l_type = F_UNLCK;
  if (fcntl(fdbloqueo, F_SETLKW, &cerrojo) == -1) {
    perror("\nProxy: error al liberar el cerrojo");
    exit(EXIT_FAILURE);
  }

  //Cerramos el fifo del proxy
  sprintf(nombrefifo, "fifo.%d", mypid);
  if (unlink (nombrefifo) == -1) {
    perror("\nError al destruir el fifo del proxy");
    exit(EXIT_FAILURE);
  }

printf("Proxy: termina ejecución %d\n", mypid);
  //El archivo temporal se destruye automáticamente al terminar el programa
  return 0;
}
