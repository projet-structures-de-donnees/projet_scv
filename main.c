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
#define SIZE_COMMIT 10


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

int file_exists2 (char *file) { 
	struct stat buffer;
	return (stat (file, &buffer) == 0);
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

	if(! file_exists2(from)){
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
	wt->tab[wt->n].hash = strdup(hash);/* Gérez si on entre un hash null ?? */
	wt->tab[wt->n].mode = mode;
	//printf("Ajout dans le wt =%s %s %d\n",wt->tab[wt->n].name,wt->tab[wt->n].hash,wt->tab[wt->n].mode);


	wt->n = wt->n+1;

	return 1;
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
		//printf("%d\n",i);
		//printf("Usage stwt : contenue pas au bon format\n");
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
			//printf("content=%s\n",content);
			//printf("mode=%d  %s  %s\n",atoi(mode),hash, name);
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

	/*begin est le dernier contenue il possède deja '\0'
	 donc on peut ommetre la taille de begin*/
	content = strcpy(content,begin);

	/*Mettre le marqueuf de fin de chaine car probleme si la prochaine chaine est plus petite que la précédente */
	//content[size_content+1] = '\0';
	i = sscanf(content,"%s\t%s\t%s",name, hash, mode);

	//Si ce n'est pas le bon format
	if(i == 3){
		//printf("%s %s %d\n", name, hash, atoi(mode));
		//printf("content=%s\n",content);
		//printf("mode=%d\n",atoi(mode));
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
	if (f == NULL){
		return NULL;
	}
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
	sprintf(buff,"chmod %o %s", mode, path);
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

    if(directory != NULL){
     closedir(directory);
     return 0;//si c'est un directory
    }

    if(errno == ENOTDIR){
     return 1;//si c'est un fichier
    }

    return -1;//si le fichier ou le directory n'existe pas
}

char * conc(char* char1, char* char2){
	char *  res = malloc(sizeof(char)*strlen(char1)+strlen(char2)+2);
	if(res == NULL){
		return NULL;
	}
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
	//printf("n =%d\n",wt->n);
	for(i=0; i<wt->n; i++){
		//printf("i =%d\n",i);
		//printf("nom courant =%s\n",wt->tab[i].name );
		char* hash=malloc(sizeof(char)*255);
		int mode;
		if (isFile(conc(path,wt->tab[i].name))==1){
			blobFile(conc(path,wt->tab[i].name));
			hash=sha256file(conc(path,wt->tab[i].name));
			mode=getChmod(conc(path,wt->tab[i].name));
			wt->tab[i].mode=mode;
			wt->tab[i].hash=hash;
			//printf("name %s \nmode %d \n hash %s \n",wt->tab[i].name,wt->tab[i].mode,wt->tab[i].hash);
		}else{
			WorkTree* newWT=initWorkTree();
			List *L=listdir(conc(path,wt->tab[i].name));
			Cell *ptr = *L;
			while(ptr!= NULL){
				if(strncmp(ptr->data,".",1) != 0){
					//printf("%s\n",ptr->data);
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
	//printf("%s -- %s\n",key,val);
	keyVal->key=strdup(key);
	keyVal->value=strdup(val);
	//printf("%s\n",kvts(keyVal));
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
	//printf("stkv =%s\t",str);
	if (sscanf(str,"%s :%s",key, value) != 2){
		//printf("NON VALIDE\n");
		return NULL;
	}
	//("VALIDE\n");
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
	int i;
	for (i=0; i<commit->n; i++){
		commit->T[i] = NULL;
	}
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
	if (c == NULL){
		printf("commitSet :parm c == NULL\n");
		return;
	}
	if(c->n == c->size){//tableau remplie
		printf("commitSet :REMPLI\n");
		return;
	}
	int nb_commit = c->n;
	kvp* k = createKeyVal(key, value);
	unsigned long hash = sdbm(key);
	unsigned long probing_line;
	int i = 0;
	while(nb_commit == c->n){
		probing_line = (hash + i)%c->size ;
		//printf("i =%d\n",i);
		//printf("hash + %lu modulo %d=%lu\n",i,c->size,(hash + i)%c->size);
		// Si la case et vide -> on insert
		if(c->T[probing_line] == NULL ){
			c->T[probing_line] = k;
			//printf("CommitSet =%s\n",kvts(c->T[(hash + i)%c->size]));
			//printf("A la position =%lu\n",(hash + i)%c->size);
			c->n++;
		}else{
			// Si la case n'est pas vide mais qu'ils ont la meme clé, on met a jours. Mais on augmente pas le nombre d'éléments du tableau
			if (strcmp((c->T[probing_line])->key,key)==0){
				c->T[probing_line] = k;
				return ;
			}
		}
		i++;
	}

	/*for (int i = 0; i<c->size; i++){
		printf("%s\n",kvts(c->T[i]));
	}*/

}
Commit *createCommit(char* hash){
	Commit *c = initCommit();
	if(c == NULL){
		return NULL;
	}
	commitSet(c, "tree",hash);
	return c;
}
char* commitGet(Commit* c, char* key){
	if(c == NULL || key == NULL){
		return NULL;
	}
	unsigned long hash = sdbm(key);
	unsigned long probing_line;
	int i = 0;
	while (i < c->size){
		probing_line = (hash + i)%c->size ;
		//printf("pbl =%lu\n", probing_line);
		if(c->T[probing_line] != NULL){ //Si c'est NULL (DESSUS PENDANT 30min depuis fct printBranch)
			if(strcmp(c->T[probing_line]->key, key) == 0){
				return c->T[probing_line]->value;
			}
		}
		if (c->T[probing_line] == NULL){
			return NULL;
		}
		i++;
	}
	return NULL;
}

char* cts(Commit* c){
	if(c == NULL){
		return NULL;
	}
	int nb_recup = 0;
	char *buff = (char *)malloc(sizeof(char)*255*c->n);
	int i = 0;
	// Tant qu'on a pas parcouru toute la table ou que nous avons tous recupéré
	while(i<c->size || (nb_recup < c->n)){
		if(c->T[i] != NULL){
			strcat(buff,kvts(c->T[i]));
			strcat(buff,"\n");
			nb_recup ++;
		}
		i++;
	}
	return buff;
}

Commit* stc(char* ch){
	if(ch == NULL){
		return NULL;
	}
	Commit *commit = initCommit();
	char* begin = ch;
	char* end = strstr(ch,"\n");
	char* buff_word = malloc(sizeof(char)*500);
	int size_word;
	kvp* kv = NULL;
	while(end != NULL){
		size_word = strlen(begin)-strlen(end); 
		strncpy(buff_word,begin,size_word);
		buff_word[size_word]='\0';// Si la nouvelle chaine recupéré est plus petite que la précédente
		//printf("buff_word =%s\n",buff_word);
		kv = stkv(buff_word);
		if(kv != NULL){// Si buff_word est dans le bon format 
			commitSet(commit,kv->key, kv->value);
		}
		begin = end+1;
		end = strstr(begin,"\n");
	}
	//Pour la fin de la chaine
	kv = stkv(begin);
		if(kv != NULL){
			commitSet(commit,kv->key, kv->value);
		}
	return commit;
}

void ctf(Commit* c, char* file){
	if (c == NULL){
		return ;
	}
	FILE *f = fopen(file,"w");
	if(f == NULL){
		return ;
	}
	int i = 0;
	int trouve = 0;
	// On s'arrete si on a finit de parcourirs la table ou si on a trouvé toutes les valeurs
	while(i < c->size || trouve < c->n){
		if (c->T[i] != NULL){
			fprintf(f,"%s :%s\n",c->T[i]->key,c->T[i]->value);
			//printf("%d\n",i);
			trouve ++;
		}
		i++;
	}
	fclose(f);
}


Commit* ftc(char* file){
	if(file == NULL){
		return NULL;
	}
	FILE* f = fopen(file,"r");
	if(f == NULL){
		if(errno == ENOENT){
			//printf("ftc: Le fichier %s n'existe pas !! \n",file);
			return NULL;
		}else{
			//printf("ftc: autre erreur fopen\n");
			return NULL;
		}
	}
	Commit *c = initCommit();
	char key[255];
	char value[255];
	char buff[255];
	while(fgets(buff,255,f) != NULL){
		if (sscanf(buff,"%s :%s",key,value) == 2){
		commitSet(c,key,value);
		}
	}
	fclose(f);
	return c;
}

char* blobCommit(Commit* c){
	//reprise du code de blobWorktree, façon Commit
	if(c == NULL){
		return NULL;
	}

	static char template[] = "myfileXXXXXX";
	char fname[1000];
	strcpy(fname,template);
	mkstemp(fname);

	ctf(c,fname);
	char* hash = sha256file(fname);
	char* hashPath = hashToPath(hash);
	char* hash_return = malloc(sizeof(char)*255);
	sprintf(hash_return,"%s.c",hash);


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
	sprintf(chemin,"%s.c",hashPath);
	
	char buff_touch[1500];
	sprintf(buff_touch,"touch %s",chemin);
	system(buff_touch);
	
	cp(chemin,fname);

	char buffSup[1500];
	sprintf(buffSup,"rm %s",fname);
	system(buffSup);

	return hash_return;

}

/* Exercice 7 – Gestion temporelle des commits de manière linéaire */
/*MANIPULATION DES REEFERENCES*/

void initRefs(){
	//printf("ENTER initRefs\n");

/*cree le repertoire cache .refs (s’il n’existe pas
deja), puis cree les fichiers master et HEAD (vides)*/
	DIR * dir = opendir(".refs");
	if(dir == NULL){
		system("mkdir .refs");
		system("touch .refs/master");
		system("touch .refs/HEAD");
		return;
	}
	system("touch .refs/master");
	system("touch .refs/HEAD");
	closedir(dir);
}

void createUpdateRef(char* ref_name, char* hash){
	if((ref_name == NULL) || (hash == NULL)){
		return;
	}
	//printf("ENTER createUpdateRef\n");
	initRefs();
	// Permet de rentrer dans le repertoire .refs
	char buff_path_to_ref[255];
	sprintf(buff_path_to_ref,".refs/%s",ref_name);
	FILE* f = fopen(buff_path_to_ref,"w");
	if(f == NULL){
		return;
	}
	//Ne pas mettre de saut de ligne sinon, probleme pour ouvrir le fichier dans les fonction d'apres(ex : printBranch)
	fprintf(f, "%s", hash);
	fclose(f);
}

void deleteRef(char* ref_name){

	// Permet de rentrer dans le repertoire .refs
	char buff_path_to_ref[255];
	sprintf(buff_path_to_ref,".refs/%s",ref_name);
	FILE* f = fopen(buff_path_to_ref,"r");

	// Si le file n'existe pas 
	if (f == NULL){
		return ;
	}
	if(remove(buff_path_to_ref) == 0){
		//printf("%s a ete supp !!! \n",ref_name);
	}else{
		//printf("%s n'a pas ete supp !!! \n",ref_name);
		fclose(f);
	}
	return;
}

char* getRef(char* ref_name){
	//printf("ENTER getRef\n");

	// Permet de rentrer dans le repertoire .refs
	char buff_path_to_ref[255];
	sprintf(buff_path_to_ref,".refs/%s",ref_name);
	FILE* f = fopen(buff_path_to_ref,"r");
	//printf("PATH =%s\n",buff_path_to_ref);
	if(f == NULL){
		return NULL;
	}
	char *buff_hash = (char*)malloc(sizeof(char)*255);
	if(fgets(buff_hash,200,f) == NULL){
		char* buff_vide = malloc(sizeof(char)*2);
		sprintf(buff_vide," ");
		fclose(f);
		return buff_vide;
	}
	//printf("%s",buff_hash);
	fclose(f);
	return buff_hash;
}

/* SIMULATION DE LA COMMANDE GIT ADD */
void myGitAdd(char* file_or_folder){
	if(file_or_folder == NULL){
		return;
	}
	FILE* f = fopen(".add","r");
	WorkTree* wt;
	if (f == NULL) {
		//printf(".add existe pas, donc on va le créer\n");
        if(errno == ENOENT){// Si Le fichier n'existe pas
			f = fopen(".add","w"); // On le créer
			if(f != NULL){
				//printf(".add a été créer\n");
				wt = initWorkTree();
				appendWorkTree(wt,file_or_folder,"hash_test",0);
				fclose(f);
				wttf(wt, ".add");
			}
			return;
		}else{ //Erreur fopen
			return;
        }
    }else{
		wt = ftwt(".add");
		appendWorkTree(wt,file_or_folder,"hash_test",0);
        fclose(f);
		wttf(wt, ".add");
    }
}
/* SIMULATION DE LA COMMANDE GIT COMMIT */
void myGitCommit(char* branch_name, char* message){
	DIR* dir = opendir(".refs");
	if(dir == NULL){ 
		printf("Initialiser d'abord les références du projet\n");
		return;
	}
	char buff_branch[255];
	sprintf(buff_branch,".refs/%s",branch_name);
	FILE* f = fopen(buff_branch,"r");
	if(f == NULL){
		printf("La branche n’existe pas\n");
		return;
	}
	if(strcmp(getRef("HEAD"),getRef(branch_name)) != 0){
		printf("HEAD doit pointer sur le dernier commit de la branche\n");
		return;
	}
	fclose(f);
	WorkTree* wt = ftwt(".add");
	if(wt == NULL){
		return;
	}
	if(remove(".add") == 0){
		//printf(".add a ete supprimé !!! \n");
	}else{
		//printf(".add n'a pas ete supprimé !!! \n");
		return ;
	}
	char* hash_wt = saveWorkTree(wt, ".");
	if(hash_wt == NULL){
		return ;
	}
	Commit *c = createCommit(hash_wt);
	char *predecessor = getRef(branch_name);
	if(strcmp(predecessor," ") != 0){
		commitSet(c, "predecessor", predecessor);
	}
	if(message != NULL){
		commitSet(c, "message", message);
	}
	char *hash_commit = blobCommit(c);
	createUpdateRef(branch_name, hash_commit);
	createUpdateRef("HEAD", hash_commit);
}

/* --------------------- PARTIE 4 --------------------- */
/* Gestion d’une timeline arborescente */

/* Exercice 8 – Fonctions de base de manipulation des branches */

void initBranch(){
/*cree le fichier cache .current_branch contenant
la chaine de caracteres master*/
	FILE* f = fopen(".current_branch","w");
	if(f == NULL){
		return;
	}
	fprintf(f,"master");
	fclose(f);
}

int branchExists(char* branch){
/*verifie l’existence d’une branche*/
	char path_branch[255];
	sprintf(path_branch,".refs/%s",branch);
	return file_exists2(path_branch);
}

void createBranch(char* branch){
/*cree une reference qui pointe vers le meme commit que la reference HEAD*/
	createUpdateRef(branch, getRef("HEAD"));
}

char* getCurrentBranch(){
	//printf("ENTER getCurrentBranch\n");

/*lit le fichier cache .current_branch pour retourner le nom de la branche courante*/
	FILE* f = fopen(".current_branch","r");
	if(f == NULL){
		return NULL;
	}
	char *name_curr_branch = malloc(sizeof(char)*255); 
	if(fgets(name_curr_branch,255,f) == NULL){
		return NULL;
	}
	fclose(f);
	return name_curr_branch;
}

void printBranch(char* branch){
/*parcourt la branche appelee branch, et pour chacun de ses commits,
 affiche son hash et son message descriptif (s’il en existe un).*/
	if(branch == NULL || !branchExists(branch)){
		return ;
	}
	printf("\n");
	char* hash_commit = getRef(branch);
	Commit *c = NULL;
	char* message_c;
	char* predecessor_c;
	do{
		c = ftc(hashToPath(hash_commit));
		if(c == NULL){
			return;
		}
		message_c = commitGet(c, "message");
		if(message_c != NULL){
			printf("commit %s\nmessage\t%s\n\n",hash_commit,message_c);
		}else{
			printf("commit %s\n\n",hash_commit);
		}
		predecessor_c = commitGet(c, "predecessor");

		/*if(predecessor_c == NULL){
			printf("PAS DE PREDECESSOR\n");
		}else{
			printf("predecessor =%s\n",predecessor_c);
		}*/

		hash_commit = predecessor_c; 
	}while(predecessor_c != NULL);
}

List* branchList(char* branch){
	if(branch == NULL){
		return NULL;
	}

	char* hash_commit = getRef(branch);

	if((hash_commit == NULL) || (strcmp(hash_commit," ") == 0)){ //file non existant ou pas de commit sur la branch
		//printf("NULL\n");
		return NULL;
	}


	Commit *c = NULL;
	char* predecessor_c;
	List *list_hash = initList();
	do{
		c = ftc(hashToPath(hash_commit));
		if(c == NULL){
			return NULL;
		}
		predecessor_c = commitGet(c, "predecessor");
		insertFirst(list_hash, buildCell(hash_commit));
		hash_commit = predecessor_c; 
	}while(predecessor_c != NULL);
	return list_hash;
}

List* getAllCommits(){
	
	List* list_branch = listdir(".refs");
	List* list_tmp = initList(); //Liste commit de la branche courante
	List* list_hash = initList(); //Liste a retournée
	Cell* tmp = NULL;
	int i = 0;
	Cell* cell_branch = listGet(list_branch, 0);
	Cell* cell_tmp = NULL;
	while(cell_branch != NULL){// Tant qu'on a des branches
		//printf("%s\n",cell_branch->data);
		if(cell_branch->data[0] != '.'){
			list_tmp = branchList(cell_branch->data);// on stock les commit de la branche 
			//printf("%s\n",ltos(list_tmp));
			int j = 0;
			cell_tmp = listGet(list_tmp, 0);
			//printf("%s\n",ltos(list_tmp));
			//printf("%s\n",cell_tmp->data);
			while(cell_tmp != NULL){
				if(searchList(list_hash,cell_tmp->data) == NULL){
					//printf("%s\n",ltos(list_tmp));
					//printf("INSERTION DE =%s\n",cell_tmp->data);
					insertFirst(list_hash,buildCell(cell_tmp->data));// On duplique la cellule dans la liste final
					//printf("\n\nRESULTAT FINAL  =%s\n\n",ltos(list_hash));
					//printf("%s\n",ltos(list_tmp));

				}
				j++;
				//printf("%d\n",j);
				//printf("%s\n",ltos(list_tmp));
				cell_tmp = listGet(list_tmp,j);
				if(cell_tmp != NULL){
					//printf("%s\n",cell_tmp->data);
				}
				
			}
		}
		i++;
		cell_branch = listGet(list_branch, i);
	}
	printf("\n\n\n");
	return list_hash;
}

/* Exercice 9 – Simulation de la commande git checkout */

void restoreCommit(char*hash_commit){
	//2eme partie du ou debug de 1h
	if((hash_commit == NULL) || strcmp(hash_commit," ") == 0){//Si NULL ou si pas de commit
		return ;
	}

	char* cur_branch = getCurrentBranch();

	List* list_commit = branchList(cur_branch);

	Cell* commit = searchList(list_commit,hash_commit);
	//printf("%s\n",commit->data);
	if(commit == NULL){
		return;
	}

	char* path_commit = hashToPath(commit->data); //commit->data  avec le point c (a creer commit_to_path)
	WorkTree* wt_commit = ftwt(hashToPath(commitGet(ftc(path_commit), "tree")));
	restoreWorkTree(wt_commit,".");
}

void myGitCheckoutBranch(char* branch){
	if(branch == NULL){
		return;
	}

	FILE* f = fopen(".current_branch","w");
	if(f == NULL){
		return;
	}
	fprintf(f,"%s",branch);
	fclose(f);

	createUpdateRef("HEAD",getRef(branch));
	char* ref = getRef("HEAD");
	restoreCommit(ref);

}

List* filterList(List* L,char* pattern){
	if((pattern==NULL) || (L==NULL)){
		return NULL;
	}
	int size = strlen(pattern);
	List* res = initList();
	Cell * cell = *L;
	while(cell != NULL){
		if(strncmp(cell->data,pattern,size)==0){
			insertFirst(res,cell);
			
		}
		cell=cell->next;
	}
	return res;
}

void myGitCheckoutCommit(char* pattern){

	if(pattern == NULL){
		return ;
	}

	List* list_commit=filterList(branchList(getCurrentBranch()),pattern);
	if(*list_commit == NULL){
		printf("Commit non existant\n");
		return;
	}
	if(listGet(list_commit, 1) != NULL){
		printf("Précisez la requête du hash\n");
		return;
	}

	createUpdateRef("HEAD",(*list_commit)->data);
	char* ref = getRef("HEAD");
	restoreCommit(ref);
}

WorkTree* mergeWorkTrees(WorkTree* wt1, WorkTree* wt2 , List** conflicts){

	if(wt1==NULL || wt2==NULL || **conflicts==NULL){
		return NULL;
	}
	if(wt1->n==0){
		if (wt2->n==0){
			return initWorkTree();
		}
		return stwt(wtts(wt2));// duplique wt2 et le retourne
	}

	WorkTree *wt_merge = initWorkTree();
	WorkFile* current_wf = NULL;
	int i;
	int pos = 0;

	for(i=0; i<=wt1->n; i++){
		current_wf = &(wt1->tab[i]);
		pos = inWorkTree(wt2,current_wf->name);
		if(pos != -1){ // Si le le fichier/reperoire et dans les deux wt
			printf("meme nom =%s\n",current_wf->name);
			if(strcmp(current_wf->hash,wt2->tab[pos].hash) == 0){ //Si pas de conflits
				if(appendWorkTree(wt_merge,current_wf->name,current_wf->hash,current_wf->mode) != 1){
					printf("merge: erreur appendWt\n");
					return NULL;
				}else{

				}
			}else{ // Si conflit
				insertFirst(*conflicts,buildCell(current_wf->name));
			}
		}else{
			// A FINIR
		}
	}
	return wt_merge;

}
