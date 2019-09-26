
#include <stdio.h>
#include "graph.h"

#define TRUE   1
#define FALSE  0

char *titulo = "A mover el círculo";
int puntoRefX = 10;
int puntoRefY = 40;
int finX, finY;

//circulo
int xC, yC, rC;

void dibujarSuperficieDibujo();
void dibujarValores();
void dibujarInstrucciones();



int main() {
	int opcion;
	int terminarPrograma = FALSE;
	
	xC = 300;
	yC = 200;
	rC = 10;
	
	initwindow(700, 500);
	finX = getmaxx() - 10;
	finY = (int)(getmaxy() * 0.6);
	// -----
	
	do{
						
		//marco y titulo
		setcolor(COLOR(114, 242, 111));
		rectangle(5, 5, getmaxx() - 5, getmaxy() - 5);
		settextstyle(SANSSERIF_FONT, HORIZ_DIR, 3);
		outtextxy((getmaxx() - textwidth(titulo))/2, 6, titulo);
		
		dibujarSuperficieDibujo();
		dibujarValores();
		dibujarInstrucciones();	
		
		opcion = getch(); // espera a que pulse una tecla	
		switch(opcion){
			case KEY_ESC: //terminar el programa
				terminarPrograma = TRUE;
				break;
			case KEY_LEFT:
				xC--;
				break;
			case KEY_RIGHT:
				xC++;
				break;
			case KEY_UP:
				yC--;
				break;
			case KEY_DOWN:
				yC++;
				break;						
			case KEY_F2:
				rC++;
				break;
			case KEY_F3:
				if(rC > 3){
					rC--;	
				}				
				break;
			default:
				break;
		}
	
	}while(terminarPrograma != TRUE);
	closegraph();
	return 0;
}

void dibujarSuperficieDibujo(){
	int separacion = 50;	
	int x, y;
	char num[3];
	
	setfillstyle(SOLID_FILL, COLOR(255, 255, 255));
	bar(puntoRefX, puntoRefY, finX, finY);
	//lineas escala
	setcolor(LIGHTGRAY);
	settextstyle(SMALL_FONT, HORIZ_DIR, 4);
	for(x = 0; x < finX; x += separacion){
		line(puntoRefX + x, puntoRefY, puntoRefX + x, finY);	
		sprintf(num, "%d", x);
		outtextxy(puntoRefX + x, puntoRefY + 2, num);
	}
	
	for(y = 0; y < finY; y += separacion){
		line(puntoRefX, puntoRefY + y, finX, puntoRefY + y);	
		sprintf(num, "%d", y);
		outtextxy(puntoRefX + 2, puntoRefY + y, num);
	}
	
	setcolor(BLACK);
	circle(xC + puntoRefX, yC + puntoRefY, rC);
	//relleno
	setfillstyle(SOLID_FILL, GREEN);
	floodfill(xC + puntoRefX, yC + puntoRefY, BLACK);
}

void dibujarValores(){
	
}

void dibujarInstrucciones(){
	
	
}
