#include "math.h"
/* Fuente:
http://stackoverflow.com/questions/2284860/how-does-c-compute-sin-and-other-math-functions
*/
int abs(int n){
	return n > 0 ? n : -n ;
}

/* Aproximacion de la raíz cuadrada*/
int sqrt(float n){
 float x = n;
 float y = 1;
 float e = 0.000001;

 while(x - y > e) {
 	x = (x + y)/2;
 	y = n/x;
 }
 return x;
}

double pow(double a, double b) {
    double c = 1;
    for (int i=0; i<b; i++)
        c *= a;
    return c;
}

double fact(double x) {
    double ret = 1;
    for (int i=1; i<=x; i++) 
        ret *= i;
    return ret;
}

double sin(double x) {
    double y = x;
    double s = -1;
    for (int i=3; i<=100; i+=2) {
        y+=s*(pow(x,i)/fact(i));
        s *= -1;
    }  
    return y;
}
double cos(double x) {
    double y = 1;
    double s = -1;
    for (int i=2; i<=100; i+=2) {
        y+=s*(pow(x,i)/fact(i));
        s *= -1;
    }  
    return y;
}
double tan(double x) {
     return (sin(x)/cos(x));  
}