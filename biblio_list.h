#ifndef __BIBLIO_LIST_H__
#define __BIBLIO_LIST_H__


/* --------------------- PARTIE 1 --------------------- */
typedef struct cell { 
	char* data;
	struct cell* next; 
} Cell;

typedef Cell* List;


/*EX 1*/
int hashFile(char* source, char* dest);
char * sha256file(char* file);

/*EX 2*/
List* initList();
Cell* buildCell(char* ch);
void insertFirst(List *L, Cell* C);
char* ctos(Cell* c);
char* ltos(List* l);
Cell* listGet(List* L, int i);
Cell* searchList(List* L, char* str);
List* stol(char* s);
void ltof(List* L, char* path);
List* ftol(char* path);


/*EX 3*/
List* listdir(char* root_dir);
int file_exists2 (char *file);
int file_exists(char *file);
void cp(char *to, char *from);
char* hashToPath(char* hash);
void blobFile(char* file);

/* --------------------- PARTIE 2 --------------------- */


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
char* wfts(WorkFile* wf);
WorkFile* stwf(char* ch);

/* MANIPULATION DE WORKTREE */
WorkTree* initWorkTree();
int inWorkTree(WorkTree* wt, char* name);
int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode);
char* wtts(WorkTree* wt);
WorkTree* stwt(char* ch);
int wttf(WorkTree* wt, char* file);
WorkTree* ftwt(char* file);


/* Exercice 5 – Enregistrement instantané et restauration d’un WorkTree */

char * conc(char* char1, char* char2);
int isFile(const char* name);
int getChmod(const char *path);
char* blobWorkTree(WorkTree* wt);
char* saveWorkTree(WorkTree* wt, char* path);
void restoreWorkTree(WorkTree* wt, char* path);

/* --------------------- PARTIE 3 --------------------- */


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
static unsigned long sdbm(char *str);
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




#endif
