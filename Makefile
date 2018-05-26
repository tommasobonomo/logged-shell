PNAME = xlogger
DEBUGGUARD = $(BIN)/DEBUG

SRC = ./src
BIN = ./bin
LOGS = /tmp/xlogger-ZZN5V7KH3H
LIBRARY = $(SRC)/lib
PARSER = $(SRC)/parser
EXECUTER = $(SRC)/executer
STATISTICS = $(SRC)/statistics
DAEMON = $(SRC)/daemon

FLAGS = -std=gnu90 -lpthread

# list of object files, needs to be kept updated
OBJ = 	$(BIN)/main.o \
		$(BIN)/parser.o \
		$(BIN)/errors.o \
		$(BIN)/utilities.o \
		$(BIN)/syscalls.o \
		$(BIN)/executer.o \
		$(BIN)/statHelper.o \
		$(BIN)/daemon_api.o \
		$(BIN)/core.o \
		$(BIN)/get_daemon.o \
		$(BIN)/commands.o


.PHONY = build debug checkDebug clean

# build rule, the standard one
build: checkDebug $(BIN) $(OBJ)
	@gcc -o $(BIN)/$(PNAME) $(OBJ) $(FLAGS)
	@echo Finished building

# debug rule, use it when debugging. It sets custom flags
debug: FLAGS += -Wall -Wextra -DDEBUG -g
debug: checkNotDebug $(BIN) $(OBJ)
	@gcc -o $(BIN)/$(PNAME) $(OBJ) $(FLAGS)
	@touch $(DEBUGGUARD)
	@echo Finished building in debug mode

# if DEBUGGUARD is present, it removes all the content of ./bin
checkDebug:
	@if [ -d $(BIN) ] && [ -f $(DEBUGGUARD) ]; then \
		rm -rf $(BIN)/*; \
		echo Removed $(BIN) folder content; \
	fi

# if DEBUGGUARD is not present, it removes all the content of ./bin
checkNotDebug:
	@if [ -d $(BIN) ] && [ ! -f $(DEBUGGUARD) ]; then \
		rm -rf $(BIN)/*; \
		echo Removed $(BIN) folder content; \
	fi

# creates ./bin folder if it doesn't exist
$(BIN):
	@mkdir -p $(BIN)
	@echo Created $(BIN) folder

# object files
$(BIN)/main.o: $(SRC)/main.c
	gcc -c $(SRC)/main.c -o $(BIN)/main.o $(FLAGS)

$(BIN)/commands.o: $(LIBRARY)/commands.c $(LIBRARY)/commands.h
	gcc -c $(LIBRARY)/commands.c -o $(BIN)/commands.o $(FLAGS)

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

$(BIN)/statHelper.o: $(STATISTICS)/statHelper.c $(STATISTICS)/statHelper.h
	gcc -c $(STATISTICS)/statHelper.c -o $(BIN)/statHelper.o $(FLAGS)

$(BIN)/daemon_api.o: $(DAEMON)/daemon_api.c $(DAEMON)/daemon.h
	gcc -c $(DAEMON)/daemon_api.c -o $(BIN)/daemon_api.o $(FLAGS)

$(BIN)/get_daemon.o: $(DAEMON)/get_daemon.c $(DAEMON)/daemon.h
	gcc -c $(DAEMON)/get_daemon.c -o $(BIN)/get_daemon.o $(FLAGS)

$(BIN)/core.o: $(DAEMON)/core.c $(DAEMON)/daemon.h
	gcc -c $(DAEMON)/core.c -o $(BIN)/core.o $(FLAGS)

# clean rule, it completely removes ./bin folder
clean:
	@if [ -d $(BIN) ]; then \
		rm -rf $(BIN); \
		echo Removed $(BIN) folder; \
	fi
	@if [ -d $(LOGS) ]; then \
		rm -rf $(LOGS); \
		echo Removed $(LOGS) folder; \
	fi