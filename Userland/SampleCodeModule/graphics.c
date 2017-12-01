#include "paint.h"
#include "stdlib.h"
#include "syscalls.h"
#include "math.h"

#define OK 0
#define ERROR 1
void draw_circle(int x0, int y0, int radius, char r, char g, char b);
void draw_line(int x0, int y0,int x1, int y1, char r, char b, char g);
void draw_filledCircle(int x0, int y0, int radius, char r, char g, char b);
void draw_filledCircle2(int x0, int y0, int radius , char r, char g, char b);
void draw_ellipse (int xc, int yc, int width, int height, char r, char g, char b);

/* MidPoint Circle Algorithm
** Fuente: wikipedia.org/wiki/Midpoint_circle_algorithm */
void draw_circle(int x0, int y0, int radius, char r, char g, char b) {

	int x = radius;
	int y = 0;
	int err = 0;

	while( x >= y) {
		paint_pixel(r,g,b,x0 + x, y0 + y);
		paint_pixel(r,g,b,x0 + y, y0 + x);
		paint_pixel(r,g,b,x0 - y, y0 + x);
		paint_pixel(r,g,b,x0 - x, y0 + y);
		paint_pixel(r,g,b,x0 - x, y0 - y);
		paint_pixel(r,g,b,x0 - y, y0 - x);
		paint_pixel(r,g,b,x0 + y, y0 - x);
		paint_pixel(r,g,b,x0 + x, y0 - y);

		y += 1;
		err += 1 + 2*y;

		if( 2*(err - x ) + 1 > 0 ) {
			x -= 1;
			err += 1 - 2*x;
		}
	}
}

/* Queda con dibujito :) */
void draw_filledCircle(int x0, int y0, int radius, char r, char g, char b) {
	int x = radius;

	while(x >= 0) {
		draw_circle(x0,y0,x,r,g,b);
		x -= 1;
	}
}

/* Dibuja un circulo completo */
void draw_filledCircle2(int x0, int y0, int radius, char r, char g, char b) {

	int x = radius;
	int y = 0;
	int err = 0;

	while( x >= y) {
		draw_line(x0+x, y0+y,x0-x, y0+y,r,g,b);
		draw_line(x0+x, y0+y,x0-x, y0+y,r,g,b);
		draw_line(x0+y,y0+x,x0-y, y0+x,r,g,b);
		draw_line(x0-x, y0-y,x0+x, y0-y,r,g,b);
		draw_line(x0-y, y0-x,x0+y, y0-x,r,g,b);

		y += 1;
		err += 1 + 2*y;

		if( 2*(err - x ) + 1 > 0 ) {
			x -= 1;
			err += 1 - 2*x;
		}
	}

}

/* Bresenham's Algorithm
** Fuente: https://sites.google.com/site/ruslancray/lab/projects/bresenhamscircleellipsedrawingalgorithm/bresenham-s-circle-ellipse-drawing-algorithm */
void draw_ellipse (int xc, int yc, int width, int height, char r, char g, char b){
    int a2 = width * width;
    int b2 = height * height;
    int fa2 = 4 * a2, fb2 = 4 * b2;
    int x, y, sigma;

    /* first half */
    for (x = 0, y = height, sigma = 2*b2+a2*(1-2*height); b2*x <= a2*y; x++){
        paint_pixel(r,g,b,xc + x, yc + y);
        paint_pixel(r,g,b,xc - x, yc + y);
        paint_pixel(r,g,b,xc + x, yc - y);
        paint_pixel(r,g,b,xc - x, yc - y);
        if (sigma >= 0)
        {
            sigma += fa2 * (1 - y);
            y--;
        }
        sigma += b2 * ((4 * x) + 6);
    }

    /* second half */
    for (x = width, y = 0, sigma = 2*a2+b2*(1-2*width); a2*y <= b2*x; y++){
        paint_pixel(r,g,b,xc + x, yc + y);
        paint_pixel(r,g,b,xc - x, yc + y);
        paint_pixel(r,g,b,xc + x, yc - y);
        paint_pixel(r,g,b,xc - x, yc - y);
        if (sigma >= 0)
        {
            sigma += fb2 * (1 - x);
            x--;
        }
        sigma += a2 * ((4 * y) + 6);
    }
}

/* Algoritmo de Bresenham
** Fuente : Rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C */
void draw_line(int x0, int y0,int x1, int y1, char r, char g, char b){
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = (dx>dy ? dx : -dy)/2, e2;

	for(;;){
		paint_pixel(r,g,b,x0,y0);

		if(x0==x1 && y0==y1) break;
		e2 = err;

		if ( e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}
