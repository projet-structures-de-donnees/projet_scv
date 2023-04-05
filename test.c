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
#include "main.c"

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
		char* resultat = sha256file(argv[1]);
		printf("%s",resultat);
	}



	/*List* a = initList();
	
	Cell* c1 = buildCell("élément de c1");
	Cell* c2 = buildCell("élément de c2");
	Cell* c3 = buildCell("élément de c3");
	Cell* c4 = buildCell("élément de c4");
	Cell* c5 = buildCell("élément de c5");


	insertFirst(a,c1);
	insertFirst(a,c2);
	insertFirst(a,c3);
	insertFirst(a,c4);
	insertFirst(a,c5);


	
	//Cell* cellule1 = listGet(a,6);
	//printf("%s\n", cellule1->data);

//TEST AFFICHAGE
	
	printf("c1= %p\n",c1);
	printf("a= %p\n",a);
	printf("*a= %p\n",*a);
	printf("%s\n", (*a)->data);


	cellule1 = searchList(a, "élément de c7");
	printf("%s\n",cellule1->data);


	printf("%s\n",ltos(a));

	char* chaine = strstr("chaine d|e caractere", "|");
	printf("%s\n", chaine); 
	*/

	/*List* l2 = stol("||listofil|la|chaine1|1|2|a|b|| |c2|lc3|Chaine n4");

	printf("%s\n",ltos(l2));
	printf("%s\n",ltos(a));


	//Cell* cellule2 = searchList(l2, "lc3");
	
	
	//if(cellule2){
	//	printf("%s\n",cellule2->data);	
	//}else{
	//	printf("cellule 2 VIDE !!\n");
	//}
	
	ltof(l2,"file_from_ltof");

	List* l3 = ftol("file_from_ltof");

	
	
	//Cell* ptr = *l3;while(ptr){
	//	printf("while=%p\n",ptr);
	//	ptr=ptr->next;}
	

	printf("%s\n",ltos(l3));



	List* l4 = listdir(".");
	printf("%s\n",ltos(l4));
	printf("EXIST  -> %d\n",file_exists("test"));

	//cp("cp_function","main.c");

	printf("%s\n",hashToPath(sha256file(argv[1])));*/
	
	//cp("./test",argv[1]);

	/*blobFile(argv[1]);


    blobFile("./Makefile");
    blobFile("./essay.c");*/

    printf("FILE_EXISTS  = %d\n",file_exists2("./rep1/file1.txt"));
    printf("FILE_EXISTS  = %d\n",file_exists("./rep1/file1.txt"));


/* PARTIE 2 */
	printf("\n\n *$*$*$*$*$*$*$*$* PARTIE 2 *$ *$*$*$*$*$*$*$* \n");
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
	WorkTree* wt2 = stwt("\nessay.c	949494949494949494949	94\nrrt\ne\ngr\nkejn kjfej\nbiblio_list.h	929é94949494949494949	92\nMakefile	929é94949494949494949	92\nrep1	929é669494949494    94\n\nkr\nlkfne");
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

	saveWorkTree(wt2,".");
	//restoreWorkTree(wt3, ".");



	return 0;
}
