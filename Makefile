PNAME = out
SRC = ./src
BIN = ./bin
LIBRARY = ./src/lib
PARSER = ./src/parser

OBJ = 	$(BIN)/main.o \
		$(BIN)/parser.o \
		$(BIN)/errors.o \
		$(BIN)/utilities.o

#FLAGS = -Wall -Wextra -DDEBUG


.PHONY = build clean

build: $(OBJ)
	@mkdir -p bin
	@gcc -o $(BIN)/$(PNAME) $(OBJ) $(FLAGS)
	@echo Finished building

$(BIN)/main.o: $(SRC)/main.c $(LIBRARY)/commands.h
	gcc -c $(SRC)/main.c $(FLAGS)
	@mv main.o $(BIN)/main.o

$(BIN)/parser.o: $(PARSER)/parser.c $(PARSER)/parser.h
	gcc -c $(PARSER)/parser.c $(FLAGS)
	@mv parser.o $(BIN)/parser.o

$(BIN)/errors.o: $(LIBRARY)/errors.c $(LIBRARY)/errors.h
	gcc -c $(LIBRARY)/errors.c $(FLAGS)
	@mv errors.o $(BIN)/errors.o

$(BIN)/utilities.o: $(LIBRARY)/utilities.c $(LIBRARY)/utilities.h
	gcc -c $(LIBRARY)/utilities.c $(FLAGS)
	@mv utilities.o $(BIN)/utilities.o

clean:
	rm $(BIN)/$(PNAME) $(OBJ)
