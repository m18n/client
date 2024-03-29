#include"client.h"
bool cl_show_log=true;
void show_m(char* msg,bool error){
	if(error==true){
		printf("ERROR: %s",msg);
	}else{
		if(cl_show_log==true){
        	printf("LOG: %s",msg);
    	} 
	}
}
void cl_CreatePackReq(cl_packreq_t* pack){
    pack->indexpack=-1;
    pack->JsonToObject=NULL;
    pack->ProcessPack=NULL;
}
int cl_client_addresfunction(cl_client_t* cl,void(*Result)(cl_packreq_t* pack,void* data),void* data){
	cl_inforesfunction_t* fun=cl->resfunction.data;
    for(int i=0;i<cl->resfunction.realsize;i++){
        if(fun[i].Result==NULL){
			fun[i].indexpack=i;
			fun[i].Result=Result;
			fun[i].data=data;
            return i;
        }
    }
	cl_inforesfunction_t info;
	info.indexpack=cl->resfunction.realsize;
	info.Result=Result;
	info.data=data;
	cl_arrayd_addelement(&cl->resfunction,&info);
	return info.indexpack;
}
void cl_client_adduserpacks(cl_client_t* cl,void(*CreatePack)(cl_packreq_t* self),int sizeuserpack,int idpack){
	cl_infopackreq_t info;
    info.CreatePack=CreatePack;
    info.idpack=idpack;
    info.sizepack=sizeuserpack;
    cl_arrayd_addelement(&cl->userpacks,&info);
}
void cl_InitClient(cl_client_t* client){
	pthread_mutex_init(&client->sendmutex, NULL);
	cl_InitArrayd(&client->userpacks,0,sizeof(cl_infopackreq_t));
	cl_InitArrayd(&client->resfunction,0,sizeof(cl_inforesfunction_t));
}

int cl_ClientConnect(cl_client_t* client,const char* ip,int port){
	
	client->port=port;
    strcpy(client->ip,ip);
	//Create socket
	client->sock_conn = socket(AF_INET , SOCK_STREAM , 0);
	if (client->sock_conn == -1)
	{
		show_m("Could not create socket\n",true);
		return -1;
	}
		
	client->address.sin_addr.s_addr = inet_addr(client->ip);
	client->address.sin_family = AF_INET;
	client->address.sin_port = htons(client->port);
    client->sizepacks=100;

	//Connect to remote server
	if (connect(client->sock_conn, (struct sockaddr *)&client->address, sizeof(client->address)) < 0)
	{
		show_m("connect error\n",true);
		client->sock_conn=-1;
		return 1;
	}
	pthread_t process_pack;
	pthread_create( &process_pack, NULL, cl_StartProcess, (void*) client);
	return 0;
}
void cl_CreatePackRes(cl_packres_t* pack){
    pack->idpack=-1;
    pack->GetJsonPack=NULL;
}
void cl_CreateInfoPackReq(cl_infopackreq_t* self){
    self->CreatePack=NULL;
    self->sizepack=0;
    self->idpack=-1;
}
void cl_CreateInfoResFunction(cl_inforesfunction_t* self){
	self->Result=NULL;
	self->indexpack=-1;
	self->data=NULL;
}

int cl_sendall(cl_client_t* client, char *buf, int *len)
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
int cl_sender(cl_client_t* client,char* buf,int len){
	int mylen=len;
	int res=-1;
	do
	{
		mylen=len;
		res=cl_sendall(client,buf,&mylen);
		if(res==-1){
			show_m("NONE CONNECT\n",true);
			close(client->sock_conn);
			client->sock_conn=-1;
			cl_ClientConnect(client,client->ip,client->port);
			
		}
	} while (res!=0);
	 
	
	return mylen;
}

void cl_SendPack(cl_client_t* client,cl_packres_t* pack,void(*Result)(cl_packreq_t* pack,void* data),void* data){
    int indexpack=-1;
	if(Result!=NULL){
		indexpack=cl_client_addresfunction(client,Result,data);
	}
    cl_json_construct_t j=pack->GetJsonPack(pack);
    cl_json_item_t jidpack;
    cl_CreateJson_Item(&jidpack);
    cl_json_item_setname(&jidpack,"idpack");
    cl_json_item_setintvalue(&jidpack,pack->idpack);
    cl_json_construct_addelement(&j,jidpack);
    cl_json_item_t jindexpack;
    cl_CreateJson_Item(&jindexpack);
    cl_json_item_setname(&jindexpack,"indexpack");
    cl_json_item_setintvalue(&jindexpack,indexpack);
    cl_json_construct_addelement(&j,jindexpack);
	int size=0;
    char* d=cl_json_construct_getstring_SEND(&j,&size);
    //printf("DATA: %s\n",&d[4]);
	pthread_mutex_lock(&client->sendmutex);
	cl_sender(client,d,size);
	pthread_mutex_unlock(&client->sendmutex);
    free(d);
    cl_DestroyJson_Construct(&j);
	//printf("SEND PACK\n");	
}
void cl_SendJson(cl_client_t* client,char* json,int length){
	char datajson[2000];
	datajson[4]='\0';
	char* startdata=&datajson[4];
	memcpy(startdata,json,length);
	int32_t size=length;
	memcpy(datajson,&size,4);
	size+=4;
	//printf("DATA %s\n",&datajson[4]);
	cl_sendall(client->sock_conn,datajson,&size);
}
cl_infopackreq_t cl_client_getinfopackbyid(cl_client_t* cl,int idpack){
	cl_infopackreq_t* array=cl->userpacks.data;
	cl_infopackreq_t res;
	cl_CreateInfoPackReq(&res);
	for(int i=0;i<cl->userpacks.realsize;i++){
		if(array[i].idpack==idpack){
			return array[i];
		}
	}
	return res;	
}
cl_inforesfunction_t cl_client_get_infofunction_byindex(cl_client_t* cl,int indexpack){
	cl_inforesfunction_t* array=cl->resfunction.data;
	cl_inforesfunction_t res;
	cl_CreateInfoResFunction(&res);
	for(int i=0;i<cl->resfunction.realsize;i++){
		if(array[i].indexpack==indexpack){
			return array[i];
		}
	}
	return res;	
}
void cl_client_infofunction_finish(cl_client_t* cl,int indexpack){
	cl_inforesfunction_t* array=cl->resfunction.data;
	for(int i=0;i<cl->resfunction.realsize;i++){
		if(array[i].indexpack==indexpack){
			array[i].Result=NULL;
			return;
		}
	}
}
void cl_StartProcess(cl_client_t* client){
	
	
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
				show_m("ERROR SERVER\n",true);
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
            json_value* val=json_parse(data,size);
            if(val!=NULL){
                json_object_entry* jidpack=cl_GetNameKey(val,"idpack");
				json_object_entry* jindexpack=cl_GetNameKey(val,"indexpack");
                if(jidpack!=NULL&&jidpack->value->type==json_integer){
					int idpack=jidpack->value->u.integer;
					cl_infopackreq_t info=cl_client_getinfopackbyid(client,idpack);
					cl_packreq_t* pack=NULL;
					if(info.CreatePack!=NULL){
						pack=malloc(info.sizepack);
						info.CreatePack(pack);
						bool j=pack->JsonToObject(pack,val);
						if(!j){
							free(pack);
							show_m("ERROR PARSE VALUE PACK\n",true);
							//printf("ERROR PARSE VALUE PACK\n");
							break;
						}
					}else{
						show_m("ERROR SEARCH PACK\n",true);
						break;
					}
					if(jindexpack!=NULL&&jindexpack->value->type==json_integer&&jindexpack->value->u.integer!=-1){
						//res
						int indexpack=jindexpack->value->u.integer;
						cl_inforesfunction_t infofun=cl_client_get_infofunction_byindex(client,indexpack);
						if(infofun.Result!=NULL){
							infofun.Result(pack,infofun.data);
							cl_client_infofunction_finish(client,indexpack);
						}
					}else{
						//update
						pack->ProcessPack(pack);
					}
					free(pack);
                }else{
                    show_m("ERROR idpack\n",true);
					break;
                }
            }else{
                show_m("ERROR PARSE PACK\n",true);
				break;
            }  
            
            //кінець обработки
        }
		if(length==0){
            show_m("DISCONNECT\n",false);
            break;
        }else if(length<0){
           	show_m("ERROR RECV\n",true);
            break;
        }
            
    }
	close(client->sock_conn);
	free(data);
}

