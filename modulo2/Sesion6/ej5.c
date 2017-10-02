/*
  Ejercicio 5: Escribir un programa, similar a la orden cp, que utilice para su implementación la
  llamada al sistema mmap() y una función de C que nos permite copiar memoria, como por
  ejemplo memcpy(). Para conocer el tamaño del archivo origen podemos utilizar stat() y para
  establecer el tamaño del archivo destino se puede usar ftruncate().
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

int main (int argc, char *argv[]) {
  struct stat sb;
  char *map1, *map2;
  int fd_read, fd_write;

  //Comprobamos que los argumentos introducidos son correctos
  if (argc != 3) {
    printf("Uso: %s <archivo> <destino>\n", argv[0]);
    exit (EXIT_FAILURE);
  }

  //Abrimos los archivos
  if ((fd_read = open (argv[1], O_RDONLY)) == -1) {
    perror("Error al abrir el origen");
    exit(EXIT_FAILURE);
  }

  if ((fd_write = open(argv[2], O_CREAT|O_TRUNC|O_RDWR, S_IRWXU)) < 0) {
    perror("Error al abrir el destino");
    exit(EXIT_FAILURE);
  }

  //Extraemos los datos del origen y comprobamos que es un directorio
  if (fstat (fd_read, &sb) == -1) {
    perror("Error al hacer stat");
    exit(EXIT_FAILURE);
  }

  if (!S_ISREG (sb.st_mode)) {
    printf("%s no es un archivo regular\n", argv[1]);
    exit(0);
  }

  //Establecemos el tamaño del destino al mismo que el origen
  if (ftruncate(fd_write, sb.st_size) == -1) {
    perror("Error en el ftruncate");
    exit(EXIT_FAILURE);
  }

  //Proyectamos ambos archivos en memoria
  map1 = (char *) mmap (0, sb.st_size, PROT_READ, MAP_SHARED, fd_read, 0);
  if (map1 == MAP_FAILED) {
    perror("Fallo el mapeo del origen");
    exit(EXIT_FAILURE);
  }

  map2 = (char *) mmap (0, sb.st_size, PROT_WRITE, MAP_SHARED, fd_write, 0);
  if (map2 == MAP_FAILED) {
    perror("Fallo el mapeo del destino");
    exit(EXIT_FAILURE);
  }

  //Cerramos ambos archivos
  if (close (fd_read) == -1) {
    perror("Error al cerrar el origen");
    exit(EXIT_FAILURE);
  }

  if (close (fd_write) == -1) {
    perror("Error al cerrar el destino");
    exit(EXIT_FAILURE);
  }

  //Copiamos el origen en el destino
  memcpy(map2, map1, sb.st_size);

  //Cerramos las proyecciones
  if (munmap (map1, sb.st_size) == -1) {
    perror("Error al cerrar la proyeccion del origen");
    exit(EXIT_FAILURE);
  }

  if (munmap (map2, sb.st_size) == -1) {
    perror("Error al cerrar la proyeccion del destino");
    exit(EXIT_FAILURE);
  }

  return 1;
}
