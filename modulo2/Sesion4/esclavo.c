/*
  Programa esclavo. Se le pasan el inicio y el final del intervalo e
  imprime por la salida estándar los primos entre ellos.
*/

#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<math.h>

//Comprueba is un número es primo. Devuelve 1 en caso de que lo sea,
//0 en caso contrario.
int EsPrimo(int n) {
  int tope = sqrt(n);
  int es_primo = 1;

  for (int i = 2; es_primo == 1 && i<=tope; ++i)
    if (n % i == 0)
      es_primo = 0;

  return es_primo;
}

int main (int argc, char *argv[]) {
  if (argc != 3) {
    printf("Modo de empleo: esclavo <inicio-del-intervalo> <final-del-intervalo>\n");
    exit(0);
  }

  int tope = atoi(argv[2]);

  for(int i = atoi(argv[1]); i<=tope; ++i)
    if (EsPrimo(i) == 1)
      printf("%d\t", i);

  return(0);
}
