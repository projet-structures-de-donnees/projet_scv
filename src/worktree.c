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
#include "worktree.h"

#include <errno.h>//pour isFile
#define TAILLE_MAX_DATA 255
#define SIZE_MAX_WORKTREE 100
#define SIZE_COMMIT 10

/* --------------------- PARTIE 2 --------------------- */
/* Enregistrement de plusieurs instantanés */

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

		/*Mettre le marqueur de fin de chaine car probleme si la prochaine chaine est plus petite que la précédente */
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

int isFile(const char* name){
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
