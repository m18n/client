#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <vector>

#include "exeception.h"
#include "parser.h"
namespace client {
class pack_req {
 protected:
  int idpack;

 public:
  virtual bool JsonToData(json_value* json) = 0;
  virtual void ProcessPack() = 0;
  int GetIdPack() { return idpack; }
};
class pack_res {
 protected:
  int idpack=-1;

 public:
 int GetIdPack(){
    return idpack;
 }
  virtual pars::json_construct GetJsonPack() = 0;
};
struct callback {
  int callback_id = 0;
  void (*ProcessPack)(void* pack) = NULL;
};
struct pack_req_info {
  pack_req* ptrpack = NULL;
  int sizepack = 0;
};
class Client {
 private:
  int sock_conn;
  std::string ip;
  int port;
  struct sockaddr_in address;
  std::vector<pack_req_info> userpacks;
  std::vector<callback> callbacks;
  bool processgetpack;
  std::vector<char> recvbuffer;

 private:
  int sendall(char* buf, int len,int* t);
  void ProcessGetPack();
  pack_req_info SearchPack(int idpack);
  callback SearchCallBack(int indexpack);
  void DeleteCallBack(int indexpack);
  int GetCallbackId();
 public:
  Client();
  Client(std::string ip, int port);
  void AddPack(pack_req* p, int sizepack);
  void ConnectToServer(std::string ip, int port, bool loop);
  
  void SendPack(pack_res* res,void(*Res)(void* pack));
  void StartGetPack();
  void StopGetPack();
  ~Client();
};
};  // namespace client