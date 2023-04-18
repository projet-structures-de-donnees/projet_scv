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
        return -1;
    }else{
        if(strcmp(argv[1],"--help") == 0){
            printf("\tmyGit init\n");
            printf("\tmyGit list-refs\n");
            printf("\tmyGit create-ref <name> <hash>\n");
            printf("\tmyGit delete-ref <name>\n");
            printf("\tmyGit add <elem> [<elem2> <elem3> ...]\n");
            printf("\tmyGit list-add\n");
            printf("\tmyGit get-current-branch : affiche le nom de la branche courante.\n");
            printf("\tmyGit branch <branch-name>:crée une branche <branch-name> \n");
            printf("\tmyGit branch-print <branch-name> : affiche le hash de tous les commits de la branche\n");
            printf("\tmyGit checkout-branch <branch-name>:réalise un déplacement sur la branche <branch-name>\n");
            printf("\tmyGit checkout-commit <pattern> : réalise un déplacement sur le commit qui commence par <pattern>\n");
            printf("\t/myGit merge <branch> <message> : réalise le merge entre la branche courante et <branch>\n");
            return 1;
        }
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
                    createDeletionCommit(argv[2],l_conflicts,argv[3]);
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
                    l_conflicts2 = merge(argv[2],argv[3]);
                    if(l_conflicts2 == NULL){
                        printf("Fusion réalisé\n");
                    }else{
                        printf("Error merge\n");
                    }
                    return 1;
                }

                if(choix == 3){
                    printf("Pour chaque fichiers/repertoires choisir la branches sur laquelles vous voulez garder la bonne versions\n");
                    printf("pour la branche courante (%s) - 1\n",getCurrentBranch());
                    printf("pour la branche distantes - 2\n");

                    List* l_current = initList();
                    List* l_branch = initList();
                    int i = 0;
                    Cell* elem = listGet(l_conflicts,i);
                    while(elem != NULL){
                        printf("%s \t [1/2] ?\n",elem->data);
                        scanf("%d",&choix);
                        if(choix == 1){ //current 
                            printf("Choix 1\n");
                            *l_conflicts = (*l_conflicts)->next;
                            insertFirst(l_current,elem);
                            elem = *l_conflicts;
                        }
                        if(choix == 2){//branch
                            printf("Choix 2\n");
                            *l_conflicts = (*l_conflicts)->next;
                            insertFirst(l_branch,elem);
                            elem = *l_conflicts;
                        }
                        i++;
                    }
                    //printf("current :\n%s\n",ltos(l_current));
                    //printf("branch :\n%s\n",ltos(l_branch));

                    createDeletionCommit(getCurrentBranch(),l_branch,argv[3]);// Supprime de current ce que l'on veux pas garder
                    if(merge(argv[2],argv[3]) != NULL){
                        printf("fusion delete ERRERUE\n");
                    }
                    createDeletionCommit(argv[2],l_current,argv[3]);//Supprime de branch ce que l'on veux pas garder
                    if(merge(argv[2],argv[3]) != NULL){
                        printf("fusion merge ERREUR\n");
                    }
                    printf("Fusion réalisé\n");
                        
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