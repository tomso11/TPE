#ifndef COMMANDS_H
#define COMMANDS_H

#define VALID 1
#define UNSUPPORTED 0
#define INVALID_ARGS -1

#define DEFAULT_PHILOSOPHERS 5
#define DEFAULT_PRODCON_BUFFER 50

int execute(const char *name, const char *args, int foreground);

#endif
