#pragma once
#include "json.h"
namespace client
{
    namespace pars{
        int jsonSearchNameKey(json_value *json, const char *name);
        json_object_entry * jsonGetNameKey(json_value *json, const char *name);
    }
    
};