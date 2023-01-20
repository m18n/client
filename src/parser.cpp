#include "parser.h"
int client::pars::jsonSearchNameKey(json_value *json, const char *name)
{
    for (int i = 0; i < json->u.object.length; i++)
    {
        if (strcmp(json->u.object.values[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}
json_object_entry *client::pars::jsonGetNameKey(json_value *json, const char *name)
{
    for (int i = 0; i < json->u.object.length; i++)
    {
        if (strcmp(json->u.object.values[i].name, name) == 0)
        {
            return &json->u.object.values[i];
        }
    }
    return NULL;
}
