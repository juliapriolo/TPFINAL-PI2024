#include <stdio.h>
#include <stdlib.h>
#include "infractionsADT.h"

#define HEADER1 "infraction;tickets\n"
#define HEADER2 "issuingAgency;infraction;tickets\n"
#define HEADER3 "infraction;plate;tickets\n"
#define HEADER4 "year;ticketsTop1Month;ticketsTop2Month;ticketsTop3Month\n"
#define DELIMITER ";"
#define CANT_QUERY 4
#define MAX_LINE 100

int readId (FILE * file, int id, int infraction, infractionSystemADT infractionSystem);     //Funcion para leer archivos csv



//los parametros id e station son el numero de columna que se quiere leer en el archivo csv!!
//Hay que ver si estamos manejando bien la memoria
int readId ( FILE * file, int id, int infraction, infractionSystemADT infractionSystem ){
    char line[MAX_LINE];
    int ok = 0, infractionId, i=0;

    fgets (line, MAX_LINE, file);                                  //Linea de titulos se descarta

    while (fgets(line, MAX_LINE, file) != NULL){

        char * tok = strtok(line, DELIMITER);
        char * infractionType ;

        while (tok != NULL) {
            if (i == infraction) {
                infractionType = tok;
            } else if (i == id) {
                infractionId = atoi(tok);
            }
            tok = strtok(NULL, DELIMITER);
            i++;
        }
        
        if(infractionType != NULL){
            ok = addInfraction(infractionSystem, infractionType, infractionId);
        }
    }

    return (ok == 0);
}
        
