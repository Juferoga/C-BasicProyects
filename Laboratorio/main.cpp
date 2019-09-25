#include <iostream>
#include <stdio.h>
#include <string.h>
#include "consola.h"

using namespace std;

int res[3][3];
int numeros[3][3];
int opcion=0; //opcion del menu
bool hemosTerminado;

char menu[11][16] = {
                    "1  Agregar     ", 
			        "2  Eliminar    ", 
			        "3  Ver         ",
					"4  Sumar       ",
					"5  Restar      ",
					"6  Multiplicar ",
					"7  Determinante",
					"8  Inversa     ",
					"9  Transpuesta ",
			        "10 Salir       "};

void verMenu();
void ejecutarOpcion(int);
void agregar();
void agregar(){
	clearScreen();
	gotoXY(25, 10);
		for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
	
	        clearScreen();
	        gotoXY(25, 10);
			cout<<"Digite un numero ["<<i<<"]["<<j<<"]: ";
			cin>>numeros[i][j];
			
		}
	}
	clearScreen();
	gotoXY(79,24);
	void verMenu();
}
void eliminar();
void eliminar(){
   //numeros[3][3]={0};
}
void ver();
void ver(){
	clearScreen();
	gotoXY(25, 10);
	cout<<"\nMatriz Normal\n";
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			gotoXY(25+i, 10+j);
			cout<<numeros[i][j]<<" ";
		}
		gotoXY(25, 10);
		cout<<"\n";
			
	}
	waitKey();
}
void sumar();
void sumar(){
	clearScreen();
	 for( int i=0;i<3;i++){
     for(int j=0;j<3;j++){
            res[i][j]=numeros[i][j]+numeros[i][j];
      }
 }
  for( int i=0;i<3;i++){
     for(int j=0;j<3;j++){
 
            cout<<res[i][j]<<endl;
      }
  }
 	waitKey();

}
void restar();
void restar(){
	clearScreen();
	for( int i=0;i<3;i++){
     for(int j=0;j<3;j++){
            res[i][j]=numeros[i][j]-numeros[i][j];
      }
 }
  for( int i=0;i<3;i++){
     for(int j=0;j<3;j++){
 
            cout<<res[i][j]<<endl;
      }
  }
 	waitKey();
}
void multiplicar();
void multiplicar(){
	clearScreen();
	for( int i=0;i<3;i++){
     for(int j=0;j<3;j++){
            res[i][j]=numeros[i][j]*numeros[i][j];
      }
 }
  for( int i=0;i<3;i++){
     for(int j=0;j<3;j++){
 
            cout<<res[i][j]<<endl;
      }
  }
 	waitKey();

}
void determinante();
void determinante(){
	clearScreen();
 int det, pos, neg;
    pos=(numeros[0][0]*numeros[1][1]*numeros[2][2])+(numeros[1][0]*numeros[2][1]*numeros[0][2])+(numeros[0][1]*numeros[1][2]*numeros[2][0]);
    neg=(numeros[0][2]*numeros[1][1]*numeros[2][0])+(numeros[1][2]*numeros[2][1]*numeros[0][0])+(numeros[0][1]*numeros[1][0]*numeros[2][2]);
    det=pos-neg;
    cout << "El determinante es: "<<det<<endl;
     	waitKey();

}
void inversa();
void inversa(){
clearScreen();

 	waitKey();
}
void TP();
void TP(){
	clearScreen();
	gotoXY(25, 10);
	cout<<"\nMatriz Transpuesta\n";
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			gotoXY(25+i, 10+j);
			cout<<numeros[j][i]<<" ";
		}
		gotoXY(25, 10);
		cout<<"\n";
	}
	waitKey();
}


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
					opcion = 9;
				}
				break;
			case 80: // flecha abajo
				opcion++; 
				if(opcion > 9){
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
	cuadroDoble(34,5,53,16,"MENU"); // Menu de opciones
	// Imprimimos las opciones
	setTextColor(BLUE);
	for(i=0; i<11; i++){
		if(i==opcion){
			setBackColor(LIGHT_GRAY);
			setTextColor(BLACK);
		}else{
			setBackColor(BLACK);
			setTextColor(WHITE);
		}
		gotoXY(35,6+i);
		cout<<menu[i];
		gotoXY(35,6+opcion);
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
		case 2: // ver
			ver();
			break;
		case 3: // Sumar
			sumar();
			break;
		case 4: // Restar
			restar();
			break;
		case 5: // Multiplicar
			multiplicar();
			break;
		case 6: // Determinante
			determinante();
			break;
		case 7: // Inversa
			inversa();
			break;
		case 8: // Transpuesta
			TP();
			break;
		case 9: //salir
			setBackColor(BLACK);
			setTextColor(WHITE);
			clearScreen();
			hemosTerminado = true;
			break;
	}
}

