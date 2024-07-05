#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "infractionsADT.h"

#define MONTHS 12
#define MAX_AG 30 //Despues cambiar el valor al que deberia ser
#define EMPTY "EMPTY"

enum Meses {Enero = 0,Febrero,Marzo,Abril,Mayo,Junio,Julio,Agosto,Septiembre,Octubre,Noviembre,Diciembre};
static char *monthNames[] = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};

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
    size_t dim;
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
    size_t dimAgency;
    size_t **arrYears; //vector de años y meses (multas)
    size_t minYear; //años para la query 4
    size_t maxYear;
    TId *arrId; // vector con los id ordenados
    size_t dim; //dimension total
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

char *nextByAgency(infractionSystemADT a){
    if(!hasNextByAgency(a))
        exit(1);
    char *ans=a->iterAgency->agencyName;
    a->iterAgency=a->iterAgency->tail;
    return ans;
}

//Funciones free

static void freeTicketList(TListTickets list){
    if(list == NULL){
        return;
    }
    freeTicketList(list->tail);
    free(list->plate);
    free(list);
}


static void freeLInfractionsRec(TListInfractions list){
    if(list == NULL){
        return;
    }
    freeLInfractionsRec(list->tail);
    freeTicketList(list->firstTicket);
    free(list->description);
    free(list);
}

static void freeID(TId * arreglo, size_t dim){
    for(int i = 0; i < dim; i++){
        free(arreglo[i].pNode);            //nose si hay que hacer arreglo[i].id o solo arreglo[i] !!!!
    }
    free(arreglo);
}

static void freeArrYears(size_t **arrYears, size_t minYear, size_t maxYear) {   
    for (size_t i = minYear; i < maxYear; i++) {
        free(arrYears[i-minYear]);
    }
    
    free(arrYears);
}

void freeInfractionSystem(infractionSystemADT infractionSystem){
    freeLInfractionsRec(infractionSystem->firstInfraction);
    freeLAgencyRec(infractionSystem->firstAgency);
    freeID(infractionSystem->arrId, infractionSystem->dim);
    freeArrYears(infractionSystem->arrYears, infractionSystem->minYear, infractionSystem->maxYear);//liberar arrYears
    free(infractionSystem);
}

//Funcion para copiar
static char *copyStr(char *s) {
    errno = 0;
    char *ans = malloc(strlen(s) + 1);
    if (ans == NULL || errno == ENOMEM) {
        return NULL;
    }
    strcpy(ans, s);
    return ans;
}


//QUERY 1 Total de multas por infraccion.
//funciones iteracion y free Query1 (las vamos a necesitar para el main)
void toBeginQ1(TQuery1 * query1){
    query1->iter = query1->first;
}

int hasNextQ1(TQuery1 * query1){
    return query1->iter != NULL;
}

void * nextQ1(TQuery1 * query1){
    if(!hasNextQ1(query1)){
        return NULL;
    }
    TListQ1 ans = query1->iter;
    query1->iter = query1->iter->tail;
    return ans;
}

static void freeQ1Rec(TListQ1 listQ1){
    if(listQ1 == NULL){
        return;
    }
    freeQ1Rec(listQ1->tail);
    free(listQ1->infraction);
    free(listQ1);
}


void freeQ1(TQuery1 * query1){
    freeQ1Rec(query1->first);
    free(query1);
}

static TListQ1 addRecQ1(TListQ1 list, char * infractionName, size_t total){
    int c;
    if(list == NULL || (c = list->totalInfracctions - total) < 0 || (c == 0 && strcmp(list->infraction, infractionName) > 0)){
        TListQ1 newNode = malloc(sizeof(TNodeQ1));
        
        if(newNode == NULL || errno == ENOMEM){                 //Chequeo de memoria
            return NULL;
        }
        newNode->infraction = malloc(strlen(infractionName)+1);
        if(newNode->infraction == NULL){
            free(newNode);
            return NULL;
        }
        strcpy(newNode->infraction, infractionName);
        newNode->totalInfracctions = total;
        newNode->tail = list;
        return newNode;
    }
    list->tail = addRecQ1(list->tail, infractionName, total);
    return list;
}

static char * getInfName(infractionSystemADT infractionSystem){
    if(!hasNext(infractionSystem)){
        return 0;
    }
    return infractionSystem->iterInfractions->description;
}


TQuery1 * query1(infractionSystemADT infractionSystem){
    size_t total;
    errno = 0;

    TQuery1 * ans = calloc(1, sizeof(TQuery1));
    if(ans == NULL || errno == ENOMEM){
        return NULL;
    }

    toBegin(infractionSystem);          //inicializa el iterador de lista de infracciones

    while(hasNext(infractionSystem)){
        total = infractionSystem->iterInfractions->totalFines;
        char *infName = getInfName(infractionSystem);
        ans->first = addRecQ1(ans->first, infName, total);
        next(infractionSystem);
    }
    return ans;
}


static TQuery2 *searchMostPopular(TAgencyInfraction *infractions,size_t dim,TId *arr){
    int i=0;
    int maxId=0;
    int maxCount=0;
    TListInfractions maxIdNode = NULL;
    TQuery2 *ans = malloc(sizeof(*ans));

    while(i < dim){
        if(maxCount < infractions[i].fineCount){
            maxCount = infractions[i].fineCount;
            maxId = infractions[i].id;
            maxIdNode = binarySearch(arr, infractions[i].id, 0, dim-1);
        } else if(maxCount == infractions[i].fineCount){
            maxIdNode = binarySearch(arr, maxId, 0, dim-1);
            TListInfractions currentIdNode = binarySearch(arr, infractions[i].id, 0, dim-1);
            if(strcasecmp(maxIdNode->description, currentIdNode->description) > 0){ // Me quedo con el orden alfabetico
                maxId = infractions[i].id;
                maxCount = infractions[i].fineCount;
                maxIdNode = currentIdNode;
            }
        }
        i++;
    }

    ans->fineCount = maxCount;
    ans->mostPopularInf = malloc(strlen(maxIdNode->description) + 1);
    strcpy(ans->mostPopularInf, maxIdNode->description);
    return ans;
}

//QUERY 2

TQuery2 *query2(infractionSystemADT system){ 
    TQuery2 *newQ2 = calloc(system->dimAgency, sizeof(TQuery2));
    int i = 0;

    toBeginByAgency(system);
    while (hasNextByAgency(system)) {
        if (system->iterAgency->infractions != NULL) {
            newQ2[i] = *searchMostPopular(system->iterAgency->infractions, system->dim, system->arrId);
            newQ2[i].agency = system->iterAgency->agencyName;
        }
        nextByAgency(system);
        i++;
    }

    return newQ2;
}

//QUERY 3: Infraccion con la patente con mayor cantidad de multas


static TListQuery3 addQuery3(TQuery3 * myQuery, size_t queryFineAmount, char ** queryPlate, char * queryInfraction, size_t queryTotalInfractions){
    errno = 0;

    TListQuery3 aux = malloc(sizeof(ListQuery3)); // quiero agregar un nodo nuevo
    if(aux == NULL || errno == ENOMEM){
        return NULL;
    }

    aux->infraction = malloc((strlen(queryInfraction) + 1) * sizeof(char));
    if(aux->infraction == NULL || errno == ENOMEM){
        free(aux);
        return NULL;
    }
    strcpy(aux->infraction, queryInfraction);

    aux->plate = malloc((strlen(*queryPlate) + 1) * sizeof(char));
    if(aux->plate == NULL || errno == ENOMEM){
        free(aux->infraction);
        free(aux);
        return NULL;
    }
    strcpy(aux->plate, *queryPlate);

    aux->totalInfractions = queryTotalInfractions;
    aux->tail = NULL;

    if(myQuery->first == NULL){
        myQuery->first = aux;
    } else{
        myQuery->last->tail = aux;
    }
    myQuery->last = aux;
    return myQuery->first;
}

TQuery3 * query3(infractionSystemADT infractionSystem){
    errno = 0;

    TQuery3 * myQuery = calloc(1, sizeof(TQuery3));
    if(errno == ENOMEM || myQuery == NULL){
        return NULL;
    }

    size_t queryFinesAmount = 0; // inicializo una var temporal que la voy cambiando a medida que encuentre la patente con mas multas
    
    char ** queryPlate = malloc((strlen(infractionSystem->firstInfraction->firstTicket->plate) + 1) * sizeof(char *));
    if(errno == ENOMEM || queryPlate == NULL){
        return NULL;
    }

    char * queryInfraction = NULL;
    size_t queryTotalInfractions = 0;

    toBegin(infractionSystem); // inicializo el iterador en el primer nodo de la lista de infracciones
    
    while(hasNext(infractionSystem)){
        int flag = 0; //para ver si tiene patentes, si no tiene voy a tener que frear queryInfraction;
        queryInfraction = realloc(queryInfraction, ((strlen(infractionSystem->iterInfractions->description)+1) * sizeof(char))); //sea cual sea la cantidad, va a ser de esta infraccion
        if(queryInfraction == NULL || errno == ENOMEM){
            free(myQuery);
            return NULL;
        }

        TListTickets currentTicket = infractionSystem->iterInfractions->firstTicket;
        while(currentTicket != NULL){
            flag = 1; //como entro al while, tiene al menos una patente
            if(currentTicket->fineCount > queryFinesAmount){
                queryFinesAmount = currentTicket->fineCount;
                *queryPlate = currentTicket->plate; // me guardo en el puntero la patente asi no lo copio siempre, sino que recien lo copio cuando encontre el max de multas
            }else if(currentTicket->fineCount == queryFinesAmount){
                if(strcasecmp(*queryPlate, currentTicket->plate) > 0){
                    *queryPlate = currentTicket->plate; //si plate esta antes alfabeticamente, lo quiero
                }
            }
            currentTicket = currentTicket->tail;
        }
        if(flag == 1){
            strcpy(queryInfraction, infractionSystem->iterInfractions->description);
            queryTotalInfractions++; // como agregue los datos de una infraccion, ya que tenia al menos una patente, sumo a la cantidad de infracciones
            myQuery->first = addQuery3(myQuery, queryFinesAmount, queryPlate, queryInfraction, queryTotalInfractions);
        }
        next(infractionSystem);
    }
    free(queryPlate);
    free(queryInfraction);
    return myQuery;
}


//QUERY 4: top 3 meses por año

// Función para el query4
TQuery4 *query4(infractionSystemCDT *infractionSystem) {
    if (infractionSystem == NULL || infractionSystem->arrYears == NULL) {
        return NULL;
    }

    size_t dim = infractionSystem->maxYear - infractionSystem->minYear + 1;
    TQuery4 *ans = calloc(dim, sizeof(TQuery4));
    if (ans == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < dim; i++) {
        char *m1 = NULL, *m2 = NULL, *m3 = NULL;
        int n1 = 0, n2 = 0, n3 = 0;

        for (size_t j = 0; j < MONTHS; j++) {
            int current = infractionSystem->arrYears[i][j];
            if (current > n1) {
                n3 = n2; m3 = m2;
                n2 = n1; m2 = m1;
                n1 = current;
                m1 = monthNames[j];
            } else if (current > n2) {
                n3 = n2; m3 = m2;
                n2 = current;
                m2 = monthNames[j];
            } else if (current > n3) {
                n3 = current;
                m3 = monthNames[j];
            }
        }

        ans->vec[i].year = infractionSystem->minYear + i;
        ans->vec[i].monthTop1 = (m1 != NULL) ? copyStr(m1) : copyStr(EMPTY);
        ans->vec[i].monthTop2 = (m2 != NULL) ? copyStr(m2) : copyStr(EMPTY);
        ans->vec[i].monthTop3 = (m3 != NULL) ? copyStr(m3) : copyStr(EMPTY);
    }

    return ans;
}