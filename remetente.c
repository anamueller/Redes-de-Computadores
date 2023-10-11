#include <stdio.h>			/* See NOTES */
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_LEN 1000
#define TIMEOUT 5

void use_data_in_aplication(char* ip_sender, int porta, char* texto);
int test_seq_Number(int texto, int seqNumber);

typedef struct{
	char texto[MAX_LEN];
	int seqNumber;
	int checksum;
} mensagem;

int seqNumberVariable = 0;

int rdt_recv_ACK(int fd, int porta, int seqNumber){
	int recv_ACK;
	struct sockaddr_in caddr;
	caddr.sin_family = AF_INET;
	caddr.sin_port = htons(porta);
	caddr.sin_addr.s_addr = INADDR_ANY; 

	mensagem msg;

	int nr; 
	socklen_t caddr_len;

	bzero(&msg, sizeof(mensagem));	
	caddr_len = sizeof(caddr);
	nr = recvfrom(fd, (void *)&msg, sizeof(mensagem), 0, (struct sockaddr *)&caddr,&caddr_len);

	if(nr > 0){
		use_data_in_aplication(inet_ntoa(caddr.sin_addr),htons(caddr.sin_port),msg.texto);
		recv_ACK = test_seq_Number(atoi(msg.texto), seqNumber);
	}
	else if(nr == -1){
		perror("recvfrom()");
		return -1;
	}
	
	return recv_ACK;
}
	
void use_data_in_aplication(char* ip_sender, int porta, char* texto){
	printf("%s:%d -- %s \n", ip_sender, porta, texto);
}	

int test_seq_Number(int texto, int seqNumber){
	if(texto == seqNumber){
		return 2;
	}
	else{
		return -2;
	}
}

int checksum(int count){
    return 1; //deu certo
}

int rdt_send(int sd, int porta, char *ip, void *buf, int buf_size){
	
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(porta);
	saddr.sin_addr.s_addr = inet_addr(ip);

	socklen_t saddr_len;
	saddr_len = sizeof(saddr);
	
	mensagem msg;
	/*monta_mensagem_begining*/
	bzero(&msg, sizeof(mensagem));
	memcpy(msg.texto, buf, buf_size);
	msg.seqNumber = seqNumberVariable;
	seqNumberVariable = (seqNumberVariable + 1)%2;
	/*monta_mensagem_ending*/

	if(sendto(sd, &msg, sizeof(mensagem), 0, (struct sockaddr *)&saddr,saddr_len) == -1){
		perror("sendto");
		return -1;
	}

	struct timeval timer;
    timer.tv_sec=0;
    timer.tv_usec=TIMEOUT;
    fd_set read_fds;
	int status = select(sd+1, &read_fds, NULL, NULL, &timer);
	if (status==-1){  //select() deu erro
        perror("select");
        exit(4);
    }

	/*recv_ack_begining*/
	if(rdt_recv_ACK(sd, porta, msg.seqNumber) == 2 && status!=0){ //certo
		if(checksum(buf_size)==1){
			return 0;
		}
		else{//retransmitir porque o pacote foi corrompido
			printf("mensagem corrompida");
			timer.tv_sec = 0;
	  		timer.tv_usec = TIMEOUT; //recomeçar tudo
			rdt_send(sd, porta, ip, buf, buf_size);
		}
	}
	else{
		//retransmitir pacote que nao chegou ao destinatario
        timer.tv_sec = 0;
	  	timer.tv_usec = TIMEOUT; //recomeçar tudo
		rdt_send(sd, porta, ip, buf, buf_size);
	}
	/*recv_ack_endning*/
	return 0;
}

int main(int argc, char **argv) {
	if(argc != 2){
		printf("%s <porta>\n",argv[0]);
		return 0;
	}
	int sd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (sd == -1){
		perror("socket()");
		return -1;
	}
	char* ip = "10.0.0.24";
	char* buf = "Meu teste";
	//int porta = atoi(argv[1]);//htons eh feita nas rdts()

	rdt_send(sd,2000,ip, buf, strlen(buf));

	return 0;
}