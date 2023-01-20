#include<iostream>
class NetworkExeption:public std::exception{
    private:
    std::string message;
    public:
    NetworkExeption(std::string message){
        this->message="Exeption Network: "+message+"\n";
    }
    char* what();
};
