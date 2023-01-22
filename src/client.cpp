#include "client.h"
int client::Client::sendall(char* buf, int len) {
  int total = 0; // сколько байт мы послали
  int bytesleft = len; // сколько байт осталось послать
  int n;
  while(total < len) {
  n = send(sock_conn, buf+total, bytesleft, MSG_NOSIGNAL);
  if (n == -1) { break; }
  total += n;
  bytesleft -= n;
  }
  return n==-1?-1:0; // вернуть -1 при сбое, 0 при успехе
}
// int client::Client::sender(char* buf, int len) {
//   int mylen=len;
// 	int res=-1;
// 	do
// 	{
// 		mylen=len;
// 		res=cl_sendall(buf,&mylen);
// 		if(res==-1){
// 			show_m("NONE CONNECT\n",true);
// 			close(client->sock_conn);
// 			client->sock_conn=-1;
// 			cl_ClientConnect(client,client->ip,client->port);
			
// 		}
// 	} while (res!=0);
	 
	
// 	return mylen;
// }

client::pack_req_info client::Client::SearchPack(int idpack) {
  for (int i = 0; i < userpacks.size(); i++) {
    if (userpacks[i].ptrpack->GetIdPack() == idpack) {
      return userpacks[i];
    }
  }
  return pack_req_info();
}
client::callback client::Client::SearchCallBack(int indexpack) {
  for (int i = 0; i < callbacks.size(); i++) {
    if (callbacks[i].callback_id == indexpack) {
      return this->callbacks[i];
    }
  }
  return callback();
}
void client::Client::DeleteCallBack(int indexpack) {
  for (int i = 0; i < callbacks.size(); i++) {
    if (callbacks[i].callback_id == indexpack) {
      callbacks.erase(callbacks.begin() + i);
    }
  }
}
void client::Client::ProcessGetPack() {
  int sizelast = 0;
  int realsize = 0;
  int length = 0;

  int size = 0;
  while (processgetpack == true) {
    if (sizelast == 0) {
      char buffheadpack[4];
      length = recv(sock_conn, buffheadpack, 4, NULL);
      if (length == 4) {
        int sizepack = -1;
        memcpy(&sizepack, buffheadpack, 4);
        length = recv(sock_conn, &recvbuffer[0], sizepack, NULL);
        if (length != sizepack) {
          sizelast = sizepack;
          realsize = length;
        } else {
          size = sizepack;
        }
      } else {
        std::cout << "ERROR SERVER";
      }
    } else {
      length =
          recv(sock_conn, &recvbuffer[realsize], sizelast - realsize, NULL);
      realsize += length;
      if (sizelast == realsize) {
        // пакет повний
        size = sizelast;
        sizelast = 0;
        realsize = 0;
      }
    }
    if (sizelast == 0 && length > 0) {
      // обработка пакету
      recvbuffer[size - 1] = '\0';
      json_value* val = json_parse(&recvbuffer[0], size);
      if (val != NULL) {
        json_object_entry* jidpack = pars::jsonGetNameKey(val, "idpack");
        json_object_entry* jindexpack = pars::jsonGetNameKey(val, "indexpack");
        if (jidpack != NULL && jidpack->value->type == json_integer) {
          int idpack = jidpack->value->u.integer;
          pack_req_info info = SearchPack(idpack);
          pack_req* pk = NULL;
          pk = (pack_req*)malloc(info.sizepack);
          pk->InitPack();
          bool j = pk->JsonToData(val);
          if (!j) {
            free(pk);
            std::cout << "ERROR PARSE VALUE PACK\n";
            // printf("ERROR PARSE VALUE PACK\n");
            break;
          }

          if (jindexpack != NULL && jindexpack->value->type == json_integer &&
              jindexpack->value->u.integer != -1) {
            // res
            int indexpack = jindexpack->value->u.integer;
            callback clb = SearchCallBack(indexpack);
            if (clb.ProcessPack != NULL) {
              clb.ProcessPack(pk);
              DeleteCallBack(indexpack);
            }
          } else {
            // update
            pk->ProcessPack();
          }
          free(pk);
        } else {
          std::cout << "ERROR idpack\n";
          break;
        }
      } else {
        std::cout << "ERROR PARSE PACK\n";
        break;
      }

      // кінець обработки
    }
    if (length == 0) {
      std::cout << "DISCONNECT\n";
      break;
    } else if (length < 0) {
      std::cout << "ERROR RECV\n";
      break;
    }
  }
}
client::Client::Client() {
  recvbuffer.resize(100000);
  processgetpack = false;
}
client::Client::Client(std::string ip, int port) : Client() {
  ConnectToServer(ip, port, false);
}
void client::Client::AddPack(pack_req* p, int sizepack) {
  pack_req_info info;
  info.ptrpack = (pack_req*)malloc(sizepack);
  memcpy(info.ptrpack, p, sizepack);
  free(p);
  userpacks.push_back(info);
}

void client::Client::ConnectToServer(std::string ip, int port, bool loop) {
  this->port = port;
  this->ip = ip;
  // Create socket
  sock_conn = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_conn == -1) {
    throw NetworkExeption("ERROR SOCKET\n");
  }
  address.sin_addr.s_addr = inet_addr(ip.c_str());
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  if (loop) {
    int r = -1;
    while (r < 0) {
      r = connect(sock_conn, (struct sockaddr*)&address, sizeof(address));
    }
  } else {
    if (connect(sock_conn, (struct sockaddr*)&address, sizeof(address)) < 0) {
      throw NetworkExeption("ERROR Connect\n");
    }
  }

  printf("OK Connect\n");
}
int client::Client::GetCallbackId() {
  if (callbacks.size() != 0) {
    int r = callbacks[callbacks.size() - 1].callback_id + 1;
    return r;
  } else {
    return 1;
  }
}
void client::Client::SendPack(pack_res* res, void (*Res)(void* pack)) {
  int callback_id = -1;
  if (Res != NULL) {
    callback cl;
    callback_id = GetCallbackId();
    cl.callback_id = callback_id;
    cl.ProcessPack = Res;
    callbacks.push_back(cl);
  }
  pars::json_construct json = res->GetJsonPack();
  pars::json_item jidpack;
  jidpack.init("idpack", res->GetIdPack());
  pars::json_item jcallback_id;
  jcallback_id.init("indexpack", callback_id);
  json.addelement(jidpack);
  json.addelement(jcallback_id);
  std::string send = json.getstringofsend();
  // printf("DATA: %s\n",&d[4]);
  sendall(&send[0], send.size());
}
void client::Client::StartGetPack() {
  processgetpack = true;
  std::thread th(&Client::ProcessGetPack, this);
  th.detach();
}
void client::Client::StopGetPack() {
  processgetpack = false;
}
client::Client::~Client() {
  StopGetPack();
  for (int i = 0; i < userpacks.size(); i++) {
    free(userpacks[i].ptrpack);
  }
}