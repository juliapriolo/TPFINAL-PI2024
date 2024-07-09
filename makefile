COMPILER = gcc
FLAGS = -Wall -pedantic -std=c99 -fsanitize=address -g
OUTPUT_FILE_NYC = parkingTicketsNYC
OUTPUT_FILE_CHI = parkingTicketsCHI
DEPENDENCIES = infractionsADT.c main.c htmlTable.c
QUERIES_CSV = query1.csv query2.csv query3.csv query4.csv
QUERIES_HTML = query1.html query2.html query3.html query4.html

NYC:
	$(COMPILER) -o $(OUTPUT_FILE_NYC) $(DEPENDENCIES) $(FLAGS) -DNYC

CHI:
	$(COMPILER) -o $(OUTPUT_FILE_CHI) $(DEPENDENCIES) $(FLAGS)

all: NYC CHI

clean: cleanNYC cleanCHI cleanQueries

cleanNYC:
	rm -rf $(OUTPUT_FILE_NYC) *.o

cleanCHI:
	rm -rf $(OUTPUT_FILE_CHI) *.o

cleanQueries:
	rm -rf $(QUERIES_CSV) $(QUERIES_HTML)