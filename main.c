#include <stdio.h>
#include <stdlib.h>
#include "infractionsADT.h"
#include <errno.h>
#include "htmlTable.h"

#define HEADER1 "infraction;tickets\n"
#define HEADER2 "issuingAgency;infraction;tickets\n"
#define HEADER3 "infraction;plate;tickets\n"
#define HEADER4 "year;ticketsTop1Month;ticketsTop2Month;ticketsTop3Month\n"
#define DELIMITER ";"
#define CANT_QUERY 4
#define FILES 2
#define MAX_LINE 100
#define FIRST 0

//si esto confunde saquemoslo
#define HTMLH1 "infraction"
#define HTMLH2  "tickets"
#define HTMLH3  "issuing agency"
#define HTMLH4  "plate"

#define INVALID_YEAR -1
#define ERROR_PAR 1
#define ERROR_FILE 2
#define OK 0

#define PROGRAM 0
#define NY 1
#define CHI 2
#define MIN_YEAR 3
#define MAX_YEAR 4
#define MAX_ARG 5
#define MIN_ARG 4

//enum arguments{PROGRAM=0, NY, CHI, MIN_YEAR, MAX_YEAR};   preguntarle al resto del grupo que les parece mejor

#ifdef NY
#define MAX_LEN_AGENCY 35 //largo maximo para una agencia en Nueva York
#define MAX_LEN_DESCR 30 //largo maximo para el nombre de una infracción en Nueva York
#else
#define MAX_LEN_AGENCY 13 //largo maximo para una agencia en Chicago
#define MAX_LEN_DESCR 50 //largo maximo para el nombre de una infraccion en Chicago
#endif

int readInfraction(FILE *file, int idColumn, int infractionColumn, infractionSystemADT infractionSystem);
int readTickets(FILE *file, int plateColumn, int dateColumn, int idColumn, int agencyColumn, infractionSystemADT system);
FILE *newCSV(const char *fileName, char *header);   //funcion que crea un nuevo archivo csv y verifica que se haya creado bien
void closeCSV(FILE *files[], int fileQuantity); //funcion que cierra los archivos csv
void closeHTML(htmlTable files[], int fileQuantity);    //funcion que cierra los archivos html que estan en el arreglo files
int valid( const char* s);  //se asegura que el string sea todo de numeros


int main(int argc, char *argv[]){

    if(argc > MAX_ARG || argc<MIN_ARG){
        fprintf(stderr, "Incorrect amount of arguments supplied\n");
        exit(ERROR_PAR);
    }
    int minYear, maxYear = INVALID_YEAR; //no estamos seguras si hay que inicializar el minYear o no

    if ( argc >= MIN_ARG ){ //Vemos si minYear es un tipo de dato valido
        if( !valid( argv[MIN_YEAR])){
            fprintf( stderr, "Incorrect type for the minimum year\n");
            exit(ERROR_PAR); //vamos a hacer los enums para los distintos tipos de errores??
            }
        minYear = atoi(argv[MIN_YEAR]);
    }
    if ( argc == 5 ){    //Vemos si maxYear es un tipo de dato valido
        if(!valid( argv[MAX_YEAR]) ){
            fprintf( stderr, "Incorrect type for the maximum year\n");
            exit( ERROR_PAR);
            }
        maxYear = atoi(argv[MAX_YEAR]);

        if ( minYear > maxYear ){   //Vemos que el año minimo no sea mayor que el año maximo
            fprintf(stderr, "Invalid: minYear can not be greater than maxYear\n");
            exit(ERROR_PAR);
        }
    }
    return 0;

    //Inicializacion de archivos de lectura
    FILE * infractions = fopen( args[INFRACTIONS], "r");              // archivo de infractions. Definir INFRACTIONS y TICKETS
    FILE * tickets = fopen( args[TICKETS], "r");                      // archivo de estaciones 
    FILE * data_files[] = {infractions, tickets};

    //Chequeo de open files exitoso
    if (data_files[INFRACTIONS - 1] == NULL || data_files[TICKETS - 1]) {
        fprintf(stderr, "Error opening files\n");           
        closeCSV(data_files, FILES);                                
        exit(ERROR_FILE);                                            //No se bien si hay que poner ese error
    }

    //Inicializacion de archivos de escritura
    FILE * q1CSV= newCSV( "query1.csv", HEADER1);
    FILE * q2CSV= newCSV( "query2.csv",HEADER2);
    FILE * q3CSV= newCSV( "query3.csv",HEADER3);
    FILE * q4CSV= newCSV( "query4.csv",HEADER4);
    FILE * filesCSV[]={q1CSV, q2CSV, q3CSV, q4CSV};

    htmlTable q1HTML= newTable( "query1.html", 2, HTMLH1, HTMLH2);
    htmlTable q2HTML= newTable( "query2.html", 3, HTMLH3 , HTMLH1, HTMLH2);
    htmlTable q3HTML= newTable( "query3.html", 3, HTMLH1, HTMLH4, HTMLH2);
    htmlTable q4HTML= newTable( "query4.html", 4, "year", "ticketsTop1Month", "ticketsTop2Month", "ticketsTop3Month");
    //Decidir si dejamos los strings constantes o los define (si hacemos define faltan los de q4HTML)
    htmlTable filesHTML[]={q1HTML, q2HTML, q3HTML, q4HTML};

    //Falta chequeo de memoria en ambos archivos!!


    // Carga de Query 1 
    char infraction[MAX_LINE];
    char total[MAX_LINE];

    TQuery1 * q1 = query1(infractionSystem);

    toBeginQ1(q1);

    //Encabezado archivo csv:
    fprintf(filesCSV[FIRST], "infraction;totaltickets\n");    

    while ( hasNextQ1(q1)){
        fprintf(filesCSV[FIRST], "%s;%ld\n" , q1->iter->infraction, q1->iter->totalInfracctions);     
        //No se bien que argumento iria en filesCSV
        sprintf (infraction, "%s", q1->iter->infraction);
        sprintf (total, "%ld", q1->iter->totalInfracctions);
    
        addHTMLRow(filesHTML[FIRSTQ], infraction, total);     //no se si faltaria algun parametro mas, FIRSTQ especifica 

        nextQ1(q1);
    }
    freeQ1(q1);
    
    
    //Carga de Query 2
    TQuery2 * q2 = query2(infractionSystem);

    toBeginQ2(q2);

    // Encabezados CSV
    fprintf(filesCSV[FIRST], "issuingAgency;infraction;tickets\n");
    char agency[MAX_LINE];
    char infraction[MAX_LINE];
    char tickets[MAX_LINE];

    while (hasNextQ2(q2)) {
        fprintf(filesCSV[FIRST], "%s;%s;%ld\n", q2->iter->agency, q2->iter->mostPopularInf, q2->iter->fineCount);

        // Preparar para HTML
        sprintf(agency, "%s", q2->iter->agency);
        sprintf(infraction, "%s", q2->iter->mostPopularInf);
        sprintf(tickets, "%ld", q2->iter->fineCount);

        // Agregar fila HTML
        addHTMLRowQuery2(filesHTML[FIRST], agency, infraction, tickets);

        q2->iter = nextQ2(q2);
    }
    freeQ2(q2);

}



int readInfraction(FILE *file, int idColumn, int infractionColumn, infractionSystemADT infractionSystem){
    char currentLine[MAX_LINE];
    int succed = 0;

    fgets(currentLine, MAX_LINE, file);

    while (fgets(currentLine, MAX_LINE, file) != NULL) {
        int columnIdx = 0;
        int id = 0;
        char *infraction = NULL;
        
        char *token = strtok(currentLine, DELIMITER);

        while (token != NULL) {
            if (columnIdx == idColumn) {
                id = atoi(token);
            } else if (columnIdx == infractionColumn){
                char *newline = strchr(token,'\n'); //HACER UNA FUNCION
                if(newline){
                    *newline = '\0';
                }
                infraction = token;
            }
            token = strtok(NULL, DELIMITER);
            columnIdx++;
        }

        if (infraction != NULL) {
            succed = addInfraction(infractionSystem, infraction, id);

            if (!succed) {
                printf("Error al agregar infracción: %s\n", infraction);    //CAMBIAR      
            }
        }
    }
    return succed;
}

int readTickets(FILE *file, int plateColumn, int dateColumn, int idColumn, int agencyColumn, infractionSystemADT system){
    char currentLine[MAX_LINE];
    int succesAgency = 0;
    int succesPlate = 0;

    fgets(currentLine, MAX_LINE, file);

    while(fgets(currentLine, MAX_LINE, file) != NULL){
        int columnIdx=0;
        int year = 0;
        int month = 0;
        int id = 0;
        char *plate = NULL;
        char *date = NULL;
        char *agency = NULL;
    
        char *token = strtok(currentLine,DELIMITER);

        while(token != NULL){
            if(columnIdx == plateColumn){
                plate = token;
            }else if(columnIdx == idColumn){
                id = atoi(token);
            }else if(columnIdx == agencyColumn){
                char *newline = strchr(token,'\n');
                if(newline){
                    *newline = '\0';
                }
                agency = token;
            }else if(columnIdx == dateColumn){
                sscanf(token, "%d-%d", &year, &month);//PROBAR QUE FUNCIONES  
            }
            token = strtok(NULL,DELIMITER);
            columnIdx++;
        }
        if(agency != NULL){
            succesAgency = addAgency(system,agency,id);
            if(!succesAgency){
                printf("Error al agregar agencia: %s\n", agency);   //CAMBIAR         

            }
        }
        if(plate != NULL){
           succesPlate = addTicket(system,id,plate);
            if(!succesPlate){
                printf("Error al agregar patente: %s\n", plate);    //CAMBIAR     

            }
        }
    }
    return (succesAgency && succesPlate);
}

        
//el header se pasa por parametro y dependiendo del query uso el respectivo header del define
FILE *newCSV(const char *fileName, char *header){
    FILE *file=fopen(fileName, "w");
    errno=0;
    if(file==NULL){
        perror("Error al abrir el archivo\n");
        return NULL;
    }
    if(errno==ENOMEM){
        perror("Error al abrir el archivo\n");
        fclose(file);
        return NULL;
    }
    fprintf(file, "%s\n", header);
    return file;
}

void closeCSV(FILE *files[], int fileQuantity){
    for(int i=0;i<fileQuantity;i++){
        if(files[i]!=NULL){
            fclose(files[i]);
        }
    }
}

//funcion que cierra los archivos html que estan en el arreglo files
void closeHTML(htmlTable files[], int fileQuantity){
    for(int i=0;i<fileQuantity;i++){
        if(files[i]!=NULL)
            closeHTMLTable(files[i]);
    }
}

int valid( const char* s){
    for ( int i =0; s[i]; i++){
        if ( !isdigit(s[i]))
            return 0;
    }
    return 1;
}