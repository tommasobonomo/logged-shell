
PNAME = out
SRC = ./src
BIN = ./bin
LIBRARY = $(SRC)/lib
PARSER = $(SRC)/parser
EXECUTER = $(SRC)/executer
DEBUGGUARD = $(BIN)/DEBUG

# list of object files, needs to be kept updated
OBJ = 	$(BIN)/main.o \
		$(BIN)/parser.o \
		$(BIN)/errors.o \
		$(BIN)/utilities.o \
		$(BIN)/syscalls.o \
		$(BIN)/executer.o


.PHONY = build debug checkDebug clean

# build rule, the standard one
build: $(BIN) checkDebug $(OBJ)
	@gcc -o $(BIN)/$(PNAME) $(OBJ)
	@echo Finished building

# debug rule, use it when debugging. It sets custom flags
debug: FLAGS = -Wall -Wextra -DDEBUG -g
debug: $(BIN) checkNotDebug $(OBJ)
	@gcc -o $(BIN)/$(PNAME) $(OBJ) $(FLAGS)
	@touch $(DEBUGGUARD)
	@echo Finished building in debug mode

# if DEBUGGUARD is present, it removes all the content of /bin
checkDebug:
	@if [ -f $(DEBUGGUARD) ]; then \
		rm -f $(BIN)/$(PNAME) $(OBJ) $(DEBUGGUARD); \
	fi

# if DEBUGGUARD is not present, it removes all the content of /bin
checkNotDebug:
	@if [ ! -f $(DEBUGGUARD) ]; then \
		rm -f $(BIN)/$(PNAME) $(OBJ) $(DEBUGGUARD); \
	fi

# creates /bin folder if it doesn't exist
$(BIN):
	@mkdir -p bin

# object files
$(BIN)/main.o: $(SRC)/main.c $(LIBRARY)/commands.h
	gcc -c $(SRC)/main.c -o $(BIN)/main.o $(FLAGS)

$(BIN)/parser.o: $(PARSER)/parser.c $(PARSER)/parser.h
	gcc -c $(PARSER)/parser.c -o $(BIN)/parser.o $(FLAGS)

$(BIN)/errors.o: $(LIBRARY)/errors.c $(LIBRARY)/errors.h
	gcc -c $(LIBRARY)/errors.c -o $(BIN)/errors.o $(FLAGS)

$(BIN)/utilities.o: $(LIBRARY)/utilities.c $(LIBRARY)/utilities.h
	gcc -c $(LIBRARY)/utilities.c -o $(BIN)/utilities.o $(FLAGS)

$(BIN)/syscalls.o: $(LIBRARY)/syscalls.c $(LIBRARY)/syscalls.h
	gcc -c $(LIBRARY)/syscalls.c -o $(BIN)/syscalls.o $(FLAGS)

$(BIN)/executer.o: $(EXECUTER)/executer.c $(EXECUTER)/executer.h
	gcc -c $(EXECUTER)/executer.c -o $(BIN)/executer.o $(FLAGS)

# clean rule, it completely removes /bin folder
clean:
	@rm -rf $(BIN)
	@echo Removed /bin folder