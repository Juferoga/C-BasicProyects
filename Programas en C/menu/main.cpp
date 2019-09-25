#include <stdio.h>
#include "consola.h"

// declaracion de variables globales
float n1, n2;

// declaracion de funciones
void ingresar();
void sumar();
void restar();
void ver();


// metodo main
int main(int argc, char** argv) {
	
	int opcion;
	bool hemosTerminado = false;
	
	do{
		clearScreen();  //limpiar la pantalla
		setBackColor(BLUE); // color de fondo
		setTextColor(YELLOW); // color de texto
		gotoXY(38, 2);  // posicion del cursor
		printf("MENU");
		setBackColor(BLACK);
		printf("\n1. Ingresar los numeros");
		printf("\n2. Sumar");
		printf("\n3. Restar");
		printf("\n4. Ver");
		printf("\n5. Salir");
		printf("\n\n Seleccione una opcion => ");
		scanf("%d", &opcion);
		
		switch(opcion){
			case 1:
				ingresar();
				break;
			case 2:
				sumar();
				break;
			case 3:
				restar();
				break;
			case 4:
				ver();
				break;
			case 5:
				hemosTerminado = true;
				break;
			default:
				printf("Opcion no válida!");
				break;
		}
		waitKey();
	}while(hemosTerminado != true);
	
	
	
	return 0;
}

// implementacion de funciones

void ingresar(){
	setTextColor(RED);
	printf("\nIngrese el valor para el primer número => ");
	scanf("%f", &n1);
	printf("\nIngrese el valor para el segundo número => ");
	scanf("%f", &n2);
}

void sumar(){
	printf("El resultado de la suma es %.3f", n1+n2);
}

void restar(){
	printf("El resultado de la resta es %.3f", n1-n2);
}

void ver(){
	printf("Los numeros guardados en memoria son: \nn1 -> %.2f\nn2 -> %.2f\n\n\n", n1, n2);
}
