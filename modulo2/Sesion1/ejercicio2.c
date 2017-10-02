#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


int main(int argc, char *argv[]){
	int fdread, fdwrite, read_length;
	const int BLOCK_SIZE = 80;
	char block[BLOCK_SIZE];
	char pathname[1024];

	if(argc < 2)
		fdread = STDIN_FILENO;
	else{
		strcpy(pathname,argv[1]);
		if((fdread=open(pathname,O_RDONLY))<0) {
			perror("\nError en open 1");
			exit(EXIT_FAILURE);
		}
	}

	//Revisar el modo de acceso, el priemro solo lo abrimos para leer y el segundo solo pa estudiar.

	if((fdwrite=open("salida.txt",O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR))<0) {
		perror("\nError en open 2");
		exit(EXIT_FAILURE);
	}

	int i=0;

	//Dejamos unos bites libres al principio para poder escribir la cabecera más adelante.
	if(lseek(fdwrite,45,SEEK_SET) < 0) {
		perror("\nError en lseek 1");
		exit(EXIT_FAILURE);
	}

	do{
		if((read_length = read(fdread, block, BLOCK_SIZE)) < 0){
			perror("\nError en lectura");
			exit(EXIT_FAILURE);
		}

		//Si el último bloque es justamente de 0 bytes, no imprimimos nada, ni aumetamos el contador.
		if (read_length > 0)
			dprintf(fdwrite, "\n\n\t%s%d:\n", "Bloque ", ++i);

		if(write(fdwrite,block,read_length) < read_length) {
			perror("\nError en escritura");
			exit(EXIT_FAILURE);
		}

	}while (read_length == BLOCK_SIZE);

	if(lseek(fdwrite,0,SEEK_SET) < 0) {
		perror("\nError en lseek 2");
		exit(EXIT_FAILURE);
	}

	dprintf(fdwrite, "%s%d", "El número de bloques es ", i);

	return 0;
}
