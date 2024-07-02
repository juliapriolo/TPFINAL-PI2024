COMPILER = gcc
FLAGS = -Wall -pedantic -std=c99 -fsanitize=address -g
OUTPUT_FILE_NY = infractionsNY
OUTPUT_FILE_CHI = infractionsCHI
DEPENDENCIES = infractionsADT.c main.c htmlTable.c

NY:
	$(COMPILER) -o $(OUTPUT_FILE_NY) $(DEPENDENCIES) $(FLAGS) -DNY

CHI:
	$(COMIPLER) -o $(OUTPUT_FILE_CHI) $(DEPENDENCIES) $(FLAGS) -DCHI

all: NY CHI

clean: cleanNY cleanCHI

cleanNY:
	rm -rf $(OUTPUT_FILE_NY) *.o

cleanCHI:
	rm -rf $(OUTPUT_FILE_CHI) *.o