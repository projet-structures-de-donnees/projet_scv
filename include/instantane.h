#ifndef __INSTANTANE_H__
#define __INSTANTANE_H__


/* --------------------- PARTIE 1 --------------------- */
/* Vers la création d’enregistrements instantanés */

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
void freeCell(Cell* c);
void insertFirst(List *L, Cell* C);
void freeList(List *L);
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

#endif