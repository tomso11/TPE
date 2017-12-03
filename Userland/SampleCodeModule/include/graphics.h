#ifndef GRAPHICS_H
#define GRAPHICS_H

/*Dibuja un circulo en pantalla */
void draw_circle(int x0, int y0, int radius, char r, char g, char b);

/*Dibuja una linea en pantalla */
void draw_line(int x0, int y0,int x1, int y1, char r, char b, char g);

/*Dibuja un circulo casi lleno en pantalla */
void draw_filledCircle(int x0, int y0, int radius, char r, char g, char b);

/*Dibuja un circulo lleno en pantalla */
void draw_filledCircle2(int x0, int y0, int radius , char r, char g, char b);

/* Dibuja una elipse */
void draw_ellipse (int xc, int yc, int width, int height, char r, char g, char b);

#endif