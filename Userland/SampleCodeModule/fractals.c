/* Fuentes:
** HSV to RGB -> http://www.splinter.com.au/converting-hsv-to-rgb-colour-using-c/
** Fractal de Julia -> http://lodev.org/cgtutor/juliamandelbrot.html
*/
#include "paint.h"
#include "fractals.h"
#include "syscalls.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

static int screen_width = DEFAULT_WIDTH;
static int screen_height = DEFAULT_HEIGHT;

static void HsvToRgb(unsigned char *r, unsigned char *g, unsigned char *b, unsigned char h, unsigned char s, unsigned char v);
static int endOfParams(fractalParams p);

static fractalParams * param;
static int parameters_amount = 0;

/* Carga el vector de parametros de fractales */
void load_fractals(fractalParams * ptr) {
  param = (fractalParams *) ptr;
  while (!endOfParams(param[parameters_amount])) // Cuenta la cantidad de parametros
    parameters_amount++;
}

static int endOfParams(fractalParams p) {
  return p.zoom == -1 && p.moveX == -1 && p.moveY == -1 && p.maxIterations == 0 && p.cRe == -1 && p.cIm == -1;
}

/* Retorna la cantidad de fractales dibujables */
int fractals_size() {
  return parameters_amount;
}

/* Dibuja un fractal */
int draw_fractal(int index) {
  if (index < 0 || index >= fractals_size())
    return 0;
  drawJuliaFractal(param[index].zoom, param[index].moveX, param[index].moveY, param[index].maxIterations, param[index].cRe, param[index].cIm);
  return 1;
}

/* Establece la resolución de los fractales a dibujar */
int set_fractals_resolution(int w_res, int h_res) {
  if (w_res < 0 || h_res < 0 || w_res > screen_Xresolution() || h_res > screen_Yresolution())
    return 0; // resolucion invalida

  screen_width = w_res;
  screen_height = h_res;
  return 1;
}

/* Dibuja un fractal con los parámetros dados */
void drawJuliaFractal(double zoom, double moveX, double moveY, unsigned int maxIterations, double cRe, double cIm)
{

  //each iteration, it calculates: new = old*old + c, where c is a constant and old starts at current pixel
  //real and imaginary part of the constant c, determinate shape of the Julia Set

  double newRe, newIm, oldRe, oldIm;   //real and imaginary parts of new and old
  //you can change these to zoom and change position
  //after how much iterations the function should stop

  //pick some values for the constant c, this determines the shape of the Julia Set

  //loop through every pixel
  for(int y = 0; y < screen_height; y++)
  for(int x = 0; x < screen_width; x++)
  {
    //calculate the initial real and imaginary part of z, based on the pixel location and zoom and position values
    newRe = 1.5 * (x - screen_width / 2) / (0.5 * zoom * screen_width) + moveX;
    newIm = (y - screen_height / 2) / (0.5 * zoom * screen_height) + moveY;
    //i will represent the number of iterations
    int i;
    //start the iteration process
    for(i = 0; i < maxIterations; i++)
    {
      //remember value of previous iteration
      oldRe = newRe;
      oldIm = newIm;
      //the actual iteration, the real and imaginary part are calculated
      newRe = oldRe * oldRe - oldIm * oldIm + cRe;
      newIm = 2 * oldRe * oldIm + cIm;
      //if the point is outside the circle with radius 2: stop
      if((newRe * newRe + newIm * newIm) > 4) break;
    }
    //use color model conversion to get rainbow palette, make brightness black if maxIterations reached
    unsigned char outr, outg, outb;
    HsvToRgb(&outr, &outg, &outb, i % 256, 255, 255 * (i < maxIterations));

    //draw the pixel
    paint_pixel(outr, outg, outb, x, y);
  }
}

/* Traduce colores HSV a RGB */
static void HsvToRgb(unsigned char *r, unsigned char *g, unsigned char *b, unsigned char h, unsigned char s, unsigned char v)
{
    unsigned char region, fpart, p, q, t;

    if(s == 0) {
        /* color is grayscale */
        *r = *g = *b = v;
        return;
    }

    /* make hue 0-5 */
    region = h / 43;
    /* find remainder part, make it from 0-255 */
    fpart = (h - (region * 43)) * 6;

    /* calculate temp vars, doing integer multiplication */
    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * fpart) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;

    /* assign temp vars based on color cone region */
    switch(region) {
        case 0:
            *r = v; *g = t; *b = p; break;
        case 1:
            *r = q; *g = v; *b = p; break;
        case 2:
            *r = p; *g = v; *b = t; break;
        case 3:
            *r = p; *g = q; *b = v; break;
        case 4:
            *r = t; *g = p; *b = v; break;
        default:
            *r = v; *g = p; *b = q; break;
    }

    return;
}
