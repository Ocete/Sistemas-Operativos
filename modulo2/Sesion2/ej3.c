#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>

struct pareja{
  int num_archivos;
  unsigned long long int size;
};

struct pareja Recorre(char pathname[1024], struct pareja pareja){
  struct stat atributos;
  DIR *directorio;
  struct dirent *entrada;
  const char* dir1 = ".";
  const char* dir2 = "..";

  //Abrimos el directorio, obteniendo un puntero a su DIR
  if((directorio = opendir(pathname)) == NULL){
      perror("\nError la apertura del directorio directorio");
      exit(EXIT_FAILURE);
  }

  //Cambiamos el directorio en el que estamos
  if((chdir(pathname)) != 0){
    perror("\nError en el cambio de directorio");
    exit(EXIT_FAILURE);
  }

  //Iteramos en el directorio actual
  while ((entrada = readdir(directorio)) != NULL){
    //Estudiamos los archivos que no son ni "." ni ".."
    if(strcmp(entrada->d_name, dir1) && strcmp(entrada->d_name, dir2)){
      //Obtenemos los datos del archivo
      if(lstat(entrada->d_name, &atributos) != 0){
        perror("\nError en lstat de atributos antiguos");
        exit(EXIT_FAILURE);
      }
      //Si se trata de un fichero, entramos en el archivo y lo estudiamos
      if ((atributos.st_mode & S_IFMT) == S_IFDIR)
        pareja = Recorre(entrada->d_name, pareja);
      else{
        //Incrementamos el contador de archivos si los permisos encajan
        //Aumentamos el tamaño total e imprimimos los datos de este archivo
        if ((atributos.st_mode & S_IXGRP) && (atributos.st_mode & S_IXOTH))
          ++pareja.num_archivos;
        pareja.size += atributos.st_size;
        printf("%s %d\n\n", entrada->d_name, entrada->d_ino);
      }
    }
  }

  //Volvemos al directorio padre
  if((chdir(dir2)) != 0){
    perror("\nError en el regreso al directorio padre");
    exit(EXIT_FAILURE);
  }

  //Devolvemos la pareja donde almacenamos todos los datos
  return pareja;
}

int main(int argc, char *argv[]){

  if (argc < 2){
    printf ("Sintaxis de ejecución: %s <directorio> \n", argv[0]);
    exit(-1);
  }
  else{
    //Creamos la struct donde almacenaremos los datos y la inicializamos
    struct pareja pareja;
    pareja.num_archivos = 0;
    pareja.size = 0;
    //Llamamos a la función e imprimimos los resultados
    printf("Los i-nodos son:\n\n");
    pareja = Recorre(argv[1], pareja);
    printf("Existen %d archivos regulares con permiso x para grupo y otros\n\n", pareja.num_archivos);
    printf("El tamaño total ocupado por dichos archivos es %u bytes\n\n", pareja.size);
  }

  return(0);
}
