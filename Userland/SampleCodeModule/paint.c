#include "syscalls.h"
#include "paint.h"

/* Llama al system call paint con los parametros dados */
int paint_pixel(char red, char green, char blue, int x, int y) {
  int color = 0;

  /* 
  ** Se traducen los colores a un solo parametro donde el byte menos significativo corresponde
  ** al color azul, el siguiente al verde y el siguiente al rojo.
  */

  color += red;
  color = color << 8;
  color += green;
  color = color << 8;
  color += blue;
  return paint(color, x, y);
}
