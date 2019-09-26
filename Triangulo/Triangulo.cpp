#include <bits/stdc++.h>//Utilizamos la biblioteca estandar de c++

using namespace std;

// DEFINICION DE FUNCION QUE DIBUJARÁ EL TABLERO 
void drawtablero();
void drawtriangle();// Funcion para el dibujo del triangulo 

//Definicion de la funcion que calcurará el perimetro del triangulo
double perimetro(double lado1,double lado2,double lado3);

//Definición de la función que calculará el área del triangulo
double area(double semiperimetro,double distanciaAB,double ditanciaBC,double distanciaCA );

//Definición de la función que calculará el semiperimetro del triangulo
double Semiperimetro(double distanciaAB,double distanciaBC, double distanciaCA);

//Definición de la función que realizará el movimiento de los puntos en el eje
void moverpunto(int x,int y);

//Definición de la función que calculará la distancia entre puntos
double distancia_entre_puntos(int PuntoA[2],int PuntoB[2]);

// DEFINICION de variables globales que se utilizarán como puntos que marcan los vertices
// del triangulo

int punto1[2] ;	// Primer punto 
//   Asignacion de valor en x,y 
int punto2[2] ;	// Segundo punto  
int punto3[2] ;	// Tercer punto


int main(){
drawtablero();
//   Asignacion de valor en x,y 
punto1[0]=380/2;
punto1[1]=20;
//
punto2[0]=(380/2)/2;
punto2[1]=70;
//
punto3[0]=((380/2)/2)+(380/2);
punto3[1]=70;
//

for (true){
	double p = perimetro();
	double sp = semiperimetro();
	double a = area();
/*
 * 	MOVER PUNTO DE UN TRIANGULO
 * 	int Mov_x;//posiciones movida en x
 * 	int Mov_y;//posiciones movida en y 
	switch(){

		case F1:
			for (this =! ESCape key){
				case flechaArriba:
				                // Mover el punto hacia arriba
							drawtriangle();
							punto1[1]=punto1[1]+1;
						  break;
				case flechaAbajo:
						// Mover el punto hacia abajo
							drawtriangle();
							punto1[1]=punto1[1]-1;
						break;
				case flechaDerecha: 
						// Mover el punto hacia la derecha
							drawtriangle();
							punto1[0]=punto1[0]+1;
						break;
				case flechaIzquierda;
						// Mover el punto hacia la izquierda
							drawtriangle();
							punto1[0]=punto1[0]-1;
						break;
				case ESC:
						imprimir("TRIANGULO MODIFICADO"); __
					break;
			}
			}
			break;

                case F2:
                        for (this =! ESCape key){
                                case flechaArriba:
                                                // Mover el punto hacia arriba
                                                        drawtriangle();
                                                        punto2[1]=punto2[1]+1;
                                                  break;
                                case flechaAbajo:
                                                // Mover el punto hacia abajo
                                                        drawtriangle();
                                                        punto2[1]=punto2[1]-1;
                                                break;
                                case flechaDerecha: 
                                                // Mover el punto hacia la derecha
                                                        drawtriangle();
                                                        punto2[0]=punto2[0]+1;
                                                break;
                                case flechaIzquierda;
                                                // Mover el punto hacia la izquierda
                                                        drawtriangle();
                                                        punto2[0]=punto2[0]-1;
                                                break;
                                case ESC:
                                                imprimir("TRIANGULO MODIFICADO"); __
                                        break;
                        }
                        }
                        break;

		case F3:
                        for (this =! ESCape key){
                                case flechaArriba:
                                                // Mover el punto hacia arriba
                                                        drawtriangle();
                                                        punto3[1]=punto3[1]+1;
                                                  break;
                                case flechaAbajo:
                                                // Mover el punto hacia abajo
                                                        drawtriangle();
                                                        punto3[1]=punto3[1]-1;
                                                break;
                                case flechaDerecha: 
                                                // Mover el punto hacia la derecha
                                                        drawtriangle();
                                                        punto3[0]=punto3[0]+1;
                                                break;
                                case flechaIzquierda;
                                                // Mover el punto hacia la izquierda
                                                        drawtriangle();
                                                        punto3[0]=punto3[0]-1;
                                                break;
                                case ESC:
                                                imprimir("TRIANGULO MODIFICADO"); __
                                        break;
                        }
                        }
                        break;
	}
*/

drawtablero();
drawtriangle();

}
}

double perimetro(double lado1,double lado2,double lado3){
	double Perimetro= lado1+lado2+lado3;
	return Perimetro;
}


double area(){
	double Area;
	Area= sqrt(semiperimetro*(semiperimetro-distanciaAB)*(semiperimetro-distanciaBC)*(semiperimetro-distanciaCA));
	return Area;
}

double distancia_entre_puntos(int PuntoA[2],int PuntoB[2]){
	double Distancia = sqrt  ( (pow(PuntoA[0]-PuntoB[0],2))+(pow(PuntoA[1]-PuntoB[1],2)));
	return Distancia;
}

double Semiperimetro(double distanciaAB,double distanciaBC, double distanciaCA){
	double semiperimetro = (distanciaAB+distanciaBC+distanciaCA)/2 ;
	return semiperimetro;
}

void moverpunto(int x , int y, int numerodelpunto){
	switch (numerodelpunto)
	{
		case 1:
			punto1[0]= x;
			punto1[1]= y;
			break;
		case 2:
			punto2[0]= x;
			punto2[1]= y;
			break;
		case 3: 
			punto3[0]= x;
			punto3[1]= y;
			break;

	}
} 

void drawtablero(){
  // Cambiamos el color de la letra a azul
  setTextColor(BLUE);
  // Iniciamos la ventana con 600px de ancho(x) y 400px de altura(y)
  initwindow(600,400);
  // Imprimios en pantalla el texto inicial (SPLASH SCREEN)
  outtextxy(250,200,"Pantalla de Inicio XD");
  outtextxy(30,300,"pulse una tecla para continuar ...");
  // Funcion para obtener un carcter. / sin argumentos permite una pausa 
  // y espera que se oprima una tecla
  getch();


  
}

void drawtriangle(){

}
