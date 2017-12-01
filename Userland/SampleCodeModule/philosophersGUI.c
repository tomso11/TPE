#include "stdio.h"
#include "philo.h"
#include "time.h"
#include "graphics.h"
#include "math.h"

char * stateStrings[] = {"THINKING", "HUNGRY", "EATING", "DYING"};

int prevPhilosopherCount;

static void get_state_color(int philoState, int * r, int * g, int * b);

static void cleanTable(int x, int radius, double offset);

void renderText(philosopher_data * philos, int philosopherCount) {
	if (philosopherCount == 0)
		printf("There are no philosophers left\n");
	else {
		for(int i = 0; i < philosopherCount; i++) {
			printf("Philosopher %d: %s\n", i, stateStrings[philos[i].state]);
		}
		putchar('\n');
	}

	sleep(500);
}

void renderGraphics(philosopher_data * philos, int philosopherCount) {
	if (philosopherCount == 0) {
		printf("There are no philosophers left\n");
		return;
	}

	int i;
	int r, g, b;
	double dishAngle, dishXpos, dishYpos;
	double table_r = TABLE_RADIUS * (1 + philosopherCount * 0.1);
	double dishSpace = (2 * PI * table_r) / philosopherCount;
	double dishRadius = dishSpace * 0.4;

	if (prevPhilosopherCount != philosopherCount)
		cleanTable(TABLE_X_POS, table_r, dishRadius);

	prevPhilosopherCount = philosopherCount;

	for (i = 0; i < philosopherCount; i++) {
		dishAngle = (2 * PI * i) / philosopherCount;
		dishXpos = table_r * cos(dishAngle) + TABLE_X_POS;
		dishYpos = table_r * sin(dishAngle) + TABLE_Y_POS;

		get_state_color(philos[i].state, &r, &g, &b);

		draw_filledCircle2(dishXpos, dishYpos, dishRadius, r, g, b);
	}
}

void print_colors_instructions() {
  printf("What do the colours mean?\n");
  printf("Red - EATING\n");
  printf("Yellow - HUNGRY\n");
  printf("Blue - THINKING\n");
  printf("Grey - About to be removed\n");
}

void print_commands() {
  printf("\nCommands\n");
  printf("h     Prints this help.\n");
  printf("w     Adds a philosopher.\n");
  printf("s     Removes a philosopher.\n");
  printf("e     Removes all philosophers instantly. Terminates philosophers problem.\n");
  printf("q     Removes philosophers one by one. Terminates philosophers problem.\n");
  printf("p     Toggles pause. Philosophers may not be modified during pause.\n");
  printf("g     Toggles graphic and text mode.\n");
  putchar('\n');
}

static void get_state_color(int philoState, int * r, int * g, int * b) {
	switch(philoState) {
		case EATING:
		*r = 255; *g = 0; *b = 0; /* Rojo */
		break;
		case HUNGRY:
		*r = 255; *g = 187; *b = 0; /* Amarillo */
		break;
		case THINKING:
		*r = 112; *g = 142; *b = 224; /* Celeste */
		break;
		case DYING:
		*r = 25; *g = 25; *b = 25; /* Gris */
		break;
	}
}

static void cleanTable(int x, int radius, double offset) {
	int i, j;
	int max_y_res = screen_Yresolution();
	for (i = x - offset * 2 - radius; i <= x + offset * 2 + radius; i++) {
		for (j = 0; j <= max_y_res; j++) {
			paint(0, i, j);
		}
	}
}
