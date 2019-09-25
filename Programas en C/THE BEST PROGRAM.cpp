#include <iostream>
using namespace std;
int main(){

	
	system("color 8f");
		cout <<"________________________________________________________________________________"<<endl;
		cout <<"______________________    CALCULADORA AREA TRIANGULO    ________________________"<<endl;
		cout <<"________________________________________________________________________________"<<endl;cout<<endl;cout<<endl;
	double B,H,R,P=1;
		while (1==P){
		cout<<"                           Digite Base Del Triangulo:                             "<<endl<<endl;
			cout<<"                                  -> "; cin>>B; cout<<endl;
		cout<<"                           Digite Altura Del Triangulo                            "<<endl<<endl;
			cout<<"                                  -> ";cin>>H;cout<<endl;
	R=(B*H)/2;
		cout<<"                                 El Area Es: " << R<<endl;
		cout<<"digite 1 para finalizar el programa:";
		cin>>P;
		if (P==1){
		P=3;
		}
}
	system("pause");
}
