#include <stdio.h>
#include <stdlib.h>


int main() {
	
	int numeroVocales = 0;
	char nombre[15];
	int posicion;
	char caracter;
	
	// ´pedirle al usiario que digite el nombre y se guarda
	printf("Digite un nombre: ");
	scanf("%s", &nombre);
	
	// contar las vocales
	
	for(posicion = 0; posicion < 15; posicion++){
		caracter = nombre[posicion];
		printf("\nAnalizando %c :", caracter);	
		if(caracter == 'A' || caracter == 'E' || caracter == 'I' || caracter == 'O' || caracter == 'U'
			|| caracter == 'a' || caracter == 'e' || caracter == 'i' || caracter == 'o' || caracter == 'u')
		{
			numeroVocales++;
			printf(" Es vocal");
		}else{
			if(caracter == '\0'){
				printf(" FIN");
				break;
			}
			printf(" Es consonante");
		}				
	}
	
	
	//mostrar resultados
	printf("\n\nSu nombre es %s y tiene %d vocales\n\n", nombre, numeroVocales);
	
	return 0;
}


