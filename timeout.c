#include <sys/time.h>

#define TIMEOUT 5

int main(){

    struct timeval  timer;
    timer.tv_sec=0;
    timer.tv_usec=TIMEOUT;
    fd_set read_fds; 
     if ((status=select(fd+1,&read_fds,NULL,NULL,&timer))==-1){  //select() failed
            perror("select");
            exit(4);
        }
        else if (status==0){  //timeout occured
            printf("rdt_send: Timeout!! Retransmit the packet (seq# %d) again\n",current_pkt);
            udt_send(fd,packet,pktLen,0);   //retransmit the packet
            continue; //restart select() & timeout timer
        }
        if (FD_ISSET(fd,&read_fds)){ //packet arrival
        }
    return 0;
}