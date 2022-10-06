/* 
 * File:   app_module_ethernet.h
 * Author: Blazej Kita
 *
 * Created on 25 lipiec 2018, 14:31
 */

#include "app_module.h"

#ifndef APP_MODULE_ETHERNET_H
#define	APP_MODULE_ETHERNET_H

#include <netdb.h>
 

class app_module_ethernet : public app_module{
public:
              app_module_ethernet(gxh_scope* appHomeHandle); 
              virtual ~app_module_ethernet();       
   
    
                /**
              * Execute interrupt from GXHSystem
              * @param inMsg
              * @param outMsg
              * @return 
              */
             bool execute(gxh_message* inMsg, gxh_message* outMsg);
             
             /**
              * Method is call when GXHSystem is going to delete object from memory
              */
             void forceCancel();      
             void forceSynchronize();  
             void onClick(int ih_driver_port_id, int listener_status);
 
    
 private: 
          void getHost(char* from, char* to,char* param);
          bool sendEmailSMS(gxh_message* inMsg, gxh_message* outMsg, int type);
          bool php(gxh_message* inMsg, gxh_message* outMsg);
 
   
};



extern "C" app_module* get_module(gxh_scope* appHomeHandle);

#endif	/* APP_MODULE_ETHERNET_H */


