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
#define SIZE_COMMIT 10




int main(int argc, char** argv){
    if(argc < 2){
        printf("\tmyGit init\n");
        printf("\tmyGit list-refs\n");
        printf("\tmyGit create-ref <name> <hash>\n");
        printf("\tmyGit delete-ref <name>\n");
        printf("\tmyGit add <elem> [<elem2> <elem3> ...]\n");
        printf("\tmyGit list-add\n");
        printf("\tmyGit clear-add\n");

        return -1;
    }else{
        if(strcmp(argv[1],"init") == 0){
            initRefs();
            initBranch();
            return 1;
        }
        if(strcmp(argv[1],"list-refs") == 0){
            printf("%s\n",ltos(listdir(".refs")));
            return 1;
        }
        if((strcmp(argv[1],"create-ref") == 0) && (argc >=4)){
            if( ftc(hashToPath(argv[3])) != NULL){
                createUpdateRef(argv[2], argv[3]);
            }else{
                printf("Cette référence ne représente pas un commit\n");
            }
            return 1;
        }
        if(strcmp(argv[1],"delete-ref") == 0){
            if((strcmp(argv[2],"master") == 0)  || (strcmp(argv[2],"HEAD") == 0)){
                printf("%s ne peut pas être supprimé\n",argv[1]);
            }else{
                deleteRef(argv[2]);
            }
            return 1;
        }
        if(strcmp(argv[1],"add") == 0){
            int i;
            for(i=2; i< argc; i++){
                if(strcmp(argv[i], ".")==0){
                    printf("Le répertoire courant %s ne peut pas etre pris en compte\n",argv[i]);
                }else{
                    myGitAdd(argv[i]);
                }
            }
            return 1;
        }
        if(strcmp(argv[1],"list-add") == 0){
            WorkTree* wt = ftwt(".add");
            if(wt != NULL){
                for(int i = 0; i<wt->n; i++){
                    printf("%s\n",wt->tab[i].name);
                }
            }
            return 1;
        }
        if(strcmp(argv[1],"clear-add") == 0){
            FILE* f = fopen(".add","w");
            fclose(f);
            return 1;
        }       
        if((strcmp(argv[1],"commit") == 0) && (argc >=3)){
            if(strcmp(argv[3],"-m") == 0){
                myGitCommit(argv[2],argv[4]);
            }else{
                myGitCommit(argv[2],NULL);
            }
            return 1;
        }
        if(strcmp(argv[1],"get-current-branch") == 0){
            printf("%s\n",getCurrentBranch());

            return 1;
        }  
        if(strcmp(argv[1],"branch") == 0  && (argc >= 3)){
            if(!branchExists(argv[2])){
                 createBranch(argv[2]);
            }else{
                printf("Branche %s deja existante \n",argv[2]);
            }
            return 1;
        }
        if(strcmp(argv[1],"branch-print") == 0  && (argc >= 3)){
            if(!branchExists(argv[2])){
                printf("Branche %s non existante \n",argv[2]);
                
            }else{
                printBranch(argv[2]);
            }
            return 1;
        }
        if(strcmp(argv[1],"checkout-branch") == 0  && (argc >= 3)){
            if(!branchExists(argv[2])){
                printf("Branche %s non existante \n",argv[2]);
            }else{
                myGitCheckoutBranch(argv[2]);
             }
        return 1;
        }

        if(strcmp(argv[1],"checkout-commit") == 0  && (argc >= 3)){
            myGitCheckoutCommit(argv[2]);
            return 1;
        }
        if(strcmp(argv[1],"merge") == 0  && (argc >= 3)){
            List *l_conflicts = merge(argv[2],argv[3]);
            if(l_conflicts != NULL){
                printf("Les fichiers suivants sont en conflits: \n%s\n",ltos(l_conflicts));
                int choix ;
                printf("Conflits:\n");
                printf("1- Garder les fichiers de la branche courante (%s) ?\n",getCurrentBranch());
                printf("2- Garder les fichiers de la branche %s ?\n",argv[2]);
                printf("3- Choisir manuellement \n");
                scanf("%d",&choix);

                if(choix == 1){
                    List *l_conflicts2 = initList();
                    createDeletionCommit(argv[2],l_conflicts2,argv[3]);
                    l_conflicts2 = merge(argv[2],argv[3]);
                    if(l_conflicts2 == NULL){
                        printf("Fusion réalisé\n");
                    }else{
                        printf("Error merge\n");
                    }
                    return 1;
                }

                if(choix == 2){
                    List *l_conflicts2 = initList();
                    createDeletionCommit(getCurrentBranch(),l_conflicts,argv[3]);
                    l_conflicts2 = merge(getCurrentBranch(),argv[3]);
                    if(l_conflicts2 == NULL){
                        printf("Fusion réalisé\n");
                    }else{
                        printf("Error merge\n");
                    }
                    return 1;
                }

                if(choix == 3){
                    // A continuer
                    return 1;
                }else{
                    printf("choix non existant\n");
                    
                }
            }else{
                printf("Fusion réalisé \n");
            }
            return 1;
        }
    
    printf("NOT FOUND\n");

    }
    return 0;
}