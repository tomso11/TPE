#include "stdio.h"
#include "strings.h"
#include "shell.h"
#include "commands.h"
#include "lib.h"
#include "process_info.h"
#include "executer.h"

#define UNSUPPORTED_COMMAND "Not a valid command\n"
#define INVALID_ARGUMENTS "Invalid Arguments\n"

#define BACKGROUND_CHAR '&'

#define MAX_CMD_SIZE 256

static int extract_cmd_name(char * cmd_name, const char * str);
static void handle_validity(int valid);
static int check_foreground(char * buffer);

int shell() {
    int run = 1;
    char buffer[MAX_CMD_SIZE];
    char cmd_name[MAX_CMD_SIZE];
    int len = MAX_CMD_SIZE;
    int name_len;
    int valid;
    int arguments_flag; // 1 si se enviaron argumentos
    int foreground;

    while (run) {
        printf("> shell:");
        readline_no_spaces(buffer, len);
        if (buffer[0] != '\0') { // Se escribió algo
          name_len = extract_cmd_name(cmd_name, buffer);
          foreground = check_foreground(buffer + name_len);
          arguments_flag = buffer[name_len] != '\0';
          valid = execute(cmd_name, buffer+name_len+arguments_flag, foreground); // Nombre y argumentos
          handle_validity(valid);
        }
    }

    return 0;
}

static int check_foreground(char * buffer) {
  int len = str_len(buffer);

  if (buffer[len-1] == BACKGROUND_CHAR && (len == 1 || buffer[len-2] == ' ')) {
    buffer [len - (len == 1 ? 1 : 2)] = '\0';
    return 0;
  }

  return 1;
}


/* Retorna la longitud del comando ingresado y guarda su contenido en un vector recibido como parámetro */
static int extract_cmd_name(char * cmd_name, const char * str) {
  int i;
  for (i = 0; str[i] != '\0' && str[i] != ' '; i++)
    cmd_name[i] = str[i];
  cmd_name[i] = '\0';
  return i;
}

/* Imprime en pantalla en caso de haber ingresado un comando inválido */
static void handle_validity(int valid) {
  switch (valid) {
    case UNSUPPORTED:
      printf("%s", UNSUPPORTED_COMMAND);
      break;
    case INVALID_ARGS:
      printf("%s", INVALID_ARGUMENTS);
      break;
  }
}