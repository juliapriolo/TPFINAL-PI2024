#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "infractionsADT.h"

#define MONTHS 12
#define MAX_AG 30 //Despues cambiar el valor al que deberia ser

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
    size_t dimInfractions;      //cantidad de tipos de infracciones por agencia
    struct agency *tail;
}TAgency;

typedef struct agency *TListAgency;

typedef struct id{
    size_t id;
    TListInfractions pNode;
}TId;

typedef struct tickets{
    char *plate;
    size_t fineCount;
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
    size_t size; //espacio ocupado (Creo que no va a hacer falta)
}infractionSystemCDT;


infractionSystemADT newInfractionSystem(size_t minYear, size_t maxYear){
        infractionSystemADT newSystem = calloc(1,sizeof(infractionSystemCDT));
        
        errno = 0;
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

static void addAgencyInfraction(TQuery2 * infVec, size_t * dim, char * description){
    for(int i=0; i < *dim; i++){
        //el tipo de infraccion ya se encontraba en el vector
        if(strcasecmp(infVec[i].description, description) == 0){
            infVec->fineCount++;
            return;
        }
    }

    //nuevo tipo de infraccion
    *dim += 1;
    errno = 0;

    infVec = realloc(infVec, sizeof(TQuery2)*(*dim));

    if(infVec == NULL || errno == ENOMEM){
        return 0;
    }

    infVec[*dim-1].description = malloc(strlen(description)+1);
    //Podriamos directamente hacer que el puntero apunte a description en la lista de infracciones
    if(infVec[*dim-1].description == NULL || errno == ENOMEM){
        return 0;
    }

    strcpy(infVec[*dim-1].description, description);
    infVec[*dim-1].fineCount = 1;
}


static TListAgency addAgencyRec(TListAgency list, char * agName, char * description){
    int c;
    errno = 0;
    if(list == NULL || (c = strcasecmp(list->agencyName, agName) > 0)){
        //si la agencia no estaba tampoco habia ninguna infraccion
        TListAgency newAgency = malloc(sizeof(TAgency));
        if(newAgency == NULL || errno == ENOMEM){
            return 0;
        }
        newAgency->agencyName = malloc(strlen(agName)+1);
        
        if(newAgency->agencyName == NULL || errno == ENOMEM){
            return 0;
        }

        newAgency->agencyName = strncpy(newAgency->agencyName, description, MAX_AG);
        addAgencyInfraction(newAgency->infractions, &(newAgency->dimInfractions), description);
        newAgency->tail = list;
        return newAgency;
    }

    if(c==0){
        addAgencyInfraction(list->infractions, &(list->dimInfractions), description);
        return list;
    }

    list->tail = addAgencyRec(list->tail, agName, description);
    return list;

}

void addAgency(infractionSystemADT infractionSystem, char * agency, char * description){
    infractionSystem->firstAgency = addAgencyRec(infractionSystem->firstAgency, agency, description);
}


TListTickets addTicketRec(TListTickets list, char *plate, size_t *added){

}

//Si encuentra el id en el vector devuelve el puntero sino devuelve NULL. HACERLA ITERATIVA.

static TListInfractions binarySearch(TId arr, size_t id, size_t left, size_t right){

}

/*
    Buscar en el vector de ids si el id que le pasan es valido, lo busca con busqueda binaria.
    Si no es valido retorna 0.
    Si es valido, usa el puntero que se encuentra en el vector para acceder al first correspondiente. A partir de ahi,
    si la patente no se encuentra entonces la añade de forma alfabetica, sino, aumenta el contador de multas para dicha patente. En ambos casos retorna 1.
*/

int addTicket(TListInfractions infractions,size_t id,size_t fineCount ,char *plate){
    size_t added = 0;
}
