#include "client.h"
client::packinfo client::Client::SearchPack(int idpack)
{
    for (int i = 0; i < userpacks.size(); i++)
    {
        if (userpacks[i].ptrpack->GetIdPack() == idpack)
        {
            return userpacks[i];
        }
    }
    return packinfo();
}
client::callback client::Client::SearchCallBack(int indexpack){
    for(int i=0;i<callbacks.size();i++){
        if(callbacks[i].callback_id==indexpack){
            return this->callbacks[i];
        }
    }
    return callback();
}
void client::Client::DeleteCallBack(int indexpack){
    for(int i=0;i<callbacks.size();i++){
        if(callbacks[i].callback_id==indexpack){
            callbacks.erase(callbacks.begin()+i);
        }
    }
}
void client::Client::ProcessGetPack()
{
    int sizelast = 0;
    int realsize = 0;
    int length = 0;

    int size = 0;
    while (processgetpack == true)
    {
        if (sizelast == 0)
        {
            char buffheadpack[4];
            length = recv(sock_conn, buffheadpack, 4, NULL);
            if (length == 4)
            {
                int sizepack = -1;
                memcpy(&sizepack, buffheadpack, 4);
                length = recv(sock_conn, &recvbuffer[0], sizepack, NULL);
                if (length != sizepack)
                {
                    sizelast = sizepack;
                    realsize = length;
                }
                else
                {
                    size = sizepack;
                }
            }
            else
            {
                std::cout << "ERROR SERVER";
            }
        }
        else
        {
            length = recv(sock_conn, &recvbuffer[realsize], sizelast - realsize, NULL);
            realsize += length;
            if (sizelast == realsize)
            {
                // пакет повний
                size = sizelast;
                sizelast = 0;
                realsize = 0;
            }
        }
        if (sizelast == 0 && length > 0)
        {
            // обработка пакету
            recvbuffer[size - 1] = '\0';
            json_value *val = json_parse(&recvbuffer[0], size);
            if (val != NULL)
            {
                json_object_entry *jidpack = pars::jsonGetNameKey(val, "idpack");
                json_object_entry *jindexpack = pars::jsonGetNameKey(val, "indexpack");
                if (jidpack != NULL && jidpack->value->type == json_integer)
                {
                    int idpack = jidpack->value->u.integer;
                    packinfo info = SearchPack(idpack);
                    pack *pk = NULL;
                    pk = (pack *)malloc(info.sizepack);
                    pk->InitPack();
                    bool j = pk->JsonToData(val);
                    if (!j)
                    {
                        free(pk);
                        std::cout << "ERROR PARSE VALUE PACK\n";
                        // printf("ERROR PARSE VALUE PACK\n");
                        break;
                    }

                    if (jindexpack != NULL && jindexpack->value->type == json_integer && jindexpack->value->u.integer != -1)
                    {
                        // res
                        int indexpack = jindexpack->value->u.integer;
                        callback clb=SearchCallBack(indexpack);
                        if (clb.ProcessPack != NULL)
                        {
                            clb.ProcessPack(pk);
                            DeleteCallBack(indexpack);
                        }
                    }
                    else
                    {
                        // update
                        pk->ProcessPack();
                        
                    }
                    free(pk);
                }
                else
                {
                    std::cout << "ERROR idpack\n";
                    break;
                }
            }
            else
            {
                std::cout << "ERROR PARSE PACK\n";
                break;
            }

            // кінець обработки
        }
        if (length == 0)
        {
            std::cout << "DISCONNECT\n";
            break;
        }
        else if (length < 0)
        {
            std::cout << "ERROR RECV\n";
            break;
        }
    }
}