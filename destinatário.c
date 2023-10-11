#include <stdio.h>			/* See NOTES */
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define MAX_LEN 1000

char* find_seq_number(int seqNumber);
void use_data_in_aplication(char* ip_sender, int porta, char* texto);

typedef struct{
	char texto[MAX_LEN];
	int seqNumber;
	int checksum;
} mensagem;

int seqNumberVariable = 0;

int rdt_send_ACK(int sd, void *buf, int buf_size, struct sockaddr_in saddr){
	
	socklen_t saddr_len;
	saddr_len = sizeof(saddr);

	mensagem msg;

	bzero(&msg, sizeof(mensagem));
	memcpy(msg.texto, buf, buf_size);
		
	if(sendto(sd, &msg, sizeof(mensagem), 0, (struct sockaddr *)&saddr,saddr_len) == -1){
		perror("sendto");
		return -1;
	}

	return 0;
}

int checksum(){
    return 1; //deu certo
}

int rdt_recv(int fd, int porta, struct sockaddr_in saddr, struct sockaddr_in caddr){

	mensagem msg;

	int nr; 
	socklen_t caddr_len;
	//while(1){
		bzero(&msg, sizeof(mensagem));
		caddr_len = sizeof(caddr);
		nr = recvfrom(fd, ( void *)&msg, sizeof(mensagem), 0, (struct sockaddr *)&caddr,&caddr_len);
	
		if(nr > 0){
			use_data_in_aplication(inet_ntoa(caddr.sin_addr),htons(caddr.sin_port),msg.texto);
			char* seqNumber = find_seq_number(msg.seqNumber);
			if(checksum()==1){
				rdt_send_ACK(fd, seqNumber,sizeof(int), caddr);	
			}	
		}
		else if(nr == -1){
			perror("recvfrom()");
			return -1;
		}
	//}
	return 0;
}

void use_data_in_aplication(char* ip_sender, int porta, char* texto){
	printf("%s:%d -- %s \n", ip_sender, porta, texto);
}

char* find_seq_number(int seqNumber){
	if (seqNumber == 1) {
		return "1";
	}
	return "0";
}

int main(int argc, char **argv) {
	/*if(argc != 2){
		printf("%s <porta>\n",argv[0]);
		return 0;
	}*/

	int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd == -1){
		perror("socket()");
		return -1;
	}

	//int porta = atoi(argv[1]);//htons eh feita nas rdts()

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(porta);
	saddr.sin_addr.s_addr = INADDR_ANY;
	struct sockaddr_in caddr;
	caddr.sin_family = AF_INET;
	caddr.sin_port = htons(porta);
	caddr.sin_addr.s_addr = INADDR_ANY; 
	if(bind(fd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1){
		perror("bind()");
		return -1;
	}
	while(1){
		rdt_recv(fd,2000, saddr, caddr);
		sleep(5000);
	}
	return 0;
}