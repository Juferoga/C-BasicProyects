Algoritmo nomina 
	SB=reaL
	N=CARACTER 
	G=REAL
	M=REAL
	
	IMPRIMIR "digite su nombre" 
	LEER N
	imprimir "digite su genero H-hombre M-mujer"
	leer G 
	imprimir "digite su sueldo"
	leer SB
	imprimir "dijite sus meses trabajando"
	leer M
	
	SI G=H ENTONCES 
		SI SB<=800
		
	FIN SI 
	GH<-SB 
	SI SB>800 Entonces
		SB<-SB-(SB*0.5/100)
	FinSi
	IMPRIMIR "(SB)" 
FinAlgoritmo
