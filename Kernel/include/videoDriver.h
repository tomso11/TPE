#ifndef VIDEO_DRIVER__H
#define VIDEO_DRIVER__H

#include "font.h"

#include <stdint.h>

void load_vDriver();

void print_str(const char *str, int row, int col);
void put_char(char c);
void put(const char *str, int len);
void put_str(const char *str);
void print_char(char c, int row, int col);
void print_num(int num, int row, int col);
void print(const char *str, int len, int row, int col);
int fill(char red, char green, char blue, int x, int y);
void clear();
int scr_x_res();
int scr_y_res();
int text_rows();
int text_cols();
int set_bg_color(uint64_t r, uint64_t g, uint64_t b);
int set_char_color(uint64_t r, uint64_t g, uint64_t b);
void current_char_color(unsigned char *r, unsigned char *g, unsigned char *b);
void current_bg_color(unsigned char *r, unsigned char *g, unsigned char *b);


#define SCREEN_HEIGHT y_res
#define SCREEN_WIDTH  x_res

#define HEIGHT (SCREEN_HEIGHT/CHAR_HEIGHT)
#define WIDTH (SCREEN_WIDTH/CHAR_WIDTH)

#define MAX_RGB 255
#define MIN_RGB 0

#define DEFAULT_CHAR_COLOR {255,255,255} // Blanco
#define DEFAULT_BG_COLOR {0,0,0}  // Negro

#endif
