/* 
 * File:   app_driver.cpp
 * Author: Blazej Kita
 * 
 * Created on 30 sierpień 2016, 14:35
 */

#include "app_driver.h"


app_driver* get_moduleDriver(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id, const char* name, const char* address, int delay_before_read_ms)
{         
    return new app_driver(appHomeHandle, appPortHandle, ih_driver_id, name,address,delay_before_read_ms);
}
 



app_driver::app_driver(gxh_scope* appHomeHandle, app_port* appPortHandle,int ih_driver_id,const char* name, const char* address, int delay_before_read_ms) 
{
    
    memset(this->name, 0, 50);
    memset(this->address, 0, 10);
    
    
    this->appHandle     = appHomeHandle;
    this->appPort       = appPortHandle;
    this->ih_driver_id  = ih_driver_id;
    
    memcpy(this->name, name, strlen(name));
    memcpy(this->address,address, strlen(address));
    
    this->delay_before_read_ms = delay_before_read_ms;
    
    this->debugLevel = 1;
    this->operationTyp = APP_DRIVER_EXECUTE_RESUME;    
    
   // this->appHandle->show_log_driver( "app_driver","app_driver","start" );
       
}

 

app_driver::~app_driver() {  
   // this->appHandle->show_log_driver("app_driver","app_driver","stop");
}

int app_driver::getOperationType()
{
    return  this->operationTyp;
}

 
void app_driver::setOperationType(int type)
{
    this->operationTyp = type;
}

int app_driver::getIhDriverId()
{
    return this->ih_driver_id;
}
void app_driver::setIhDriverId(int ih_driver_id)
{
     this->ih_driver_id = ih_driver_id;
}

int app_driver::getName(char* out)
{
    memcpy(out, this->name, strlen(this->name));
    return strlen(this->name);
}

int app_driver::getAddress(char* out)
{
    memcpy(out, this->address, strlen(this->address));
    return strlen(this->address);
}



char* app_driver::getAddressPtr()
{
    return (char*)this->address;
}

int app_driver::getDelayBeforeRead()
{
    return this->delay_before_read_ms;
}

void app_driver::setDelayBeforeRead(int delay)
{
    this->delay_before_read_ms = delay;
}


void app_driver::execute(gxh_message* command)
{
    
}

bool app_driver::resume()
{
    return false;
}

gxh_scope* app_driver::getAppHandle()
{
    return this->appHandle;
}

app_port* app_driver::getPortHandle()
{
    return this->appPort;
}
