//Nicola Sebastianelli 0000722894 Esercitazione 3.1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define DIM_BUFF 256

int main(int argc, char *argv[])
{
	int sd, port, fd_sorg, fd_dest, nread;
	char buff[DIM_BUFF],car,c;
	// FILENAME_MAX: lunghezza massima nome file. Costante di sistema.
	char nome_sorg[FILENAME_MAX+1], nome_dest[FILENAME_MAX+1];
	struct hostent *host;
	struct sockaddr_in servaddr;


	/* CONTROLLO ARGOMENTI ---------------------------------- */
	if(argc!=3){
		printf("Error:%s serverAddress serverPort\n", argv[0]);
		exit(1);
	}

	/* INIZIALIZZAZIONE INDIRIZZO SERVER -------------------------- */
	memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	host = gethostbyname(argv[1]);

	/*VERIFICA INTERO*/
	nread=0;
	while( argv[2][nread]!= '\0' ){
		if( (argv[2][nread] < '0') || (argv[2][nread] > '9') ){
			printf("Secondo argomento non intero\n");
			exit(2);
		}
		nread++;
	}
	port = atoi(argv[2]);

	/* VERIFICA PORT e HOST */
	if (port < 1024 || port > 65535){
		printf("%s = porta scorretta...\n", argv[2]);
		exit(2);
	}
	if (host == NULL){
		printf("%s not found in /etc/hosts\n", argv[1]);
		exit(2);
	}else{
		servaddr.sin_addr.s_addr=((struct in_addr *)(host->h_addr))->s_addr;
		servaddr.sin_port = htons(port);
	}

	/* CORPO DEL CLIENT:
	ciclo di accettazione di richieste da utente ------- */
	printf("Ciclo di richieste di raddoppio carattere a EOF\n");
	printf("Nome del file da raddoppiare, EOF per terminare: ");
	while (gets(nome_sorg)){
		printf("File da inviare: __%s__\n", nome_sorg);
		/* Verifico l'esistenza del file */
		if((fd_sorg=open(nome_sorg, O_RDONLY))<0){
			perror("open file sorgente");
			printf("Nome del file da raddoppiare, EOF per terminare: ");
			continue;
		}

		printf("Inserire carattere da inviare, EOF per fine:");
				if(scanf(" %c",&car)== EOF )
					break;
				while ((c = getchar()) != '\n' && c != EOF);
				printf("Carattere da inviare: __%c__\n", car);

		printf("Nome del file raddoppiato: ");
				if (gets(nome_dest)==0) break;

				/*Verifico creazione file*/
				if((fd_dest=open(nome_dest, O_WRONLY|O_CREAT, 0644))<0){
					perror("open file destinatario");
					printf("Nome del file da raddoppiare, EOF per terminare: ");
					continue;
				}

		/* CREAZIONE SOCKET ------------------------------------ */
		sd=socket(AF_INET, SOCK_STREAM, 0);
		if(sd<0) {perror("apertura socket"); exit(1);}
		printf("Client: creata la socket sd=%d\n", sd);

		/* Operazione di BIND implicita nella connect */
		if(connect(sd,(struct sockaddr *) &servaddr, sizeof(struct sockaddr))<0)
		{perror("connect"); exit(1);}
		printf("Client: connect ok\n");

		/*INVIO File*/
		printf("Client: invio carattere %c\n",car);
		write(sd,&car,sizeof(char));
		printf("Client: invio file da raddoppiare\n");
		while((nread=read(fd_sorg, buff, DIM_BUFF))>0){
			write(1,buff,nread);	//stampa
			write(sd,buff,nread);	//invio
		}
		printf("Client: file inviato\n");
		/* Chiusura socket in spedizione -> invio dell'EOF */
		shutdown(sd,1);

		/*RICEZIONE File*/
		printf("Client: ricevo e stampo file raddoppiato\n");
		while((nread=read(sd,&c,sizeof(char)))>0){
			write(fd_dest,&c,nread);
			write(1,&c,nread);
		}
		printf("Traspefimento terminato\n");
		/* Chiusura socket in ricezione */
		shutdown(sd, 0);
		/* Chiusura file */
		close(fd_sorg);
		close(fd_dest);

		printf("Nome del file da raddoppiare, EOF per terminare: ");
	}//while
	printf("\nClient: termino...\n");
	exit(0);
}



