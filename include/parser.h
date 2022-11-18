#pragma once
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
#include"json.h"
typedef struct v2{
    void* p1;
    void* p2;
}v2_t;
int StringAddString(char* string,int sizestring,const char* str,int sizestr);
int StringAddInt(char* string,int sizestring,int number);
int GetLengthInt(int number);
int SearchStringInArray(char* array,int size,int startindex,const char* search,int count);

int SearchNameKey(json_value* json,const char* name);
json_object_entry* GetNameKey(json_value* json,const char* name);
typedef struct arrayd{
    void* data;
    int sizeelement;
    int realsize;
    int allsize;
}arrayd_t;
void CreateArrayD(arrayd_t* array);
void InitArrayd(arrayd_t* array,int sizearray,int sizeelement);
void arrayd_addelement(arrayd_t* array,void* element);
void DestroyArrayD(arrayd_t* array);
typedef struct json_item{
    char name[200];
    char* strvalue;
    int intvalue;
    bool bint;
}json_item_t;
void CreateJson_Item(json_item_t* item);
void json_item_setname(json_item_t* item,const char* name);
void json_item_setintvalue(json_item_t* item,int value);
void json_item_setstrvalue(json_item_t* item,const char* value);
typedef struct json_construct{
    arrayd_t jsonitem;
}json_construct_t;
void CreateJson_Construct(json_construct_t* json);
char* json_construct_getstring(json_construct_t* json_c);
void json_construct_addelement(json_construct_t* json,json_item_t item);
char* json_construct_getstring(json_construct_t* json_c);
char* json_construct_getstring_SEND(json_construct_t* json_c,int* size);
void DestroyJson_Construct(json_construct_t* json);