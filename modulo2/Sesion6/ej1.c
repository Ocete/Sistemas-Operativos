/*
  Ejercicio 1. Implementa un programa que admita t argumentos. El primer argumento será una
  orden de Linux; el segundo, uno de los siguientes caracteres “<” o “>”, y el tercero el nombre de
  un archivo (que puede existir o no). El programa ejecutará la orden que se especifica como
  argumento primero e implementará la redirección especificada por el segundo argumento hacia
  el archivo indicado en el tercer argumento. Por ejemplo, si deseamos redireccionar la entrada
  estándar de sort desde un archivo temporal, ejecutaríamos:
  $> ./mi_programa sort “<” temporal
  Nota. El carácter redirección (<) aparece entre comillas dobles para que no los interprete el
  shell sino que sea aceptado como un argumento del programa mi_programa.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main (int argc, char *argv[]) {
  int fd_new, fd_closing;
  const char ID_ENTRADA = '<', ID_SALIDA = '>';
  char pathname[1024];

  //Comprobamos que los parámetros son correctos
  if (argc != 4 || argv[2][0] != ID_ENTRADA && argv[2][0] != ID_SALIDA) {
    printf("Uso del programa: ./ej1 <orden> <caracter '%c' o '%c'> <archivo>\n",
      ID_ENTRADA, ID_SALIDA);
    exit(-1);
  }

  //Abrimos el archivo y obtenemos su fd
  strcpy(pathname,argv[3]);
  if ((fd_new = open(pathname, O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR)) < 0) {
    perror("\nError en open");
    exit(EXIT_FAILURE);
  }

  //Cerrar el descriptor que corresponde
  if (argv[2][0] == ID_ENTRADA)
    fd_closing = 0;
  else
    fd_closing = 1;
  close(fd_closing);

  //Duplicamos el descriptor de archivo en la entrada correspondiente
  if (fcntl(fd_new, F_DUPFD, fd_closing) == -1 ) {
    perror("\nFallo en fcntl");
    exit(-1);
  }

  execlp(argv[1], argv[1], NULL);
}
