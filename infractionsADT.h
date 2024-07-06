#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct infractionSystemCDT * infractionSystemADT;

/*
    Crea un nuevo sistema de infracciones.
    Recibe el rango de años necesario para el query 4.
*/
infractionSystemADT newInfractionSystem(size_t minYear, size_t maxYear);

/*
    Añade las infracciones en orden alfabetico sin repetidos. 
    Guarda los valores de id y el tipo de infraccion.
    Inicializa en NULL la lista para las patentes que cometieron la infraccion.
    Retorna 1 si la agrego o 0 si no.
*/
int addInfraction(infractionSystemADT infractionSystem,char *description,size_t id);

/*
    Añade las agencias en orden alfabetico o agrega una infraccion si la agencia ya se encontraba en la lista.
    Almacena en un vector de estructuras el tipo de infraccion y la cantidad de multas de dicha infraccion.
*/
int addAgency(infractionSystemADT infractionSystem, char * agency, size_t id);

/*
    Buscar en el vector de ids si el id que le pasan es valido, si no es valido retorna 0.
    Si es valido y la patente no se encuentra entonces la añade de forma alfabetica, sino,
    aumenta el contador de multas para dicha patente. En ambos casos retorna 1.
*/
int addTicket(infractionSystemADT infractionSystem, size_t id, char *plate);

//funciones de iteracion para la lista de infracciones
void toBegin(infractionSystemADT infractionSystem);

int hasNext(infractionSystemADT infractionSystem);

void * next(infractionSystemADT infractionSystem);

//funciones de iteracion para las agencias

void toBeginByAgency(infractionSystemADT a);

int hasNextByAgency(infractionSystemADT a);

char *nextByAgency(infractionSystemADT a);

//funcion free infraction
void freeInfractionSystem(infractionSystemADT infractionSystem);

//Estructuras para los queries
typedef struct nodeQuery1{
    char * infraction; //nombre de la infraccion
    size_t totalInfracctions; //cantidad de infracciones;
    struct nodeQuery1 * tail; //proxima infraccion
}TNodeQ1;

typedef TNodeQ1 * TListQ1;

typedef struct TQuery1{
    TListQ1 first;
    TListQ1 iter;
}TQuery1;

typedef struct vecQuery2{
    char *agency; //nombre de la agencia emisora
    char *mostPopularInf; // infraccion con mas multas de cada agencia
    size_t fineCount; // cantidad de multas de la infraccion 
}vecQuery2;

typedef struct query2{
    vecQuery2 * dataVec; // vector de tipo quer2, cada posicion tiene los datos que piden del query
    size_t dim; // dimension del vector
}TQuery2;

typedef struct listQuery3{
    char * infraction; //nombre infraccion
    char * plate; //numero de patente
    size_t totalInfractions; //cantidad de infracciones
    struct listQuery3 * tail;
}ListQuery3;

typedef ListQuery3 * TListQuery3;

typedef struct query3{
    TListQuery3 first;
    TListQuery3 iter;
    TListQuery3 last;
}TQuery3;

typedef struct nodeq4{
    size_t year;                    
    char * monthTop1; //mes con mayor cantidad de infracciones
    char * monthTop2; //mes con segunda mayor cantidad de infracciones
    char * monthTop3; //mes con tercera mayor cantidad de infracciones
}TNodeq4;

typedef struct Tquery4{
    TNodeq4 * vec;
    size_t dim;
}TQuery4;

//QUERY 1: Total de multas por infraccion en forma descendente por la cantidad total de multas
TQuery1 * query1(infractionSystemADT infractionSystem);

//QUERY 2: Infraccion mas popular de cada agencia emisora, con su cantidad respectiva de multas, en orden alfabetico por agencia emisora
TQuery2 *query2(infractionSystemADT infractionSystem);

//QUERY 3: La cantidad mayor de multas de una patente por infraccion en orden alfabetico
TQuery3 * query3(infractionSystemADT infractionSystem);

//QUERY 4: Top 3 meses con mas multas por anio, ordenado cronologicamente por anio
TQuery4 *query4(infractionSystemADT infractionSystem);

//funciones iteracion y free Query1 (las vamos a necesitar para el main)
void toBeginQ1(TQuery1 * query1);
int hasNextQ1(TQuery1 * query1);
void * nextQ1(TQuery1 * query1);
void freeQ1(TQuery1 * query1);

//funciones iteracion y free Query2 (las vamos a necesitar para el main)
void toBeginQ2(TQuery2* query2);
int hasNextQ2(TQuery2* query2);
void *nextQ2(TQuery2* query2);
void freeQ2(TQuery2* query2);

//funciones iteracion y free Query3 (las vamos a necesitar para el main)
void toBeginQ3(TQuery3* query3);
int hasNextQ3(TQuery3* query3);
void *nextQ3(TQuery3* query3);
void freeQ3(TQuery3* query3);