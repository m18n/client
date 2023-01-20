#pragma once
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include"exeception.h"
#include<iostream>
#include<vector>
#include<thread>
#include"parser.h"
namespace client{
    class pack{
    protected:
        int idpack;
    public:
        virtual bool JsonToData(json_value* json)=0;
        virtual void ProcessPack()=0;
        virtual void InitPack()=0;
        int GetIdPack(){
            return idpack;
        }
    };
    struct callback{
        int callback_id=0;
        void(*ProcessPack)(pack* pack)=NULL;
    };
    struct packinfo{
        pack* ptrpack=NULL;
        int sizepack=0;
    };
    class Client{
    private:
        int sock_conn;
        std::string ip;
        int port;
        struct sockaddr_in address;
        std::vector<packinfo> userpacks;
        std::vector<callback> callbacks;
        bool processgetpack;
        std::vector<char>recvbuffer;
    private:
        int sendall(char *buf, int *len);
        int sender(char* buf,int len);
        void ProcessGetPack();
        packinfo SearchPack(int idpack);
        callback SearchCallBack(int indexpack);
        void DeleteCallBack(int indexpack);
    public:
        Client(){
            recvbuffer.resize(100000);
            processgetpack=false;
        }
        Client(std::string ip,int port):Client(){
            ConnectToServer(ip,port,false);
        }
        void AddPack(pack* p,int sizepack){
            packinfo info;
            info.ptrpack=(pack*)malloc(sizepack);
            memcpy(info.ptrpack,p,sizepack);
            free(p);
            userpacks.push_back(info);
            
        }

        void ConnectToServer(std::string ip,int port,bool loop){
            this->port=port;
            this->ip=ip;
            //Create socket
            sock_conn = socket(AF_INET , SOCK_STREAM , 0);
            if (sock_conn == -1)
            {
                throw NetworkExeption("ERROR SOCKET\n");
            }
            address.sin_addr.s_addr = inet_addr(ip.c_str());
            address.sin_family = AF_INET;
            address.sin_port = htons(port);
            if(loop){
                int r=-1;
                while(r<0){
                    r=connect(sock_conn, (struct sockaddr *)&address, sizeof(address));
                }
            }else{
                if(connect(sock_conn, (struct sockaddr *)&address, sizeof(address))<0){
                    throw NetworkExeption("ERROR Connect\n");
                }
            }
        
            printf("OK Connect\n");
        }
        
        void StartGetPack(){
            processgetpack=true;
            std::thread th(&Client::ProcessGetPack,this);
            th.detach();
        }
        void StopGetPack(){
            processgetpack=false;
        }
        ~Client(){
            StopGetPack();
            for(int i=0;i<userpacks.size();i++){
                free(userpacks[i].ptrpack);
            }
        }
    };
};