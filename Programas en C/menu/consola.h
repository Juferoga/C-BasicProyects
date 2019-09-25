#ifndef _CONSOLA_H_
#define _CONSOLA_H_

#include <iostream>

// normal
#define BLACK 			0
#define BLUE 			1
#define GREEN			2
#define CYAN			3
#define RED				4
#define PURPLE			5
#define BROWN			6
#define LIGHT_GRAY		7
#define DARK_GRAY		8

//alta intensidad
#define LIGHT_BLUE		9
#define LIGHT_GREEN		10
#define LIGHT_CYAN		11
#define LIGHT_RED		12
#define LIGHT_PURPLE	13
#define YELLOW			14
#define WHITE			15

int colorTexto = LIGHT_GRAY;
int colorFondo = BLACK;

/*
void gotoXY(int x, int y); 
void clearScreen();
void setTextColor(int);
void setBackColor(int);
int getTextColor();
int getBackColor();
void cuadroSencillo(int, int, int, int, char *);
void cuadroDoble(int, int, int, int, char *);
char waitKey();
*/

#ifdef __linux__
	#define SO "Linux"	

void clearScreen(){
	system("clear");
}

char waitKey(){
	
}

/*
Códigos de escape ANSI.

Colores
A continuación describiremos el esquema general que hay que seguir para imprimir textos coloreados, independientemente de la paleta de colores que se utilice:

"\033[1;XYmTEXTO"

\033[   Estos caracteres son obligatorios al principio del código de color. Indican que se va a utilizar un carácter ANSI. \x1b en hexadecimal

1;      Es opcional. Indica que el color a utilizar es brillante. Elimínalo si quieres un color normal.

X       Indica si el color que se va a cambiar es del texto o el del fondo.
        3 Color del texto
        4 Color del fondo

Y       Selecciona el color // Si el color es 8, es un color personalizado

m       Indica el final del código de color.

TEXTO   El texto que quieres colorear. Se pueden insertar en él tantos códigos de color como se quiera.
        Si un código de color cambiar el color del texto pero no el color del fondo, el color del texto se reemplaza por el nuevo color pero el del fondo no, y viceversa

Colores de 24 bits (personalizados):

Siguen el siguiente esquema:

"\033[1;X8;2;RRR;GGG;BBBmTEXTO"

Donde:

X 3 para el color del texto, 4 para el del fondo.

2; indica que la paleta de colores que se va a utilizar es de 24 bits.

RRR es un número de 0 a 255 que indica la cantidad de rojo que contendrá el color seleccionado.

GGG es un número de 0 a 255 que indica la cantidad de verde que contendrá el color seleccionado.

BBB es un número de 0 a 255 que indica la cantidad de azul que contendrá el color seleccionado.

CONTROL DEL CURSOR

 \033[y;xH   Using ANSI escape sequence, where ESC[y;xH moves curser to row y, col x:
*/

void gotoXY(int x, int y) 
{ 
	// Se usa ANSI
	printf("\033[%d;%dH", y, x);
    // Asigna posición del cursor	AH = 02h	BH = Página, DH = Fila, DL = Columna
}

int colors[16] = {0,4,2,6,1,5,3,7,7,64,62,66,61,65,63,67};

/*
    normal (40-47)
# BLACK            0
# BLUE             4
# GREEN            2
# CYAN	      	   6
# RED              1
# PURPLE           5
# BROWN            3
# GRAY        	   7
# DARK_GRAY		   7

	alta intensidad (100-107)
# LIGHT_BLUE       64
# LIGHT_GREEN      62
# LIGHT_CYAN       66
# LIGHT_RED        61
# LIGHT_PURPLE     65
# YELLOW           63
# WHITE            67
*/

void setBackColor(int color)
{
    if(color < 0 || color > 15) return;    
    colorFondo = color;
    printf("\033[%dm", 40 + colors[colorFondo]);
    return;
}

void setTextColor(int color)
{
    if(color < 0 || color > 15) return;    
    colorTexto = color;
    printf("\033[%dm", 30 + colors[colorTexto]);
    return;
}

int getTextColor(){
	return colorTexto;
}

int getBackColor(){
	return colorFondo;
}

#elif _WIN32
	#define SO "Windows"

	#include "conio.h"
	#include <windows.h>


HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;
int colors[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

char waitKey(){
	return _getch();
}

void gotoXY(int x, int y) 
{ 
	CursorPosition.X = x; // Locates column
	CursorPosition.Y = y; // Locates Row
	SetConsoleCursorPosition(console,CursorPosition); // Sets position for next thing to be printed 
}

void clearScreen(){
	system("cls");
}

/*
Se puede utilizar para establecer colores, el comando COLOR del sistema:

COLOR XX
En C se puede invocar así:    system("COLOR XX");

Los atributos de color están especificados con dos dígitos hex (el primero corresponde al segundo plano o fondo; el segundo al primer plano). Los 
dígitos pueden ser cualquiera de los siguientes valores:

   0 = Negro       8 = Gris
   1 = Azul        9 = Azul claro
   2 = Verde       A = Verde claro
   3 = Aguamarina  B = Aguamarina claro
   4 = Rojo        C = Rojo claro
   5 = Púrpura     D = Púrpura claro
   6 = Amarillo    E = Amarillo claro
   7 = Blanco      F = Blanco brillante

Si no se indican argumentos, este comando restaura el color que tenía cuando se inició CMD.EXE. El comando COLOR configura ERRORLEVEL a 1 si se realiza un 
intento de ejecutar el comando COLOR con el mismo color de primer y segundo plano.

ó

SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),X); 

Con este comando se le puede cambiar el color de texto y de fondo a lo que se muestre despues de que la linea se ejecute, siendo X un numero entre 0 y 255. 
Este número se obtiene así:

X = A*16 + B; 

A=color de fondo 
B=color de texto 

Nota: (De la A a la F los valores son de 10 a 15 respectivamente) 

Asi, si queremos por ejemplo un texto azul claro sobre fondo amarillo: 

X=6(fondo amarillo)*16+9(texto azul claro)= 105 

*/

void setTextColor(unsigned short c){
	if(c < 0 || c > 15)
		return;
	colorTexto = c;
	WORD wColor = ((colors[colorFondo] & 0x0F) << 4) + (colors[colorTexto] & 0x0F);
	SetConsoleTextAttribute(console, wColor);
}

void setBackColor(unsigned short c){
	if(c < 0 || c > 15)
		return;
	colorFondo = c;
	WORD wColor = ((colors[colorFondo] & 0x0F) << 4) + (colors[colorTexto] & 0x0F);
	SetConsoleTextAttribute(console, wColor);
}

int getTextColor(){
	return colorTexto;
}

int getBackColor(){
	return colorFondo;
}

#endif


void cuadroSencillo(int x0, int y0, int x1, int y1, char *titulo){
	int x, y;
	int longTitulo, anchoCuadro;
	
	gotoXY(x0, y0);
	std::cout<<(char)218; // esquina superior izq
	
	gotoXY(x1, y0);
	std::cout<<(char)191; // esquina superior der
	
	gotoXY(x0, y1);
	std::cout<<(char)192; // esquina inferior izq
	
	gotoXY(x1, y1);
	std::cout<<(char)217; // esquina inferior der
	
	for(x = x0 + 1; x < x1; x++){
		// lineas horizontales
		gotoXY(x, y0);
		std::cout<<(char)196;  // superior
		gotoXY(x, y1);
		std::cout<<(char)196; // inferior		
	}
	
	for(y = y0 + 1; y < y1; y++){
		// lineas verticales
		gotoXY(x0, y);
		std::cout<<(char)179;  // izq
		gotoXY(x1, y);
		std::cout<<(char)179; // der		
	}
	
	anchoCuadro = x1 - x0;
	longTitulo = strlen(titulo);
	if(longTitulo != 0){
		gotoXY(x0 + (int)((anchoCuadro - longTitulo)/2), y0);
		std::cout<<titulo;
	}
	gotoXY(79,24);
}

void cuadroDoble(int x0, int y0, int x1, int y1, char *titulo){
	int x, y;
	int longTitulo, anchoCuadro;
	
	gotoXY(x0, y0);
	std::cout<<(char)201; // esquina superior izq
	
	gotoXY(x1, y0);
	std::cout<<(char)187; // esquina superior der
	
	gotoXY(x0, y1);
	std::cout<<(char)200; // esquina inferior izq
	
	gotoXY(x1, y1);
	std::cout<<(char)188; // esquina inferior der
	
	for(x = x0 + 1; x < x1; x++){
		// lineas horizontales
		gotoXY(x, y0);
		std::cout<<(char)205;  // superior
		gotoXY(x, y1);
		std::cout<<(char)205; // inferior		
	}
	
	for(y = y0 + 1; y < y1; y++){
		// lineas verticales
		gotoXY(x0, y);
		std::cout<<(char)186;  // izq
		gotoXY(x1, y);
		std::cout<<(char)186; // der		
	}
	anchoCuadro = x1 - x0;
	longTitulo = strlen(titulo);
	if(longTitulo != 0){
		gotoXY(x0 + (int)((anchoCuadro - longTitulo)/2), y0);
		std::cout<<titulo;
	}
	gotoXY(79,24);
	gotoXY(79,24);
}


#endif //_CONSOLA_H_

