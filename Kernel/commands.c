#include "commands.h"
#include "stdio.h"
#include "strings.h"
#include "time.h"
#include "ctype.h"
#include "lib.h"
#include "philo.h"
#include "producerConsumer.h"
#include "time.h"
#include "executer.h"
#include "process_info.h"
#include "shell.h"
#include "process.h"

#include <stdint.h>

/* Longitud maxima de un comando ingresado por el usuario */
#define CMDS_SIZE (sizeof(commands)/sizeof(commands[0]))

#define MAX_PROCESSES 128

static void printWithTimeFormat(unsigned int n);
static int isnum(const char *str);

static int help(int argc, char * argv[]);
static int clear(int argc, char * argv[]);
static int getTime(int argc, char * argv[]);
static int echo(int argc, char * argv[]);

static int philosophersProblem (int argc, char * argv[]);
static int philosophersProblemText (int argc, char * argv[]);

static int producerConsumer (int argc, char * argv[]);
static int producerConsumer2 (int argc, char * argv[]);

static int test(int argc, char * argv[]);
static int kill_command(int argc, char * argv[]);

static int write_test(int argc, char * argv[]);
static int read_test(int argc, char * argv[]);

static int testFifos (int argc, char * argv[]);

static int ps(int argc, char * argv[]);
static void print_single_process(int pid);

static int processWrite();
static int processRead();

static int print_process(int argc, char * argv[]);
static int print_ipcs(int argc, char * argv[]);

static int print_process(int argc, char * argv[]) {
  int i;

  if (argc < 1) {
    printf("Usage: print_process [PID1, PID2, ...]\n");
    return INVALID_ARGS;
  }

  for (i = 0; i < argc; i++) {
    print_single_process(atoi(argv[i]));
  }

  return VALID;
}

static void print_single_process(int pid) {
  process_info pi;
  char buffer[MAX_PROCESS_STRING];

  if (get_process_info_by_pid(&pi, pid)) {
    process_string(&pi, buffer);
    printf("%s\n", buffer);
  }
}

static int ps(int argc, char * argv[]) {
  int pid_array[MAX_PROCESSES], i;

  get_current_pids(pid_array);
  
  for (i = 0; pid_array[i] != -1; i++) {
    print_single_process(pid_array[i]);
  }

  //print_processes();
  return VALID;
}


/* Estructura que representa un comando de la Shell */
typedef struct {
	const char * name;  /* Nombre del comando */
	int (*function) (int argc, char *argv[]);  /* Funcion correspondiente al comando */
} command;


/* COMMANDS ARRAY */
static command commands[]= {{"help", help},
							{"time", getTime},
							{"clear", clear},
							{"echo", echo},
              {"test", test},
							{"philo", philosophersProblemText},
          		{"prodcon", producerConsumer},
              {"write", write_test},
              {"read", read_test},
							{"test2", testFifos},
              {"kill", kill_command},
              {"print", print_process},
              {"ps", ps},
              {"ipcs", print_ipcs},
              {"shell", shell}
							};

static int test_num = 0;

static int test (int argc, char * argv[]) {
  int tn = test_num++;

  int m_key = mutex_open("test_mutex");

  mutex_lock(m_key);
  printf("Tengo el lock. Soy %d\n", tn);

  sleep(3000);

  printf("Soltando lock. Soy %d\n", tn);

  mutex_unlock(m_key);

  return VALID;
}

static int write_test(int argc, char * argv[]) {
  int fds = fifo_open("test_fifo");
  int i;

  printf("Escribiendo a fifo con fds %d\n", fds);

  for(i = 0; i < argc; i++) {
    sys_write(fds, argv[i], str_len(argv[i]));
    sys_write(fds, " ", 1);
  }

  return VALID;
}

static int read_test(int argc, char * argv[]) {
  int fds = fifo_open("test_fifo");
  int n;
  char buffer[5];

  printf("Leyendo de fifo con fds %d\n", fds);

  n = sys_read(fds, buffer, 4);

  buffer[n] = '\0';

  printf("Lei %s\n", buffer);

  return VALID;
}

/* EXECUTE */

/*
** Ejecuta la funcion name pasandole como parametro el string args.
** Se asegura desde la shell que toda palabra se encuentra separada
** por un unico espacio y que no existen espacios al comienzo o al
** de la cadena.
** Se asegura tambien que args[0] es el caracter nulo ('\0') si no se
** enviaron argumentos o el comienzo del primer argumento sino.
** La funciones son responsables de parsear los argumentos, retornando
** INVALID_ARGS en caso de que estos sean incorrectos.
*/
int execute(const char *name, const char *args, int foreground) {
	int i;
  int pid;
  int valid = UNSUPPORTED;
	for (i = 0; i < CMDS_SIZE; i++) {
		if (strcmp(name, commands[i].name) == 0) {
      valid = VALID;
      pid = execp (commands[i].function, args, name);
      if (foreground)
			 sys_set_foreground(pid);
    }
	}
	return valid;
}

/* COMANDOS */

/* Muestra en pantalla texto de ayuda al usuario, por ejemplo comandos existentes */
static int help(int argc, char * argv[]){
  printf("HELP \n The supported commands are clear, help, time, ps, philo and prodcon. \n");
  return VALID;
}

/* Retorna 1 si una cadena de caracteres representa a un numero */
static int isnum(const char *str) {
  if (!isdigit(str[0]) && str[0] != '-')
    return 0;
  if (str[0] == '-' && !isdigit(str[1]))
    return 0;
  int sign = str[0] == '-';
  int i;
  for (i = sign; str[i] != '\0' && str[i] != ' '; i++)
    if (!isdigit(str[i]))
      return 0;
  return 1;
}

/*Imprime en pantalla la hora actual */
static int getTime(int argc, char * argv[]) {
  int h = hour(), m = minutes(), s = seconds();
  printf("Current time: ");
  printWithTimeFormat(h);
  printf(":");
  printWithTimeFormat(m);
  printf(":");
  printWithTimeFormat(s);
  printf("\n");
  return VALID;
}

/* Imprime un numero en formato [xx]*/
static void printWithTimeFormat(unsigned int n) {
    if (n < 10)
        printf("0");
    printf("%d", n);
}

/* Limpia la pantalla de la terminal*/
static int clear(int argc, char * argv[]) {
  int rows = text_rows();
  for (int i = 0; i < 2*rows; i++)
    putchar('\n');
  putchar('\r'); // Vuelve el cursor al inicio de la pantalla
  return VALID;
}

/*Imprime la cadena de argumentos pasadas como parametros a salida estandar */
static int echo(int argc, char * argv[]) {
  int i;
  for (i = 0; i < argc; i++)
    printf("%s ", argv[i]);
  putchar('\n');
  return VALID;
}


static int philosophersProblem (int argc, char * argv[]) {
	int p = DEFAULT_PHILOSOPHERS;

	if (isnum(argv[0]))
		p = atoi(argv[0]);

	if (p < 1) {
    printf("At least one philosopher\n");
		return INVALID_ARGS;
  }

	start_philosophers_problem(p);
	return VALID;
}

static int philosophersProblemText (int argc, char * argv[]) {
  return philosophersProblem(argc, argv);
}

static int producerConsumer (int argc, char * argv[]) {
	int b = DEFAULT_PRODCON_BUFFER;

	if (isnum(argv[0]))
		b = atoi(argv[0]);

	if (b < 1) {
		printf("Buffer has to have at least one slot\n");
		return INVALID_ARGS;
	}

	start_producer_consumer_problem(b);
	return VALID;
}


static int testFifos (int argc, char * argv[]) {
	execpn(processRead, "fifo_test_read");
  sys_yield();
	execpn(processWrite, "fifo_test_write");
	return VALID;
}

static int processWrite() {
	int fd = fifo_open("TestFifo");
	char c = 'A';
	while(1) {
    printf("Trying to write\n");
    sleep(5000);
		sys_write(fd, &c, 1);
		printf("WROTE\n");
	}
  return 0;
}

static int processRead() {
	int fd = fifo_open("TestFifo");
	char c;
	while(1) {
		sleep(1000);
		printf("Trying to read\n");
		sys_read(fd, &c, 1);
		printf("READ %c ASCII %d\n", c, c);
	}
  return 0;
}

static int kill_command(int argc, char * argv[]) {
  int pid = atoi(argv[0]);
  int valid = 0;

  if (pid != 0)
    valid = sys_kill(pid);

  if (valid)
    printf("Killed process with PID %d\n", pid);

  return valid ? VALID : INVALID_ARGS;
}

static int print_ipcs(int argc, char * argv[]) {
  print_ipc_info();
  return VALID;
}
