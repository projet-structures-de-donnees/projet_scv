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
#include "branch.h"

#include <errno.h>//pour isFile
#define TAILLE_MAX_DATA 255
#define SIZE_MAX_WORKTREE 100
#define SIZE_COMMIT 10


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
	return file_exists(path_branch);
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
		//printf("Cas - 0");
		return;
	}
	//printf("Cas -1");

	FILE* f = fopen(".current_branch","w");
	if(f == NULL){
		return;
	}

	fprintf(f,"%s",branch);
	fclose(f);

	//printf("Cas -6");
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
