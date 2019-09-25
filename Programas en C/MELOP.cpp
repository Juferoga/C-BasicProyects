#include<iostream>
using namespace std ;
main(){
	cout<<"    --------------------------------------------------------------------------------------------------------------"<<endl;
	cout<<"   |C A L C U L A D O R A  D E  M I N I M O  C O M U N  M U L T I P L O  Y  M A X I M O  C O M U N  D I V I S O R |"<<endl;
	cout<<"    --------------------------------------------------------------------------------------------------------------"<<endl;
	system("color 1B");
 int no1,no2,r,mcd,mcm,aux;
 cout<<endl;
 do{
  cout<<"                                         Ingrese primer numero :                                                "<<endl;
  cout<<"                                                 "; cin>>no1;
  cout<<"                                         Ingrese segundo numero :                                               "<<endl;
  cout<<"                                                 "; cin>>no2;
 }while(no1<0 && no2<0);
 mcd=no1;
 aux=no2;
 do{
  r=mcd%aux;
  mcd=aux;
  aux=r;
 } while(r!=0);
 mcm=no1*no2/mcd ;
 cout<<endl;
 cout<<endl;
 cout<<"                                 -----------------------------------------                                                                  "<<endl;
 cout<<"                                |   El maximo comum divisor es  : |"<<mcd<<endl;
 cout<<"                                 -----------------------------------------                                                                  "<<endl;
 cout<<"                                |   El minimo comum multiplo es : |"<<mcm<<endl;
 cout<<"                                 -----------------------------------------                                                                  JF"<<endl;
 return 0;
}
