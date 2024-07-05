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

#ifdef NY
#define MAX_LEN_AGENCY 35 //largo maximo para una agencia en Nueva York
#define MAX_LEN_DESCR 30 //largo maximo para el nombre de una infracción en Nueva York
#else
#define MAX_LEN_AGENCY 13 //largo maximo para una agencia en Chicago
#define MAX_LEN_DESCR 50 //largo maximo para el nombre de una infraccion en Chicago
#endif

int readId (FILE * file, int id, int infraction, infractionSystemADT infractionSystem);     //Funcion para leer archivos csv
FILE *newCSV(const char *fileName, char *header);   //funcion que crea un nuevo archivo csv y verifica que se haya creado bien
void closeCSV(FILE *files[], int fileQuantity); //funcion que cierra los archivos csv


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
        if(files[i]!=NULL)
            fclose(files[i]);
    }
}

//funcion que cierra los archivos html que estan en el arreglo files
void closeHTML(htmlTable files[], int fileQuantity){
    for(int i=0;i<fileQuantity;i++){
        if(files[i]!=NULL)
            closeHTMLTable(files[i]);
    }
}