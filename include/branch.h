#ifndef __BRANCH_H__
#define __BRANCH_H__
#include "commit.h"

void initBranch();
int branchExists(char* branch);
void createBranch(char* branch);
char* getCurrentBranch();
void printBranch(char* branch);
List* branchList(char* branch);
List* getAllCommits();

/* Exercice 9 – Simulation de la commande git checkout */

void restoreCommit(char* hash_commit);
void myGitCheckoutBranch(char* branch);
List* filterList(List* L,char* pattern);
void myGitCheckoutCommit(char* pattern);

#endif
