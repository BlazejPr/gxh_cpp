/* 
 * File:   app_module_satel.cpp
 * Author: Blazej Kita
 * 
 * Created on 30 lipiec 2018, 14:31
 */

#include "app_module_satel.h"


 
app_module* get_module(gxh_scope* appHomeHandle )
{
    return new app_module_satel(appHomeHandle); 
};
 
//-------------------------------------------------------------------------------
  
app_module_satel::app_module_satel(gxh_scope* appHomeHandle) : app_module(appHomeHandle) 
{           
    this->getAppHandle()->show_log_module("app_module_satel","app_module_satel","start",2 );         
    
 
   
}

//------------------------------------------------------------------------------

app_module_satel::~app_module_satel() 
{     
    this->getAppHandle()->show_log_module("app_module_satel","app_module_satel","stop",2 );
    
 
}


//-------------------------------------------------------+-----------------------

void app_module_satel::onClick(int ih_driver_port_id, int listener_status)
{
}

//-------------------------------------------------------+-----------------------

void app_module_satel::forceCancel()
{  
      this->getAppHandle()->show_log_module("app_module_satel","app_module_satel","forceCancel",2);
      
    
      this->forceSynchronize();
      
}

//-------------------------------------------------------+-----------------------

void app_module_satel::forceSynchronize()
{  
     this->getAppHandle()->show_log_module("app_module_satel","forceSynchronize","start",20);          
     
     
     
     this->getAppHandle()->show_log_module("app_module_satel","forceSynchronize","stop",20);          
}

//-------------------------------------------------------+-----------------------


bool app_module_satel::execute(gxh_message* inMsg,gxh_message* message_ret)  
{ 
    
          
      gxh_message message;
      memcpy(&message, inMsg, sizeof(gxh_message));
    
     if(message.func_number == IH_MODULE_FUNCTION_OUTS_ON )     //wymuś włączenie
           {
             
           }
      
      
          
      
         
     
          const char* response = "SATEL MODULE"; 
          gxh_message msg;
          msg.gxh_handle = message.gxh_handle;
          memcpy(msg.dataStruct, response, strlen(response) );
          msg.sizeDataStruct = sizeof(response) ;
          msg.system_code = GXH_MESSAGE_SUCCESS;
            
          memcpy(message_ret, &msg, sizeof(gxh_message));            
          return true;
          
}

//------------------------------------------------------------------------------
