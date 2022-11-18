#pragma once
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"parser.h"
#include<stdbool.h>
typedef struct packreq{
    int indexpack;
    bool(*JsonToObject)(struct packreq* pack,json_value* json);
    void(*ProcessPack)(struct packreq* pack);
}packreq_t;
void CreatePackReq(packreq_t* pack);
typedef struct infopackreq{
    int sizepack;
    int idpack;
    void(*CreatePack)(packreq_t* req);
}infopackreq_t;
void CreateInfoPackReq(infopackreq_t* self);
typedef struct inforesfunction{
    void(*Result)(packreq_t* pack);
    int indexpack;
}inforesfunction_t;
void CreateInfoResFunction(inforesfunction_t* self);
typedef struct client{
    int sock_conn;
    char ip[200];
    struct sockaddr_in address;
    int port;
    int sizepacks;
    arrayd_t userpacks;
    arrayd_t resfunction;
}client_t;
int sendall(client_t* client, char *buf, int *len);
int sender(client_t* client,char* buf,int len);
infopackreq_t client_getinfopackbyid(client_t* cl,int idpack);
inforesfunction_t client_getinfofunctionbyindex(client_t* cl,int indexpack);
void client_addresfunction(client_t* cl,inforesfunction_t info);
void client_adduserpacks(client_t* cl,void(*CreatePack)(packreq_t* self),int sizeuserpack,int idpack);
void GetPacks(client_t* client);
int ClientConnect(client_t* client,const char* ip,int port);
void ProcessPacks(client_t* client);
// void GetPacks(v2_t* v);
typedef struct packres{
    int idpack;
    json_construct_t(*GetJsonPack)(struct packres* pk);
}packres_t;
void CreatePackRes(packres_t* pack);
void SendPack(client_t* client,packres_t* pack,void(*Result)(packreq_t* pack));
// typedef struct pk_start_send_file{
//     pack_t pk;
//     char namefile[200];
//     int sizefile;
//     char* file;
// }pk_start_send_file_t;