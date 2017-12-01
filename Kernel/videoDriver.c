#include "videoDriver.h"
#include "strings.h"
#include "font.h"
#include "lib.h"

#define MAX_DIGITS 15

#define ROW(p) ((p)/WIDTH)
#define COL(p) ((p)%WIDTH)
#define CURSOR_LIMIT HEIGHT*WIDTH

typedef struct {
	unsigned char red, green, blue;
} color;

static int cursor = 0;
static int x_res = 0;
static int y_res = 0;

static unsigned int count_digits(int num);
static char valid_pos(int row, int col);
static void print_char_line(char line, int x, int y);
static int valid_pixel(int x, int y);
static void scroll();
static void clear_line(int line);
static unsigned char * video_base_ptr();
static int get_res(unsigned char * ptr);
static int set_color (color * c, uint64_t r, uint64_t g, uint64_t b);
static int valid_colors(uint64_t r, uint64_t g, uint64_t b);
static void fillChar(int x, int y);
static void fillBackground(int x, int y);

static color char_color = DEFAULT_CHAR_COLOR;
static color bg_color = DEFAULT_BG_COLOR;

static unsigned char * video_base_ptr() {
	unsigned char ** VBEPhysBasePtr = (unsigned char**) 0x0005C28;
	return * VBEPhysBasePtr;
}

/* Retorna la resolucion de la pantalla(horizontal) */
int scr_x_res() {
	return x_res;
}

/* Retorna la resolucion de la pantalla(vertical) */
int scr_y_res() {
	return y_res;
}
/* Retorna la cantidad de filas de la pantalla para caracteres */
int text_rows() {
	return HEIGHT;
}

/* Retorna la cantidad de columnas de la pantalla para caracteres */
int text_cols() {
	return WIDTH;
}

/* Carga las resoluciones de la pantalla */
void load_vDriver(int x, int y) {
	x_res = get_res((unsigned char *)0x0005C12);
	y_res = get_res((unsigned char *)0x0005C14);
}

/* Carga los colores de la letra */
void current_char_color(unsigned char *r, unsigned char *g, unsigned char *b) {
	*r = char_color.red;
	*g = char_color.green;
	*b = char_color.blue;
}

/* Carga los colores del fondo */
void current_bg_color(unsigned char *r, unsigned char *g, unsigned char *b) {
	*r = bg_color.red;
	*g = bg_color.green;
	*b = bg_color.blue;
}

// Agarra dos bytes que son los correspondientes a la resolucion en VESA.
static int get_res(unsigned char * ptr) {
	unsigned char * res_byte = ptr;
    int res = 0;
    res += *(res_byte+1);
    res = res << 8;
    res += *res_byte;
	return res;
}

/* Retorna 1 si los colores corresponden a una configuracion RGB */
static int valid_colors(uint64_t r, uint64_t g, uint64_t b) {
	return r >= MIN_RGB && r <= MAX_RGB && g >= MIN_RGB 
			&& g <= MAX_RGB && b >= MIN_RGB && b <= MAX_RGB;
}

/* Retorna 1 si un pixel es válido, es decir, esta dentro de los limites de la pantalla */
static int valid_pixel(int x, int y) {
	return (x >= 0 && x <= SCREEN_WIDTH && y >= 0 && y <= SCREEN_HEIGHT);
}

/* Equivale a mover cada linea una fila hacia arriba. La línea de la primer fila es perdida. */
static void scroll() {
	unsigned char * linearVESABuffer = video_base_ptr();
	unsigned char * second_line = linearVESABuffer + (3 * SCREEN_WIDTH * CHAR_HEIGHT);
	uint64_t length = (SCREEN_HEIGHT * SCREEN_WIDTH * 3) -
						(CHAR_HEIGHT * SCREEN_WIDTH * 3);
	memcpy(linearVESABuffer, second_line, length);
	clear_line(HEIGHT - 1);
}

/* Limpia una linea, es decir, coloca espacios en toda la fila */
static void clear_line(int line) {
	if (line >= 0 && line < HEIGHT) {
		int i;
		for (i=0; i < WIDTH; i++)
			print_char(' ', line, i);
	}
}

/* Setea un color de fondo */
int set_bg_color(uint64_t r, uint64_t g, uint64_t b) {
	return set_color(&bg_color, r, g, b);
}

/* Setea un color de fondo */
int set_char_color(uint64_t r, uint64_t g, uint64_t b) {
	return set_color(&char_color, r, g, b);
}

/* Setea un color de fondo fijandose que sean validos los parametros */
static int set_color (color * c, uint64_t r, uint64_t g, uint64_t b) {
	if (!valid_colors(r,g,b))
		return 0;
	c->red = r;
	c->green = g;
	c->blue = b;
	return 1;
}

/* Pinta de blanco un pixel */
static void fillChar(int x, int y) {
	fill(char_color.red, char_color.green, char_color.blue, x, y); // pinta de blanco
}

/* Pinta de negro un pixel */
static void fillBackground(int x, int y) {
	fill(bg_color.red, bg_color.green, bg_color.blue, x, y); // pinta de negro
}

/* Pinta un pixel de un color recibido por parámetro como RGB */
int fill(char red, char green, char blue, int x, int y) {
	if (!valid_pixel(x, y))
		return 0;

	unsigned char * linearVESABuffer = video_base_ptr();
	unsigned char * VBEAdress = linearVESABuffer + 3 * (x + y * SCREEN_WIDTH);
	VBEAdress[0] = blue;
	VBEAdress[1] = green;
	VBEAdress[2] = red;
	return 1;
}

/* Imprime un caracter en pantalla */
void print_char(char c, int row, int col) {
	if (valid_pos(row, col)) { // Ver que onda los tamaños
		int y = row * CHAR_HEIGHT;
		int x = col * CHAR_WIDTH;

		int i = 0;
		unsigned char * char_map = pixel_map(c); // Donde empiezan los mapeos a ascii en el arreglo font.

		for(i = 0; i < CHAR_HEIGHT; i++) {
			unsigned char pixel = char_map[i];
			print_char_line(pixel, x, y+i);
		}
	}
}

static void print_char_line(char line, int x, int y) {
	char mask[] = {128, 64, 32, 16, 8, 4, 2, 1};
	int j = 0;
	char aux;
	for (j = 0; j < CHAR_WIDTH; j++) {
		aux = line & mask[j];
		if (aux != 0) { // Tiene que escribir en ese lugar
			fillChar(x+j, y);
		} else {
			fillBackground(x+j, y);
		}
	}
}

/* Imprime un caracter*/
void put_char(char c) {
	if (c == '\b') {
		cursor--;
		print_char(' ', ROW(cursor), COL(cursor));
	}
	else if (c == '\n') {
		cursor += WIDTH - COL(cursor); /* cursor al comienzo de nueva linea */
	}
	else if (c == '\r') { /* reset cursor */
		cursor = 0;
	} 
	else if (c != '\t') { /* tab no hace nada */
		print_char(c, ROW(cursor), COL(cursor));
		cursor++;
	}
	
	if (cursor < 0)
		cursor = 0;
	if (cursor >= CURSOR_LIMIT) {
		scroll();
		cursor = CURSOR_LIMIT - WIDTH;
	}
}

/* DEBUG KERNEL 
** Las funciones debajo son de uso exclusivo para debuggear kernel.
*/

/* Retorna 1 si la posicion row, col es válida, es decir esta dentro de los limites de la pantalla */
static char valid_pos(int row, int col) {
	return row < HEIGHT && col < WIDTH && row >= 0 && col >= 0;
}


/* Imprime una cadena de caracteres en una fila y columna dados */
void print_str(const char *str, int row, int col) {
	int backup = cursor;
	cursor = row * WIDTH + col;
	while (*str != '\0')
		put_char(*str++);
	cursor = backup;
}
/* Imprime una cadena de caracteres hasta una longitud dada */
void put(const char *str, int len) {
	int i;
	for (i = 0; i < len; i++)
		put_char(str[i]);
}

/* Imprime una cadena de caracteres */
void put_str(const char *str) {
	while(*str != '\0')
		put_char(*str++);
}

/* Imprime un numero*/
void print_num(int num, int row, int col) {
	unsigned int digits = count_digits(num);
	char str[MAX_DIGITS];
	int i;
	int k = 0;

	if (num < 0) {
		num *= -1;
		str[0] = '-';
		digits += 1;
		k = 1;
	}

	for (i = digits-1; i >= k; i--) {
		str[i] = (num % 10) + '0';
		num /= 10;
	}

	print(str, digits, row, col);
}

/* Imprime una cadena de caracteres en una fila y columna dados*/
void print(const char *str, int len, int row, int col) {
	if (valid_pos(row, col)) {
		int i;
		int pos = row * WIDTH + col;
		for (i = 0; i < len && valid_pos(ROW(pos), COL(pos)); i++, pos++)
			print_char(str[i], ROW(pos), COL(pos));
	}
}
/* Limpia la pantalla */
void clear() {
	int i = 0;
	for (i = 0; i < WIDTH*HEIGHT; i ++)
		print_char(' ', ROW(i), COL(i));
	cursor = 0;
}

/* Cuenta los digitos de un numero */
static unsigned int count_digits(int num) {
	int digits = 1;
	if (num < 0)
		num *= -1;
	while ((num /= 10) > 0)
		digits++;
	return digits;
}
