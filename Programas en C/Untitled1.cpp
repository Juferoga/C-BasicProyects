#include <iostream>
using namespace std;
main(){
	int OPC;
	cout<<"digite 1 para prog. de menu 2 para prog. de ...";
	cin>>OPC;
	switch(OPC){
	case 1:
		system("color 8f");
		cout <<"________________________________________________________________________________"<<endl;
		cout <<"_____________________________    M E N U    ____________________________________"<<endl;
		cout <<"________________________________________________________________________________"<<endl;cout<<endl;cout<<endl;

	int opc,opcq;
cout<<"digite 1 si desea pollo, 2 para hamburguesa, 3 para pizza, 4 para corrientazo"<<endl;
cin>>opc;
switch (opc){
	case 1 :
		cout<< "ud desea pollo"<<endl;
		break;
	case 2 :
		cout<< "si la desea con queso digite 1 si no digite 2"<<endl;
		cin>>opcq;
		if(opcq==1){
			cout<< "ud desea hamburguesa con queso"<<endl;
		}
		else{
			cout<< "ud desea hamburguesa sin queso"<<endl;
		}
		break;
	case 3 :
		cout<< "ud desea pizza"<<endl;
		break;
	case 4 :
		cout<< "ud desea corrientazo"<<endl;
		break;
	default :
		cout<< "ud no desea ningun producto"<<endl;
		break;
}
break;
	case 2 :
		
		break;
	default:
	cout<<"gracias por no usar el programa"<<endl;					
}
}
