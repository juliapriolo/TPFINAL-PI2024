#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "infractionsADT.h"

#define MONTHS 12

typedef struct infractions{
    char *description;
    size_t id;
    size_t totalFines;// total de multas por infraccion
    struct infractions *tail;
    TListTickets firstTicket;
}TInfractions;

typedef struct infractions *TListInfractions;

typedef struct query2{
    char *description;
    size_t fineCount; // por agencia
}TQuery2;

typedef struct agency{
    char *agencyName;
    TQuery2 *infractions; 
    struct agency *tail;
}TAgency;

typedef struct agency *TListAgency;

typedef struct id{
    char exists;
    TListInfractions pNode;
}TId;

typedef struct tickets{
    char *plate;
    char *issuingAgency;
    char *issueDate; //Ver bien el tipo de dato
    size_t infractionId;
    struct tickets *tail;
}TTickets;

typedef struct tickets *TListTickets;

typedef struct infractionSystemCDT{
    TListInfractions firstInfraction;
    TListAgency firstAgency;
    size_t *arrYears[MONTHS]; //vector de años y meses (multas)
    size_t minYear; //años para la query 4
    size_t maxYear;
    TId arrId; // vector con los id ordenados
    size_t dim; //dimension total
    size_t size; //espacio ocupado
}infractionSystemCDT;


infractionSystemADT newInfractionSystem(size_t minYear, size_t maxYear){
        infractionSystemADT newSystem = calloc(1,sizeof(infractionSystemADT));
        errno = 0;
        //podriamos hacer una macro !!!  
        if(newSystem == NULL || errno == ENOMEM){
            return NULL;
        }           
        newSystem->minYear = minYear;
        newSystem->maxYear = maxYear;
        return newSystem;
}

