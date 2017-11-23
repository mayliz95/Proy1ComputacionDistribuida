#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <sstream>
#include <omp.h>
#include <time.h>
#include <math.h>
#include <vector>

using namespace std;
const int p=omp_get_max_threads();
std::vector<int> limitesP;
std::vector<string> Compuestos;
std::vector<string> Indices;
std::vector< std::vector<string> > resultado;
std::vector<int> numCarac;
int caracter[256];
string aux = "";

int contarCarac(string cadena) {
   int i, caracteres_dif = 0;
   int total = 0;
   for (i = 0; cadena[i] != '\0'; i++) // \0 final de string
   		if ((unsigned char)cadena[i] == 64){		//64 corresponde al @ si se encuentra esta letra solo se guarda 1 vez
        	caracter[(unsigned char) cadena[i]] = 1; //asigna codigo ascci segun el caracter /
    	} else {
        	caracter[(unsigned char) cadena[i]]++; //asigna codigo ascci segun el caracter
    	}  

   for (i = 0 ; i < 256; i++) {
      if (caracter[i] > 0 && caracter[i]!='\n'){																																																																																																																																																																																																																																																																																																																																																																																																																																																																														       
        total += caracter[i];
        caracter[i]=0;					//variable global para cada cadena tiene que volver a estar en 0
      }
   }
   return total;
}

int numComunes(string c1,string c2){
    char bu[256]; int b;				//Bu elementos que se analizan, b se usa como bandera
    int c=0,r1,r2,i,j,k,l,total = 0;
    for(i=0;i<c1.size();i++){
    b=1; //Indica que la letra aun no se analiza
        for(k=0;bu[k]!='\0'&&b==1;k++) { //Para no volver a analizar la misma letra
            if(c1[i]==bu[k]) {
                b=0; 				//Indica que la letra ya se analizo, deja de comparar
            }
        }
        if(b==1){   // si no se encuentra que la letra ya se analizo
            bu[c++]=c1[i];
            r1=0;b=0; //Aun no se analiza
            for(j=0;j<c2.size();j++) {
                if(c1[i]==c2[j] && c1[i]=='@'){
                    r1=1;b=1;} // b=1 ya se analizo y se marco como comun
                else if(c1[i]==c2[j]) {
                        b=1; r1++;
                }
            }
            if(b==1){
                r2=1; //Porque se detecto en la cadena 1
                for(l=i+1;l<c1.size();l++) {  //analiza la cadena 1 desde donde esta hasta el final
                	if(c1[i]==c1[l] && c1[l]=='@'){
                    	r2=1;}
                	else if(c1[i]==c1[l]) {
                        r2++;
                	}                   
                }
                if (r2 < r1) {
                    total += r2;
                } else {
                    total+=r1;
                }
            }
        }
    }
    if(r2==0)
        total =0;

    return total;
}

void listaNroElemComp() {
    int i;
    for (i=0;i< Compuestos.size(); i++) {                        
        numCarac.push_back(contarCarac(Compuestos[i]));            
    }    
}

double calculoCoefJT(int Na, int Nb, string Ca, string Cb) {
    int Nc;
    double res=0;
    Nc = numComunes(Ca,Cb);
    res = 100*(Nc / ((double) Na+Nb-Nc));
    return round(res)/100; //Para tener 2 decimales
}

int imprimir() {

    int i= 0;
    ofstream ficheroSalida;
    ficheroSalida.open ("soluciones.tsv", ios::out);
    for (int i = 0; i < resultado.size(); i++)
    {
    	for (int j = 0; j < resultado[i].size(); j++)
    	{
    		ficheroSalida << resultado[i][j];
    	}
    }       
    ficheroSalida.close();
    return 0;    
}

void limites() {

	limitesP.resize(p+1);
	int i=0;	
	for (i=0; i <=p; i++) {
		limitesP[i] = round(sqrt(i/(double)p)*Compuestos.size());
	}	
}

//void comparar(std::vector<string> &lista, int i) {
void comparar(int i, int id) {
    int j=0, k=0;        
    //for (i = 0; i < Compuestos.size(); i++) {
    for (j=0; j<i; j++) {          
        std::ostringstream os; //para cambiar a string el resultado
        os << calculoCoefJT(numCarac[i], numCarac[j], Compuestos[i], Compuestos[j]);
        std::string str = os.str();
        resultado[id].push_back(Indices[i] + "\t" + Indices[j] + "\t" +str+"\n");            
    }                                
}

void leerArchiv() {

    fstream ficheroEntrada;
    string frase;    

    ficheroEntrada.open ("ZINC_chemicals.tsv" , ios::in);
    ficheroEntrada.seekp(44, ios::beg);       

    while (! ficheroEntrada.eof() ) {
        getline(ficheroEntrada,frase,'\t');
        getline(ficheroEntrada,frase,'\t');
        Indices.push_back(frase);            
        getline(ficheroEntrada,frase,'\t');
        getline(ficheroEntrada,frase,'\n');
        Compuestos.push_back(frase);                   
    }
    ficheroEntrada.close();
    cout << "acaba de leer"<<endl;    
}

int main()
{
    time_t start,end;
    time(&start);
    double segu; 
    int i=0;    
    leerArchiv();
    listaNroElemComp();        
    limites();
    cout << "inicia Comparacion"<<endl;            
    resultado.resize(p);    
    #pragma omp parallel num_threads(p)    
    {
    	int id, i,istart,iend;
    	id = omp_get_thread_num();    	
    	istart =limitesP[id];    	
		iend = limitesP[id+1];				
    	for (i = istart; i < iend; i++) {        	
        	comparar(i,id);
    	}    
    }
    time(&end);	
    imprimir();
	segu = difftime(end,start);
	cout << "tiempo " <<segu<< endl;
    return 0;
}
