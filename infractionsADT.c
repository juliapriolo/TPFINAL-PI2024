#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    size_t id;
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
    TId *arrId; // vector con los id ordenados
    size_t dim; //dimension total
    size_t size; //espacio ocupado
}infractionSystemCDT;


infractionSystemADT newInfractionSystem(size_t minYear, size_t maxYear){
        infractionSystemADT newSystem = calloc(1,sizeof(infractionSystemCDT));
        errno = 0;
        //podriamos hacer una macro !!!  
        if(newSystem == NULL || errno == ENOMEM){
            return NULL;
        }           
        newSystem->minYear = minYear;
        newSystem->maxYear = maxYear;
        return newSystem;
}

static int cmpIds(const void *a, const void *b){
    const TId *idA = (const TId *)a;
    const TId *idB = (const TId *)b;
    return (idA->id - idB->id);
}


static TId *fillArr(TId *array, size_t dim, TListInfractions pInfraction){
    array  = realloc(array,sizeof(*array) * dim);
    //chequear realloc
    array[dim-1].pNode = pInfraction;
    array[dim-1].id = pInfraction->id;
    qsort(array,dim,sizeof(TId),cmpIds);
    return array;
}

static TListInfractions addInfractionRec(TListInfractions list,char *description,size_t id,size_t *added,TListInfractions *pInfractions){
    int c;
    if(list == NULL || (c = strcasecmp(list->description,description)) > 0){
        TListInfractions newNode = malloc(sizeof(TInfractions));
        newNode->description = malloc(strlen(description) + 1);
        //Verificar que los malloc hayan funcionado
        strcpy(newNode->description,description); //chequear esto
        newNode->firstTicket = NULL;
        newNode->tail = list;
        newNode->totalFines = 0;
        newNode->id = id;
        *added = 1;
        *pInfractions = newNode;
        return newNode;
    }
    else if(c < 0){
        list->tail = addInfractionRec(list->tail,description,id,added,pInfractions);
    }
    return list;
}

int addInfraction(infractionSystemADT infractionSystem,char *description,size_t id){
    size_t added = 0; // flag para saber si agrego la infraccion
    TListInfractions pInfractions; // variable con los datos del nodo agregado
    infractionSystem->firstInfraction = addInfractionRec(infractionSystem->firstInfraction,description,id,&added,&pInfractions);

    if(added){
        infractionSystem->dim++;
        infractionSystem->arrId = fillArr(infractionSystem->arrId,infractionSystem->dim,pInfractions);
    }

    return added;
}
