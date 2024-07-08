#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "infractionsADT.h"
#include <errno.h>
#include "htmlTable.h"
#include <strings.h>

#define HEADER1 "infraction;tickets"
#define HEADER2 "issuingAgency;infraction;tickets"
#define HEADER3 "infraction;plate;tickets"
#define HEADER4 "year;ticketsTop1Month;ticketsTop2Month;ticketsTop3Month"
#define DELIMITER ";"
#define CANT_QUERY 4
#define FILES 2
#define MAX_LINE 100

#define INVALID_YEAR -1
#define ERROR_PAR 1
#define ERROR_FILE 2
#define ERROR_AG 3
#define OK 0


#define MAX_ARG 5
#define MIN_ARG 4
#define DESCR 1
#define ID_INFR 0

enum files{FIRST = 0, SECOND, THIRD, FOURTH};    //Enum para buscar los archivos filescsv y fileshtml

enum programArguments{PROGRAM = 0,TICKETS,INFRACTIONS, MIN_YEAR, MAX_YEAR};  

#ifdef NY
#define MAX_LEN_AGENCY 35 //largo maximo para una agencia en Nueva York
#define MAX_LEN_DESCR 30 //largo maximo para el nombre de una infracción en Nueva York
#define LEN_PLATE 7
enum arguments{PLATE=0, DATE, ID, FINE_AMOUNT, AGENCY}; //NY
#else
#define MAX_LEN_AGENCY 13 //largo maximo para una agencia en Chicago
#define MAX_LEN_DESCR 50 //largo maximo para el nombre de una infraccion en Chicago
#define LEN_PLATE 10
enum arguments{DATE=0, PLATE, AGENCY, ID, FINE_AMOUNT}; //CHI
#endif

int readInfraction(FILE *file, int idColumn, int infractionColumn, infractionSystemADT infractionSystem);
int readTickets(FILE *file, int plateColumn, int dateColumn, int idColumn, int agencyColumn, infractionSystemADT system);
FILE *newCSV(const char *fileName, char *header);   //funcion que crea un nuevo archivo csv y verifica que se haya creado bien
void closeCSV(FILE *files[], int fileQuantity); //funcion que cierra los archivos csv
void closeHTML(htmlTable files[], int fileQuantity);    //funcion que cierra los archivos html que estan en el arreglo files
int valid( const char* s);  //se asegura que el string sea todo de numeros
void closeRFile(infractionSystemADT infractionSystem, FILE *data_files[], int error, int fileQuantity);
void closeWFile(infractionSystemADT infractionSystem, FILE *csvFile[], htmlTable htmlFile[], int error, int queryQuantity);


int main(int argc, char *argv[]){

    if(argc > MAX_ARG || argc<MIN_ARG){
        fprintf(stderr, "Incorrect amount of arguments supplied\n");
        exit(ERROR_PAR);
    }
    int minYear, maxYear = INVALID_YEAR; 

    if ( argc >= MIN_ARG ){ //Vemos si minYear es un tipo de dato valido
        if( !valid( argv[MIN_YEAR])){
            fprintf( stderr, "Incorrect type for the minimum year\n");
            exit(ERROR_PAR); //vamos a hacer los enums para los distintos tipos de errores??
            }
        minYear = atoi(argv[MIN_YEAR]);
    }
    if ( argc == MAX_ARG ){    //Vemos si maxYear es un tipo de dato valido
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

    //Inicializacion de archivos de lectura
    FILE * infractions = fopen(argv[INFRACTIONS], "r");              // archivo de infractions. Definir INFRACTIONS y TICKETS
    FILE * tickets = fopen( argv[TICKETS], "r");                      // archivo de tickets
    FILE * data_files[] = {tickets,infractions};

    //Chequeo de open files exitoso
    if (data_files[INFRACTIONS - 1] == NULL || data_files[TICKETS - 1] == NULL) {
        fprintf(stderr, "Error opening files\n");           
        closeCSV(data_files, FILES);                                
        exit(ERROR_FILE);                                            //No se bien si hay que poner ese error
    }

    errno = 0;
    infractionSystemADT infractionSystem = newInfractionSystem(minYear, maxYear);

    //Chequeo inicializacion del TAD exitosa
    if(infractionSystem == NULL || errno == ENOMEM){
        fprintf(stderr, "Error creating infractionSystem\n");
        closeCSV(data_files, FILES);
        freeInfractionSystem(infractionSystem);
        exit(ENOMEM);
    }

    int infractionSuccess=readInfraction(data_files[INFRACTIONS-1], ID_INFR, DESCR, infractionSystem); //readInfraction returns 1 if it was successfull
    if(!infractionSuccess){
        fprintf(stderr, "Could not read infractions\n");
        closeRFile(infractionSystem, data_files, ERROR_FILE, FILES);
    }

    int ticketSuccess = readTickets(data_files[TICKETS-1], PLATE, DATE, ID, AGENCY, infractionSystem);  //readTickets returns 1 if it was successfull
    if(!ticketSuccess){
        fprintf(stderr, "Could not read tickets\n");
        closeRFile(infractionSystem, data_files, ERROR_FILE, FILES);
    }

    closeCSV(data_files, FILES);

    //Inicializacion de archivos de escritura
    FILE * q1CSV = newCSV( "query1.csv", HEADER1);
    FILE * q2CSV = newCSV( "query2.csv",HEADER2);
    FILE * q3CSV = newCSV( "query3.csv",HEADER3);
    FILE * q4CSV = newCSV( "query4.csv",HEADER4);
    FILE * filesCSV[] = {q1CSV, q2CSV, q3CSV, q4CSV};

    htmlTable q1HTML = newTable( "query1.html", 2, "infraction", "tickets");
    htmlTable q2HTML = newTable( "query2.html", 3, "issuing agency" , "infraction", "tickets");
    htmlTable q3HTML = newTable( "query3.html", 3, "infraction", "plate", "tickets");
    htmlTable q4HTML = newTable( "query4.html", 4, "year", "ticketsTop1Month", "ticketsTop2Month", "ticketsTop3Month");
    htmlTable filesHTML[] = {q1HTML, q2HTML, q3HTML, q4HTML};

    //Chequeo de error en la creacion de archivos CSV y HTML
    for(size_t i = 0; i < CANT_QUERY; i++){
        if((filesCSV[i] == NULL) || (filesHTML[i] == NULL)){
            fprintf(stderr, "Could not open file\n");
            closeWFile(infractionSystem, filesCSV, filesHTML, ERROR_FILE, CANT_QUERY);
        }
    }

    // Carga de Query 1 
    char infractionq1[MAX_LINE];
    char totalq1[MAX_LINE];


    TQuery1 * q1 = query1(infractionSystem);

    toBeginQ1(q1);
 

    while ( hasNextQ1(q1)){
        fprintf(filesCSV[FIRST], "%s;%ld\n" , q1->iter->infraction, q1->iter->totalInfracctions);     
        //No se bien que argumento iria en filesCSV
        sprintf (infractionq1, "%s", q1->iter->infraction);
        sprintf (totalq1, "%ld", q1->iter->totalInfracctions);
        //Agrego una nueva fila al archivo
        addHTMLRow(filesHTML[FIRST], infractionq1, totalq1);     

        nextQ1(q1);
    }
    freeQ1(q1);
    fclose(filesCSV[FIRST]);
    
    //Carga de Query 2
    int dim = dimAgency(infractionSystem);
    TQuery2 * q2 = query2(infractionSystem);


    char agencyq2[MAX_LINE];
    char infractionq2[MAX_LINE];
    char ticketsq2[MAX_LINE];

    for(size_t i = 0; i < dim; i++) {
        fprintf(filesCSV[SECOND], "%s;%s;%ld\n", q2[i].agency, q2[i].mostPopularInf, q2[i].fineCount);

        // Preparar para HTML
        sprintf(agencyq2, "%s", q2[i].agency);
        sprintf(infractionq2, "%s", q2[i].mostPopularInf);
        sprintf(ticketsq2, "%ld", q2[i].fineCount);

        // Agregar fila HTML
        addHTMLRow(filesHTML[SECOND], agencyq2, infractionq2, ticketsq2);
    }
    freeQ2(q2);

    // Carga de Query 3 
    char infractionq3[MAX_LINE];
    char plateq3[MAX_LINE];
    char totalq3[MAX_LINE];
    // puts("Empezando Query3");

    TQuery3 * q3 = query3(infractionSystem);

    //Encabezado archivo csv:
    fprintf(filesCSV[THIRD], "infraction;plate;tickets\n");    

    for(size_t i = 0; i < q3->dim; i++){
        fprintf(filesCSV[THIRD], "%s;%s;%ld\n", q3->vectorDeDatos[i].infraction, q3->vectorDeDatos[i].plate, q3->vectorDeDatos[i].fineAmount);
        sprintf(infractionq3, "%s", q3->vectorDeDatos[i].infraction);
        sprintf(plateq3, "%s", q3->vectorDeDatos[i].plate);
        sprintf(totalq3, "%ld", q3->vectorDeDatos[i].fineAmount);

        addHTMLRow(filesHTML[THIRD], infractionq3, plateq3, totalq3);
    }
    
    freeQ3(q3);

    //Carga el Query 4
    TQuery4 * q4 = query4(infractionSystem);
    int q4Dim = dimArr(infractionSystem);

    char year[MAX_LINE];
    char topMonth1[MAX_LINE];
    char topMonth2[MAX_LINE];
    char topMonth3[MAX_LINE];

    for(size_t i = 0; i < q4Dim; i++){
        fprintf(filesCSV[FOURTH], "%ld;%s;%s;%s\n", q4->vec[i].year, q4->vec[i].monthTop1, q4->vec[i].monthTop2, q4->vec[i].monthTop3);

        sprintf(year, "%ld", q4->vec[i].year);
        sprintf(topMonth1, "%s", q4->vec[i].monthTop1);
        sprintf(topMonth2, "%s", q4->vec[i].monthTop2);
        sprintf(topMonth3, "%s", q4->vec[i].monthTop3);

        addHTMLRow(filesHTML[FOURTH], year, topMonth1, topMonth2, topMonth3);
    }
    freeQ4(q4);

    closeWFile(infractionSystem, filesCSV, filesHTML, OK, CANT_QUERY);
    return OK;

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
                char *newline = strchr(token,'\n');
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

            // if (!succed) {
            //     printf("Error al agregar infracción: %s\n", infraction);    //CAMBIAR      
            // }
        }
    }
    return succed;
}

int readTickets(FILE *file, int plateColumn, int dateColumn, int idColumn, int agencyColumn, infractionSystemADT system){
    char currentLine[MAX_LINE];
    int succesAgency = 0;
    int succesPlate = 0;
    int succesDate = 0;

    fgets(currentLine, MAX_LINE, file);

    while(fgets(currentLine, MAX_LINE, file) != NULL){
        int columnIdx=0;
        int year = 0;
        int month = 0;
        int id = 0;
        char *plate = NULL;
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
            // if(!succesAgency){
            //     printf("Error al agregar agencia: %s\n", agency);   //CAMBIAR         

            // }
        }
        if(plate != NULL){
            succesPlate = addTicket(system,id,plate);
            // if(!succesPlate){
            //     printf("Error al agregar patente: %s\n", plate);    //CAMBIAR     

            // }
        }
        if(year != 0 && month != 0){
            succesDate = addDate(system,year,month);
        }
        
    }
    return (succesAgency && succesPlate && succesDate);
}

        
//el header se pasa por parametro y dependiendo del query uso el respectivo header del define
FILE *newCSV(const char *fileName, char *header){
    FILE *file = fopen(fileName, "w");
    errno = 0;
    if(file == NULL){
        perror("Error al abrir el archivo\n");
        return NULL;
    }
    if(errno == ENOMEM){
        perror("Error al abrir el archivo\n");
        fclose(file);
        return NULL;
    }
    fprintf(file, "%s\n", header);
    return file;
}

void closeCSV(FILE *files[], int fileQuantity){
    for(int i = 0; i < fileQuantity; i++){
        if(files[i] != NULL){
            fclose(files[i]);
        }
    }
}

//funcion que cierra los archivos html que estan en el arreglo files
void closeHTML(htmlTable files[], int fileQuantity){
    for(int i = 0; i < fileQuantity; i++){
        if(files[i] != NULL)
            closeHTMLTable(files[i]);
    }
}

// Cierra los archivos de lectura y libera el sistema, en caso de que halla un error, aborta
void closeRFile(infractionSystemADT infractionSystem, FILE *data_files[], int error, int fileQuantity){
    freeInfractionSystem(infractionSystem);
    closeCSV(data_files, fileQuantity);
}

// Cierra los archivos de escritura y libera el sistema, en caso de que halla un error, aborta
void closeWFile(infractionSystemADT infractionSystem, FILE *csvFile[], htmlTable htmlFile[], int error, int queryQuantity){
    freeInfractionSystem(infractionSystem);
    closeCSV(csvFile, queryQuantity);
    closeHTML(htmlFile, queryQuantity);
    if(error){
        exit(error);
    }    
}

int valid( const char* s){
    for ( int i = 0; s[i]; i++){
        if ( !isdigit(s[i]))
            return 0;
    }
    return 1;
}