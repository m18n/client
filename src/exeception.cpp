#include"exeception.h"
char* NetworkExeption::what(){
    return &message[0];
}