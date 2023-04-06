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
#include "biblio_list.h"

#include <errno.h>//pour isFile
#define TAILLE_MAX_DATA 255
#define SIZE_MAX_WORKTREE 100
#define SIZE_COMMIT 255


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
	cellule->data=strdup(ch); 
	cellule->next = NULL;
	return cellule;
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


//Q : 2.4
char* ctos(Cell* c){
/* c ne doit pas etre null  */
	return c->data;
}

char* ltos(List* l){
	if(*l == NULL){
		return NULL;
	}
	char *dest=(char*)malloc(sizeof(char)*TAILLE_MAX_DATA*100);
	if(dest == NULL){
		printf("Erreur allocation\n");
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
	int j = 0;
	Cell *ptr = *L;
	while(ptr){
		if(i == j){
			return ptr;   
		}
		j++;
		ptr = ptr->next;
	}
	printf("Usage listGet: Indice trop grand, dernier indice = %d.\n",j );
	return NULL;
}

//Q : 2.6
Cell* searchList(List* L, char* str){
	if (*L == NULL){
		printf("Usage searchList : la liste en paramètre est vide\n");
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
	if (strlen(s) == 0){
		printf("Usage stol : la chaine en paramètre est vide\n");	
		return NULL;
	}
	List* l = initList();
	Cell* cell = NULL;
	char* begin = s;
	char* end = strstr(s, "|");
	int size_word;

	char* word = (char*)malloc(sizeof(char)*TAILLE_MAX_DATA);
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
	if (*L == NULL){
		printf("Usage ltof : la liste en paramètre est vide\n");
		return ;
	}
	FILE* f = fopen(path,"w");
	Cell* ptr = *L;

	while(ptr){
		fprintf(f, "%s\n",ptr->data);
		ptr = ptr->next;
	}


	fclose(f);
}

List* ftol(char* path){

	FILE* f =fopen(path,"r");
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
	return l;
}

int file_exists(char *file){
/*retourne 1 si le fichier existe dans le 
répertoire courant et 0 sinon*/
	List* l = listdir(".");
	Cell* cell = searchList(l, file);
	if(cell != NULL)return 1;
	
	return 0;

}
/*La fonction file_exists ne trouve pas les fichiers qui ne sont pas dans le
repertoire courant*/
int file_exists2 (char *file) { 
	struct stat buffer;
	return (stat (file, &buffer) == 0);
}

void cp(char *to, char *from){
/*copie le contenu d’un fichier vers un autre, en faisant 
une lecture ligne par ligne du fichier source*/	

	char buff_print[255];
	sprintf(buff_print,"cat %s",from);
	printf("%s\n",buff_print);
	system(buff_print);

	if(! file_exists2(from)){
		printf("Sortie -- 1\n");
		return;
	}
	FILE* f_from = fopen(from,"r");
	FILE* f_to = fopen(to,"w");
	if((f_from == NULL) || (f_to == NULL)){
		//printf("Usage cp : Probleme FILE*\n");
		printf("Sortie -- 2\n");
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

void blobFile2(char* file){
	char* hash = sha256file(file); 
	char* ch2 = strdup(hash); 
	ch2[2] = '\0';
if (!file_exists(ch2)) {
	char buff [100];
	sprintf(buff, "mkdir %s",ch2); 
	system(buff);
}
	char* ch = hashToPath(hash);
	cp(ch, file);
}
void blobFile(char* file){
/*enregistre un instantané du fichier donné en entrée*/
	char* hash = hashToPath(sha256file(file));
	hash[64]='\0';

	char repertoire[3];
	repertoire[0]=hash[0];
	repertoire[1]=hash[1];
	repertoire[2]='\0';
	printf("%s\n",repertoire);
	
	
	char buff[255];
	if(!file_exists(repertoire)){
		sprintf(buff,"mkdir %s",repertoire);
		system(buff);
	}

	// Permet d'enlever les espaces et le tiret qui se trouve à la fin du buff
	// Finalement geré dans hashToPath
	//hash[38]='\0';
	sprintf(buff,"touch %s/%s",repertoire,hash+3);
	system(buff);
	//sprintf(buff,"%s/%s",repertoire,hash+3);
	
	cp(hash,file);

}

/* --------------------- PARTIE 2 --------------------- */
/* Exercice 4 – Fonctions de manipulation de base */

/* MANIPULATION DE WORKFILE */
WorkFile* createWorkFile(char* name){
/* permet de créer un WorkFile et de l’initialiser */
	if (name == NULL) return NULL;
	WorkFile* wf = (WorkFile*)malloc(sizeof(WorkFile));
	wf->name = strdup(name);
	wf->hash = NULL;
	wf->mode = 0;
	return wf;
}

char* wfts(WorkFile* wf){
/* permet de convertir un WorkFile en chaîne de caractères contenant 
les différents champs séparés par des tabulations */
	if (wf == NULL) return NULL;
	char* res = (char*)malloc(sizeof(char)*255);
	sprintf(res,"%s\t%s\t%d",wf->name, wf->hash, wf->mode);
	return res;

}

WorkFile* stwf(char* ch){
/*  permet de convertir une chaîne de caractères 
représentant un WorkFile en un WorkFile. */
/* Suppose que ch n'est pas vide, c-a-d que ch != "" */
	if (ch == NULL) return NULL;		
	char name[255];
	char hash[255];
	char mode[4];
	sscanf(ch,"%s\t%s\t%s",name, hash, mode);
	WorkFile* wf = createWorkFile(name);
	wf->hash = strdup(hash);
	wf->mode = atoi(mode);
	return wf;
}

/* MANIPULATION DE WORKTREE */
WorkTree* initWorkTree(){
	WorkTree* wt = (WorkTree*)malloc(sizeof(WorkTree));
	wt->tab = (WorkFile*)malloc(sizeof(WorkFile)*SIZE_MAX_WORKTREE);
	wt->size = SIZE_MAX_WORKTREE;
	wt->n = 0;

	return wt;
}

int inWorkTree(WorkTree* wt, char* name){
	if ((wt == NULL) || (name == NULL)) return -1;

	// si il y a des élément dans le wt
	if (wt->n > 0){
		int i;
		for(i=0; i<wt->n; i++){
			if(strcmp(wt->tab[i].name,name)==0) return i;
		}
	}
	return -1;
}


int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode){
	if((wt == NULL) || (inWorkTree(wt,name) != -1)) return -1;
	if(wt->n >= wt->size) return -1;
	wt->tab[wt->n] = *createWorkFile(name);
	wt->tab[wt->n].hash = strdup(hash);
	wt->tab[wt->n].mode = mode;

	wt->n = wt->n+1;

	return 0;
}

char* wtts(WorkTree* wt){
	if(wt == NULL) return NULL;
	char* res = (char*)malloc(sizeof(char)*(100*100));
	int i; 
	for(i=0; i < wt->n; i++){
		res = strcat(res,wfts(&wt->tab[i]));
		if(i < wt->n-1) res = strcat(res,"\n");
	}
	return res;
}

//WorkTree* stwt(char* ch){
WorkTree* stwt(char* ch){

	char name[255];
	char hash[255];
	char mode[255];

	char* begin = ch;
	char* end = strchr(ch,'\n');
	int i;
	i = sscanf(begin,"%s\t%s\t%s",name, hash, mode);
	if((end == NULL) && (i != 3)){
		printf("%d\n",i);
		printf("Usage stwt : contenue pas au bon format\n");
		return NULL;	
	} 

	WorkTree* wt = initWorkTree();

	char* content = (char*)malloc(sizeof(char)*TAILLE_MAX_DATA);
	int size_content;

	while(end != NULL){
		size_content = end - begin;
		content = strncpy(content,begin,size_content);

		/*Mettre le marqueuf de fin de chaine car probleme si la prochaine chaine est plus petite que la précédente */
		content[size_content+1] = '\0';
		i = sscanf(content,"%s\t%s\t%s",name, hash, mode);

		//Si ce n'est pas le bon format
		if(i == 3){
			//printf("%s %s %d\n", name, hash, atoi(mode));
			printf("content=%s\n",content);
			printf("mode=%d  %s  %s\n",atoi(mode),hash, name);
			appendWorkTree(wt,name, hash, atoi(mode));

		}
		begin = end+1;

		//Si plsrs saut de ligne a la suite
		while(begin[0]=='\n'){
			begin ++;
		}
		end = strchr(begin,'\n');
	}

	/*Pour le dernier contenue :
	car la boucle s'arrete à l'avant derniere
	car pour le dernier contenue end vaut NULL*/

	//size_content = end - begin;

	/*begin est le dernier contenue il possède deja '\0'
	 donc on peut ommetre la taille de begin*/
	content = strcpy(content,begin);

	/*Mettre le marqueuf de fin de chaine car probleme si la prochaine chaine est plus petite que la précédente */
	//content[size_content+1] = '\0';
	i = sscanf(content,"%s\t%s\t%s",name, hash, mode);

	//Si ce n'est pas le bon format
	if(i == 3){
		//printf("%s %s %d\n", name, hash, atoi(mode));
		printf("content=%s\n",content);
		printf("mode=%d\n",atoi(mode));
		appendWorkTree(wt,name, hash, atoi(mode));

	}

	return wt;

}

int wttf(WorkTree* wt, char* file){
	if(wt == NULL) return -1;
	FILE* f = fopen(file,"w");
	if(f == NULL) return -2;

	fprintf(f,"%s\n",wtts(wt));
	fclose(f);

	return 0;
}

WorkTree* ftwt(char* file){
	FILE* f = fopen(file,"r");
	char content_buff[255];
	char name[255];
	char hash[255];
	char mode[255];
	WorkTree* wt = initWorkTree();
	while(fgets(content_buff, 255, f)){
		sscanf(content_buff,"%s\t%s\t%s",name, hash, mode);
		appendWorkTree(wt,name, hash, atoi(mode));
	}

	fclose(f);
	return wt;
}

/* Exercice 5 – Enregistrement instantané et restauration d’un WorkTree */

int getChmod(const char *path){

	struct stat ret;

	if (stat(path, &ret) == -1) {
		 return -1;
	}

	return (ret.st_mode & S_IRUSR)|(ret.st_mode & S_IWUSR)|(ret.st_mode & S_IXUSR)|/* 
	owner*/
		(ret.st_mode & S_IRGRP)|(ret.st_mode & S_IWGRP)|(ret.st_mode & S_IXGRP)|/*
			 group*/
		(ret.st_mode & S_IROTH)|(ret.st_mode & S_IWOTH)|(ret.st_mode & S_IXOTH);/*
		 other*/
}

void setMode(int mode, char* path){
	char buff[100];
	sprintf(buff,"chmod %d %s", mode, path);
	system(buff);
}

char* blobWorkTree(WorkTree* wt){

	static char template[] = "myfileXXXXXX";
	char fname[1000];
	strcpy(fname,template);
	mkstemp(fname);

	wttf(wt,fname);
	char* hash = sha256file(fname);
	char* hashPath = hashToPath(hash);
	char* hash_return = malloc(sizeof(char)*255);
	sprintf(hash_return,"%s.t",hash);


	char repertoire[3];
	repertoire[0]=hashPath[0];
	repertoire[1]=hashPath[1];
	repertoire[2]='\0';

	char buff_mkdir[255];
	if(!file_exists(repertoire)){
		sprintf(buff_mkdir,"mkdir %s",repertoire);
		system(buff_mkdir);
	}

	char chemin[255];
	sprintf(chemin,"%s.t",hashPath);
	
	char buff_touch[1500];
	sprintf(buff_touch,"touch %s",chemin);
	system(buff_touch);
	
	cp(chemin,fname);


	//char* hash = sha256file(fname);

	char buffSup[1500];
	sprintf(buffSup,"rm %s",fname);
	system(buffSup);


	return hash_return;
}




/*int isFile(char * path){
	DIR* dp = opendir(path);
	struct dirent *ep;
	ep=readdir(dp);
	printf("%s\n",ep->d_name);
	// Si c'est un fichier
	if (ep->d_type==8 && (strcmp(path,ep->d_name)==0)){
	//printf("%hhu\t",ep->d_type);
	//printf("%s\n",ep->d_name);
		return 1;
	}
	return 0;
				
}*/


int isFile(const char* name)
{
    DIR* directory = opendir(name);

    if(directory != NULL)
    {
     closedir(directory);
     return 0;//si c'est un directory
    }

    if(errno == ENOTDIR)
    {
     return 1;//si c'est un fichier
    }

    return -1;//si le fichier ou le directory n'existe pas
}

char * conc(char* char1, char* char2){
	char *  res = malloc(sizeof(char)*strlen(char1)+strlen(char2)+2);
	sprintf(res,"%s/%s",char1,char2);
	return res;
}	

char* saveWorkTree(WorkTree* wt, char* path){
	if(wt == NULL || wt->n<=0 ){ // Si pas d'élément dans tab
		return NULL;
	}
	//system("pwd");
	//printf("path =%s\n",path);
	//printf("DEBUT WT =%s\n",wtts(wt));
	int i;
	printf("n =%d\n",wt->n);
	for(i=0; i<wt->n; i++){
		printf("i =%d\n",i);
		printf("nom courant =%s\n",wt->tab[i].name );
		char* hash=malloc(sizeof(char)*255);
		int mode;
		if (isFile(conc(path,wt->tab[i].name))==1){
			blobFile(conc(path,wt->tab[i].name));
			hash=sha256file(conc(path,wt->tab[i].name));
			mode=getChmod(conc(path,wt->tab[i].name));
			wt->tab[i].mode=mode;
			wt->tab[i].hash=hash;
			printf("name %s \nmode %d \n hash %s \n",wt->tab[i].name,wt->tab[i].mode,wt->tab[i].hash);
		}else{
			WorkTree* newWT=initWorkTree();
			List *L=listdir(conc(path,wt->tab[i].name));
			Cell *ptr = *L;
			int k = 0;
			while(ptr!= NULL){
				if(strncmp(ptr->data,".",1) != 0){
					printf("%s\n",ptr->data);
					appendWorkTree(newWT,ptr->data,"lalal",0);
				}
				ptr=ptr->next;
			}
			wt->tab[i].hash = saveWorkTree(newWT,conc(path,wt->tab[i].name));
			wt->tab[i].mode = getChmod(conc(path,wt->tab[i].name));
		}
	}
		return blobWorkTree(wt) ;
}
void restoreWorkTree(WorkTree* wt, char* path){
	if(wt == NULL || wt->n<=0 ){ // Si pas d'élément dans tab
		return ;
	}

	WorkFile* wf_current = NULL;
	int i;
	for (i = 0; i < wt->n; i++){
		wf_current = &(wt->tab[i]);
		int nb_char = strlen(wf_current->hash);
		if(wf_current->hash[nb_char-2] != '.'){// S'il sagit d'un fichier
			cp(conc(path,wf_current->name),hashToPath(wf_current->hash));
			setMode(wf_current->mode,conc(path,wf_current->name));

		}else{
			WorkTree* newWT = ftwt(hashToPath(wf_current->hash));
			restoreWorkTree(newWT,conc(path,wf_current->name));
		}

	}
}
/* --------------------- PARTIE 3 --------------------- */
kvp* createKeyVal(char* key, char* val){
/* permettant d’allouer et d’initialiser un  élément. */	
	kvp *keyVal = (kvp*)malloc(sizeof(kvp));
	if (keyVal == NULL){
		printf("Erreur allocation kvp : \n%s %s",key,val);
		return NULL;
	}
	
	keyVal->key=strdup(key);
	keyVal->value=strdup(val);
	return keyVal;
}

void freeKeyVal(kvp* kv){
/* permettant de libérer la mémoire associé à un  élément. */
	if(kv == NULL){
		return ;
	}
	free(kv->key);
	free(kv->value);
	free(kv);
}

char* kvts(kvp* k){
/* permet de convertir un élément en une chaine de 
caractères de la forme "clé :valeur" */
	if(k == NULL){
		return NULL;
	}
	char *chaine = (char *)malloc(sizeof(char)*255);
	sprintf(chaine,"%s :%s",k->key,k->value);
	return chaine;
}

kvp* stkv(char* str){
	if(str == NULL){
		return NULL;
	}
	char key[255];
	char value[255];
	sscanf(str,"%s :%s",key, value);
	kvp *k = createKeyVal(key,value);
	return k;
}

Commit* initCommit(){
	Commit *commit = (Commit*)malloc(sizeof(Commit));
	if(commit == NULL){
		printf("Erreur allocation Commit");
		return NULL;
	}
	commit->size = SIZE_COMMIT;
	commit->n = 0;
	commit->T = (kvp**)malloc(sizeof(kvp*)*commit->size);
	return commit;
}

//static unsigned long sdbm(unsigned char *str)
static unsigned long
sdbm(str)
char *str;
{
    unsigned long hash = 0;
    int c;
    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

   return hash;
}
void commitSet(Commit* c, char* key, char* value){


}