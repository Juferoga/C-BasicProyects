#include <stdio.h>
#include <stdlib.h>


int main() {
	
	int opcion;
	
	printf("       MENU DEL DIA\n\n");
	printf("1. Pollo\n");
	printf("2. Carne\n");
	printf("3. Cerdo\n");
	printf("4. Salmón\n");
	printf("5. Lasagna\n");
	printf("6. Vegetariano\n\n");
	printf("Digite su opcion => ");
	
	opcion = getche();
	
	switch(opcion){
		case 49:
			printf("\n\nExcelente opcion, que disfute su pollo\n");
			break;
		case '4':
			printf("\n\nEres un oso al salir de hibernación\n");
			break;
		case 50:
			printf("\n\nMala elección para las vacas\n");
			break;
		case 53:
			printf("\n\nIl divo decizione!\n");
			break;
		case '3':
			printf("\n\nOicnk oink me comiste\n");
			break;
		case '6':
			printf("\n\nCon razon tan flaco\n");
			break;
		default:
			printf("\n\nLo siento, pero no tene,os ese plato\n");
	}
	return 0;
}
