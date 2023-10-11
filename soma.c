int checksum(dado){
    //count tamanho do buffer
    register long sum = 0;
    while( count > 1 )  { 
        sum += * (unsigned short) addr++;
        count -= 2;
    }
    if( count > 0 ){
        sum += * (unsigned char *) addr;    
    }
    while (sum>>16){
        sum = (sum & 0xffff) + (sum >> 16);
    } 
    checksum = ~sum;
    return 1; //deu certo
}