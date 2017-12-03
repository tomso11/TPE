#ifndef _FRACTALS_H_
#define _FRACTALS_H_

#include "syscalls.h"

typedef struct {
  double zoom;
  double moveX;
  double moveY;
  unsigned int maxIterations;
  double cRe;
  double cIm;
} fractalParams;

void load_fractals();
int fractals_size();
int draw_fractal(int index);
void drawJuliaFractal(double zoom, double moveX, double moveY, unsigned int maxIterations, double cRe, double cIm);
int set_fractals_resolution(int x_res, int y_res);

#endif
