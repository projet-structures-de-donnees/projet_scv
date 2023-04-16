#ifndef __WORKTREE_H__
#define __WORKTREE_H__
#include "instantane.h"

/* --------------------- PARTIE 2 --------------------- */
/* Enregistrement de plusieurs instantanés */
typedef struct { 
	char* name;
	char* hash;
	int mode; 
} WorkFile;

typedef struct { 
	WorkFile* tab;
	int size;
	int n;
} WorkTree;


/* Exercice 4 – Fonctions de manipulation de base */
/* MANIPULATION DE WORKFILE */
WorkFile* createWorkFile(char* name);
void freeWorkFile(WorkFile *wf);
char* wfts(WorkFile* wf);
WorkFile* stwf(char* ch);

/* MANIPULATION DE WORKTREE */
WorkTree* initWorkTree();
void freeWorkTree(WorkTree* wt);
int inWorkTree(WorkTree* wt, char* name);
int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode);
char* wtts(WorkTree* wt);
WorkTree* stwt(char* ch);
int wttf(WorkTree* wt, char* file);
WorkTree* ftwt(char* file);


/* Exercice 5 – Enregistrement instantané et restauration d’un WorkTree */

char * conc(char* char1, char* char2);
int isFile(const char* name);// rep->0; file->1; sinon -1
int getChmod(const char *path);
char* blobWorkTree(WorkTree* wt);
char* saveWorkTree(WorkTree* wt, char* path);
void restoreWorkTree(WorkTree* wt, char* path);


#endif
