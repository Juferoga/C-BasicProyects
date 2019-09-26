#include <stdio.h>
#include <stdlib.h>



int main() {
	
	int edad;
	
	//pedir datos
	printf("Digite su edad => ");
	scanf("%d", &edad);
	
	// calcular rangos
	if(edad < 2){
		printf("Eres un bebé genio");			
	}else if(edad >= 2 && edad < 12){
		printf("Eres un niño");
	}else if(edad >= 12 && edad < 18){
		printf("Eres un adolescente");
	}else if (edad >= 18 && edad < 30){
		printf("Eres un joven");
	}else{
		printf("Eres un adulto");
	}
		
	return 0;
}
