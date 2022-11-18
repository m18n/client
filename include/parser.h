#pragma once
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
#include"json.h"
typedef struct cl_v2{
    void* p1;
    void* p2;
}cl_v2_t;
int cl_StringAddString(char* string,int sizestring,const char* str,int sizestr);
int cl_StringAddInt(char* string,int sizestring,int number);
int cl_GetLengthInt(int number);
int cl_SearchStringInArray(char* array,int size,int startindex,const char* search,int count);

int cl_SearchNameKey(json_value* json,const char* name);
json_object_entry* cl_GetNameKey(json_value* json,const char* name);
typedef struct cl_arrayd{
    void* data;
    int sizeelement;
    int realsize;
    int allsize;
}cl_arrayd_t;
void cl_CreateArrayD(cl_arrayd_t* array);
void cl_InitArrayd(cl_arrayd_t* array,int sizearray,int sizeelement);
void cl_arrayd_addelement(cl_arrayd_t* array,void* element);
void cl_DestroyArrayD(cl_arrayd_t* array);
typedef struct cl_json_item{
    char name[200];
    char* strvalue;
    int intvalue;
    bool bint;
}cl_json_item_t;
void cl_CreateJson_Item(cl_json_item_t* item);
void cl_json_item_setname(cl_json_item_t* item,const char* name);
void cl_json_item_setintvalue(cl_json_item_t* item,int value);
void cl_json_item_setstrvalue(cl_json_item_t* item,const char* value);
typedef struct cl_json_construct{
    cl_arrayd_t jsonitem;
}cl_json_construct_t;
void cl_CreateJson_Construct(cl_json_construct_t* json);
char* cl_json_construct_getstring(cl_json_construct_t* json_c);
void cl_json_construct_addelement(cl_json_construct_t* json,cl_json_item_t item);
char* cl_json_construct_getstring(cl_json_construct_t* json_c);
char* cl_json_construct_getstring_SEND(cl_json_construct_t* json_c,int* size);
void cl_DestroyJson_Construct(cl_json_construct_t* json);