#include "parser.h"
int client::pars::jsonSearchNameKey(json_value* json, const char* name) {
  for (int i = 0; i < json->u.object.length; i++) {
    if (strcmp(json->u.object.values[i].name, name) == 0) {
      return i;
    }
  }
  return -1;
}
json_object_entry* client::pars::jsonGetNameKey(json_value* json,
                                                const char* name) {
  for (int i = 0; i < json->u.object.length; i++) {
    if (strcmp(json->u.object.values[i].name, name) == 0) {
      return &json->u.object.values[i];
    }
  }
  return NULL;
}
void client::pars::json_item::setname(std::string name) {
  this->name = name;
}
void client::pars::json_item::setvalue_int(int value) {
  this->intvalue = value;
  this->bint = true;
}
void client::pars::json_item::setvalue_str(std::string strvalue) {
  this->strvalue = strvalue;
}
void client::pars::json_item::init(std::string name, std::string strvalue) {
  setname(name);
  setvalue_str(strvalue);
}
void client::pars::json_item::init(std::string name, int value) {
  setname(name);
  setvalue_int(value);
}
std::string client::pars::json_item::getname(){
  return this->name;
}
std::string client::pars::json_item::getvalue_str(){
  return this->strvalue;
}
int client::pars::json_item::getvalue_int(){
  return this->intvalue;
}
bool client::pars::json_item::bvalueint(){
  return this->bint;
}
int client::pars::json_item::getstrlength(){
  int allsize=0;
  allsize+=name.size()+5;// 5=|"":, |
  if(bint!=false){
    char intvaluestr[10];
    sprintf(intvaluestr, "%d", this->intvalue);
    allsize += strlen(intvaluestr);
  }else{
    allsize+=strvalue.size()+2;// 2=|""|
  }
  return allsize;
}
std::string client::pars::json_construct::getstring() {
  int allsize = 0;
  for (int i = 0; i < json_items.size(); i++) {
      allsize+=json_items[i].getstrlength();
  }
  std::string json;
  json.reserve(allsize);
  json+="{";
  for (int i = 0; i < json_items.size(); i++) {
    json+="\""+json_items[i].getname()+"\":";
    if(json_items[i].bvalueint()!=false){
      json+=std::to_string(json_items[i].getvalue_int());
    }else{
      json+="\""+json_items[i].getvalue_str()+"\"";
    }
    if (i != json_items.size()-1) {
      json+=", ";
    }
  }
  json+="}";
  return json;
}
void client::pars::json_construct::addelement(client::pars::json_item item) {
  this->json_items.push_back(item);
}
std::string client::pars::json_construct::getstringofsend() {
  int allsize = 0;
  for (int i = 0; i < json_items.size(); i++) {
      allsize+=json_items[i].getstrlength();
  }
  std::string json;
  json.reserve(allsize+4);
  json.resize(4);
  memcpy(&json[0],&allsize,4);
  json+="{";
  for (int i = 0; i < json_items.size(); i++) {
    json+="\""+json_items[i].getname()+"\":";
    if(json_items[i].bvalueint()!=false){
      json+=std::to_string(json_items[i].getvalue_int());
    }else{
      json+="\""+json_items[i].getvalue_str()+"\"";
    }
    if (i != json_items.size()-1) {
      json+=", ";
    }
  }
  json+="}";
  return json;
}