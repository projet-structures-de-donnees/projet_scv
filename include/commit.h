#ifndef __COMMIT_H__
#define __COMMIT_H__

#include "worktree.h"


/* --------------------- PARTIE 3 --------------------- */
/* Gestion des commits */

typedef struct key_value_pair{ 
	char* key;
	char* value;
}kvp;

typedef struct hash_table{ 
	kvp** T;
	int n;
	int size; 
}HashTable;

typedef HashTable Commit;

/* Exercice 6 – Fonctions de base pour les commits */
kvp* createKeyVal(char* key, char* val);
void freeKeyVal(kvp* kv);
kvp* stkv(char* str);
char* kvts(kvp* k);
Commit* initCommit();
void freeCommit(Commit* c);
unsigned long sdbm(char *str);
void commitSet(Commit* c, char* key, char* value);
Commit* createCommit(char* hash);
char* commitGet(Commit* c, char* key);
char* cts(Commit* c);
Commit* stc(char* ch);
void ctf(Commit* c, char* file);
Commit* ftc(char* file);
char* blobCommit(Commit* c);

/* Exercice 7 – Gestion temporelle des commits de manière linéire */
/*MANIPULATION DES REEFERENCES*/
void initRefs();
void createUpdateRef(char* ref_name, char* hash);
void deleteRef(char* ref_name);
char* getRef(char* ref_name);

/* SIMULATION DE LA COMMANDE GIT ADD */
void myGitAdd(char* file_or_folder);

/* SIMULATION DE LA COMMANDE GIT COMMIT */
void myGitCommit(char* branch_name, char* message);

#endif