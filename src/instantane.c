#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#include <fcntl.h> // pour la fonction getChmod
#include <sys/types.h> // pour la fonction getChmod
#include <sys/stat.h> // pour la fonction getChmod
#include "instantane.h"

#include <errno.h>//pour isFile
#define TAILLE_MAX_DATA 255
#define SIZE_MAX_WORKTREE 100
#define SIZE_COMMIT 10

/* --------------------- PARTIE 1 --------------------- */
/* Vers la création d’enregistrements instantanés */

/*Exercice 1*/
int hashFile(char* source, char* dest){
	char buff[255];
	//sprintf(buff,"cat %s |  sha256sum >> %s",source,dest);
	/* Utilisation mac book : J'ai mis en commentaire la ligne du haut car sa ne marche pas sur mon mac */
	sprintf(buff,"cat %s |  shasum > %s",source,dest);
	return system(buff);

}

char * sha256file(char* file){
	static char template[]="myfileXXXXXX";
	char fname[1000];
	strcpy(fname,template);
	mkstemp(fname);
	 
	hashFile(file,fname);
	FILE * f = fopen(fname,"r");
	char *buff;
	// Ne pas oublier de free buff
	buff = (char*)malloc(sizeof(char)*255);
	fgets(buff,255,f);
	fclose(f);
	
	char buffSup[1500];
	sprintf(buffSup,"rm %s",fname);
	system(buffSup);

	// Permet d'enlever les espaces et le tiret qui se trouve à la fin du buff
	buff[40]='\0';
	return buff;

}


/*Exercice 2*/

List* initList(){
	List* l = malloc(sizeof(List));
	*l = NULL;
	return l;
}

Cell* buildCell(char* ch){
	if(ch == NULL){
		return NULL;
	}
	Cell* cellule = (Cell*)malloc(sizeof(Cell));
	if(cellule == NULL){
		return NULL;
	}
	cellule->data=strdup(ch); 
	cellule->next = NULL;
	return cellule;
}

void freeCell(Cell* c){
	if(c == NULL){
		return ;
	}
	free(c->data);
	free(c);
}


void insertFirst(List *L, Cell* C){
	//printf("insertfirst L=%p\n",L);
	//printf("insertfirst *L=%p\n",*L);
	if (L == NULL) return ;
	if (C == NULL) return ;
	C->next = *L;
	*L = C;	
	//printf("insertfirst=%p\n",C->next);
}

void freeList(List *L){
	if(*L == NULL){
		return;
	}
	Cell *cell = *L;
	while(cell != NULL){
		*L = (*L)->next;
		free(cell);
		cell = *L;
	}
	free(L);
}


//Q : 2.4
char* ctos(Cell* c){
/* c ne doit pas etre null  */
	if(c == NULL){
		return NULL;
	}
	return c->data;
}

char* ltos(List* l){
	if(*l == NULL){
		return NULL;
	}
	char *dest=(char*)malloc(sizeof(char)*TAILLE_MAX_DATA*100);
	if(dest == NULL){
		printf("Erreur allocation\n");
		return NULL;
	}

	Cell* ptr = *l;
	while(ptr){
		dest = strcat(dest,ptr->data);
		if(ptr->next){
			dest = strcat(dest,"|");	
		}
		ptr = ptr->next;
		//printf("%p\n",ptr);
		//printf("dest=%s\n",dest);

	}
	return dest;
}

//Q : 2.5
Cell* listGet(List* L, int i){
	if(*L == NULL){
		return NULL;
	}
	int j = 0;
	Cell *ptr = *L;
	while(ptr){
		//printf("lisGet j =%d\n",j);
		if(i == j){
			return ptr;   
		}
		j++;
		ptr = ptr->next;
	}
	//printf("Usage listGet: Indice trop grand, dernier indice = %d.\n",j );
	return NULL;
}

//Q : 2.6
Cell* searchList(List* L, char* str){
	if (*L == NULL || str == NULL){
		return NULL;
	}

	Cell *ptr = *L;
	while(ptr){
		if (strcmp(str,ptr->data)==0){
			return ptr;
		}
		ptr = ptr->next;
	}
	//printf("contenue non existant\n");
	return NULL;
} 

//Q : 2.7
List* stol(char* s){
/*Permet de transformer une chaîne de caractères repréesentant une liste en une liste chaînée.*/
	if (s == NULL){
		//printf("Usage stol : la chaine en paramètre est vide\n");	
		return NULL;
	}
	List* l = initList();
	Cell* cell = NULL;
	char* begin = s;
	char* end = strstr(s, "|");
	int size_word;

	char* word = (char*)malloc(sizeof(char)*TAILLE_MAX_DATA);
	if(word == NULL){
		return NULL;
	}
	while(end){
		size_word = strlen(begin)-strlen(end); 
		if(size_word){
			strncpy(word , begin, size_word);
			word[size_word]='\0';
			cell = buildCell(word);
			insertFirst(l, cell);
			}

		begin = end + 1;
		end = strstr(begin, "|");

	}
	//Pour la derniere sous chaine :
	//car la boucle s'arrete à l'avant derniere
	//car pour la derniere sous chaine end vaut NULL
	size_word = strlen(begin); 
	if(size_word){

		strncpy(word , begin, size_word);
		word[size_word]='\0';
		cell = buildCell(word);
		insertFirst(l, cell);
	}

	free(word);
	return l;
}

void ltof(List* L, char* path){
/*Permet d’écrire une liste dans un fichier*/
	if (*L == NULL || path == NULL){
		return ;
	}
	FILE* f = fopen(path,"w");
	if(f == NULL){
		return ;
	}
	Cell* ptr = *L;
	while(ptr){
		fprintf(f, "%s\n",ptr->data);
		ptr = ptr->next;
	}
	fclose(f);
}

List* ftol(char* path){
	if(path == NULL){
		return NULL;
	}
	FILE* f =fopen(path,"r");
	if(f == NULL){
		return NULL;
	}
	List* l = initList();
	Cell* cell = NULL;

	char s[255]; 
	while(fgets(s,256,f)){
		s[strlen(s)-1]='\0';
		cell = buildCell(s);
		insertFirst(l,cell);
		//printf("ftol=%p\n",cell->next);
	}

	fclose(f);
	return l;
} 

/*Exercice 3: Gestion de fichiers sous git*/

List* listdir(char* root_dir){
/*prend en paramètre une adresse et renvoie une liste contenant 
le noms des fichiers et répertoires qui s’y trouvent*/
	DIR* dp = opendir(root_dir);
	if(dp == NULL){
		return NULL;
	}
	struct dirent *ep;
	List* l = initList();

	if(dp != NULL){
		Cell* cell = NULL;
		while((ep=readdir(dp)) != NULL){
			//printf("%hhu\t",ep->d_type);
			//printf("%d\t",ep->d_type==8); 8=file
			cell = buildCell(ep->d_name);
			insertFirst(l,cell);
		}
	}
	closedir(dp);
	return l;
}

int file_exists(char *file){
/*retourne 1 si le fichier existe dans le 
répertoire courant et 0 sinon*/
	FILE* f = fopen(file,"r");
	if(f != NULL){
		fclose(f);
		return 1;
	}
	return 0;
}

void cp(char *to, char *from){
/*copie le contenu d’un fichier vers un autre, en faisant 
une lecture ligne par ligne du fichier source*/	
	if((to == NULL)||(from == NULL)){
		return;
	}
	char buff_print[255];
	sprintf(buff_print,"cat %s",from);
	//printf("%s\n",buff_print);
	system(buff_print);

	if(! file_exists(from)){
		return;
	}
	FILE* f_from = fopen(from,"r");
	FILE* f_to = fopen(to,"w");
	if((f_from == NULL) || (f_to == NULL)){
		//printf("Usage cp : Probleme FILE*\n");
		return ;
	}
	char buffer[256];

	while(fgets(buffer,255,f_from)){
		fprintf(f_to, "%s",buffer);
	}
	fclose(f_from);
	fclose(f_to);
}

char* hashToPath(char* hash){
/*retourne le chemin d’un fichier à partir de son hash*/
	if(strlen(hash) < 3){
		//printf("Usage hashToPath : Le hash en entré n'est aps assez long (3 caractere minimum)\n");
		return NULL;
	}

	char* buffer_exit=(char*)malloc(sizeof(char)*255);
	int i=0;
	buffer_exit[0]=hash[0];
	buffer_exit[1]=hash[1];
	buffer_exit[2]='/';

	for(i=2; hash[i]!= '\0'; i++){	
		buffer_exit[i+1]=hash[i];
	}
	return buffer_exit;
}

void blobFile(char* file){
/*enregistre un instantané du fichier donné en entrée*/
	char* hash = hashToPath(sha256file(file));
	hash[64]='\0';

	char repertoire[3];
	repertoire[0]=hash[0];
	repertoire[1]=hash[1];
	repertoire[2]='\0';
	//printf("%s\n",repertoire);
	
	char buff[255];
	// Sinon on ne pourra pas utiliser touch 
	if(!file_exists(repertoire)){
		sprintf(buff,"mkdir %s",repertoire);
		system(buff);
	}
	sprintf(buff,"touch %s/%s",repertoire,hash+3);
	system(buff);
	//sprintf(buff,"%s/%s",repertoire,hash+3);
	
	cp(hash,file);

}

