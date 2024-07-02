#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

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
    char * monthTop1;               
    char * monthTop2;
    char * monthTop3;
    struct query4 * tail;
}Tquery4;




