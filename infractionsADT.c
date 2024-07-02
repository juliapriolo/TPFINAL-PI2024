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


static void addAgencyInfraction(TQuery2 * infVec, size_t * dim, char * description){
    for(int i=0; i < *dim; i++){
        if(strcasecmp(infVec->description, description) == 0){
            infVec->fineCount++;
            return;
        }
    }
    *dim += 1;
    infVec = realloc(infVec, sizeof(TQuery2)*(*dim));
    infVec[*dim-1].description = strcpy(infVec[*dim-1].description, description);
    infVec[*dim-1].fineCount = 1;
    return;
}

static TListAgency addAgencyRec(TListAgency list, char * agName, size_t id, char * description, size_t totalInf){
    int c;
    if(list == NULL || (c = strcasecmp(list->agencyName, agName) > 0)){
        //si la agencia no estaba tampoco habia ninguna infraccion
        TListAgency newAgency = malloc(sizeof(TAgency));
        newAgency->agencyName = strncpy(newAgency->agencyName, description, MAX_AG);
        addAgencyInfraction(newAgency->infractions, &(newAgency->dimInfractions), description);
        newAgency->tail = list;
        return newAgency;
    }

    if(c==0){
        addAgencyInfraction(list->infractions, list->dimInfractions, description);
        return list;
    }

    list->tail = addAgencyRec(list->tail, agName, description, totalInf);
    return list;

}

void addAgency(infractionSystemADT infractionSystem, char * agency, char * description){
    infractionSystem->firstAgency = addAgencyRec(infractionSystem->firstAgency, agency, description);
}    size_t id;    
    TListInfractions pNode;
}TId;

typedef struct tickets{
    char *plate;
    //char *issuingAgency;
    char *issueDate; //Ver bien el tipo de dato
    size_t infractionId;
    size_t infractionAmount; //cantidad de infracciones para la patente
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



static void addAgencyInfraction(TQuery2 * infVec, size_t * dim, char * description){
    for(int i=0; i < *dim; i++){
        if(strcasecmp(infVec->description, description) == 0){
            infVec->fineCount++;
            return;
        }
    }
    *dim += 1;
    infVec = realloc(infVec, sizeof(TQuery2)*(*dim));
    infVec[*dim-1].description = strcpy(infVec[*dim-1].description, description);
    infVec[*dim-1].fineCount = 1;
    return;
}

static TListAgency addAgencyRec(TListAgency list, char * agName, size_t id, char * description, size_t totalInf){
    int c;
    if(list == NULL || (c = strcasecmp(list->agencyName, agName) > 0)){
        //si la agencia no estaba tampoco habia ninguna infraccion
        TListAgency newAgency = malloc(sizeof(TAgency));
        newAgency->agencyName = strncpy(newAgency->agencyName, description, MAX_AG);
        addAgencyInfraction(newAgency->infractions, &(newAgency->dimInfractions), description);
        newAgency->tail = list;
        return newAgency;
    }

    if(c==0){
        addAgencyInfraction(list->infractions, list->dimInfractions, description);
        return list;
    }

    list->tail = addAgencyRec(list->tail, agName, description, totalInf);
    return list;

}

void addAgency(infractionSystemADT infractionSystem, char * agency, char * description){
    infractionSystem->firstAgency = addAgencyRec(infractionSystem->firstAgency, agency, description);
}

//falta TODO lo que esta relacionado al issue date(no terminamos de definirlo todavia) y CHEQUEAR si hace falta el agency o no
static TListTickets addTicketRec(TListTickets list, char *patente, char *agency, int *flag, size_t infractionId){
    if(list==NULL || list->infractionAmount==0){
        TListTickets new=malloc(sizeof(TTickets));
        new->plate=malloc(strlen(plate+1));
        strcpy(new->plate, patente);
        //new->issuingAgency=malloc(strlen(agency+1));
        //strcpy(new->issuingAgency, agency);
        new->infractionId=infractionId;
        new->infractionAmount=1;
        *flag=1;
        return new;
    }
    if(strcasecmp(list->patente, patente)==0){
        TListTickets new=malloc(sizeof(TTickets));
        new->infractionId=infractionId;
        new->infractionAmount++;
        return new;
    }
    list->tail=addTicketRec(list->tail, patente, agency, flag, infractionId);
    return list;
}

int addTicket(TListInfractions first, char *patente, char *agency){
    int flag=0;
    
    first->firstTicket=addTicketRec(first->firstTicket, patente, agency, &flag, first->id);
}