/* 
 * File:   app_port.cpp
 * Author: Blazej Kita
 * 
 * Created on 31 sierpień 2016, 10:48
 */

#include "app_port.h"




app_port::app_port(gxh_scope* appHomeHandle, int ih_computer_port_id)
{
    this->appHandle         = appHomeHandle;   
    this->ih_computer_port_id = ih_computer_port_id; 
}


app_port::app_port(gxh_scope* appHomeHandle, const char* nameOnComputer,const char* pathToPort,const char* portType, int ih_computer_port_id,const char* parity,const char* stop_bit, int speed,const char* data)
{
    this->appHandle         = appHomeHandle;
    
    memset(this->name_on_computer,0, 50);
    memset(this->path,0, 255);
    memset(this->typ,0, 2);
    memset(this->parity,0, 2);
    memset(this->stop_bit,0, 2);
    memset(this->data,0, 2);
    
    memcpy(this->name_on_computer, nameOnComputer, strlen(nameOnComputer));
    memcpy(this->path,pathToPort, strlen(pathToPort));
    memcpy(this->typ,portType, strlen(portType));
    this->ih_computer_port_id = ih_computer_port_id;
    memcpy(this->parity,parity, strlen(parity));
    memcpy(this->stop_bit, stop_bit, strlen(stop_bit) );
    this->speed             = speed;
    memcpy(this->data,data, strlen(data));
    
    std::cout<<"\nName|"<<this->name_on_computer<<"|";
    std::cout<<"\nPath|"<<this->path<<"|";
    std::cout<<"\nTyp|"<<this->typ<<"|";
 
}


app_port::~app_port() {             
}

 
gxh_scope* app_port::getAppHandle() {
    return this->appHandle;
}

int app_port::getIhComputerPortId() {
    return this->ih_computer_port_id;
}

int app_port::readData(char* buffer, int size)
{
    return -1;
}
 
 int app_port::writeData(char* buffer, int size)
 {
     return -1;
 }

 void app_port::flush()
 {
     
 }
 
 bool app_port::changePortParameter( char parity, char stop_bit, int speed, char data)
 {
     return false;
 }
 
 
bool app_port::setDefaultPortParameter( )
{
    return false;
}
 

bool app_port::isCorrect()
{
    return this->workCorrect;
}
 



 

 
