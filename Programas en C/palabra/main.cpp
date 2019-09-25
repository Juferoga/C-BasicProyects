#include <iostream>
#include "consola.h"
#include <string.h>

char* invertir(char []);
void contar(char *);

using namespace std;

int numVocales, numConsonantes, numOtros;

int main(int argc, char** argv) {
	
	char palabra[30];
	char p2[30];
	
	numConsonantes = 0;
	numVocales = 0;
	numOtros = 0;
	
	clearScreen();
	setTextColor(YELLOW);
	gotoXY(5, 2);
	cout<<"Escriba una palabra ";
	setTextColor(RED);
	cout<<"=> ";
	setTextColor(WHITE);
	cin>>palabra;
	
	setTextColor(YELLOW);
	gotoXY(5, 2);
	cout<<"La palabra invertida es: ";
	setTextColor(LIGHT_CYAN);
	strcpy(p2, invertir(palabra));
	cout<<p2;
	contar(palabra);
	gotoXY(10, 12);
	cout<<"Longitud total: "<<strlen(palabra);
	gotoXY(10, 13);
	cout<<"La cantidad de vocales: "<<numVocales;
	gotoXY(10, 14);
	cout<<"La cantidad de consonantes: "<<numConsonantes;
	gotoXY(10, 15);
	cout<<"La cantidad de simbolos: "<<numOtros;
	return 0;
}

char* invertir(char p[]){
	
	char respuesta[30];
	char c;
	int i; // variable contador para recorrido del vector
	// Vamos a recorrer el arreglo original hasta encontrar '\0' que es el fin de la cadena
	
	int longitud = 0;	
	longitud = strlen(p); // Retorna el tamaño del string
		
	for(i=0; i<longitud;i++){
		respuesta[longitud-1-i] = p[i];
	}
	respuesta[longitud]='\0';
	return respuesta;	
	
}

void contar(char *p){
	int i;
	char c;
	for(i=0; i<strlen(p); i++){
		c = p[i];
		if((c>=65 && c<=90)||(c>=97 && c<=122)){
			switch(c){
				case 65:
				case 97:
				case 'E':
				case 'e':
				case 'I':
				case 'i':
				case 'O':
				case 'o':
				case 'U':
				case 'u':
					numVocales++;
					break;
				default:
					numConsonantes++;
			}
		}else{
			numOtros++;
		}
	}
	return;
}
