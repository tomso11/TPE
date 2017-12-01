#include "commands.h"
#include "stdio.h"
#include "strings.h"
#include "fractals.h"
#include "time.h"
#include "paint.h"
#include "ctype.h"
#include "syscalls.h"
#include "stdlib.h"
#include "graphics.h"
#include "philo.h"
#include "producerConsumer.h"
#include "producerConsumer2.h"
#include "time.h"
#include "executer.h"
#include "process_info.h"
#include "shell.h"

#include <stdint.h>

/* Longitud maxima de un comando ingresado por el usuario */
#define CMDS_SIZE (sizeof(commands)/sizeof(commands[0]))

#define MAX_PROCESSES 128

static void printWithTimeFormat(unsigned int n);
static int isnum(const char *str);

static int help(int argc, char * argv[]);
static int fractals(int argc, char * argv[]);
static int clear(int argc, char * argv[]);
static int getTime(int argc, char * argv[]);
static int echo(int argc, char * argv[]);

static int philosophersProblem (int argc, char * argv[], int graphic);
static int philosophersProblemText (int argc, char * argv[]);
static int philosophersProblemGUI (int argc, char * argv[]);

static int producerConsumer (int argc, char * argv[]);
static int producerConsumer2 (int argc, char * argv[]);

static int set_GMT(int argc, char * argv[]);
static int change_char_color (int argc, char * argv[]);
static int change_bg_color (int argc, char * argv[]);
static int test(int argc, char * argv[]);
static int kill_command(int argc, char * argv[]);

static int write_test(int argc, char * argv[]);
static int read_test(int argc, char * argv[]);

static int testFifos (int argc, char * argv[]);
static int set_fg(int argc, char * argv[]);

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

  if (get_process_info(pid, &pi)) {
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

  return VALID;
}


/* Estructura que representa un comando de la Shell */
typedef struct {
	const char * name;  /* Nombre del comando */
	int (*function) (int argc, char *argv[]);  /* Funcion correspondiente al comando */
} command;


static int set_fg(int argc, char * argv[]) {
  int pid, valid;

  if (argc != 1) {
    printf("Usage: set_fg [PID]\n");
    return INVALID_ARGS;
  }

  if (!isnum(argv[0])) {
    printf("Argument must be a integer PID\n");
    return INVALID_ARGS;
  }

  pid = atoi(argv[0]);

  yield();

  valid = set_foreground(pid);

  if (!valid) {
    printf("Invalid pid\n");
  }

  return valid ? VALID : INVALID_ARGS;
}

/* COMMANDS ARRAY */
static command commands[]= {{"help", help},
							{"set_GMT", set_GMT},
							{"fractals", fractals},
							{"time", getTime},
							{"clear", clear},
							{"echo", echo},
              {"char_color", change_char_color},
              {"bg_color", change_bg_color},
              {"test", test},
							{"philo", philosophersProblemText},
              {"philogui", philosophersProblemGUI},
							{"prodcon", producerConsumer},
              {"prodcon2", producerConsumer2},
              {"write", write_test},
              {"read", read_test},
							{"test2", testFifos},
              {"kill", kill_command},
              {"print", print_process},
              {"ps", ps},
              {"fg", set_fg},
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
    write(fds, argv[i], strlen(argv[i]));
    write(fds, " ", 1);
  }

  return VALID;
}

static int read_test(int argc, char * argv[]) {
  int fds = fifo_open("test_fifo");
  int n;
  char buffer[5];

  printf("Leyendo de fifo con fds %d\n", fds);

  n = read(fds, buffer, 4);

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
			 set_foreground(pid);
    }
	}
	return valid;
}

/* COMANDOS */

/* Muestra en pantalla texto de ayuda al usuario, por ejemplo comandos existentes */
static int help(int argc, char * argv[]){
  printf("HELP FUNCTION -- shows the principal User Commands and its description\n\n");
  printf(" echo [args...]");
  printf("    Write arguments to the standard output. Display the args, separated by a single space character\n");
  printf("                   and followed by a newline, on the standard output.\n");

  printf(" clear                Clear the terminal screen.\n");
  printf(" time                 Display the current time on the standard output using 24hr format [hh:mm:ss]\n");
  printf(" set_GMT [GMT]        Set new Greenwich Mean Time. Displays new current time afterwards\n");
  printf(" fractals [*option]   Display a new fractal drawing on the standard output.\n");
  printf("                      If a specific fractal is desired, a number from 1 to %d may be sent as parameter.\n", fractals_size());
  printf("                      If no parameter is sent a random fractal will be drawed.\n");
  printf(" char_color [r,g,b]   Changes character color.\n");
  printf(" bg_color   [r,g,b]   Changes background color. Clears screen.\n");
  printf(" prodcon    [size]    Commences producer consumer problem resolved with fifos.\n");
  printf(" prodcon2   [size]    Commences producer consumer problem resolved with variable conditions.\n");
  printf(" philo      [N]       Commences philosophers problem with N philosophers. Max %d.\n", MAX_PHILOSOPHERS);
  printf(" philogui   [N]       Commences philosophers problem with graphics.\n");
  printf(" shell                User shell.\n" );
  printf(" fg         [PID]     Gives foreground to process.\n");


  return VALID;
}

/* Setea GMT del reloj y muestra la hora actual en pantalla*/
static int set_GMT (int argc, char * argv[]) {
  if (argc != 1 || !isnum(argv[0]))
    return INVALID_ARGS;

  int valid = setGMT(atoi(argv[0]));
  if (!valid)
  	return INVALID_ARGS;
  getTime(0, argv);
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

/*
** Muestra en pantalla imagenes de fractales.
** En caso de recibir un numero como parámetro muestra el fractal correspondiente,
** si no se recibió ningun parametro muestra un fractal al azar
*/

static int fractals(int argc, char * argv[]) {
  int index = -1;

  if(argc > 1) {
    printf("Usage: fractals [fractal_num]\n");
    return INVALID_ARGS;
  }

  if (argc == 0) // No se enviaron parametros --> fractal al azar
    index = seconds() % fractals_size();
  else if (isnum(argv[0]))
    index = atoi(argv[0])-1;

  if (index < 0 || index >= fractals_size()) {
    printf("Fractal number must be between 1 and %d\n", fractals_size());
    return INVALID_ARGS;
  }

  draw_fractal(index);

  return VALID;
}

/* Cambia el color de la letra */
static int change_char_color (int argc, char * argv[]) {
  int red, green, blue;

  if (argc != 3)
    printf("Usage char_color [r,g,b]");

  if (!isnum(argv[0]) || !isnum(argv[1]) || !isnum(argv[2]))
    printf("The colors must be integers\n");

  red = atoi(argv[0]);
  green = atoi(argv[1]);
  blue = atoi(argv[2]);

  return set_char_color(red, green, blue) == 0 ? INVALID_ARGS : VALID;
}

/* Cambia el color del fondo */
static int change_bg_color (int argc, char * argv[]) {
  int red, green, blue;

  if (argc != 3)
    printf("Usage bg_color [r,g,b]");

  if (!isnum(argv[0]) || !isnum(argv[1]) || !isnum(argv[2]))
    printf("The colors must be integers\n");

  red = atoi(argv[0]);
  green = atoi(argv[1]);
  blue = atoi(argv[2]);

  if(set_bg_color(red, green, blue)) {
    clear(0, argv);
    return VALID;
  }

  return INVALID_ARGS;
}

static int philosophersProblem (int argc, char * argv[], int graphic) {
	int p = DEFAULT_PHILOSOPHERS;

	if (isnum(argv[0]))
		p = atoi(argv[0]);

	if (p < 1) {
    printf("At least one philosopher\n");
		return INVALID_ARGS;
  }

	start_philosophers_problem(graphic, p);
	return VALID;
}

static int philosophersProblemText (int argc, char * argv[]) {
  return philosophersProblem(argc, argv, 0);
}

static int philosophersProblemGUI (int argc, char * argv[]) {
  return philosophersProblem(argc, argv, 1);
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

static int producerConsumer2 (int argc, char * argv[]) {
  int b = DEFAULT_PRODCON_BUFFER;

  if (isnum(argv[0]))
    b = atoi(argv[0]);

  if (b < 1) {
    printf("Buffer has to have at least one slot\n");
    return INVALID_ARGS;
  }

  start_producer_consumer_problem2(b);
  return VALID;
}

static int testFifos (int argc, char * argv[]) {
	execpn(processRead, "fifo_test_read");
  yield();
	execpn(processWrite, "fifo_test_write");
	return VALID;
}

static int processWrite() {
	int fd = fifo_open("TestFifo");
	char c = 'A';
	while(1) {
    printf("Trying to write\n");
    sleep(5000);
		write(fd, &c, 1);
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
		read(fd, &c, 1);
		printf("READ %c ASCII %d\n", c, c);
	}
  return 0;
}

static int kill_command(int argc, char * argv[]) {
  int pid = atoi(argv[0]);
  int valid = 0;

  if (pid != 0)
    valid = kill(pid);

  if (valid)
    printf("Killed process with PID %d\n", pid);

  return valid ? VALID : INVALID_ARGS;
}

static int print_ipcs(int argc, char * argv[]) {
  print_ipc_info();
  return VALID;
}
