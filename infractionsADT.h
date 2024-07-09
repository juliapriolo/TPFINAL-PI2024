typedef struct infractionSystemCDT * infractionSystemADT;

/*
    Crea un nuevo sistema de infracciones.
    Recibe el rango de años necesario para el query 4.
*/
infractionSystemADT newInfractionSystem(size_t minYear, size_t maxYear);

/*
    Añade las infracciones en orden alfabetico sin repetidos. 
    Guarda los valores de id y el tipo de infraccion.
    Inicializa en NULL la lista para las patentes que cometieron la infraccion.
    Retorna 1 si la agrego o 0 si no.
*/
int addInfraction(infractionSystemADT infractionSystem,char *description,size_t id);

/*
    Añade las agencias en orden alfabetico o agrega una infraccion si la agencia ya se encontraba en la lista.
    Almacena en un vector de estructuras el tipo de infraccion y la cantidad de multas de dicha infraccion.
*/
int addAgency(infractionSystemADT infractionSystem, char * agency, size_t id);

/*
    Buscar en el vector de ids si el id que le pasan es valido, si no es valido retorna 0.
    Si es valido y la patente no se encuentra entonces la añade de forma alfabetica, sino,
    aumenta el contador de multas para dicha patente. En ambos casos retorna 1.
*/
int addTicket(infractionSystemADT infractionSystem,char *plate, size_t id);

//Agrega la fecha si esta dentro del rango especificado
void addDate(infractionSystemADT system,int year,int month);

//funciones de iteracion para la lista de infracciones
void toBegin(infractionSystemADT infractionSystem);

int hasNext(infractionSystemADT infractionSystem);

void * next(infractionSystemADT infractionSystem);

//funciones de iteracion para las agencias
void toBeginByAgency(infractionSystemADT a);

int hasNextByAgency(infractionSystemADT a);

char *nextByAgency(infractionSystemADT a);

//Devuelve la dimension de la lista de agencias
size_t dimAgencyList(infractionSystemADT system);

//Devuelve la dimension del vector de años
size_t dimArrYears(infractionSystemADT system);

//funcion para liberar el sistema completo
void freeInfractionSystem(infractionSystemADT infractionSystem);

//Estructuras para los queries
typedef struct nodeQuery1{
    char * infraction; //nombre de la infraccion
    size_t totalInfracctions; //cantidad de infracciones;
    struct nodeQuery1 * tail; //proxima infraccion
}TNodeQ1;

typedef TNodeQ1 * TListQ1;

typedef struct TQuery1{
    TListQ1 first;
    TListQ1 iter;
}TQuery1;

typedef struct query2{
    char *agency;
    char *mostPopularInf;
    size_t fineCount;
}TQuery2;

typedef struct vecQuery3{
    char * infraction; //nombre infraccion
    char * plate; //numero de patente
    size_t fineAmount; //cantidad de multas
}vecQuery3;

typedef struct query3{
    vecQuery3 * vectorDeDatos;
    size_t dim; // dimension del vector, lo voy a reallocar con bloques
}TQuery3;

typedef struct nodeq4{
    size_t year;                    
    char * monthTop1; //mes con mayor cantidad de infracciones
    char * monthTop2; //mes con segunda mayor cantidad de infracciones
    char * monthTop3; //mes con tercera mayor cantidad de infracciones
}TNodeq4;

typedef struct Tquery4{
    TNodeq4 * vec;
    size_t dim;
}TQuery4;

//QUERY 1: Total de multas por infraccion en forma descendente por la cantidad total de multas
TQuery1 * query1(infractionSystemADT infractionSystem);

//QUERY 2: Infraccion mas popular de cada agencia emisora, con su cantidad respectiva de multas, en orden alfabetico por agencia emisora
TQuery2 *query2(infractionSystemADT infractionSystem);

//QUERY 3: La cantidad mayor de multas de una patente por infraccion en orden alfabetico
TQuery3 * query3(infractionSystemADT infractionSystem);

//QUERY 4: Top 3 meses con mas multas por anio, ordenado cronologicamente por anio
TQuery4 *query4(infractionSystemADT infractionSystem);

//funciones iteracion y free para los queries
void toBeginQ1(TQuery1 * query1);

int hasNextQ1(TQuery1 * query1);

void * nextQ1(TQuery1 * query1);

void freeQ1(TQuery1 * query1);

void freeQ2(TQuery2* query2, size_t dim);

void freeQ3(TQuery3* query3);

void freeQ4(TQuery4* query4);