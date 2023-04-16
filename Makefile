CC = gcc
CFLAGS = -Wall -Iinclude
SRC = src
BIN = bin


# Création du répertoire "bin" s'il n'existe pas
$(shell mkdir -p $(BIN))



OBJ_FILES_TEST = $(BIN)/test.o $(BIN)/branch.o $(BIN)/commit.o $(BIN)/instantane.o $(BIN)/merge.o $(BIN)/worktree.o
OBJ_FILES_MYGIT = $(BIN)/myGit.o $(BIN)/branch.o $(BIN)/commit.o $(BIN)/instantane.o $(BIN)/merge.o $(BIN)/worktree.o


all : myGit
		

$(BIN)/test.o : $(SRC)/test.c 
	$(CC) $(CFLAGS) -o $@ -c $<

test : $(OBJ_FILES_TEST)
	$(CC) $^ $(CFLAGS) -o $@



$(BIN)/%.o : $(SRC)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<


myGit: $(OBJ_FILES_MYGIT)
	$(CC) $^ $(CFLAGS) -o $@ 



clean :
	rm -f main test myGit
	rm -r bin/
	