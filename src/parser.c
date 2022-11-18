#include"parser.h"
int cl_StringAddString(char* string,int sizestring,const char* str,int sizestr){
    int length=-2;
    if(sizestring==-1&&sizestr==0){
        length=strlen(string);
        strcpy(&string[length],str);
        length=strlen(string);
    }else if(sizestring==-1&&sizestr>0){
        length=strlen(string);
        memcpy(&string[length],str,sizestr);
        length=length+sizestr;
    }else if(sizestring>=0&&sizestr>0){
        memcpy(&string[sizestring],str,sizestr);
        length=sizestring+sizestr;
    }else if(sizestring>=0&&sizestr==0){
        int strl=strlen(str);
        memcpy(&string[sizestring],str,strl);
        length=sizestring+strl;
    }
   
    return length;
}
int cl_StringAddInt(char* string,int sizestring,int number){
    char buffint[12];
    sprintf(buffint, "%d", number);
    int lenbuffint=strlen(buffint);
    int length=0;
    if(sizestring>0){
        memcpy(&string[sizestring],buffint,lenbuffint);
        length=sizestring+lenbuffint;
    }else{
        length=strlen(string);
        strcpy(&string[length],buffint);
        length=strlen(string);
    }
    return length;
}

int cl_GetLengthInt(int number){
    char buffint[12];
    sprintf(buffint, "%d", number);
    return strlen(buffint);
}

int cl_SearchStringInArray(char* array,int size,int startindex,const char* search,int count)
{
    int len = strlen(search);
    int c = 0;
    
    int g=0;
        for (int i = startindex; i < size; i++)
        {
            for (int j = i; j < size; j++)
            {
                if (array[j] == search[j - i])
                {
                    if (j - i == len - 1)
                    {
                        c++;
                        g=i;
                        if (c == count)
                            return i;
                    }
                }
                else
                {
                    i += j - i;
                    break;
                }
            }
        }
        if(count==-1){
            return g;
        }
        return -1;
    
}
int cl_SearchNameKey(json_value* json,const char* name){
    for(int i=0;i<json->u.object.length;i++){
        if(strcmp(json->u.object.values[i].name,name)==0){
            return i;
        }
    } 
    return -1;
}
json_object_entry* cl_GetNameKey(json_value* json,const char* name){
    for(int i=0;i<json->u.object.length;i++){
        if(strcmp(json->u.object.values[i].name,name)==0){
            return &json->u.object.values[i];
        }
    } 
    return NULL;
}
void cl_CreateArrayD(cl_arrayd_t* array){
    array->allsize=0;
    array->realsize=0;
    array->data=NULL;
    array->sizeelement=0;
}
void cl_InitArrayd(cl_arrayd_t* array,int sizearray,int sizeelement){
    cl_CreateArrayD(array);
    array->data=malloc(sizeelement*sizearray);
    array->allsize=sizearray;
    array->sizeelement=sizeelement;
}
void cl_arrayd_addelement(cl_arrayd_t* array,void* element){
    if(array->allsize==array->realsize){
        array->allsize++;
        void* newarr=malloc(array->allsize*array->sizeelement);
        memcpy(newarr,array->data,array->realsize*array->sizeelement);
        void*el=newarr+array->realsize*array->sizeelement;
        memcpy(el,element,array->sizeelement);
        array->realsize++;
        free(array->data);
        array->data=newarr;
    }else{
        void* index=array->data+array->sizeelement*array->realsize;
        memcpy(index,element,array->sizeelement);
        array->realsize++;
    }
}
void cl_DestroyArrayD(cl_arrayd_t* array){
    free(array->data);
}
void cl_CreateJson_Item(cl_json_item_t* item){
    item->name[0]='\0';
    item->strvalue=NULL;
    item->intvalue=0;
    item->bint=false;
}
void cl_json_item_setname(cl_json_item_t* item,const char* name){
    strcpy(item->name,name);
}
void cl_json_item_setintvalue(cl_json_item_t* item,int value){
    item->intvalue=value;
    item->bint=true;
}
void json_item_setstrvalue(cl_json_item_t* item,const char* value){
    int size=strlen(value)+1;
    item->strvalue=malloc(size);
    strcpy(item->strvalue,value);
    item->strvalue[size-1]='\0';
}
void cl_CreateJson_Construct(cl_json_construct_t* json){
    cl_InitArrayd(&json->jsonitem,0,sizeof(cl_json_item_t));
}
void cl_DestroyJson_Construct(cl_json_construct_t* json){
    cl_json_item_t* items=json->jsonitem.data;
    for(int i=0;i<json->jsonitem.realsize;i++){
        if(items[i].strvalue!=NULL){
            free(items[i].strvalue);
        }
    }
    cl_DestroyArrayD(&json->jsonitem);
}
void cl_json_construct_addelement(cl_json_construct_t* json,cl_json_item_t item){
    cl_arrayd_addelement(json,&item);
}

char* cl_json_construct_getstring(cl_json_construct_t* json_c){
    int allsize=2;
    
    cl_json_item_t* array=json_c->jsonitem.data;
    
    for(int i=0;i<json_c->jsonitem.realsize;i++){
        allsize+=5+strlen(array[i].name);//5=|"":, |
        if(array[i].bint!=false){
            char intvaluestr[10];
            sprintf(intvaluestr, "%d",array[i].intvalue);
            allsize+=strlen(intvaluestr);
        }else{
            allsize+=2+strlen(array[i].strvalue);//2=|""|
        }
    }
    allsize-=2;//last |, |
    char* json=malloc(allsize+1);
    json[0]='\0';
    int indexjson=strlen(json);
    strcpy(&json[indexjson],"{");
    for(int i=0;i<json_c->jsonitem.realsize;i++){
        indexjson=strlen(json);
        strcpy(&json[indexjson],"\"");
        indexjson=strlen(json);
        strcpy(&json[indexjson],array[i].name);
        indexjson=strlen(json);
        strcpy(&json[indexjson],"\":");
        indexjson=strlen(json);
        if(array[i].bint!=false){
            char intvaluestr[10];
            sprintf(intvaluestr, "%d",array[i].intvalue);
            strcpy(&json[indexjson],intvaluestr);

        }else{
            strcpy(&json[indexjson],"\"");
            indexjson=strlen(json);
            strcpy(&json[indexjson],array[i].strvalue);
            indexjson=strlen(json);
            strcpy(&json[indexjson],"\"");
        }
        if(i!=json_c->jsonitem.realsize-1){
            indexjson=strlen(json);
            strcpy(&json[indexjson],", ");
        }
    }
    indexjson=strlen(json);
    strcpy(&json[indexjson],"}");
    indexjson=strlen(json);
    json[allsize]='\0';
    return json;
}
char* cl_json_construct_getstring_SEND(cl_json_construct_t* json_c,int* size){
    int allsize=2;
    
    cl_json_item_t* array=json_c->jsonitem.data;
    
    for(int i=0;i<json_c->jsonitem.realsize;i++){
        allsize+=5+strlen(array[i].name);//5=|"":, |
        if(array[i].bint!=false){
            char intvaluestr[10];
            sprintf(intvaluestr, "%d",array[i].intvalue);
            allsize+=strlen(intvaluestr);
        }else{
            allsize+=2+strlen(array[i].strvalue);//2=|""|
        }
    }
    allsize-=2;//last |, |
    char* json=malloc(allsize+1+4);
    int32_t s=allsize+1;
	memcpy(json,&s,4);
    int indexjson=4;
    json[4]='\0';
    strcpy(&json[indexjson],"{");
    char* t=&json[4];
    for(int i=0;i<json_c->jsonitem.realsize;i++){
        indexjson=strlen(&json[4])+4;
        strcpy(&json[indexjson],"\"");
        indexjson=strlen(&json[4])+4;
        strcpy(&json[indexjson],array[i].name);
        indexjson=strlen(&json[4])+4;
        strcpy(&json[indexjson],"\":");
        indexjson=strlen(&json[4])+4;
        if(array[i].bint!=false){
            char intvaluestr[10];
            sprintf(intvaluestr, "%d",array[i].intvalue);
            strcpy(&json[indexjson],intvaluestr);

        }else{
            strcpy(&json[indexjson],"\"");
            indexjson=strlen(&json[4])+4;
            strcpy(&json[indexjson],array[i].strvalue);
            indexjson=strlen(&json[4])+4;
            strcpy(&json[indexjson],"\"");
        }
        if(i!=json_c->jsonitem.realsize-1){
            indexjson=strlen(&json[4])+4;
            strcpy(&json[indexjson],", ");
        }
    }
    indexjson=strlen(&json[4])+4;
    strcpy(&json[indexjson],"}");
    indexjson=strlen(&json[4])+4;
    *size=allsize+1+4;
    json[allsize+4]='\0';
    return json;
}