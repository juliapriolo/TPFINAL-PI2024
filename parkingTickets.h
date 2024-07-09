//longitudes de los strings str+1
#ifdef NY
#define MAX_LEN_AGENCY 36 //largo maximo para una agencia en Nueva York
#define MAX_LEN_DESCR 31 //largo maximo para el nombre de una infracción en Nueva York
enum arguments{PLATE=0, DATE, ID, FINE_AMOUNT, AGENCY}; //NY
#else
#define MAX_LEN_AGENCY 14 //largo maximo para una agencia en Chicago
#define MAX_LEN_DESCR 51 //largo maximo para el nombre de una infraccion en Chicago
enum arguments{DATE=0, PLATE, AGENCY, ID, FINE_AMOUNT}; //CHI
#endif
#define LEN_PLATE 11