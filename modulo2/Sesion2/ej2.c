#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>

void AlterarPermisos(char pathname[1024], int mask){
  struct stat atributos_antiguos, atributos_nuevos;
  DIR *directorio;
  struct dirent *entrada;
  const char* dir1 = ".";
  const char* dir2 = "..";
  const char* sucess = "Sucess";

  //Abrimos el directorio, obteniendo un puntero a su DIR
  if((directorio = opendir(pathname)) == NULL) {
      perror("\nError la apertura del directorio directorio");
      exit(EXIT_FAILURE);
  }

  //Cambiamos el directorio actual
  if((chdir(pathname)) != 0) {
    perror("\nError en el cambio de directorio");
    exit(EXIT_FAILURE);
  }

  //Leemos la primera entrada de la struct DIR, el primer elemento del directorio
  if((entrada = readdir(directorio)) == NULL) {
    perror("\nError en la lectura del directorio ó directorio vacío.");
    exit(EXIT_FAILURE);
  }

  //Iteramos para todos los archivos del directorio
  while (entrada != NULL){
    //Estudiamos el fichero si es distinto de "." y ".."
    if (strcmp(entrada->d_name, dir1) && strcmp(entrada->d_name, dir2)) {
      //Obtenemos los datos del fichero
    	if(lstat(entrada->d_name, &atributos_antiguos) != 0) {
        perror("\nError en lstat de atributos antiguos");
        exit(EXIT_FAILURE);
      }

      //Cambiamos los permisos del fichero
      if (chmod(entrada->d_name, mask) == 0) {
        //Obtenemos los nuevos datos del fichero para poder imprimir los nuevos permisos
        if(lstat(entrada->d_name, &atributos_nuevos) != 0) {
          perror("\nError en lstat de atributos nuevos");
          exit(EXIT_FAILURE);
        }
        //Mostramos los resultados del cambio
        printf("%s : %d -> %d\n\n", entrada->d_name, atributos_antiguos.st_mode, atributos_nuevos.st_mode);
      } else {
        //Mostramos que el cambio no se ha realizado
        printf("Permisos no cambiados para %s : %d %d\n\n", entrada->d_name, errno, atributos_antiguos.st_mode);
      }
    }

    //Leemos la siguiente entrada de la struct DIR
    //No comprobamos el error porque si es NULL, saldremos del bucle
    entrada = readdir(directorio);
  }

  //Cerramos el directorio
  if (closedir(directorio) != 0){
    perror("\nError en close");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    printf ("Sintaxis de ejecución: %s <directorio> <permisos-en-octal>\n", argv[0]);
    exit(-1);
  } else {
    int permisos = strtol(argv[2], NULL, 8);
    AlterarPermisos(argv[1], permisos);
  }

  return(0);
}
