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
#include "commit.h"

#include <errno.h>//pour isFile
#define TAILLE_MAX_DATA 255
#define SIZE_MAX_WORKTREE 100
#define SIZE_COMMIT 10

/* --------------------- PARTIE 3 --------------------- */
/* Gestion des commits */

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

void freeCommit(Commit* c){
	if(c != NULL){
		int i;
		int cpt = 0;
		for(i=0; i<c->size; i++){
			if((c->T[i] != NULL) && (cpt < c->n )){
				freeKeyVal(c->T[i]);
				cpt ++;
			}
		}
		free(c->T);
		free(c);
	}

}

unsigned long sdbm(char *str){
    unsigned long hash = 0;
    int c;
    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

   return hash;
}
void commitSet(Commit* c, char* key, char* value){
	if ((c == NULL) || (key == NULL) || (value == NULL) ){
		//printf("commitSet :parm c == NULL\n");
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
		//printf("PAS DE BOL\n");
		return NULL;
	}
	char *buff_hash = (char*)malloc(sizeof(char)*255);
	if(fgets(buff_hash,200,f) == NULL){
		char* buff_vide = malloc(sizeof(char)*2);
		sprintf(buff_vide," ");
		fclose(f);
		//printf("àà:%s:\n",buff_vide);
		return buff_vide;
	}
	//printf("%s",buff_hash);
	fclose(f);
	//printf("àà:%s\n",buff_hash);
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
				//freeWorkTree(wt);

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
		//freeWorkTree(wt);
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
	//freeWorkTree(wt);
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
	//freeCommit(c);
	createUpdateRef(branch_name, hash_commit);
	createUpdateRef("HEAD", hash_commit);
}