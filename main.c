#include<stdio.h>
#include"include/client.h"
#include"pthread.h"

int main(){
    printf("HELLO ");
    client_t client;
    ClientConnect(&client,"192.168.0.105",9999);
    GetPacks(&client);
    printf("NICE\n");
    scanf("%d");

    return 0;
}