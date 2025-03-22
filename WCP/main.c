//fichier main pour tester les fonctions. (fait au tout debut du projet).
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "comptine_utils.h"
int main(int argc, char **argv)
{
    // for(++argv;*argv!=NULL;argv++)
    //   printf("%s est nom de comptine : %d\n",*argv,est_nom_fichier_comptine(*argv));
    /*int fd=open("./comptines/escargot.cpt",O_RDONLY);
    if(fd<=0){
        perror("open");
        return -1;
    }
    char *buff;
    int n=read_until_nl(fd,&buff);
    write(1,buff,n);
    */
    /*char dir_name[256];
    strcpy(dir_name,"/home/cyberozer/Bureau/SYS/projet/comptines");
    char base_name[20];
    strcpy(base_name,"escargot.cpt");
    struct comptine *comptine=init_cpt_depuis_fichier(dir_name,base_name);
    printf("nom du fichier :%s , son titre: %s",comptine->nom_fichier,comptine->titre);
    free(buff);
     */

    struct catalogue *cat = creer_catalogue("/home/cyberozer/Bureau/SYS/projet/comptines");
    for (int i = 0; i < cat->nb; ++i)
    {
        printf("nom du fichier: %s , titre : %s \n", cat->tab[i]->nom_fichier, cat->tab[i]->titre);
    }
    return 0;
}