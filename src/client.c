#include"client.h"
void CreatePackReq(packreq_t* pack){
    pack->indexpack=-1;
    pack->JsonToObject=NULL;
    pack->ProcessPack=NULL;
}
void client_addresfunction(client_t* cl,inforesfunction_t info){
	arrayd_addelement(&cl->resfunction,&info);
}
void client_adduserpacks(client_t* cl,void(*CreatePack)(packreq_t* self),int sizeuserpack,int idpack){
	infopackreq_t info;
    info.CreatePack=CreatePack;
    info.idpack=idpack;
    info.sizepack=sizeuserpack;
    arrayd_addelement(&cl->userpacks,&info);
}
int ClientConnect(client_t* client,const char* ip,int port){
	InitArrayd(&client->userpacks,0,sizeof(infopackreq_t));
	InitArrayd(&client->resfunction,0,sizeof(inforesfunction_t));
	client->port=port;
    strcpy(client->ip,ip);
	//Create socket
	client->sock_conn = socket(AF_INET , SOCK_STREAM , 0);
	if (client->sock_conn == -1)
	{
		printf("Could not create socket");
		return -1;
	}
		
	client->address.sin_addr.s_addr = inet_addr(client->ip);
	client->address.sin_family = AF_INET;
	client->address.sin_port = htons(client->port);
    client->sizepacks=100;

	//Connect to remote server
	if (connect(client->sock_conn, (struct sockaddr *)&client->address, sizeof(client->address)) < 0)
	{
		puts("connect error");
		client->sock_conn=-1;
		return 1;
	}
	
	printf("CONNECT\n");
	return 0;
}
void CreatePackRes(packres_t* pack){
    pack->idpack=-1;
    pack->GetJsonPack=NULL;
}
void CreateInfoPackReq(infopackreq_t* self){
    self->CreatePack=NULL;
    self->sizepack=0;
    self->idpack=-1;
}
void CreateInfoResFunction(inforesfunction_t* self){
	self->Result=NULL;
	self->indexpack=-1;
}

int sendall(client_t* client, char *buf, int *len)
{
 int total = 0; // сколько байт мы послали
 int bytesleft = *len; // сколько байт осталось послать
 int n;
 while(total < *len) {
 n = send(client->sock_conn, buf+total, bytesleft, MSG_NOSIGNAL);
 if (n == -1) { break; }
 total += n;
 bytesleft -= n;
 }
 *len = total; // здесь количество действительно посланных байт
 return n==-1?-1:0; // вернуть -1 при сбое, 0 при успехе
}
int sender(client_t* client,char* buf,int len){
	int mylen=len;
	int res=-1;
	do
	{
		mylen=len;
		res=sendall(client,buf,&mylen);
		if(res==-1){
			printf("NONE CONNECT\n");
			close(client->sock_conn);
			client->sock_conn=-1;
			ClientConnect(client,client->ip,client->port);
			
		}
	} while (res!=0);
	 
	
	return mylen;
}

void SendPack(client_t* client,packres_t* pack,void(*Result)(packreq_t* pack)){
    static int indexpack=0;
    json_construct_t j=pack->GetJsonPack(pack);
    json_item_t jidpack;
    CreateJson_Item(&jidpack);
    json_item_setname(&jidpack,"idpack");
    json_item_setintvalue(&jidpack,pack->idpack);
    json_construct_addelement(&j,jidpack);
    json_item_t jindexpack;
    CreateJson_Item(&jindexpack);
    json_item_setname(&jindexpack,"indexpack");
    json_item_setintvalue(&jindexpack,indexpack);
    json_construct_addelement(&j,jindexpack);
	int size=0;
    char* data=json_construct_getstring_SEND(&j,&size);
    printf("DATA: %s\n",&data[4]);
	inforesfunction_t resf;
	resf.indexpack=indexpack;
	resf.Result=Result;
	client_addresfunction(client,resf);
	sender(client,data,size);
    free(data);
    DestroyJson_Construct(&j);
	printf("SEND PACK\n");
	indexpack++;
	
}
void SendJson(client_t* client,char* json,int length){
	char datajson[2000];
	datajson[4]='\0';
	char* startdata=&datajson[4];
	memcpy(startdata,json,length);
	int32_t size=length;
	memcpy(datajson,&size,4);
	size+=4;
	printf("DATA %s\n",&datajson[4]);
	sendall(client->sock_conn,datajson,&size);
}
infopackreq_t client_getinfopackbyid(client_t* cl,int idpack){
	infopackreq_t* array=cl->userpacks.data;
	infopackreq_t res;
	CreateInfoPackReq(&res);
	for(int i=0;i<cl->userpacks.realsize;i++){
		if(array[i].idpack==idpack){
			return array[i];
		}
	}
	return res;	
}
inforesfunction_t client_getinfofunctionbyindex(client_t* cl,int indexpack){
	inforesfunction_t* array=cl->resfunction.data;
	inforesfunction_t res;
	CreateInfoResFunction(&res);
	for(int i=0;i<cl->resfunction.realsize;i++){
		if(array[i].indexpack==indexpack){
			return array[i];
		}
	}
	return res;	
}
void GetPacks(client_t* client){
	
	
    int sizelast=0;
    int realsize=0;
    int length=0;
	char* data=malloc(100000);
	int size=0;
    while(1){
        if(sizelast==0){
            char buffheadpack[4];
            length=recv(client->sock_conn,buffheadpack,4,NULL);
            if(length==4){
                int sizepack=-1;
                memcpy(&sizepack,buffheadpack,4);
                length=recv(client->sock_conn,data,sizepack,NULL);
                if(length!=sizepack){
                    sizelast=sizepack;
                    realsize=length;
                }else{
					size=sizepack;
				}
            }else{
                printf("ERROR SERVER\n");
            }
        }else{
            length=recv(client->sock_conn,&data[realsize],sizelast-realsize,NULL);
            realsize+=length;
            if(sizelast==realsize){
                //пакет повний
				size=sizelast;
                sizelast=0;
                realsize=0;
                
            }
        }
        if(sizelast==0&&length>0){
            //обработка пакету
			data[size-1]='\0';
            json_value* val=json_parse(data,length);
            if(val!=NULL){
                json_object_entry* jidpack=GetNameKey(val,"idpack");
				json_object_entry* jindexpack=GetNameKey(val,"indexpack");
                if(jidpack!=NULL&&jidpack->value->type==json_integer){
					int idpack=jidpack->value->u.integer;
					infopackreq_t info=client_getinfopackbyid(client,idpack);
					packreq_t* pack=NULL;
					if(info.CreatePack!=NULL){
						pack=malloc(info.sizepack);
						info.CreatePack(pack);
						bool j=pack->JsonToObject(pack,val);
						if(!j){
							free(pack);
							printf("ERROR PARSE VALUE PACK\n");
							break;
						}
					}else{
						printf("ERROR SEARCH PACK\n");
						break;
					}
					if(jindexpack!=NULL&&jindexpack->value->type==json_integer){
						//res
						int indexpack=jindexpack->value->u.integer;
						inforesfunction_t infofun=client_getinfofunctionbyindex(client,indexpack);
						if(infofun.Result!=NULL){
							infofun.Result(pack);
						}
					}else{
						//update
						pack->ProcessPack(pack);
					}
					free(pack);
                }else{
                    printf("ERROR idpack\n");
					break;
                }
            }else{
                printf("ERROR PARSE PACK\n");
				break;
            }  
            
            //кінець обработки
        }
		if(length==0){
            printf("DISCONNECT\n");
            break;
        }else if(length<0){
            printf("ERROR RECV\n");
            break;
        }
            
    }
	close(client->sock_conn);
	free(data);
}

