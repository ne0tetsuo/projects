/* TAKBOU MOHAMMED YANIS 12206365
Je déclare qu'il s'agit de mon propre travail.
Ce travail a été réalisé intégralement par un être humain. */
/* fichiers de la bibliothèque standard */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
/* bibliothèque standard unix */
#include <unistd.h> /* close, read, write */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
/* spécifique à internet */
#include <arpa/inet.h> /* inet_pton */
/* spécifique aux comptines */
#include "comptine_utils.h"

#define PORT_WCP 4321

void usage(char *nom_prog)
{
	fprintf(stderr, "Usage: %s addr_ipv4\n"
					"client pour WCP (Wikicomptine Protocol)\n"
					"Exemple: %s 208.97.177.124\n",
			nom_prog, nom_prog);
}

/** Retourne (en cas de succès) le descripteur de fichier d'une socket
 *  TCP/IPv4 connectée au processus écoutant sur port sur la machine d'adresse
 *  addr_ipv4 */
int creer_connecter_sock(char *addr_ipv4, uint16_t port);

/** Lit la liste numérotée des comptines dans le descripteur fd et les affiche
 *  sur le terminal.
 *  retourne : le nombre de comptines disponibles */
uint16_t recevoir_liste_comptines(int fd);

/** Demande à l'utilisateur un nombre entre 0 (compris) et nc (non compris)
 *  et retourne la valeur saisie. */
uint16_t saisir_num_comptine(uint16_t nb_comptines);

/** Écrit l'entier ic dans le fichier de descripteur fd en network byte order */
void envoyer_num_comptine(int fd, uint16_t nc);

/** Affiche la comptine arrivant dans fd sur le terminal */
void afficher_comptine(int fd);
// Televerse la comptine vers le serveur specifié par fd
int televerser_comptine(int fd, char *dir, char *name);
// envoie le choix sur 1 octet de l'utilisateur
int send_choice(int fd, uint8_t choice);
int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		usage(argv[0]);
		return 1;
	}
	int srvfd = creer_connecter_sock(argv[1], PORT_WCP);
	if (srvfd >= 0)
		printf("connexion succefful, file descriptor :%d\n", srvfd);
	else
	{
		perror("connexion unsuccesfull");
		return -1;
	}
	char name[256], dir[256], mess[256];
	uint16_t nb;
	uint16_t ic = 0;
	printf("\033[0;32m");
	printf(" __       __  ______  _______  \n");
	printf("/  |  _  /  |/      \\/       \\ \n");
	printf("$$ | / \\ $$ /$$$$$$  $$$$$$$  |\n");
	printf("$$ |/$  \\$$ $$ |  $$/$$ |__$$ |\n");
	printf("$$ /$$$  $$ $$ |     $$    $$/ \n");
	printf("$$ $$/$$ $$ $$ |   __$$$$$$$/  \n");
	printf("$$$$/  $$$$ $$ \\__/  $$ |      \n");
	printf("$$$/    $$$ $$    $$/$$ |      \n");
	printf("$$/      $$/ $$$$$$/ $$/       \n");
	printf("\n");
	printf("\033[0m");
	printf("1 - Choisir une comptine\n");
	printf("2 - Envoyer une comptine\n");
	printf("3 - Lister les comptines\n");
	printf("4 - Arrêter le client\n");
	printf("Copyright (c) 2024 Rousslin and Takbou. All rights reserved.\n");
	int tlvrs, times = 0;
	for (;;)
	{
		printf("Que voulez vous faire ? :   ");
		uint8_t choice;
		do
		{
			if (times != 0)
			{
				printf("Ressayez : ");
			}
			times++;
			scanf("%hhu", &choice);
			printf("\n\n");
		} while (choice < 1 || choice > 4);
		times = 0;
		int nbytesent = send(srvfd, &choice, 1, 0);
		if (nbytesent < 1)
		{
			perror("Error while sending the choice");
			goto exit;
		}
		switch (choice)
		{
		case 1:
			nb = recevoir_liste_comptines(srvfd);
			if (nb == 0)
			{
				printf("Connexion stopped");
				close(srvfd);
				return -1;
			}
			if ((ic = saisir_num_comptine(nb)) < nb)
			{
				envoyer_num_comptine(srvfd, ic);
				afficher_comptine(srvfd);
			};
			break;
		case 2:
			printf("Entrez le répértoire de la comptine:  ");
			scanf("%s", dir);
			printf("\n");
			printf("Entrez le nom du fichier de la comptine: ");
			scanf("%s", name);
			tlvrs = televerser_comptine(srvfd, dir, name);
			if (tlvrs >= 0)
				recv(srvfd, &mess, 256, 0);
			printf("%s \n", mess);
			break;
		case 3:
			nb = recevoir_liste_comptines(srvfd);
			if (nb == 0)
			{
				printf("Connexion stopped");
				close(srvfd);
				return -1;
			}
			break;
		case 4:
			goto exit;
		}
	}
exit:
	close(srvfd);
	return 0;
}

int creer_connecter_sock(char *addr_ipv4, uint16_t port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in srv;
	srv.sin_family = AF_INET;
	srv.sin_port = htons(port);
	if (inet_pton(AF_INET, addr_ipv4, &srv.sin_addr.s_addr) <= 0)
	{
		perror("error when formatting the address");
		close(sockfd);
		return -1;
	}
	if (connect(sockfd, (struct sockaddr *)&srv, sizeof(srv)) < 0)
	{
		perror("error when connecting to the socket ");
		close(sockfd);
		return -1;
	}
	return sockfd;
}

uint16_t recevoir_liste_comptines(int fd)
{
	uint16_t tmp, nb = 0;
	char prev = '\0', cur = '\0';
	do
	{
		prev = cur;
		if ((tmp = recv(fd, &cur, 1, 0)) < 0)
		{
			perror("recv");
			return 0;
		}
		else if (tmp == 0)
		{
			perror("socket closed");
			return 0;
		}
		if (cur == '\n')
			nb++;
		printf("%c", cur);
	} while (prev != cur || cur != '\n');
	return nb - 1;
}

uint16_t saisir_num_comptine(uint16_t nb_comptines)
{
	uint16_t num;
	do
	{
		printf("Entrez le numéro de comptine (entre 0 et %d): ", nb_comptines - 1);
		if (scanf("%hu", &num) != 1)
		{
			printf("Entrée invalide. Veuillez saisir un nombre valide.\n");
			while (getchar() != '\n')
				;
			continue;
		}
		if (num < 0 || num >= nb_comptines)
		{
			printf("Le numéro doit être compris entre 0 et %d. Réessayez.\n", nb_comptines - 1);
		}
	} while (num < 0 || num > nb_comptines);
	return num;
}
void envoyer_num_comptine(int fd, uint16_t nc)
{
	uint16_t num = htons(nc);
	int bytes_sent;
	if ((bytes_sent = send(fd, &num, sizeof(num), 0)) < 0)
	{
		perror("send");
	}
	else if (bytes_sent < sizeof(num))
	{
		perror("sent fewer bytes");
	}
}

void afficher_comptine(int fd)
{
	uint16_t tmp;
	char pprev, prev = '\0', cur = '\0';
	do
	{
		pprev = prev;
		prev = cur;
		if ((tmp = recv(fd, &cur, 1, 0)) < 0)
		{
			perror("error when receiving the comptine");
			return;
		}
		if (pprev != '\n' || prev != '\n' || cur != '\n')
			printf("%c", cur);
	} while (pprev != '\n' || prev != '\n' || cur != '\n');
}
int televerser_comptine(int fd, char *dir, char *name)
{
	char path[256], buffer[1024];
	ssize_t nbytes_read, nbytes_sent;
	if (!est_nom_fichier_comptine(name))
	{
		printf("Ceci n'est pas le nom d'un fichier comptine, réessayez. \n");
		return -1;
	}
	sprintf(path, "%s/%s", dir, name);
	int fdf = open(path, O_RDONLY);
	if (fdf < 0)
	{
		perror("Error while opening the .cpt file");
		close(fdf);
		return -1;
	}
	if ((nbytes_sent = send(fd, name, strlen(name) + 1, 0)) < 0)
	{
		perror("Error while sending filename");
		close(fdf);
		return -1;
	}
	if (nbytes_sent < strlen(name) + 1)
	{
		perror("Name was lost in the way");
		close(fdf);
		return -1;
	}
	while ((nbytes_read = read(fdf, buffer, sizeof(buffer))) > 0)
	{
		nbytes_sent = send(fd, buffer, nbytes_read, 0);
		if (nbytes_sent < 0 || nbytes_sent < nbytes_read)
		{
			perror("Error while sending comptine");
			close(fdf);
			return -1;
		}
	}
	if (nbytes_read < 0)
	{
		perror("error when reading");
		close(fdf);
		return -1;
	}
	close(fdf);
	char newline[2] = {'\n', '\n'};
	if (send(fd, &newline, 2, 0) < 0)
	{
		perror("error when sending new lines");
		return -1;
	}

	return 0;
}
int send_choice(int fd, uint8_t choice)
{
	if (send(fd, &choice, 1, 0) < 1)
	{
		perror("Sending ur choice");
		return -1;
	}
	return 0;
}