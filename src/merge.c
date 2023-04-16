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
#include "merge.h"

#include <errno.h>//pour isFile
#define TAILLE_MAX_DATA 255
#define SIZE_MAX_WORKTREE 100
#define SIZE_COMMIT 10

WorkTree* mergeWorkTrees(WorkTree* wt1, WorkTree* wt2 , List** conflicts){
	//("Debut mergeWorkTrees\n");
	if(wt1==NULL || wt2==NULL || *conflicts==NULL){
		//printf("wt1 ou wt2 ou *list est a NULL\n");
		return NULL;
	}
	if(wt1->n==0){
		//freeWorkTree(wt1);
		//printf("wt1->n est a ZERO\n");
		if (wt2->n==0){
			//freeWorkTree(wt2);
			//printf("wt2->n est a ZERO\n");
			return initWorkTree();
		}
		//printf("Mais pas wt2->n\n");
		return wt2;
	}
	//printf("Affichage des WorkTree récupéré\n");
	//printf("mergeWorkTrees\n%s\n",wtts(wt1));
	//printf("mergeWorkTrees\n%s\n",wtts(wt2));

	WorkTree *wt_merge = initWorkTree();
	WorkFile* current_wf = NULL;
	int i;
	int pos = 0;
	//printf("nb_elem dans wt1 =%d\n",wt1->n);
    for(i=0; i<wt1->n; i++){// on parcourt wt1
		//printf("i =%d\n",i);
		current_wf = &(wt1->tab[i]);
		pos = inWorkTree(wt2,current_wf->name);
		if(pos != -1){ // Si le le fichier/reperoire et dans les deux wt
			//printf("wf(%s) de wt1 est présent dans wt2\n",current_wf->name);
			if(strcmp(current_wf->hash,wt2->tab[pos].hash) == 0){ //Si pas de conflits
				//printf("Ajout dans wt_merge car meme hash\n");
				if(appendWorkTree(wt_merge,current_wf->name,current_wf->hash,current_wf->mode) != 1){
					//printf("merge: erreur appendWt\n");
					return NULL;
				}else{

				}
			}else{ // Si conflit
				insertFirst(*conflicts,buildCell(current_wf->name));
				//printf("Ajout conf car pas le meme hash\n");
				//printf("Affichage des conflits en cours\n");
				//printf("%s\n",ltos(*conflicts));
			}
		}else{// Si le wf apparait que dans un WT On ajoute
			//printf("wf(%s) de wt1 est PAS présent dans wt2\n",current_wf->name);
			if(appendWorkTree(wt_merge,current_wf->name,current_wf->hash,current_wf->mode) != 1){
					//printf("merge: erreur appendWt\n");
					return NULL;
			}else{
				//printf("Ajout de %s dans le merge via wt1\n",current_wf->name);

			}
		}
	}
	// On ajoute tous les workfile du wt2 qui ne sont pas dans wt_merge
	for(i=0; i<wt2->n; i++){
		current_wf = &(wt2->tab[i]);
		if((searchList(*conflicts, current_wf->name) == NULL) && (inWorkTree(wt_merge, current_wf->name) == -1)) { // si le wf n'est ni dans la liste ni dans wt_merge
			if(appendWorkTree(wt_merge,current_wf->name,current_wf->hash,current_wf->mode) != 1){ // ajout dans le merge
					printf("merge: erreur appendWt\n");
					return NULL;
			}else{
				//printf("Ajout de %s dans le merge via wt2\n",current_wf->name);

			}
		}

	}
	//freeWorkTree(wt1);
	//freeWorkTree(wt2);
	return wt_merge;

}
/* Fusionne branche courante avec branche paramètre,
si aucun conflit n’existe et return NULL.
Sinon retourne la liste des conflits */
List* merge(char* remote_branch, char* message){
	//printf("DEBUT merge\n");
	if((remote_branch == NULL) || (strcmp(remote_branch,getCurrentBranch()) == 0)){
		return NULL;
	}
	if(!branchExists(remote_branch)){
		printf("merge: branche %s non existant\n",remote_branch);
		return NULL;
	}

	//récupération du dernier wotkTree de la branche courante
	char* hash_last_commit_curr = getRef(getCurrentBranch());
	Commit *ct_curr = ftc(hashToPath(hash_last_commit_curr));
	WorkTree *wt_curr = ftwt(hashToPath(commitGet(ct_curr,"tree")));
	//freeCommit(ct_curr);
	//printf("Affichage last wt_curr:(%s)\n%s\n",getCurrentBranch(),wtts(wt_curr));

	//récupération du dernier wotkTree de la branche en parametre
	char* hash_last_commit_remote = getRef(remote_branch);
	Commit *ct_remote = ftc(hashToPath(hash_last_commit_remote));
	WorkTree *wt_remote = ftwt(hashToPath(commitGet(ct_remote,"tree")));
	//freeCommit(ct_remote);
	//printf("Affichage last wt_de la branch:(%s)\n%s\n",remote_branch,wtts(wt_remote));

	List* l_conflicts = initList(); 
	WorkTree *wt_fusion = mergeWorkTrees(wt_curr,wt_remote,&l_conflicts);
	if(wt_fusion == NULL){
		//printf("wt_fusion est à NULL\n");
		return NULL;
	}
	if(*l_conflicts != NULL){// Si conflits
		//printf("il y'a des conflits =%s\n",ltos(l_conflicts));
		return l_conflicts;
	}else{
		//printf("Aucun conflits !!\n");
	}

	char* hash_wt_fusion = saveWorkTree(wt_fusion, ".");
	Commit *commit = createCommit(hash_wt_fusion);
	//free(hash_wt_fusion);
	commitSet(commit, "predecessor", hash_last_commit_curr);
	//free(hash_last_commit_curr);
	commitSet(commit, "merged_predecessor", hash_last_commit_remote);
	//free(hash_last_commit_remote);
	commitSet(commit, "message", message); // commitSet gere le cas ou la value est NULL
	//printf("Affichage du commit de fusion:\n");
	//printf("%s\n",cts(commit));


	char *hash_commit = blobCommit(commit);
	createUpdateRef(getCurrentBranch(), hash_commit); // On met a jour la ref de la branche
	//printf("remote_branch a supp=%s\n",remote_branch);
	deleteRef(remote_branch);
	restoreCommit(hash_commit);
	createUpdateRef("HEAD",hash_commit);
	//free(hash_commit);
	//freeCommit(commit);

	return NULL;
}


void createDeletionCommit(char * branch, List* conflicts,char * message ){
	if(branch == NULL){
		return;
	}

	// On stock la branche de départ
	char * current_branch=getCurrentBranch();

	char * ref_current=getRef(current_branch);
	Commit *ct_branch_current= ftc(hashToPath(ref_current));
	WorkTree *wt_branch_current = ftwt(hashToPath(commitGet(ct_branch_current,"tree")));

	// deplacement sur la branche en param
	myGitCheckoutBranch(branch);

	char * new_curr_branch=getCurrentBranch();

	//recupere le derinier hash du commit de la branch
	char * ref_new_curr_branch=getRef(new_curr_branch);
	if(ref_new_curr_branch == NULL){
		printf("Pas de référence pour la branche %s\n",branch);
		return;
	}
	Commit *ct_new_curr_branch = ftc(hashToPath(ref_new_curr_branch));
	WorkTree *wt_new_curr_branch = ftwt(hashToPath(commitGet(ct_new_curr_branch,"tree")));// Récuperation du dernier Wt de branch

	// Vide la zone de préparation
	FILE *f = fopen(".add","w");
	fclose(f);

	WorkTree* wt_merge = mergeWorkTrees(wt_new_curr_branch,wt_branch_current,&conflicts);
	if(wt_merge == NULL){ //Tous en conflits
		return ;
	}

	int i;
	for (i = 0; i < wt_merge->n; i++){ 
		//if(searchList(conflicts,wt_merge->tab[i].name) == NULL){ // Si le fichier/rep n'est pas dans la liste de conflit on l'ajoute
		myGitAdd(wt_merge->tab[i].name); //On ajoute tous les workfile non conflictuel 
		//printf("ajout dans le nv wt du nvx commit =%s\n",wt_merge->tab[i].name);
		//}
	}

	myGitCommit(new_curr_branch,message);

	myGitCheckoutBranch(current_branch);


}