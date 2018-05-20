PNAME = logged_shell
DEBUGGUARD = $(BIN)/DEBUG

SRC = ./src
BIN = ./bin
LOGS = /tmp/logged_shell
LIBRARY = $(SRC)/lib
PARSER = $(SRC)/parser
EXECUTER = $(SRC)/executer
STATISTICS = $(SRC)/statistics
DAEMON = $(SRC)/daemon


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
build: $(BIN) checkDebug $(OBJ) $(LOGS)
	@gcc -std=gnu90 -o $(BIN)/$(PNAME) $(OBJ)
	@echo Finished building

# debug rule, use it when debugging. It sets custom flags
debug: FLAGS = -Wall -Wextra -DDEBUG -g
debug: $(BIN) checkNotDebug $(OBJ) $(LOGS)
	@gcc -std=gnu90 -o $(BIN)/$(PNAME) $(OBJ) $(FLAGS)
	@touch $(DEBUGGUARD)
	@echo Finished building in debug mode

# if DEBUGGUARD is present, it removes all the content of ./bin
checkDebug:
	@if [ -f $(DEBUGGUARD) ]; then \
		rm -f $(BIN)/$(PNAME) $(OBJ) $(DEBUGGUARD); \
	fi

# if DEBUGGUARD is not present, it removes all the content of ./bin
checkNotDebug:
	@if [ ! -f $(DEBUGGUARD) ]; then \
		rm -f $(BIN)/$(PNAME) $(OBJ) $(DEBUGGUARD); \
	fi

# creates ./bin folder if it doesn't exist
$(BIN):
	@mkdir -p $(BIN)
	@echo Created $(BIN) folder

# creates /tmp/logs folder if it doesn't exist
$(LOGS):
	@mkdir -p $(LOGS)
	@echo Created $(LOGS) folder

# object files
$(BIN)/main.o: $(SRC)/main.c
	gcc -std=gnu90 -c $(SRC)/main.c -o $(BIN)/main.o $(FLAGS)

$(BIN)/commands.o: $(LIBRARY)/commands.c $(LIBRARY)/commands.h
	gcc -std=gnu90 -c $(LIBRARY)/commands.c -o $(BIN)/commands.o $(FLAGS)

$(BIN)/parser.o: $(PARSER)/parser.c $(PARSER)/parser.h
	gcc -std=gnu90 -c $(PARSER)/parser.c -o $(BIN)/parser.o $(FLAGS)

$(BIN)/errors.o: $(LIBRARY)/errors.c $(LIBRARY)/errors.h
	gcc -std=gnu90 -c $(LIBRARY)/errors.c -o $(BIN)/errors.o $(FLAGS)

$(BIN)/utilities.o: $(LIBRARY)/utilities.c $(LIBRARY)/utilities.h
	gcc -std=gnu90 -c $(LIBRARY)/utilities.c -o $(BIN)/utilities.o $(FLAGS)

$(BIN)/syscalls.o: $(LIBRARY)/syscalls.c $(LIBRARY)/syscalls.h
	gcc -std=gnu90 -c $(LIBRARY)/syscalls.c -o $(BIN)/syscalls.o $(FLAGS)

$(BIN)/executer.o: $(EXECUTER)/executer.c $(EXECUTER)/executer.h
	gcc -std=gnu90 -c $(EXECUTER)/executer.c -o $(BIN)/executer.o $(FLAGS)

$(BIN)/statHelper.o: $(STATISTICS)/statHelper.c $(STATISTICS)/statHelper.h
	gcc -std=gnu90 -c $(STATISTICS)/statHelper.c -o $(BIN)/statHelper.o $(FLAGS)

$(BIN)/daemon_api.o: $(DAEMON)/daemon_api.c $(DAEMON)/daemon.h
	gcc -std=gnu90 -c $(DAEMON)/daemon_api.c -o $(BIN)/daemon_api.o $(FLAGS)

$(BIN)/get_daemon.o: $(DAEMON)/get_daemon.c $(DAEMON)/daemon.h
	gcc -std=gnu90 -c $(DAEMON)/get_daemon.c -o $(BIN)/get_daemon.o $(FLAGS)

$(BIN)/core.o: $(DAEMON)/core.c $(DAEMON)/daemon.h
	gcc -std=gnu90 -c $(DAEMON)/core.c -o $(BIN)/core.o $(FLAGS)

# clean rule, it completely removes ./bin folder
clean:
	@rm -rf $(BIN)
	@echo Removed $(BIN) folder
	@rm -rf $(LOGS)
	@echo Removed $(LOGS) folder
