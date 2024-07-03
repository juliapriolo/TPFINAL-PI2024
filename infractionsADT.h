#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct infractionSystemCDT * infractionSystemADT;

infractionSystemADT newInfractionSystem(size_t minYear, size_t maxYear);
int addInfraction(infractionSystemADT infractionSystem,char *description,size_t id);
void addAgency(infractionSystemADT infractionSystem, char * agency, char * description);

typedef struct query1{
    char * infraction;               //nombre de la infraccion
    size_t totalInfracctions;        //cantidad de infracciones;
    struct query1 * tail;            //proxima infraccion
}Tquery1;

typedef struct query2{
    char * agency;                  //nombre de la agencia emisora
    char * infraction;              //infraccion mas popular
    size_t totalInfractions;        //cantidad de infracciones
    struct query2 * tail;
}Tquery2;

typedef struct query3{
    char * infraction;               //nombre infraccion
    int plate;                      //numero de patente
    size_t totalInfractions;          //cantidad de infracciones
    struct query3 * tail;
}Tquery3;

typedef struct query4{
    size_t year;                    
    char * monthTop1;               //mes con mayor cantidad de infracciones
    char * monthTop2;               //mes con segunda mayor cantidad de infracciones
    char * monthTop3;               //mes con tercera mayor cantidad de infracciones
    struct query4 * tail;
}Tquery4;

