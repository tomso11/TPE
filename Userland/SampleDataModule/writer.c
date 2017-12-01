#include <stdio.h>

/* 
** Programa auxiliar que se encarga de generar el archivo binario correspondiente al módulo de datos
** con los parámetros de los fractales correspondientes.
*/

int main() {
	FILE *f = fopen("../0001-sampleDataModule.bin","wb");
	struct fractalParams{
		double zoom;
		double moveX;
		double moveY;
		unsigned int maxIterations;
		double cRe;
		double cIm;
		} param[] = {
		  {1265.76110029, 0.427834925, 0.42325986, 128, -0.6548832, -0.4477065},
		  {0.85035, 0.437267, -0.613638, 120, -0.47098, -0.68545},
		  {0.83035, 0.087267, -0.013638, 128, -0.77098, -0.08545},
		  {1265.7611, 0.20932, 0.10459, 256, -0.6448832, -0.447706},
		  {0.830350, 0.06595, -0.122026, 128, -0.654883, -0.4477065},
		  {1265.76110029, 0.2093925, 0.1045986, 128, -0.6548832, -0.4477065},
		  {-1,-1,-1,0,-1,-1}  /* Valor sentinela que indica fin del arreglo de parámetros */
		};
	fwrite(param, sizeof(param[0]), sizeof(param)/sizeof(param[0]), f); /* Crea el binario en cuestión */
}
