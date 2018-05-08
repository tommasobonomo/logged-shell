PNAME = out
SRC = ./src
BIN = ./bin
LIBRARY = ./src/lib
PARSER = ./src/parser

OBJ = 	$(BIN)/main.o \
		$(BIN)/parser.o \
		$(BIN)/errors.o \
		$(BIN)/utilities.o


.PHONY = build debug clean

build: $(BIN) $(OBJ)
	@gcc -o $(BIN)/$(PNAME) $(OBJ)
	@echo Finished building

debug: FLAGS = -Wall -Wextra -DDEBUG
debug: $(BIN) clean $(OBJ)
	@gcc -o $(BIN)/$(PNAME) $(OBJ) $(FLAGS)
	@echo Finished building in debug mode

$(BIN):
	@mkdir -p bin

$(BIN)/main.o: $(SRC)/main.c $(LIBRARY)/commands.h
	gcc -c $(SRC)/main.c -o $(BIN)/main.o $(FLAGS)

$(BIN)/parser.o: $(PARSER)/parser.c $(PARSER)/parser.h
	gcc -c $(PARSER)/parser.c -o $(BIN)/parser.o $(FLAGS)

$(BIN)/errors.o: $(LIBRARY)/errors.c $(LIBRARY)/errors.h
	gcc -c $(LIBRARY)/errors.c -o $(BIN)/errors.o $(FLAGS)

$(BIN)/utilities.o: $(LIBRARY)/utilities.c $(LIBRARY)/utilities.h
	gcc -c $(LIBRARY)/utilities.c -o $(BIN)/utilities.o $(FLAGS)

clean:
	@rm -f $(BIN)/$(PNAME) $(OBJ)
	@echo Removed temp files