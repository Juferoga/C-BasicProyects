#include <iostream>
#include <stdio.h>
#include <string.h>
#include "consola.h"

using namespace std;

int res[3][3];
int numeros[3][3];
int numeros2[3][3];
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
		for(int j=1;j<=3;j++){
		for(int i=1;i<=3;i++){
	
	        clearScreen();
	        gotoXY(25, 10);
			cout<<"Digite un numero ["<<i<<"]["<<j<<"]: ";
			cin>>numeros[i][j];
			
		}
	
	}
	int opc;
	gotoXY(25, 10);
		cout<<"quiere digitar una segunda matriz digite 1"<<endl;
		cin>>opc;
if(opc==1){

	gotoXY(25, 10);
	cout<<"agregue la matriz 2"<<endl;
		for(int j=1;j<=3;j++){
		for(int i=1;i<=3;i++){
	
	        clearScreen();
	        gotoXY(25, 10);
			cout<<"Digite un numero ["<<i<<"]["<<j<<"]: ";
			cin>>numeros2[i][j];
			
		}
	}
	clearScreen();
	gotoXY(79,24);
    void verMenu();
}
else{

	void verMenu();
}
}
void eliminar();
void eliminar(){
   //numeros[3][3]={0};
}
void ver();
void ver(){
	clearScreen();
	gotoXY(45, 10);
	cout<<"\nMatriz Normal 1\n";
	for(int i=1;i<=3;i++){
		for(int j=1;j<=3;j++){
			gotoXY(25+i, 10+j);
			cout<<numeros[i][j]<<" ";
		}
	}
		gotoXY(45, 15);
		cout<<"\n";
		cout<<"\nMatriz Normal 2\n";
	for(int i=1;i<=3;i++){
		for(int j=1;j<=3;j++){
			gotoXY(25+i, 15+j);
			cout<<numeros2[i][j]<<" ";
		}	
	}
	waitKey();

}
void sumar();
void sumar(){
	clearScreen();
	 for( int i=0;i<3;i++){
     for(int j=0;j<3;j++){
            res[i][j]=numeros[i][j]+numeros2[i][j];
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
            res[i][j]=numeros[i][j]-numeros2[i][j];
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
            res[i][j]=numeros[i][j]*numeros2[i][j];
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
 
    pos=(numeros[1][1]*numeros[2][2]*numeros[3][2])+(numeros[1][2]*numeros[2][3]*numeros[3][1])+(numeros[1][3]*numeros[2][1]*numeros[3][3]);
    neg=-(numeros[1][3]*numeros[2][2]*numeros[3][1])-(numeros[1][2]*numeros[2][1]*numeros[3][2])-(numeros[1][1]*numeros[3][3]*numeros[2][3]);
    det=pos+neg;
    cout<<numeros[1][1]<<numeros[2][2]<<numeros[3][3]<<"()"<<numeros[1][2]<<numeros[2][3]<<numeros[3][1]<<"()"<<numeros[1][3]<<numeros[2][1]<<numeros[3][2];
    cout << "El determinante es: "<<pos<<neg<<endl;
    cout << det ;
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
	for(int i=1;i<=3;i++){
		for(int j=1;j<=3;j++){
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

