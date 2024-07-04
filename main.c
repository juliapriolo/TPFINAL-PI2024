#include <stdio.h>
#include <stdlib.h>
#include "infractionsADT.h"
#include <errno.h>

#define HEADER1 "infraction;tickets\n"
#define HEADER2 "issuingAgency;infraction;tickets\n"
#define HEADER3 "infraction;plate;tickets\n"
#define HEADER4 "year;ticketsTop1Month;ticketsTop2Month;ticketsTop3Month\n"
#define DELIMITER ";"
#define CANT_QUERY 4
#define MAX_LINE 100

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