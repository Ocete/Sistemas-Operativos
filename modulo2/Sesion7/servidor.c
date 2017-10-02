#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int seguir_leyendo, hijos;

void handler(int sigNum) {
  if (sigNum == SIGCONT) {
    printf("Servidor: señal de cierre recibida.\n");
    seguir_leyendo = 1;
  } else if (sigNum == SIGCHLD) {
    if (wait(NULL) == -1) {
      perror("\nServidor: error en wait en el manejador de señales");
      //exit(EXIT_FAILURE);
    }
    hijos--;
  } else {
    dprintf(STDERR_FILENO, "Error en el manejo de señal: señal no válida.\n");
  }
}

int main (int argc, char *argv[]) {
  const int TAMANIO_NOMBRE = 50, TAMANIO_LECTURA = 10;
  int fde, fds, fdproxy, fdbloqueo, control_lectura;
  char lectura[TAMANIO_LECTURA], nombrefifoproxy[TAMANIO_NOMBRE],
      nombrefifoe[TAMANIO_NOMBRE], nombrefifos[TAMANIO_NOMBRE];
  struct sigaction manejador;
  pid_t pid;

  //Creamos el archivo de bloqueo
  if((fdbloqueo = open("/tmp/bloqueo", O_CREAT|O_TRUNC|O_WRONLY, S_IRWXU)) == -1) {
		perror("\nServidor: error al crear el archivo de bloqueo");
		exit(EXIT_FAILURE);
	}

  //Creamos los fifos de entrada y salida
  sprintf(nombrefifoe, "%se", argv[0]);
  sprintf(nombrefifos, "%ss", argv[0]);
  if (mkfifo(nombrefifoe, S_IRWXU) == -1) {
    perror("\nServidor: error al crear el fifo conocido de entrada");
    exit(EXIT_FAILURE);
  }
  if (mkfifo(nombrefifos, S_IRWXU) == -1) {
    perror("\nServidor: error al crear el fifo conocido de salida");
    exit(EXIT_FAILURE);
  }

  //Abrimos ambos fifos para lectura y escritura
  if((fde = open(nombrefifoe, O_RDWR)) == -1) {
    perror("\nServidor: error al abrir el fifo conocido de entrada");
    exit(EXIT_FAILURE);
  }
  if((fds = open(nombrefifos, O_RDWR)) == -1) {
    perror("\nServidor: error al abrir el fifo conocido de salida");
    exit(EXIT_FAILURE);
  }

  //Configuramos el manejador de señales
  manejador.sa_handler = handler;
  sigemptyset (&manejador.sa_mask);
  manejador.sa_flags = 0;
  if (sigaction(SIGCHLD, &manejador, NULL) == -1) {
    perror("\nServidor: error al configurar el manejador de señales para SIGCHLD");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGCONT, &manejador, NULL) == -1) {
    perror("\nServidor: error al configurar el manejador de señales para SIGCONT");
    exit(EXIT_FAILURE);
  }

  //Entramos en el bucle de lecturas
  hijos = 0;
  seguir_leyendo = 0;
  while (seguir_leyendo == 0 && (control_lectura = read(fde, &lectura, sizeof(int)))) {
    if (control_lectura == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        perror("Servidor: error al leer del fifo conocido de entrada");
        exit(EXIT_FAILURE);
      }
    }

    //Creamos un hijo
    hijos++;
    if ((pid = fork()) == -1) {
      perror("\nError en fork");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) { //hijo
      //Obtenemos el pid del hijo
      pid = getpid();

      //Creamos el fifoproxy
      sprintf(nombrefifoproxy, "fifo.%d", pid);
      if (mkfifo(nombrefifoproxy, S_IRWXU) == -1) {
        perror("\nHijo: error al crear el fifoproxy");
        exit(EXIT_FAILURE);
      }

      //Escribimos el pid del hijo en el fifo conocido de salida
      if (write(fds, &pid, sizeof(int)) == -1) {
        perror("\nHijo: error al escribir el PID del hijo en fifo conocido");
        exit(EXIT_FAILURE);
      }

      //Abrimos el fifoproxy en modo lectura
      if((fdproxy = open(nombrefifoproxy, O_RDONLY)) == -1) {
        perror("\nHijo: error al abrir el fifoproxy por parte del hijo");
        exit(EXIT_FAILURE);
      }

      //Redireccionamos la entrada estándar del hijo
      if (dup2(fdproxy, STDIN_FILENO) == -1) {
        perror("\nHijo: error al redireccionar la entrada estándar del hijo");
        exit(EXIT_FAILURE);
      }

      //Ejecutamos proxy
      sprintf(lectura, "%d", fdbloqueo);
      if (execl("./proxy","proxy", lectura, NULL) == -1) {
        perror("\nHijo: error en exec");
        exit(EXIT_FAILURE);
      }
    }
  }

  while (hijos != 0) {
    if (wait(NULL) == -1) {
      perror("\nServidor: error en wait final");
    }
    hijos--;
  }

  //Destruimos los fifos conocidos
  if (unlink (nombrefifoe) == -1) {
    perror("\nServidor: error al destruir el fifo conocido de entrada");
  }
  if (unlink (nombrefifos) == -1) {
    perror("\nServidor: error al destruir el fifo conocido de salida");
  }

  printf("Servidor cerrándose.\n");
  return 0;
}
