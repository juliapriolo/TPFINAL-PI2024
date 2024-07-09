#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "infractionsADT.h"
#include <strings.h>

#define INVALID_YEAR -1
#define MONTHS 12
#define MAX_AG 30 //Despues cambiar el valor al que deberia ser
#define EMPTY "EMPTY"
#define START 0
#define CURRENT_YEAR 2024
#define CHECK_MEMORY(ptr) if((ptr) == NULL || errno == ENOMEM) { return NULL;}
#define BLOCK 100
#define PLATE_ARR_SIZE 126

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
    TListTickets arrPlates[PLATE_ARR_SIZE];
    size_t exists;
    struct infractions *tail;
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
    size_t dim; //dimension total del arrId
}infractionSystemCDT;


infractionSystemADT newInfractionSystem(size_t minYear, size_t maxYear){
    if(minYear > maxYear){
        return NULL;
    }

    infractionSystemADT newSystem = calloc(1,sizeof(infractionSystemCDT));    
    CHECK_MEMORY(newSystem);
   
    size_t newDim = maxYear - minYear + 1;
    newSystem->arrYears = malloc(newDim * sizeof(size_t *));
    CHECK_MEMORY(newSystem->arrYears);

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
static int cmpIds(const void *a, const void *b){ //CAMBIAR
    const TId *idA = (const TId *)a;
    const TId *idB = (const TId *)b;
    return (idA->id - idB->id);
}


static TId *fillArr(TId *array, size_t dim, TListInfractions *pInfraction){
    array  = realloc(array,sizeof(*array) * dim);
    errno=0;
    if(array==NULL || errno==ENOMEM){
        return NULL;
    }
    array[dim-1].pNode = *pInfraction;
    array[dim-1].id = (*pInfraction)->id;
    qsort(array, dim, sizeof(TId), cmpIds);
    return array;
}

static TListInfractions addInfractionRec(TListInfractions list, char *description, size_t id, size_t *added, TListInfractions *pInfractions){
    int c;
    if(list == NULL || (c = strcasecmp(list->description, description)) > 0){
        TListInfractions newNode = malloc(sizeof(TInfractions));
        CHECK_MEMORY(newNode);
        newNode->description = malloc(strlen(description) + 1);
        CHECK_MEMORY(newNode->description);

        strcpy(newNode->description,description); //chequear esto
        for (int i = 0; i < PLATE_ARR_SIZE; i++) {
            newNode->arrPlates[i] = NULL;
        }
        newNode->exists = 1;
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
        infractionSystem->arrId = fillArr(infractionSystem->arrId, infractionSystem->dim, &pInfractions);
    }

    return added;
}

static void addAgencyInfraction(TAgencyInfraction ** infVec, size_t  dim, size_t id){
    int i;
    int flag = 1;
    for(i = 0; i < dim && flag; i++){
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


static TListAgency addAgencyIter(TListAgency list, char *agName, size_t id, size_t dim, size_t *added) {
    TListAgency current = list;
    TListAgency previous;
    int c;

    while (current != NULL && (c = strcasecmp(current->agencyName, agName)) < 0) {
        previous = current;
        current = current->tail;
    }

    if (current != NULL && c == 0) {
        *added = 2;
        addAgencyInfraction(&(current->infractions), dim, id);
        return list;
    }

    errno = 0;
    TListAgency newAgency = malloc(sizeof(TAgency));
    if (newAgency == NULL || errno == ENOMEM) {
        return NULL;
    }
    newAgency->agencyName = malloc(strlen(agName) + 1);
    if (newAgency->agencyName == NULL || errno == ENOMEM) {
        free(newAgency);
        return NULL;
    }
    strcpy(newAgency->agencyName, agName);
    newAgency->infractions = calloc(dim, sizeof(TAgencyInfraction));
    if (newAgency->infractions == NULL || errno == ENOMEM) {
        free(newAgency->agencyName);
        free(newAgency);
        return NULL;
    }
    addAgencyInfraction(&(newAgency->infractions), dim, id);
    newAgency->tail = current;

    if (current == list) {
        list = newAgency;
    } else {
        previous->tail = newAgency;
    }

    *added = 1;
    return list;
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


//0 si no agrego, 1 si agrego una nueva agencia, 2 si la agencia ya estaba
int addAgency(infractionSystemADT infractionSystem, char * agency, size_t id){
    size_t added = 0;
    TListInfractions ticket = binarySearch(infractionSystem->arrId,id,0,infractionSystem->dim-1);
    if(ticket != NULL){
        infractionSystem->firstAgency = addAgencyIter(infractionSystem->firstAgency, agency, id,infractionSystem->dim,&added);
        if(added == 1){
            infractionSystem->dimAgency++;
        }
    }
    return added;
}


/*
    Buscar en el vector de ids si el id que le pasan es valido, lo busca con busqueda binaria.
    Si no es valido retorna 0.
    Si es valido, usa el puntero que se encuentra en el vector para acceder al first correspondiente. A partir de ahi,
    si la patente no se encuentra entonces la añade de forma alfabetica, sino, aumenta el contador de multas para dicha patente. En ambos casos retorna 1.
*/

static TListTickets addTicketIter(TListTickets listTick, char *plate,size_t *added){
    TListTickets current = listTick;
    TListTickets previous;
    int c;

    while(current != NULL && (c = strcasecmp(plate, current->plate)) > 0){
        previous = current;
        current = current->tail;
    }

    if(current != NULL && c == 0){
        current->fineCount++;
        *added = 2;
        return listTick;
    }

    errno = 0;
    TListTickets newTicket = calloc(1, sizeof(TTickets));
    if(newTicket == NULL || errno == ENOMEM){
        return NULL;
    }
    newTicket->plate = malloc((strlen(plate) + 1));
    if(newTicket->plate == NULL || errno == ENOMEM){
        free(newTicket);
        return NULL;
    }
    strcpy(newTicket->plate, plate);
    newTicket->fineCount = 1;
    newTicket->tail = current;

    if(current == listTick){
        listTick = newTicket;
    }else {
        previous->tail = newTicket;
    }
    *added = 1;
    return listTick;
}

int addTicket(infractionSystemADT infractionSystem, size_t id,char *plate){
    size_t added = 0;
    int letter = plate[0]-1;
    TListInfractions ticket = binarySearch(infractionSystem->arrId,id,0,infractionSystem->dim-1);
    if(ticket != NULL){
        ticket->arrPlates[letter] = addTicketIter(ticket->arrPlates[letter],plate,&added);
        ticket->totalFines++;
    }
    return added;
}


int addDate(infractionSystemADT system,int year,int month){
    if(year < system->minYear || year > system->maxYear || month < Enero || month > Diciembre){ //no se si puedo hacer esto xq month no es tipo enum Meses
        return 0;
    }
    system->arrYears[year-system->minYear][month-1]++;

    return 1;
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
        return NULL;    //chequear
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
        return NULL;
    char *ans=a->iterAgency->agencyName;
    a->iterAgency=a->iterAgency->tail;
    return ans;
}

//Funciones free

static void freeArrYears(size_t **arrYears, size_t dim){
    for(int i = 0; i < dim; i++){
        if(arrYears[i] != NULL){
            free(arrYears[i]);
        }
    }
    free(arrYears);
    return;
}

static void freeTicketList(TListTickets list){
    TListTickets current = list;
    TListTickets next;
    while(current != NULL){
        next = current->tail;
        if(current->plate != NULL){
            free(current->plate);
        }
        free(current);
        current = next;
    }
    return;
}

static void freeInfractionList(TListInfractions list){
    TListInfractions current = list;
    TListInfractions next;

    while(current != NULL){
        next = current->tail;
        if(current->description != NULL){
            free(current->description);
        }
        for(int i = 0; i < PLATE_ARR_SIZE; i++){
            if(current->arrPlates[i] != NULL){
                freeTicketList(current->arrPlates[i]);
            }
        }
        free(current);
        current = next;
    }
    return;
}

void freeAgencyList(TListAgency list){
    TListAgency current = list;
    TListAgency next;
    while(current != NULL){
        next = current->tail;
        if(current->agencyName != NULL){
            free(current->agencyName);
        }
        if(current->infractions != NULL){
            free(current->infractions);
        }
        free(current);
        current = next;
    }
    return;    
}

void freeInfractionSystem(infractionSystemADT infractionSystem){
    free(infractionSystem->arrId);
    freeArrYears(infractionSystem->arrYears, dimArr(infractionSystem));
    freeInfractionList(infractionSystem->firstInfraction);
    freeAgencyList(infractionSystem->firstAgency);
    free(infractionSystem);
    return;
}

//Funcion para copiar
static char *copyStr(char *s) {
    errno = 0;
    char *ans = malloc(strlen(s) + 1);
    CHECK_MEMORY(ans);

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
        CHECK_MEMORY(newNode);

        newNode->infraction = malloc(strlen(infractionName)+1);
        CHECK_MEMORY(newNode->infraction);

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
        return NULL;
    }
    return infractionSystem->iterInfractions->description;
}


TQuery1 * query1(infractionSystemADT infractionSystem){
    size_t total;
    errno = 0;

    TQuery1 * ans = calloc(1, sizeof(TQuery1));
    CHECK_MEMORY(ans);

    toBegin(infractionSystem);          //inicializa el iterador de lista de infracciones

    while(hasNext(infractionSystem)){
        total = infractionSystem->iterInfractions->totalFines;
        char *infName = getInfName(infractionSystem);
        ans->first = addRecQ1(ans->first, infName, total);
        next(infractionSystem);
    }
    return ans;
}

//QUERY 2 Infraccion mas popular por agencia emisora.

void freeQ2(TQuery2 *query2, size_t dim){
    for(int i=0;i<dim;i++){
        if(query2->agency != NULL){
            free(query2[i].agency);
        }
        if(query2->mostPopularInf != NULL){
            free(query2[i].mostPopularInf);
        }
    }
    free(query2);
}

static void searchMostPopular(TAgencyInfraction *infractions, size_t dim, TId *arr, char **mostPopular, int *fineCount){
    int i=0;
    int maxId=0;
    int maxCount=0;
    TListInfractions maxIdNode = NULL;

    while(i < dim && infractions[i].id != 0){
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

    *fineCount = maxCount;
    *mostPopular = malloc(strlen(maxIdNode->description) + 1);
    strcpy(*mostPopular,maxIdNode->description);
    return;
}

//QUERY 2

TQuery2 *query2(infractionSystemADT system){
    TQuery2 *newQ2 = calloc(system->dimAgency, sizeof(*newQ2));
    TListAgency aux = system->firstAgency;
    int i=0;
    while(aux != NULL){
        if(aux->infractions != NULL){
            char *mostPopular = NULL;
            int fineCount = 0;
            searchMostPopular(aux->infractions, system->dim, system->arrId, &mostPopular, &fineCount);
            newQ2[i].fineCount = fineCount;
            newQ2[i].mostPopularInf = mostPopular;
            newQ2[i].agency = malloc(strlen(aux->agencyName) + 1);
            strcpy( newQ2[i].agency,aux->agencyName);     
        }
        aux = aux->tail;
        i++;
    }
    return newQ2;
}

void freeQ3(TQuery3 * query3){
    for(size_t i = 0; i < query3->dim; i++){
        free(query3->vectorDeDatos[i].infraction);
        free(query3->vectorDeDatos[i].plate);
    }
    free(query3->vectorDeDatos);
    free(query3);
}

//QUERY 3: Infraccion con la patente con mayor cantidad de multas
//funciones iteracion y free Query2 (las vamos a necesitar para el main)
static void addQuery3(TListTickets ticketList[PLATE_ARR_SIZE], size_t * maxFineAmount, char * *maxPlate){
    for(size_t i = 0; i < PLATE_ARR_SIZE; i++){
        TListTickets currentTicket = ticketList[i];
        while(currentTicket != NULL){
            if((*maxFineAmount) < currentTicket->fineCount){
                (*maxFineAmount) = currentTicket->fineCount;
                (*maxPlate) = currentTicket->plate;
            } else if(((*maxFineAmount) == currentTicket->fineCount) && (strcasecmp(currentTicket->plate, *maxPlate) < 0)){
                (*maxPlate) = currentTicket->plate;
            }
            currentTicket = currentTicket->tail;
        }
    }
}

TQuery3 * query3(infractionSystemADT system){
    errno = 0;
    TQuery3 * ans = calloc(1, sizeof(TQuery3));
    CHECK_MEMORY(ans);
    
    size_t k = 0;
    size_t qValidInfrAmmount = 0; //cada vez que una infraccion tenga patentes, es valida, lo incremento

    toBegin(system);
    while(hasNext(system)){ // recorro las infracciones
        if(system->iterInfractions->exists){
            qValidInfrAmmount++;

            size_t qFineAmount = 0;
            char *qPlate = NULL;

            addQuery3(system->iterInfractions->arrPlates, &qFineAmount, &qPlate);

            if(ans->vectorDeDatos == NULL || qValidInfrAmmount > ans->dim){
                size_t newDim = (BLOCK + ans->dim);

                ans->vectorDeDatos = realloc(ans->vectorDeDatos, newDim * sizeof(vecQuery3));
                CHECK_MEMORY(ans->vectorDeDatos);
                
                for(size_t j = ans->dim; j < newDim; j++){
                    ans->vectorDeDatos[j].fineAmount = 0;
                    ans->vectorDeDatos[j].infraction = NULL;
                    ans->vectorDeDatos[j].plate = NULL;
                }
                ans->dim = newDim;
            }

            ans->vectorDeDatos[k].infraction = malloc(strlen(system->iterInfractions->description) + 1);
            CHECK_MEMORY(ans->vectorDeDatos[k].infraction);

            ans->vectorDeDatos[k].plate = malloc((strlen(qPlate) + 1));
            CHECK_MEMORY(ans->vectorDeDatos[k].plate);

            ans->vectorDeDatos[k].fineAmount = qFineAmount;
            strcpy(ans->vectorDeDatos[k].plate, qPlate);
            strcpy(ans->vectorDeDatos[k].infraction, system->iterInfractions->description);
            k++;
           
        }
        next(system);
    }
    ans->vectorDeDatos = realloc(ans->vectorDeDatos, qValidInfrAmmount * sizeof(vecQuery3));
    CHECK_MEMORY(ans->vectorDeDatos);

    ans->dim = qValidInfrAmmount;
    return ans;
}

// Función para el query4
TQuery4 *query4(infractionSystemCDT *infractionSystem) {
    if (infractionSystem == NULL || infractionSystem->arrYears == NULL) {
        return NULL;
    }
    size_t dim;
    if(infractionSystem->maxYear == INVALID_YEAR){  //depende de si se recibio un maxYear o no
        dim = CURRENT_YEAR - infractionSystem->minYear + 1;
    }else{
        dim = infractionSystem->maxYear - infractionSystem->minYear + 1;
    }
    TQuery4 *ans = calloc(1, sizeof(TQuery4));
    CHECK_MEMORY(ans);

    ans->vec = calloc(dim,sizeof(*(ans->vec)));
    CHECK_MEMORY(ans->vec);

    ans->dim = dim;

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

        //verifico que no haya errores
        if (ans->vec[i].monthTop1 == NULL || ans->vec[i].monthTop2 == NULL || ans->vec[i].monthTop3 == NULL) {
            for (size_t j = 0; j <= i; j++) {
                free(ans->vec[j].monthTop1);
                free(ans->vec[j].monthTop2);
                free(ans->vec[j].monthTop3);
            }
            free(ans->vec);
            free(ans);
            return NULL;
        }
    }
    return ans;
}


void freeQ4(TQuery4* query4){
    if(query4 == NULL){
        return;
    }
    for(size_t i = 0; i < query4->dim; i++){
        free(query4->vec[i].monthTop1);
        free(query4->vec[i].monthTop2);
        free(query4->vec[i].monthTop3);
    }
    free(query4->vec);
    free(query4);
}


size_t dimAgency(infractionSystemADT system){
    return system->dimAgency;
}

size_t dimArr(infractionSystemADT system){
    return system->maxYear - system->minYear + 1;
}