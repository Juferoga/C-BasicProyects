#include <iostream>
#include <stdio.h>
#include <string.h>
#include "consola.h"

using namespace std;

void verMenu();
void ejecutarOpcion(int);
void agregar();
void eliminar();
void ver();

int opcion=0; //opcion del menu
bool hemosTerminado;

char menu[4][11] = {" Agregar  ", 
			        " Eliminar ", 
			        " Ver      ", 
			        " Salir    "};

int main(int argc, char** argv) {
	int tecla;	
	hemosTerminado = false;
	
	do{
		setBackColor(BLACK);
		setTextColor(WHITE);
		clearScreen();
		verMenu();
		tecla = waitKey();		
		switch(tecla){
			case 72: // flecha arriba
				opcion--; 
				if(opcion < 0){
					opcion = 3;
				}
				break;
			case 80: // flecha abajo
				opcion++; 
				if(opcion > 3){
					opcion = 0;
				}
				break;
			case 13: // enter
				ejecutarOpcion(opcion);
				break;
		}
	}while(!hemosTerminado);	
	return 0;
}

void verMenu(){	
	int i;	
	setTextColor(BLUE);
	cuadroSencillo(0,0,79,24,""); // Limite pantalla
	setTextColor(CYAN);
	cuadroDoble(34,10,45,15,"MENU"); // Menu de opciones
	// Imprimimos las opciones
	setTextColor(BLUE);
	for(i=0; i<4; i++){
		if(i==opcion){
			setBackColor(YELLOW);
			setTextColor(RED);
		}else{
			setBackColor(BLACK);
			setTextColor(WHITE);
		}
		gotoXY(35,11+i);
		cout<<menu[i];
		gotoXY(35,11+opcion);
	}	
	//cout<<sizeof(menu)/sizeof(menu[0]);  // Para saber cuantas opciones hay
}

void ejecutarOpcion(int op){
	switch(op){
		case 0: // agregar
			agregar();
			break;
		case 1: // eliminar
			eliminar();
			break;
		case 2: //ver
			ver();
			break;
		case 3: //salir
			setBackColor(BLACK);
			setTextColor(WHITE);
			clearScreen();
			hemosTerminado = true;
			break;
	}
}

void agregar(){
	clearScreen();
	gotoXY(25, 10);
	setBackColor(YELLOW);
	setTextColor(BLUE);
	cout<<"AGREGAR USUARIO AL LISTADO";
	
	gotoXY(79,24);
	waitKey();
}

void eliminar(){
	
}

void ver(){
	
}

