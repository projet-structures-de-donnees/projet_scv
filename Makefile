CC = gcc
CFLAGS = -Wall -Iinclude
SDIR = src

# Liste des fichiers source
SRC_FILES = $(SDIR)/main.c $(SDIR)/test.c $(SDIR)/myGit.c $(SDIR)/branch.c $(SDIR)/commit.c $(SDIR)/instantane.c $(SDIR)/merge.c $(SDIR)/worktree.c


all : myGit
		




test.o : $(SDIR)/test.c 
	$(CC) $(CFLAGS) -c -o test.o -c $(SDIR)/test.c

test : test.o  branch.o commit.o instantane.o merge.o worktree.o
	$(CC) $(CFLAGS) -o test test.o branch.o commit.o instantane.o merge.o worktree.o






instantane.o: $(SDIR)/instantane.c
	$(CC) $(CFLAGS) -c -o instantane.o $(SDIR)/instantane.c

worktree.o: $(SDIR)/worktree.c
	$(CC) $(CFLAGS) -c -o worktree.o $(SDIR)/worktree.c

commit.o: $(SDIR)/commit.c
	$(CC) $(CFLAGS) -c -o commit.o $(SDIR)/commit.c

branch.o: $(SDIR)/branch.c
	$(CC) $(CFLAGS) -c -o branch.o $(SDIR)/branch.c

merge.o: $(SDIR)/merge.c
	$(CC) $(CFLAGS) -c -o merge.o $(SDIR)/merge.c






myGit.o :  $(SDIR)/myGit.c 
	$(CC) $(CFLAGS) -c -o myGit.o -c $(SDIR)/myGit.c

myGit: myGit.o branch.o commit.o instantane.o merge.o worktree.o
	$(CC) $(CFLAGS) -o myGit myGit.o branch.o commit.o instantane.o merge.o worktree.o



clean :
	rm -f *.o main test myGit