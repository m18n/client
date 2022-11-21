#pragma once
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"parser.h"
#include<stdbool.h>
typedef struct cl_packreq{
    int indexpack;
    bool(*JsonToObject)(struct cl_packreq* pack,json_value* json);
    void(*ProcessPack)(struct cl_packreq* pack);
}cl_packreq_t;
void cl_CreatePackReq(cl_packreq_t* pack);
typedef struct cl_infopackreq{
    int sizepack;
    int idpack;
    void(*CreatePack)(cl_packreq_t* req);
}cl_infopackreq_t;
void cl_CreateInfoPackReq(cl_infopackreq_t* self);
typedef struct cl_inforesfunction{
    void(*Result)(cl_packreq_t* pack,void* data);
    int indexpack;
    void* data;
}cl_inforesfunction_t;
void cl_CreateInfoResFunction(cl_inforesfunction_t* self);
typedef struct cl_client{
    int sock_conn;
    char ip[200];
    struct sockaddr_in address;
    int port;
    int sizepacks;
    cl_arrayd_t userpacks;
    cl_arrayd_t resfunction;
    pthread_mutex_t sendmutex;
}cl_client_t;
void cl_InitClient(cl_client_t* client);
int cl_sendall(cl_client_t* client, char *buf, int *len);
int cl_sender(cl_client_t* client,char* buf,int len);
cl_infopackreq_t cl_client_getinfopackbyid(cl_client_t* cl,int idpack);
cl_inforesfunction_t cl_client_get_infofunction_byindex(cl_client_t* cl,int indexpack);
void cl_client_infofunction_finish(cl_client_t* cl,int indexpack);
int cl_client_addresfunction(cl_client_t* cl,void(*Result)(cl_packreq_t* pack,void* data),void* data);
void cl_client_adduserpacks(cl_client_t* cl,void(*CreatePack)(cl_packreq_t* self),int sizeuserpack,int idpack);
void cl_StartProcess(cl_client_t* client);
int cl_ClientConnect(cl_client_t* client,const char* ip,int port);
void cl_ProcessPacks(cl_client_t* client);
// void GetPacks(v2_t* v);
typedef struct cl_packres{
    int idpack;
    cl_json_construct_t(*GetJsonPack)(struct cl_packres* pk);
}cl_packres_t;
void cl_CreatePackRes(cl_packres_t* pack);
void cl_SendPack(cl_client_t* client,cl_packres_t* pack,void(*Result)(cl_packreq_t* pack,void* data),void* data);
// typedef struct pk_start_send_file{
//     pack_t pk;
//     char namefile[200];
//     int sizefile;
//     char* file;
// }pk_start_send_file_t;