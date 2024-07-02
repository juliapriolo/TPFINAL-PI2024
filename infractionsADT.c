#include <stdio.h>
#include <stdlib.h>

#define MONTHS 12


typedef struct query3{
    char *plate;
    size_t fineCount; // por patente
}TQuery3;

typedef struct infractions{
    char *description;
    TQuery3 *arrPlates;
    size_t id;
    size_t totalFines;// total de multas por infraccion
    struct infractions *tail;
}TInfractions;

typedef struct infractions *TLInfractions;

typedef struct query2{
    char *description;
    size_t fineCount; // por agencia
}TQuery2;

typedef struct agency{
    char *agencyName;
    TQuery2 *infractions; 
    struct agency *tail;
}TAgency;

typedef struct agency *TLagency;

typedef struct id{
    char exists;
    TLInfractions pNode;
}TId;

typedef struct infractionsCDT{
    TLInfractions firstI;
    TLagency firstA;
    size_t *arrYears[MONTHS]; //vector de años y meses (multas)
    size_t minYear; //años para la query 4
    size_t maxYear;
    TId arrId; // vector con los id ordenados
    size_t dim; //dimension total
    size_t size; //espacio ocupado
}infractionsCDT;
