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
#define MAX_LINE 100

#define INVALID_YEAR -1
#define ERROR 1
#define OK 0

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
        exit(ERROR);
    }
    int minYear, maxYear = INVALID_YEAR; //no estamos seguras si hay que inicializar el minYear o no

    if ( argc >= MIN_ARG ){ //Vemos si minYear es un tipo de dato valido
        if( !valid( argv[MIN_YEAR])){
            fprintf( stderr, "Incorrect type for the minimum year\n");
            exit(ERROR); //vamos a hacer los enums para los distintos tipos de errores??
            }
        minYear = atoi(args[MIN_YEAR]);
    }
    if ( cantArg == 5 ){    //Vemos si maxYear es un tipo de dato valido
        if(!valid( args[MAX_YEAR]) ){
            fprintf( stderr, "Incorrect type for the maximum year\n");
            exit( ERROR);
            }
        maxYear = atoi(args[MAX_YEAR]);

        if ( minYear > maxYear ){   //Vemos que el año minimo no sea mayor que el año maximo
            fprintf(stderr, "Invalid: minYear can not be greater than maxYear\n");
            exit(ERROR);
        }
    }
    return 0;
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