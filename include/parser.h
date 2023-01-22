#pragma once
#include <iostream>
#include <vector>
#include "json.h"
namespace client {
namespace pars {
int jsonSearchNameKey(json_value* json, const char* name);
json_object_entry* jsonGetNameKey(json_value* json, const char* name);
class json_item {
 private:
  std::string name;
  std::string strvalue;
  int intvalue;
  bool bint;

 public:
  json_item() {
    intvalue = 0;
    bint = false;
  }
  void setname(std::string name);
  void setvalue_int(int value);
  void setvalue_str(std::string strvalue);
  std::string getname();
  std::string getvalue_str();
  int getvalue_int();
  bool bvalueint();
  void init(std::string name,std::string strvalue);
  void init(std::string name,int value);
  int getstrlength();
};
class json_construct {
 private:
  std::vector<json_item> json_items;

 public:
  std::string getstring();
  void addelement(json_item item);
  std::string getstringofsend();
};
}  // namespace pars
}  // namespace client