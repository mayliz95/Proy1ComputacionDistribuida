from multiprocessing import Pool
import time

def leerArchivo(archivo):

    tuplaComp = []
    file = open(archivo,"r")    
    contenido = file.readlines()
    for linea in contenido[1:len(contenido)]:
        sep1 = linea.find('\t')
        sep2 = linea.find('\t', sep1 + 1)
        sep3 = linea.find('\t', sep2 + 1)
        if linea == len(contenido) - 1:
            sep4 = (linea.__len__())
        else:
            sep4 = linea.find('\t', sep3 + 1)

        indice = (linea[sep1 + 1:sep2])
        compuesto = (linea[sep3 + 1:sep4])
        dicCom = obtDicComp(compuesto)
        NroElemComp = obtNumComp(dicCom)        
        tuplaComp.append((indice,compuesto, dicCom, NroElemComp))      
    return tuplaComp

def escribirArchivo(lineas, nombre):
    archivo = open(nombre, "a")    
    archivo.write(lineas)
    archivo.close    

#Calculo del coeficiente
def calculoCoefJT(compuesto1, Na, compuesto2, Nb):   

    dicComunes = obtDiccComunes(compuesto1,compuesto2)
    Nc = obtNumComp(dicComunes)
    #print n1, " ", n2, " ", n3
    res = (Nc/float (Na+Nb-Nc))
    return "{0:.2f}".format(res)

def obtDicComp(comp1):
    diccionario = {}
    for letra in comp1:
    	if (diccionario.has_key(letra) and letra!="@"):
    		diccionario[letra] = diccionario[letra] + 1                    
        else:
            diccionario[letra] = 1
    #for k in diccionario.keys():
    #    if k == "@":
    #        if diccionario["@"] > 1:
    #            diccionario["@"] = 1
    return diccionario

def obtDiccComunes(dc1,dc2):
    Aux = set(dc1).intersection(set(dc2)) #Arreglo con los elementos comunes
    dic3 = {}
    for k in Aux:
        if (dc1[k] > dc2[k]):
            dic3[k] = dc2[k]
        else:
            dic3[k] = dc1[k]
    return dic3

def obtNumComp(diccionario):
    n = 0
    for k, v in diccionario.items():
        n += (int(v))
    return n

def comparar(i):
    coef = ""
    listaRes = []
    #for i in range (len(compDic)):
    for j in range (0,i):         #Triangulo inferior
        coef += listaCompuesto[i][0]+ "\t"+listaCompuesto[j][0]+"\t"+ (
        calculoCoefJT(compuesto1=listaCompuesto[i][2], Na=listaCompuesto[i][3], compuesto2=listaCompuesto[j][2], Nb=listaCompuesto[j][3])) + "\n"        
        #listaRes.append(coef)
    return coef

def main ():    
    global listaCompuesto 
    listaCompuesto= leerArchivo("ZINC_chemicals.tsv")
    inPs = time.time()
    pool = Pool(processes=4)  # Se crea el pool de procesos y se indica el numero de procesos
    print "calular"    
    endPS = time.time()
    open('soluciones.tsv', 'w').close() #Para vaciar el archivo en caso de que se haya escrito
    for i in pool.map(comparar, range(len(listaCompuesto))):        #Indica que los procesos se realicen en paralelo 
        escribirArchivo(i,"soluciones.tsv")								#solo se puede iterar un argumento/ i linea a imprimir    
    print "Tiempo de procesamiento", endPS - inPs

#Ejecucion ---------------------------------------
ini = time.time()
if __name__ == '__main__':
    main()
end = time.time()
print end-ini
