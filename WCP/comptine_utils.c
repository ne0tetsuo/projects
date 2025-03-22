/* TAKBOU MOHAMMED YANIS 12206365
Je déclare qu'il s'agit de mon propre travail.
Ce travail a été réalisé intégralement par un être humain. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "comptine_utils.h"

int read_until_nl(int fd, char **buf)
{
    ssize_t tot = 0;
    ssize_t buf_size = 128; 
    char *temp;

    *buf = (char *)malloc(buf_size);
    if (*buf == NULL)
    {
        perror("Unable to allocate buffer");
        return -1;
    }

    while (1)
    {
        if (tot >= buf_size - 1)
        {
            buf_size *= 2;
            temp = (char *)realloc(*buf, buf_size);
            if (temp == NULL)
            {
                free(*buf);
                perror("Error while allocating the buffer");
                return -1;
            }
            *buf = temp;
        }

        ssize_t bytes_read = read(fd, *buf + tot, 1);
        if (bytes_read == -1)
        {
            perror("Read error");
            free(*buf);
            return -1;
        }
        else if (bytes_read == 0)
        {
            break;
        }

        tot += bytes_read;

        if ((*buf)[tot - 1] == '\n')
        {
            break;
        }
    }

    (*buf)[tot] = '\0';
    return tot;
}

int est_nom_fichier_comptine(char *nom_fich)
{
	if (strlen(nom_fich) < 4)
	{
		return 0;
	}
	return strcmp(nom_fich + strlen(nom_fich) - 4, ".cpt") ? 0 : 1;
}

struct comptine *init_cpt_depuis_fichier(const char *dir_name, const char *base_name)
{
	struct comptine *comp = malloc(sizeof(struct comptine));
	if (comp == NULL)
	{
		perror("error when allocating the comptine");
		return NULL;
	}
	comp->nom_fichier = strdup(base_name);
	if (comp->nom_fichier == NULL)
	{
		perror("error when allocating the name of the file");
		free(comp);
		return NULL;
	}
	char path[1024];
	snprintf(path, sizeof(path), "%s/%s", dir_name, base_name);
	int fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		perror("error when opening the file");
		free(comp->nom_fichier);
		free(comp);
		return NULL;
	}
	char *buffer;
	int n = read_until_nl(fd, &buffer);
	if (n < 0)
	{
		perror("error when reading until nl");
		free(comp->nom_fichier);
		free(comp);
		close(fd);
		return NULL;
	}
	comp->titre = strdup(buffer);
	if (comp->titre == NULL)
	{
		perror("malloc");
		free(comp->nom_fichier);
		free(comp);
		close(fd);
		return NULL;
	}
	close(fd);
	return comp;
}
void liberer_comptine(struct comptine *cpt)
{
	if(cpt !=NULL){
	free(cpt->nom_fichier);
	free(cpt->titre);
	free(cpt);
	}
	return;
}

struct catalogue *creer_catalogue(const char *dir_name)
{
    struct dirent *ent;
    DIR *directory = opendir(dir_name);
    if (directory == NULL)
    {
        perror("error when opening the directory");
        return NULL;
    }
    struct catalogue *cat = malloc(sizeof(struct catalogue));
    if (cat == NULL)
    {
        perror("error when allocating the catalogue");
        closedir(directory);
        return NULL;
    }
    int cpt = 0, max = 256;
    struct comptine **list = malloc(max * sizeof(struct comptine *));
    while ((ent = readdir(directory)) != NULL)
    {
        if (est_nom_fichier_comptine(ent->d_name))
        {
            list[cpt++] = init_cpt_depuis_fichier(dir_name, ent->d_name);
            if (cpt == max)
            {
                max *= 2;
                list = realloc(list, max*sizeof(struct comptine*));
                if (list == NULL)
                {
                    perror("realloc");
                    free(cat);
                    closedir(directory);
                    return NULL;
                }
            }
        }
    }
    list = realloc(list, cpt *sizeof(struct comptine*));
    if (list == NULL)
    {
        perror("realloc");
        free(cat);
        closedir(directory);
        return NULL;
    }
    cat->tab = list;
    cat->nb = cpt;
    closedir(directory);
    return cat;
}

void liberer_catalogue(struct catalogue *c)
{
	for(int i=0;i<c->nb;++i){
		liberer_comptine(c->tab[i]);
	}
	free(c->tab);
	free(c);
}