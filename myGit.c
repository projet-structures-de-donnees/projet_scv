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
    if(argc < 2){
        printf("\tmyGit init\n");
        printf("\tmyGit list-refs\n");
        printf("\tmyGit create-ref <name> <hash>\n");
        printf("\tmyGit delete-ref <name>\n");
        printf("\tmyGit add <elem> [<elem2> <elem3> ...]\n");
        printf("\tmyGit list-add\n");
        printf("\tmyGit clear-add\n");
        printf("\tmyGit commit <branch name> [-m <message>]\n");



        return -1;
    }else{
        if(strcmp(argv[1],"init") == 0){
            initRefs();
            return 1;
        }
        if(strcmp(argv[1],"list-refs") == 0){
            printf("%s\n",ltos(listdir(".refs")));
            return 1;
        }
        if((strcmp(argv[1],"create-ref") == 0) && (argc >=4)){
            createUpdateRef(argv[2], argv[3]);
            return 1;
        }
        if(strcmp(argv[1],"delete-ref") == 0){
            deleteRef(argv[2]);
            return 1;
        }
        if(strcmp(argv[1],"add") == 0){
            int i;
            for(i=2; i< argc; i++){
                myGitAdd(argv[i]);
            }
            return 1;
        }
        if(strcmp(argv[1],"list-add") == 0){
            WorkTree* wt = ftwt(".add");
            printf("%s\n",wtts(wt));
            return 1;
        }
        if(strcmp(argv[1],"clear-add") == 0){
            FILE* f = fopen(".add","w");
            fclose(f);
            return 1;
        }       
        if((strcmp(argv[1],"commit") == 0) && (argc >=3)){
            myGitCommit(argv[2],argv[3]);
            return 1;
        }        
        printf("%s %s : NOT found\n",argv[0],argv[1]);
    }
    return 0;
}