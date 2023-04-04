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

int getChmod(const char *path);
char* blobWorkTree(WorkTree* wt);
char* saveWorkTree(WorkTree* wt, char* path);

#endif