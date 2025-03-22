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
#define SO_REUSEPORT 15
#define PORT_WCP 4321
#define NBMAXCLT 10

void usage(char *nom_prog)
{
	fprintf(stderr, "Usage: %s repertoire_comptines [NB_MAX_USERS]\n"
					"serveur pour WCP (Wikicomptine Protocol)\n"
					"Exemple: %s comptines\n",
			nom_prog, nom_prog);
}
/** Retourne en cas de succès le descripteur de fichier d'une socket d'écoute
 *  attachée au port port et à toutes les adresses locales. */
int creer_configurer_sock_ecoute(uint16_t port);

/** Écrit dans le fichier de desripteur fd la liste des comptines présents dans
 *  le catalogue c comme spécifié par le protocole WCP, c'est-à-dire sous la
 *  forme de plusieurs lignes terminées par '\n' :
 *  chaque ligne commence par le numéro de la comptine (son indice dans le
 *  catalogue) commençant à 0, écrit en décimal, sur 6 caractères
 *  suivi d'un espace
 *  puis du titre de la comptine
 *  une ligne vide termine le message */
void envoyer_liste(int fd, struct catalogue *c);

/** Lit dans fd un entier sur 2 octets écrit en network byte order
 *  retourne : cet entier en boutisme machine. */
uint16_t recevoir_num_comptine(int fd);

/** Écrit dans fd la comptine numéro ic du catalogue c dont le fichier est situé
 *  dans le répertoire dirname comme spécifié par le protocole WCP, c'est-à-dire :
 *  chaque ligne du fichier de comptine est écrite avec son '\n' final, y
 *  compris son titre, deux lignes vides terminent le message */
void envoyer_comptine(int fd, const char *dirname, struct catalogue *c, uint16_t ic);
// Fonction pour le multithreading, accepte une connexion,envoie le catalogue, recoit l'indice d'une comptine et l'envoie.
void *srv_thread(void *);
// met l'addresse ip ,le port,l'heure d'accés du client dans un log qu'elle crée s'il n'existe pas
void wrlog(struct sockaddr_in, int nb);
// transforme une addresse ipv4 binaire en une represntation en char stockée dans output
void uint32_to_ip(uint32_t ip, char *output);
// Telecharge une comptine qui est entrain d'étre téléversé par un client.
int download_comptine(int fd, char dir[]);
// Recoit le choix du client pour pouvoir repondre à sa demande
uint8_t receive_choice(int fd);
struct info_th
{
	int fdclt;
	int fdlst;
	struct sockaddr *sockaddress;
	socklen_t socklen;
	struct catalogue **cata;
	char path[256];
};
int cltcount = 0;	  // variable globale du nombre de clients
pthread_mutex_t lock; // mutex global pour faciliter son usage
int maxclt;			  // nombre max de clients synchrones
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		usage(argv[0]);
		return 1;
	}

	if (argc > 2)
		maxclt = atoi(argv[2]);
	else
		maxclt = NBMAXCLT;
	struct sockaddr_in clt[maxclt];
	int lst = creer_configurer_sock_ecoute(PORT_WCP); // Création d'une socket d'écoute rutilisable.
	char path[256];									  // Creation et initialisation du chemin du repertoire des comptines.
	sprintf(path, "%s/", argv[1]);
	struct catalogue *c = creer_catalogue(path); // Creation du catalogue à partir du répértoire de comptines
	pthread_t t[4];								 // Utilisation des threads pour servir les clients(4 clients au max pour l'instant)
	struct info_th infoclt[4];					 // Creation et initialisation d'une structure afin de multithread le processus.
	pthread_mutex_init(&lock, 0);				 // Init du mutex utilisé pour la creation de fichier, la gestion du nombre de clients etc
	for (int i = 0; i < maxclt; ++i)			 // initialisation des structures info, utile pour le multithreading, structures qui fourniront toutes les informations nécessaires au thread
	{
		infoclt[i].fdlst = lst;								 // socket d'écoute
		infoclt[i].sockaddress = (struct sockaddr *)&clt[i]; // addresses pour store les information du client
		infoclt[i].socklen = sizeof(clt[i]);				 // leur taille
		infoclt[i].cata = &c;								 // pointeur vers le catalogue pour pourvoir le mettre à jour
		strcpy(infoclt[i].path, path);						 // path du dossier des comptines qu'on utilisera pour stocker des comptines
	}
	for (;;)
	{
		infoclt[cltcount].fdclt = accept(infoclt[cltcount].fdlst, infoclt[cltcount].sockaddress, &infoclt[cltcount].socklen);
		if (cltcount < maxclt)
		{
			pthread_create(&t[cltcount], NULL, srv_thread, &infoclt[cltcount]);
			pthread_mutex_lock(&lock);
			cltcount++;
			pthread_mutex_unlock(&lock);
		}
		else
		{
			for (int j = 0; j < maxclt; ++j)
			{
				pthread_join(t[j], NULL);
				close(infoclt[j].fdclt);
			}
		}
	}
	pthread_mutex_destroy(&lock); // destruction des mutex
	close(lst);					  // arret de l'écoute
	liberer_catalogue(c);		  // Liberation du catalogue
	return 0;
}

int creer_configurer_sock_ecoute(uint16_t port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0); // Socket creation
	int opt = 1;
	if (sockfd < 0)
	{
		perror("erreur lors de la socket creation");
		close(sockfd);
		return -1;
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) // Set socket to reusable
	{
		perror("setsockopt error");
		close(sockfd);
		return -1;
	}
	struct sockaddr_in clt; // initializing the sockaddr
	clt.sin_port = htons(port);
	clt.sin_family = AF_INET;
	clt.sin_addr.s_addr = INADDR_ANY;							// any address can connect to server
	if (bind(sockfd, (struct sockaddr *)&clt, sizeof(clt)) < 0) // binding address to the socket
	{
		perror("Binding error");
		close(sockfd);
		return -1;
	}
	if (listen(sockfd, maxclt) < 0) // listening for any noise that might wake up our processes
	{
		perror("listening error");
		close(sockfd);
		return -1;
	}

	return sockfd;
}

void envoyer_liste(int fd, struct catalogue *c)
{
	for (int i = 0; i < c->nb; ++i)
	{
		if (dprintf(fd, "%d %s", i, c->tab[i]->titre) < 0) // sending every file's title from the catalogue
		{
			perror("erreur lors de l'envoi de la liste");
		}
	}
	dprintf(fd, "\n"); // sending a nl to specify that the list of file ended here according to the specified protocol
}

uint16_t recevoir_num_comptine(int fd) // receiving the index of the file that the client wants to read
{
	uint8_t buf[2];
	uint16_t num;
	int i = 0;
	do
	{
		if (recv(fd, buf + i, 1, 0) <= 0) // recceiving 2 bytes of data
		{
			perror("Erreur lors de la reception du numéro de la comptine");
		}
		i++;
	} while (i < 2);
	num = buf[1];
	num = (num << 8) + buf[0]; // putting data in the right order
	return ntohs(num);		   // returning the host form of data
}

void envoyer_comptine(int fd, const char *dirname, struct catalogue *c, uint16_t ic)
{
	char path[1024];
	sprintf(path, "%s/%s", dirname, c->tab[ic]->nom_fichier); // formatting the file path to it's right form
	int fdsrc = open(path, O_RDONLY);						  // opening the file
	if (fdsrc < 0)
	{
		perror("Erreur lors de l'ouverture du fichier de comptine");
		return;
	}
	char buffer[1024];
	ssize_t bytes_read;
	while ((bytes_read = read(fdsrc, buffer, sizeof(buffer))) > 0) // reading file content
	{
		ssize_t bytes_sent = send(fd, buffer, bytes_read, 0); // sending file content
		if (bytes_sent < 0)									  // checking for errors
		{
			perror("Erreur lors de l'envoi de la comptine");
			close(fdsrc);
			return;
		}
		if (bytes_sent != bytes_read)
		{
			perror("Envoi de la comptine incomplet");
			close(fdsrc);
			return;
		}
	}
	if (bytes_read < 0)
	{
		perror("Erreur lors de la lecture du fichier de comptine");
	}
	close(fdsrc);					// closing the file
	char newline[2] = {'\n', '\n'}; // sending two nls to specify that the comptine has ended
	if (send(fd, &newline, 2, 0) < 0)
	{
		perror("erreur lors de l'envoi des sauts de ligne");
		return;
	}
}
void *srv_thread(void *buf) // threading functrion of the server
{
	struct info_th *info = buf;
	struct catalogue *c = *(info->cata); // extracting the catalogue
	uint16_t numcpt = 0;
	uint8_t choice;
	int dw;
	while (1)
	{
		int nbytes = recv(info->fdclt, &choice, 1, 0);
		if(nbytes<0){
			goto exit;
		}
		switch (choice) // checking the choice and reacting accordingly
		{
		case 1:
			envoyer_liste(info->fdclt, c);
			numcpt = recevoir_num_comptine(info->fdclt);
			if (numcpt < c->nb)
			{
				pthread_mutex_lock(&lock);
				wrlog(*((struct sockaddr_in *)info->sockaddress), numcpt); // writing in the log if accessing a comptine
				pthread_mutex_unlock(&lock);
				envoyer_comptine(info->fdclt, "comptines", c, numcpt);
			}
			else if (numcpt >= c->nb)
			{
				pthread_mutex_lock(&lock);
				wrlog(*((struct sockaddr_in *)info->sockaddress), -3);
				pthread_mutex_unlock(&lock);
				goto exit;
			}
			break;
		case 2:
			pthread_mutex_lock(&lock);
			dw = download_comptine(info->fdclt, info->path);
			if (dw >= 0)
			{
				*(info->cata) = creer_catalogue(info->path);
				wrlog(*((struct sockaddr_in *)info->sockaddress), -1); // Writing in the log if downloading worked
				dprintf(info->fdclt, "Le fichier a pu étre televersé\n");
			}
			else
			{
				wrlog(*((struct sockaddr_in *)info->sockaddress), -2); // Writing in the log if downloading failed
				dprintf(info->fdclt, "Le fichier n'a pas pu étre televersé\n");
			}
			pthread_mutex_unlock(&lock);
			break;
		case 3:
			envoyer_liste(info->fdclt, c); // listing the comptines
			break;
		case 4:
		goto exit;
			break;
		default:
			goto exit;
		}
	}

exit:
pthread_mutex_lock(&lock);
cltcount--;
pthread_mutex_unlock(&lock);
pthread_exit(NULL);
}
void uint32_to_ip(uint32_t ip, char *output)
{
	sprintf(output, "%d.%d.%d.%d", (ip & 0xFF), (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF); // transform and ip from binary to 3-dotted notation
}
void wrlog(struct sockaddr_in addrsock, int nb)
{
	int logfd = open("log.txt", O_CREAT | O_WRONLY | O_APPEND, 0666); // creating or accessing the log file
	if (logfd < 0)
	{
		perror("Erreur lors de l'ouverture du fichier");
		return;
	}
	time_t current_time;
	struct tm *local_time;
	current_time = time(NULL);
	local_time = localtime(&current_time); // getting the time of access
	char buff[1024], ipclt[15];
	uint32_to_ip(addrsock.sin_addr.s_addr, ipclt);
	if (nb >= 0) // writing on to the buffer
	{
		sprintf(buff, "%s:%u the %02d-%02d-%d at %02d:%02d:%02d, comptine demandée : %d\n", ipclt, addrsock.sin_port, local_time->tm_mday, local_time->tm_mon + 1, local_time->tm_year + 1900, local_time->tm_hour, local_time->tm_min, local_time->tm_sec, nb);
	}
	else if (nb == -1)
	{
		sprintf(buff, "%s:%u the %02d-%02d-%d at %02d:%02d:%02d, Nouvelle comptine écrite.\n", ipclt, addrsock.sin_port, local_time->tm_mday, local_time->tm_mon + 1, local_time->tm_year + 1900, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
	}
	else if (nb == -2)
	{
		sprintf(buff, "%s:%u the %02d-%02d-%d at %02d:%02d:%02d, Tentative d'écriture d'une nouvelle comptine échouée. \n", ipclt, addrsock.sin_port, local_time->tm_mday, local_time->tm_mon + 1, local_time->tm_year + 1900, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
	}
	else
	{
		sprintf(buff, "%s:%u the %02d-%02d-%d at %02d:%02d:%02d, Tentative d'aceés à une comptine échouée. \n", ipclt, addrsock.sin_port, local_time->tm_mday, local_time->tm_mon + 1, local_time->tm_year + 1900, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
	}
	int tmp = write(logfd, buff, strlen(buff) + 1); // writing the buffer on to the file
	if (tmp < 0)
	{
		perror("Erreur lors de l'écriture dans le fichier");
		return;
	}
	else if (tmp < strlen(buff) + 1)
	{
		perror("L'écriture s'est arrêté avant la fin.");
		return;
	}
	close(logfd); // closing the file
}
int download_comptine(int fd, char dir[])
{
	ssize_t tmp, tot = 0; // same principle as afficher comptine with minor tweaks
	char name[256], c;
	do
	{
		if ((tmp = recv(fd, &c, 1, 0)) < 0)
		{
			perror("error while reading");
			return -1;
		}
		name[tot++] = c;
	} while (c != '\0');
	char rpath[512];
	sprintf(rpath, "%s/%s", dir, name);
	int fdf = open(rpath, O_CREAT | O_EXCL | O_WRONLY, 0666);
	if (fdf < 0)
	{
		perror("Error while creating the file.");
		return -1;
	}
	char pprev, prev = '\0', cur = '\0';
	do
	{
		pprev = prev;
		prev = cur;
		if ((tmp = recv(fd, &cur, 1, 0)) < 0)
		{
			perror("recv");
			return 0;
		}
		if (pprev != '\n' || prev != '\n' || cur != '\n')
			dprintf(fdf, "%c", cur);
	} while (pprev != '\n' || prev != '\n' || cur != '\n');
	close(fdf);
	return 0;
}
uint8_t receive_choice(int fd) // just receiving 1 byte from the fd file.
{
	uint8_t choice;
	int s = recv(fd, &choice, 1, 0);
	if (s < 1)
	{
		perror("Choice not received");

		return -1;
	}
	return choice;
}