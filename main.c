#include<stdio.h>
#include"include/client.h"
#include"pthread.h"

int main(){
    printf("HELLO ");
    cl_client_t client;
    cl_ClientConnect(&client,"192.168.0.105",9999);
    cl_GetPacks(&client);
    printf("NICE\n");
    scanf("%d");

    return 0;
}