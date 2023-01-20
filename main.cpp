#include<stdio.h>
#include"client.h"
using namespace client;
int main(){
    printf("HELLO \n");
    try{
    Client cl;
    cl.ConnectToServer("127.0.0.1",9999,false);
    cl.StartGetPack();
    }catch(NetworkExeption ntw){
        std::cout<<ntw.what();
    }

    std::cin.get();
    std::cin.get();
    return 0;
}