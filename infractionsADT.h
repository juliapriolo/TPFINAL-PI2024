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

//Estructura para Query 1
typedef struct nodeQuery1{
    char * infraction;                  //nombre de la infraccion
    size_t totalInfracctions;           //cantidad de infracciones;
    struct nodeQuery1 * tail;           //proxima infraccion
}TNodeQ1;

typedef TNodeQ1 * TListQ1;

typedef struct TQuery1{
    TListQ1 first;
    TListQ1 iter;
}TQuery1;

typedef struct query2{
    char * agency;                  //nombre de la agencia emisora
    char * infraction;              //infraccion mas popular
    size_t totalInfractions;        //cantidad de infracciones
    struct query2 * tail;
}Tquery2;

//salida: nombre de la infracción;patente con la mayor cantidad de multas de esa infracción;cantidad de multas.
//2 o más patentes con la misma cantidad de multas para una infracción → se ordena alfabéticamente
typedef struct listQuery3{
    char * infraction; //nombre infraccion
    char * plate; //numero de patente
    size_t totalInfractions; //cantidad de infracciones
    struct listQuery3 * tail;
}ListQuery3;

typedef ListQuery3 * TListQuery3;

typedef struct query3{
    TListQuery3 first;
    TListQuery3 last;
}TQuery3;

typedef struct nodeq4{
    size_t year;                    
    char * monthTop1;               //mes con mayor cantidad de infracciones
    char * monthTop2;               //mes con segunda mayor cantidad de infracciones
    char * monthTop3;               //mes con tercera mayor cantidad de infracciones
}TNodeq4;

typedef struct TQuery4{
    TNodeq4 * vec;
    size_t dim;
}Tquery4;
