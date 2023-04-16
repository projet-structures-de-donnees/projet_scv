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
#include "worktree.h"
#include "commit.h"
#include "branch.h"
#include "merge.h"

#include <errno.h>//pour isFile
#define TAILLE_MAX_DATA 255
#define SIZE_MAX_WORKTREE 100



int main(int argc, char** argv){
	if(argc != 2){
		printf("Usage %s : 1  argument\n",argv[0]);
		return 1;
	}else{
		//system("ls")
		//hashFile(argv[1], argv[2]);
		//char* resultat = sha256file(argv[1]);
		//printf("%s",resultat);
	}

/* --------------------- PARTIE 1 --------------------- */
/* Vers la création d’enregistrements instantanés */
printf(" --------------------- PARTIE 1 --------------------- \n Vers la création d’enregistrements instantanés \n");

	List* list1 = initList();
	
	Cell* c1 = buildCell("élément de c1");
	Cell* c2 = buildCell("élément de c2");
	Cell* c3 = buildCell("élément de c3");

	insertFirst(list1,c1);
	insertFirst(list1,c2);
	insertFirst(list1,c3);

	Cell* cellule1 = listGet(list1,6);
	assert(cellule1 == NULL);
	Cell* cellule2 = listGet(list1,2);
	assert(cellule2 == c1);
	

//TEST AFFICHAGE
	

	Cell* cellule3 = searchList(list1, "élément de c7");
	assert(cellule3 == NULL);
	Cell* cellule4 = searchList(list1, "élément de c2");
	assert(cellule4 == c2);


	printf("ltos =%s\n",ltos(list1));

	assert(list1 != NULL);
	freeList(list1);
	//assert(c1 == NULL);
	//assert(*list1 == NULL);


	List* l2 = stol("||listofil|la|chaine1|1|2|a|b|| |c2|lc3|Chaine n4");
	printf("ltos =%s\n",ltos(l2));
	cellule2 = searchList(l2, "lc3");
	//assert(strcmp(cellule2->data,"lc3")== 0);
	freeList(l2);

	ltof(l2,"file_from_ltof");
	List* l3 = ftol("file_from_ltof");

	
	
	//Cell* ptr = *l3;while(ptr){
	//	printf("while=%p\n",ptr);
	//	ptr=ptr->next;}
	

	//printf("%s\n",ltos(l3));



	//List* l4 = listdir(".");
	//printf("%s\n",ltos(l4));
	//printf("EXIST  -> %d\n",file_exists("test"));

	//cp("cp_function","main.c");

	//printf("%s\n",hashToPath(sha256file(argv[1])));*/
	
	//cp("./test",argv[1]);

	/*blobFile(argv[1]);


    blobFile("./Makefile");
    blobFile("./essay.c");*/

    //printf("FILE_EXISTS  = %d\n",file_exists("./rep1/file1.txt"));
    //printf("FILE_EXISTS  = %d\n",file_exists("./rep1/file1.txt"));





/* PARTIE 2 */
	/*printf("\n\n *$*$*$*$*$*$*$*$* PARTIE 2 *$ *$*$*$*$*$*$*$* \n");
	WorkFile* wf = createWorkFile("workfile_1");
	printf("%s\n",wfts(wf));

	WorkFile* wf2 = NULL;
	wf2 = stwf("WorkFile2	ubre34ofbu42yfvreuyb54	777");
	printf("%s\n",wfts(wf2));



	WorkTree* wt = initWorkTree();
	appendWorkTree(wt, "wf1", "hash1lkfnezmlfezfjezhi", 788);
	appendWorkTree(wt, "wf2", "hash2irhjzùmlieeqiezjzelim", 878);
	appendWorkTree(wt, "wf3", "hash3liizehjrmzeinezdsne", 888);
	appendWorkTree(wt, "wf4", "hash4lielfkijzajrlùmze", 990);

	appendWorkTree(wt, "wf25", "hash5ioezjfmqifioezj", 999);

	printf("%d\n",inWorkTree(wt,"wf1"));
	
	printf("%s\n",wtts(wt));

	printf("*$*$*$*$*$*$*$*$*$\n");
	WorkTree* wt2 = stwt("\nessay.c	3ade6c85c48640f486aab3c201bc18f79561297d	94\nrrt\ne\ngr\nkejn kjfej\nbiblio_list.h	929é94949494949494949	92\nMakefile	929é94949494949494949	92\nrep1	929é669494949494    94\n\nkr\nlkfne");
	//WorkTree* wt2 = stwt(wtts(wt));
	printf("*$*$*$*$*$*$*$*$*$\n");

	printf("%s\n",wtts(wt));
	printf("%s\n",wtts(wt2));

	wttf(wt2,"test_wt.txt");

	WorkTree* workT = ftwt("test_wt.txt");
	printf("%s\n",wtts(workT));


	//printf("%s\n",blobWorkTree(wt2));

	//printf("%d\n",getChmod("MakeFile"));
	//printf("%d\n",getChmod("main"));
	//printf("%d\n",getChmod("main.c"));
	//printf("%d\n",getChmod("6a"));



	//printf("%d\n",isFile(".git,kl"));
	//List* l4 = listdir("./rep1");
	//printf("%s\n",ltos(l4));

	//WorkTree* wt3 = stwt("\nessay.c	949494949494949494949	94\nrrt\ne\ngr\nkejn kjfej\nbiblio_list.h	929é94949494949494949	92\nMakefile	929é94949494949494949	92\nrep1	929é669494949494.t    94\n\nkr\nlkfne");
	//WorkTree* wt3 = ftwt("da/07cbed13a56810980f728c06425bad3caad280.t");

	//saveWorkTree(wt2,".");
	//restoreWorkTree(wt2, ".");



/* PARTIE 3 */
	/*printf("\n\n *$*$*$*$*$*$*$*$* PARTIE 3 *$ *$*$*$*$*$*$*$* \n");
	kvp *k =createKeyVal("clé1","valeur1");
	printf("%s\n", kvts(k));

	kvp* k2 = stkv("valeur2 :clénumber2");
	printf("%s \n%s\n",k2->key,k2->value);
	//char* data = "fdfgsdhfsdfs445d4fsd7fg/+bfrsdgf$";
	//printf("%lu\n",sdbm(data)%255);
	
	char* data1 = ("clé1");
	int b = (sdbm(data1)+0)%10;
	printf("%d\n",b);*/

	/*Commit *commit = createCommit("hash_de_tree") ;
	commitSet(commit, "clé1", "valeur1");
	commitSet(commit, "cle2", "valeur2");
	commitSet(commit, "cle3", "valeur3");
	commitSet(commit, "cle4", "valeur4");
	commitSet(commit, "cle5", "valeur5");
	commitSet(commit, "cle6", "valeur6");
	commitSet(commit, "cle7", "valeur7");
	commitSet(commit, "cle8", "valeur7");
	commitSet(commit, "cle9", "valeur7");
	commitSet(commit, "cle10", "valeur7");
	commitSet(commit, "cle11", "valeur7");
	commitSet(commit, "cltecle", "valeur7");
	commitSet(commit, "certecle", "valeur7");
	commitSet(commit, "clgrtkecle", "valeur7");
	commitSet(commit, "clgtegecle", "valeur7");


	for (int i = 0; i<commit->size; i++){
		printf("%s\n",kvts(commit->T[i]));
	}

	char *value = commitGet(commit, "cl5");
	if (value != NULL){
		printf("%s",value);
	}else{
		printf("Pas trouvéééééééé\n");
	}

	printf("%s\n",cts(commit));

	Commit* cmt1 = stc("prmclé :valeeurr1\nddxmeecllé :val222222\neof\n\n,oe\nddxmeecllé :val9999\ncle3 :val3\ncle3 :val0000");
	
	printf("\n\n%s\n",cts(cmt1));

	for (int i = 0; i<cmt1->size; i++){
		printf("%s\n",kvts(cmt1->T[i]));
	}

	//ctf(commit,"test_vers_file.txt");

	//Commit* commit_f = ftc("test_vers_file.txt");
	//printf("\n\n%s\n",cts(commit_f));
	//printf("\n%s\n",blobCommit(commit));
	
	/* Exercice 7 – Gestion temporelle des commits de manière linéire */
	//createUpdateRef("HEAD", "hash_suivie_par_HEAD");
	//createUpdateRef("master", "hash_suivie_par_master");
	//createUpdateRef("develop", "hash_suivie_par_develop");
	//deleteRef("develop");
	//printf("DEBUT:%s:FIN\n",getRef("HEAD"));
	//printf("DEBUT:%s:FIN\n",getRef("master"));
	//printf("DEBUT:%s:FIN\n",getRef("develop"));

	//myGitAdd("file.txt");
	//myGitAdd("Makefile");
	/*
	printf("*¨$`ù^$`ù^$`ù^----------ù^$``^$`$`^$^$-------\n");
	char* t = "HALLOWEEN";

	createKeyVal("tree","HALLO");
	initRefs();
	myGitAdd("Makefile");
	myGitCommit("master", "début C'est un test de message de commit pour faire des test skiuuuuu");
	*/

/* --------------------- PARTIE 4 --------------------- */
/* Gestion d’une timeline arborescente */
printf(" --------------------- PARTIE 4 --------------------- \nGestion d’une timeline arborescente \n\n");
	//initBranch();
	//initRefs();
	//printf("br_ex =%d\n",branchExists("HEAD"));
	//createBranch("feature1");
	//printf("getCurBra =%s\n",getCurrentBranch());
	/*printBranch("master");

	printf("%d\n",file_exists("."));
	printf("%s\n",ltos(branchList("HEAD")));
	

	printf("LALAL\n%s\n",ltos(listdir(".refs")));

	printf("%s\n",ltos(getAllCommits()));*/

	//restoreCommit("923ea61f8aed462921691812f74305a2a198d.c");

	/*printf(" --------------------- PARTIE EXO 11 --------------------- \nGestion d’une timeline arborescente \n\n");


	List* l = initList();
	printf("\n\n");
	printf("%s\n",wtts(mergeWorkTrees(ftwt("0d/63448bbc161a964423eda1ff6159a4247040fb.t"),ftwt("05/2cbbda0be8c7cb5d6ff21f92fc3a0322ecd990.t"),&l)));


	printf("\n\n");
	printf("%s\n",ltos(l));*/

	return 0;
}
