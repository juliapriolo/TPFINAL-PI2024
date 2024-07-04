#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "infractionsADT.h"

#define MONTHS 13
#define MAX_AG 30 //Despues cambiar el valor al que deberia ser

typedef struct tickets{
    char *plate;
    size_t fineCount;
    struct tickets *tail;
}TTickets;

typedef TTickets *TListTickets;

typedef struct infractions{
    char *description;
    size_t id;
    size_t totalFines;// total de multas por infraccion
    struct infractions *tail;
    TListTickets firstTicket;
}TInfractions;

typedef TInfractions *TListInfractions;

typedef struct agencyInfraction{
    size_t id;
    size_t fineCount; // por agencia
}TAgencyInfraction;

typedef struct agency{
    char *agencyName;
    TAgencyInfraction *infractions;
    struct agency *tail;
}TAgency;

typedef TAgency *TListAgency;

typedef struct id{
    size_t id;
    TListInfractions pNode;
}TId;

typedef struct infractionSystemCDT{
    TListInfractions firstInfraction;
    TListInfractions iterInfractions;
    TListAgency firstAgency;
    TListAgency iterAgency;
    size_t **arrYears; //vector de años y meses (multas)
    size_t minYear; //años para la query 4
    size_t maxYear;
    TId *arrId; // vector con los id ordenados
    size_t dim; //dimension total
    size_t size; //espacio ocupado (Creo que no va a hacer falta)
}infractionSystemCDT;


infractionSystemADT newInfractionSystem(size_t minYear, size_t maxYear){
    if(minYear > maxYear){
        return NULL;
    }

    infractionSystemADT newSystem = calloc(1,sizeof(infractionSystemCDT));    
    if(newSystem == NULL){ 
        return NULL; //o exit(1)
    }
   
    size_t newDim = maxYear - minYear + 1;
    newSystem->arrYears = malloc(newDim * sizeof(size_t *));
    if(newSystem->arrYears == NULL){
        free(newSystem);
        return NULL; // o exit(1)
    }

    for(size_t i = 0; i < newDim; i++){
        newSystem->arrYears[i] = calloc(MONTHS,sizeof(size_t));
        if(newSystem->arrYears[i] == NULL){
            for(size_t j = 0; j < i; j++){
                free(newSystem->arrYears[j]);
            }
            free(newSystem->arrYears);
            free(newSystem);
            return NULL;
        }
    }         
        
    newSystem->minYear = minYear;
    newSystem->maxYear = maxYear;

    return newSystem;
}

//si a>b devuelve un numero mayor a 0, si a<b,, devuelve un numero 
//menor a 0, y si son iguales devuelve 0
static int cmpIds(const void *a, const void *b){
    const TId *idA = (const TId *)a;
    const TId *idB = (const TId *)b;
    return (idA->id - idB->id);
}


static TId *fillArr(TId *array, size_t dim, TListInfractions pInfraction){
    array  = realloc(array,sizeof(*array) * dim);
    errno=0;
    if(array==NULL || errno==ENOMEM){
        return NULL;
    }
    array[dim-1].pNode = pInfraction;
    array[dim-1].id = pInfraction->id;
    qsort(array, dim, sizeof(TId), cmpIds);
    return array;
}

static TListInfractions addInfractionRec(TListInfractions list, char *description, size_t id, size_t *added, TListInfractions *pInfractions){
    int c;
    if(list == NULL || (c = strcasecmp(list->description, description)) > 0){
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

int addInfraction(infractionSystemADT infractionSystem, char *description, size_t id){
    size_t added = 0; // flag para saber si agrego la infraccion
    TListInfractions pInfractions; // variable con los datos del nodo agregado
    infractionSystem->firstInfraction = addInfractionRec(infractionSystem->firstInfraction, description, id, &added, &pInfractions);

    if(added){
        infractionSystem->dim++;
        infractionSystem->arrId = fillArr(infractionSystem->arrId, infractionSystem->dim, pInfractions);
    }

    return added;
}

static void addAgencyInfraction(TAgencyInfraction ** infVec, size_t  dim, size_t id){
    int i;
    int flag = 1;
    for(i=0; i < dim && flag; i++){
        if((*infVec)[i].id == 0){
            (*infVec)[i].fineCount = 1;
            (*infVec)[i].id = id;
            flag = 0;
        }
        else if((*infVec)[i].id == id){
            ((*infVec)[i].fineCount)++;
            return;
        }
    }
    return;
}


static TListAgency addAgencyRec(TListAgency list, char * agName,size_t id,size_t dim,size_t *added){
    int c;
    errno = 0;
    if(list == NULL || (c = strcasecmp(list->agencyName, agName)) > 0){
        //si la agencia no estaba tampoco habia ninguna infraccion
        TListAgency newAgency = malloc(sizeof(TAgency));
        if(newAgency == NULL || errno == ENOMEM){
            return 0;
        }
        newAgency->agencyName = malloc(strlen(agName)+1);

        if(newAgency->agencyName == NULL || errno == ENOMEM){
            return 0;
        }
        strcpy(newAgency->agencyName, agName);
        newAgency->infractions = calloc(dim,sizeof(TAgencyInfraction));
        addAgencyInfraction(&(newAgency->infractions), dim, id);
        newAgency->tail = list;
        *added = 1;
        return newAgency;
    }

    if(c==0){
        *added = 2;
        addAgencyInfraction(&(list->infractions), dim, id);
        return list;
    }

    list->tail = addAgencyRec(list->tail, agName, id,dim,added);
    return list;

}

//0 si no agrego, 1 si agrego una nueva agencia, 2 si la agencia ya estaba
int addAgency(infractionSystemADT infractionSystem, char * agency, size_t id){
    size_t added = 0;
    infractionSystem->firstAgency = addAgencyRec(infractionSystem->firstAgency, agency, id,infractionSystem->dim,&added);
    return added;
}


static TListInfractions binarySearch(TId *arr, size_t id, size_t left, size_t right){
    while(left <= right){
        size_t mid = left + (right - left)/2;
        if(arr[mid].id == id){
            return arr[mid].pNode;
        }
        if(arr[mid].id < id){
            left = mid + 1;
        }
        else{
            right = mid - 1;
        }
    }
    return NULL;
}

/*
    Buscar en el vector de ids si el id que le pasan es valido, lo busca con busqueda binaria.
    Si no es valido retorna 0.
    Si es valido, usa el puntero que se encuentra en el vector para acceder al first correspondiente. A partir de ahi,
    si la patente no se encuentra entonces la añade de forma alfabetica, sino, aumenta el contador de multas para dicha patente. En ambos casos retorna 1.
*/

static TListTickets addTicketRec(TListTickets listTick, char *plate, size_t *added){
    int c;
    if(listTick == NULL || (c = strcasecmp(plate, listTick->plate)) < 0 ){
        errno = 0;
        TListTickets aux = calloc(1, sizeof(TTickets)); // para iniciar el count en 0
        if(aux == NULL || errno == ENOMEM){
            return NULL;
        }
        aux->plate = malloc(((strlen(plate)) + 1));
        if(aux->plate == NULL || errno == ENOMEM){
            free(aux);
            return NULL;
        }
        strcpy(aux->plate, plate);
        aux->fineCount++;
        aux->tail = listTick;
        (*added) = 1; // se agrego una nueva patente y una multa
        return aux;
    }
    if(c == 0){
        listTick->fineCount++; // como la patente ya estaba, incremento en 1 la cant de multas
        (*added) = 1; // no se agrego una nueva patente pero si una multa
        return listTick;
    }
    listTick->tail = addTicketRec(listTick->tail, plate, added);
    return listTick;
}

int addTicket(infractionSystemADT infractionSystem, size_t id,char *plate){
    size_t added = 0;
    if(infractionSystem->arrId == NULL){
        return added;
    }
    TListInfractions ticket = binarySearch(infractionSystem->arrId,id,0,infractionSystem->dim-1);
    if(ticket != NULL){
        ticket->firstTicket = addTicketRec(ticket->firstTicket,plate,&added);
        ticket->totalFines += added;
    }
    return added;
}

//funciones de iteracion para la lista de infracciones
void toBegin(infractionSystemADT infractionSystem){
    infractionSystem->iterInfractions = infractionSystem->firstInfraction;
}

int hasNext(infractionSystemADT infractionSystem){
    return infractionSystem->iterInfractions != NULL;
}

void * next(infractionSystemADT infractionSystem){
    if(!hasNext(infractionSystem)){
        exit(1);
    }
    TListInfractions ans = infractionSystem->iterInfractions;
    infractionSystem->iterInfractions = ans->tail;
    return ans;
}

//funciones de iteracion para las agencias

void toBeginByAgency(infractionSystemADT a){
    a->iterAgency=a->firstAgency;
}

int hasNextByAgency(infractionSystemADT a){
    return a->iterAgency!=NULL;
}

void *nextByAgency(infractionSystemADT a){
    if(!hasNextByAgency(a))
        exit(1);
    char *ans=a->iterAgency->agencyName;
    a->iterAgency=a->iterAgency->tail;
    return ans;
}